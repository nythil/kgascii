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
#include <boost/shared_ptr.hpp>
#include <kgascii/sequential_asciifier.hpp>
#include <kgascii/parallel_asciifier.hpp>

namespace KG { namespace Ascii {

template<class TGlyphMatcher, class TView=typename TGlyphMatcher::ViewT>
class DynamicAsciifier: boost::noncopyable
{
public:
    typedef TGlyphMatcher GlyphMatcherT;
    typedef TView ViewT;
    typedef typename TGlyphMatcher::ContextT ContextT;

public:
    explicit DynamicAsciifier(boost::shared_ptr<const GlyphMatcherT> ctx)
    {
        setSequential(ctx);
    }

    explicit DynamicAsciifier(boost::shared_ptr<const GlyphMatcherT> ctx, unsigned thr_cnt)
    {
        setParallel(ctx, thr_cnt);
    }

public:
    boost::shared_ptr<const GlyphMatcherT> matcher() const
    {
        return strategy_->matcher();
    }

    unsigned threadCount() const
    {
        return strategy_->threadCount();
    }

public:
    void generate(const ViewT& imgv, TextSurface& text)
    {
        strategy_->generate(imgv, text);
    }

    void setSequential()
    {
        setSequential(matcher());
    }

    void setSequential(boost::shared_ptr<const GlyphMatcherT> ctx)
    {
        typedef SequentialAsciifier<GlyphMatcherT> SequentialAsciifierT;
        boost::shared_ptr<SequentialAsciifierT> impl(new SequentialAsciifierT(ctx));
        setStrategy(impl);
    }

    void setParallel(unsigned thr_cnt)
    {
        setParallel(matcher(), thr_cnt);
    }

    void setParallel(boost::shared_ptr<const GlyphMatcherT> ctx, unsigned thr_cnt)
    {
        typedef ParallelAsciifier<GlyphMatcherT, ViewT> ParallelAsciifierT;
        boost::shared_ptr<ParallelAsciifierT> impl(new ParallelAsciifierT(ctx, thr_cnt));
        setStrategy(impl);
    }

    template<class TAsciifier>
    void setStrategy(boost::shared_ptr<TAsciifier> impl)
    {
        strategy_ = Strategy<TAsciifier>::create(impl);
    }

private:
    class StrategyBase: boost::noncopyable
    {
    public:
        virtual ~StrategyBase() { }

        virtual boost::shared_ptr<const GlyphMatcherT> matcher() const = 0;

        virtual unsigned threadCount() const = 0;

        virtual void generate(const ViewT& imgv, TextSurface& text) const = 0;
    };

    template<class TAsciifier>
    class Strategy: public StrategyBase
    {
        explicit Strategy(boost::shared_ptr<TAsciifier> impl)
            :impl_(impl)
        {
        }

    public:
        static boost::shared_ptr<Strategy> create(boost::shared_ptr<TAsciifier> impl)
        {
            boost::shared_ptr<Strategy> ptr(new Strategy(impl));
            return ptr;
        }

        virtual boost::shared_ptr<const GlyphMatcherT> matcher() const
        {
            return impl_->matcher();
        }

        virtual unsigned threadCount() const
        {
            return impl_->threadCount();
        }

        virtual void generate(const ViewT& imgv, TextSurface& text) const
        {
            impl_->generate(imgv, text);
        }

    private:
        boost::shared_ptr<TAsciifier> impl_;
    };

private:
    boost::shared_ptr<const StrategyBase> strategy_;
};

} } // namespace KG::Ascii

#endif // KGASCII_DYNAMICASCIIFIER_HPP


