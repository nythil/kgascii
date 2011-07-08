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
#include <kgascii/asciifier.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/sequential_asciifier.hpp>
#include <kgascii/parallel_asciifier.hpp>

namespace KG { namespace Ascii {

template<typename TGlyphMatcherContext>
class DynamicAsciifier;

namespace Internal {

template<typename TGlyphMatcherContext>
struct Traits< DynamicAsciifier<TGlyphMatcherContext> >
{
    typedef TGlyphMatcherContext GlyphMatcherContextT;
    typedef typename TGlyphMatcherContext::GlyphMatcherT GlyphMatcherT;
    typedef typename TGlyphMatcherContext::ConstSurfaceT ConstSurfaceT;
};

} // namespace Internal

template<typename TGlyphMatcherContext>
class DynamicAsciifier: public Asciifier< DynamicAsciifier<TGlyphMatcherContext> >
{
public:
    typedef Asciifier< DynamicAsciifier<TGlyphMatcherContext> > BaseT;
    typedef typename BaseT::GlyphMatcherContextT GlyphMatcherContextT;
    typedef typename BaseT::GlyphMatcherT GlyphMatcherT;
    typedef typename BaseT::ConstSurfaceT ConstSurfaceT;

public:
    DynamicAsciifier(const GlyphMatcherContextT* c)
        :context_(c)
    {
        setSequential();
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
        setStrategy(new SequentialAsciifier<TGlyphMatcherContext>(context_));
    }

    void setParallel(unsigned cnt)
    {
        setStrategy(new ParallelAsciifier<TGlyphMatcherContext>(context_, cnt));
    }

private:
    template<typename TAsciifier>
    void setStrategy(Asciifier<TAsciifier>* impl)
    {
        strategy_.reset(new Strategy<TAsciifier>(&impl->derived()));
    }

private:
    class StrategyBase
    {
    public:
        StrategyBase()
        {
        }

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
        explicit Strategy(TAsciifier* impl)
            :impl_(impl)
        {
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


