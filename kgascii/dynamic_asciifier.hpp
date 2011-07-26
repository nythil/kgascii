// This file is part of KG::Ascii.
//
// Copyright (C) 2011 Robert Konklewski <nythil@gmail.com>
//
// KG::Ascii is free software; you can redistribute it and/or modify 
// it under the terms of the GNU Lesser General Public License as published by 
// the Free Software Foundation; either version 3 of the License, or 
// (at your option) any later version.
//
// KG::Ascii is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License 
// along with KG::Ascii. If not, see <http://www.gnu.org/licenses/>.

#ifndef KGASCII_DYNAMICASCIIFIER_HPP
#define KGASCII_DYNAMICASCIIFIER_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/pointee.hpp>
#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>
#include <kgascii/sequential_asciifier.hpp>
#include <kgascii/parallel_asciifier.hpp>

namespace KG { namespace Ascii {

template<typename TGlyphMatcherContext>
class DynamicAsciifier: boost::noncopyable
{
public:
    typedef TGlyphMatcherContext GlyphMatcherContextT;
    typedef typename TGlyphMatcherContext::GlyphMatcherT GlyphMatcherT;
    typedef typename TGlyphMatcherContext::ConstSurfaceT ConstSurfaceT;

public:
    explicit DynamicAsciifier(const GlyphMatcherContextT* ctx)
        :context_(ctx)
    {
        setSequential();
    }

    explicit DynamicAsciifier(const GlyphMatcherContextT* ctx, unsigned thr_cnt)
        :context_(ctx)
    {
        setParallel(thr_cnt);
    }

public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    unsigned threadCount() const
    {
        return strategy_->threadCount();
    }

public:
    void generate(const ConstSurfaceT& imgv, TextSurface& text)
    {
        strategy_->generate(imgv, text);
    }

    void setSequential()
    {
        typedef SequentialAsciifier<TGlyphMatcherContext> SequentialAsciifierT;
        setStrategy(boost::factory<SequentialAsciifierT*>());
    }

    void setParallel(unsigned cnt)
    {
        typedef ParallelAsciifier<TGlyphMatcherContext> ParallelAsciifierT;
        setStrategy(boost::bind(boost::factory<ParallelAsciifierT*>(), _1, cnt));
    }

    template<typename TAsciifierFactory>
    void setStrategy(TAsciifierFactory make_impl=TAsciifierFactory())
    {
        typedef typename boost::pointee<typename TAsciifierFactory::result_type>::type AsciifierT;
        boost::scoped_ptr<AsciifierT> new_impl(make_impl(context_));
        boost::scoped_ptr<StrategyBase> new_strategy(new Strategy<AsciifierT>(new_impl));
        strategy_.swap(new_strategy);
    }

private:
    class StrategyBase
    {
    public:
        virtual ~StrategyBase()
        {
        }

        virtual unsigned threadCount() const = 0;

        virtual void generate(const ConstSurfaceT& imgv, TextSurface& text) = 0;
    };

    template<typename TAsciifier>
    class Strategy: public StrategyBase
    {
    public:
        explicit Strategy(boost::scoped_ptr<TAsciifier>& impl)
        {
            impl_.swap(impl);
        }

        virtual unsigned threadCount() const
        {
            return impl_->threadCount();
        }

        virtual void generate(const ConstSurfaceT& imgv, TextSurface& text)
        {
            impl_->generate(imgv, text);
        }

    private:
        boost::scoped_ptr<TAsciifier> impl_;
    };

private:
    const GlyphMatcherContextT* context_;
    boost::scoped_ptr<StrategyBase> strategy_;
};

} } // namespace KG::Ascii

#endif // KGASCII_DYNAMICASCIIFIER_HPP


