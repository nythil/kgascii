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

#ifndef KGASCII_DYNAMIC_GLYPH_MATCHER_HPP
#define KGASCII_DYNAMIC_GLYPH_MATCHER_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/pointee.hpp>
#include <boost/bind.hpp>
#include <boost/functional/factory.hpp>

namespace KG { namespace Ascii {

template<class TFontImage, class TView=typename TFontImage::ConstViewT>
class DynamicGlyphMatcherContext: boost::noncopyable
{
public:
    typedef TFontImage FontImageT;
    typedef TView ViewT;

    class DynamicGlyphMatcher: boost::noncopyable
    {
        friend class DynamicGlyphMatcherContext;
    public:
        typedef DynamicGlyphMatcherContext GlyphMatcherContextT;
        typedef typename GlyphMatcherContextT::FontImageT FontImageT;
        typedef typename GlyphMatcherContextT::ViewT ViewT;

    public:
        const DynamicGlyphMatcherContext* context() const
        {
            return context_;
        }

        Symbol match(const ViewT& imgv)
        {
            return strategy_->match(imgv);
        }

        template<class TGlyphMatcher>
        void setStrategy(TGlyphMatcher* impl)
        {
            boost::scoped_ptr<TGlyphMatcher> impl_holder(impl);
            setStrategy(impl_holder);
        }

        template<class TGlyphMatcher>
        void setStrategy(boost::scoped_ptr<TGlyphMatcher>& impl)
        {
            boost::scoped_ptr<StrategyBase> new_strategy(new Strategy<TGlyphMatcher>(impl));
            strategy_.swap(new_strategy);
        }

    private:
        class StrategyBase
        {
        public:
            virtual ~StrategyBase()
            {
            }

            virtual Symbol match(const ViewT& imgv) = 0;
        };

        template<class TGlyphMatcher>
        class Strategy: public StrategyBase
        {
        public:
            explicit Strategy(boost::scoped_ptr<TGlyphMatcher>& impl)
            {
                impl_.swap(impl);
            }

            virtual Symbol match(const ViewT& imgv)
            {
                return impl_->match(imgv);
            }

        private:
            boost::scoped_ptr<TGlyphMatcher> impl_;
        };

    private:
        template<class TGlyphMatcher>
        explicit DynamicGlyphMatcher(const DynamicGlyphMatcherContext* c, TGlyphMatcher* impl)
            :context_(c)
        {
            setStrategy(impl);
        }

        template<class TGlyphMatcher>
        explicit DynamicGlyphMatcher(const DynamicGlyphMatcherContext* c, boost::scoped_ptr<TGlyphMatcher>& impl)
            :context_(c)
        {
            setStrategy(impl);
        }

    private:
        const DynamicGlyphMatcherContext* context_;
        boost::scoped_ptr<StrategyBase> strategy_;
    };
    typedef DynamicGlyphMatcher GlyphMatcherT;

public:
    template<class TGlyphMatcherContext>
    explicit DynamicGlyphMatcherContext(boost::scoped_ptr<TGlyphMatcherContext>& impl)
    {
        setStrategy(impl);
    }

    template<class TGlyphMatcherContext>
    explicit DynamicGlyphMatcherContext(TGlyphMatcherContext* impl)
    {
        setStrategy(impl);
    }

public:
    const FontImageT* font() const
    {
        return strategy_->font();
    }

    unsigned cellWidth() const
    {
        return strategy_->cellWidth();
    }

    unsigned cellHeight() const
    {
        return strategy_->cellHeight();
    }

    GlyphMatcherT* createMatcher() const
    {
        return strategy_->createMatcher(this);
    }

    template<class TGlyphMatcherContext>
    void setStrategy(TGlyphMatcherContext* impl)
    {
        boost::scoped_ptr<TGlyphMatcherContext> impl_holder(impl);
        setStrategy(impl_holder);
    }

    template<class TGlyphMatcherContext>
    void setStrategy(boost::scoped_ptr<TGlyphMatcherContext>& impl)
    {
        boost::scoped_ptr<StrategyBase> new_strategy(new Strategy<TGlyphMatcherContext>(impl));
        strategy_.swap(new_strategy);
    }

private:
    class StrategyBase
    {
    public:
        virtual ~StrategyBase()
        {
        }

        virtual const FontImageT* font() const = 0;

        virtual unsigned cellWidth() const = 0;

        virtual unsigned cellHeight() const = 0;

        virtual DynamicGlyphMatcher* createMatcher(const DynamicGlyphMatcherContext* ctx) const = 0;
    };

    template<class TGlyphMatcherContext>
    class Strategy: public StrategyBase
    {
    public:
        explicit Strategy(boost::scoped_ptr<TGlyphMatcherContext>& impl)
        {
            impl_.swap(impl);
        }

        virtual const FontImageT* font() const
        {
            return impl_->font();
        }

        virtual unsigned cellWidth() const
        {
            return impl_->cellWidth();
        }

        virtual unsigned cellHeight() const
        {
            return impl_->cellHeight();
        }

        virtual DynamicGlyphMatcher* createMatcher(const DynamicGlyphMatcherContext* ctx) const
        {
            typedef typename TGlyphMatcherContext::GlyphMatcherT RealGlyphMatcherT;
            boost::scoped_ptr<RealGlyphMatcherT> matcher_holder(impl_->createMatcher());
            return new DynamicGlyphMatcher(ctx, matcher_holder);
        }

    private:
        boost::scoped_ptr<TGlyphMatcherContext> impl_;
    };

private:
    boost::scoped_ptr<StrategyBase> strategy_;
};


} } // namespace KG::Ascii

#endif // KGASCII_DYNAMIC_GLYPH_MATCHER_HPP
