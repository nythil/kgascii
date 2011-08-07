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
class DynamicGlyphMatcher: boost::noncopyable
{
public:
    typedef TFontImage FontImageT;
    typedef TView ViewT;

    class DynamicContext: boost::noncopyable
    {
        friend class DynamicGlyphMatcher;
    public:
        typedef DynamicGlyphMatcher GlyphMatcherT;
        typedef typename GlyphMatcherT::FontImageT FontImageT;
        typedef typename GlyphMatcherT::ViewT ViewT;

    public:
        const DynamicGlyphMatcher* matcher() const
        {
            return matcher_;
        }

        Symbol match(const ViewT& imgv)
        {
            return strategy_->match(imgv);
        }

        template<class TContext>
        void setStrategy(TContext* impl)
        {
            boost::scoped_ptr<TContext> impl_holder(impl);
            setStrategy(impl_holder);
        }

        template<class TContext>
        void setStrategy(boost::scoped_ptr<TContext>& impl)
        {
            boost::scoped_ptr<StrategyBase> new_strategy(new Strategy<TContext>(impl));
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

        template<class TContext>
        class Strategy: public StrategyBase
        {
        public:
            explicit Strategy(boost::scoped_ptr<TContext>& impl)
            {
                impl_.swap(impl);
            }

            virtual Symbol match(const ViewT& imgv)
            {
                return impl_->match(imgv);
            }

        private:
            boost::scoped_ptr<TContext> impl_;
        };

    private:
        template<class TContext>
        explicit DynamicContext(const DynamicGlyphMatcher* c, TContext* impl)
            :matcher_(c)
        {
            setStrategy(impl);
        }

        template<class TContext>
        explicit DynamicContext(const DynamicGlyphMatcher* c, boost::scoped_ptr<TContext>& impl)
            :matcher_(c)
        {
            setStrategy(impl);
        }

    private:
        const DynamicGlyphMatcher* matcher_;
        boost::scoped_ptr<StrategyBase> strategy_;
    };
    typedef DynamicContext ContextT;

public:
    template<class TGlyphMatcher>
    explicit DynamicGlyphMatcher(boost::scoped_ptr<TGlyphMatcher>& impl)
    {
        setStrategy(impl);
    }

    template<class TGlyphMatcher>
    explicit DynamicGlyphMatcher(TGlyphMatcher* impl)
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

    ContextT* createContext() const
    {
        return strategy_->createContext(this);
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

        virtual const FontImageT* font() const = 0;

        virtual unsigned cellWidth() const = 0;

        virtual unsigned cellHeight() const = 0;

        virtual DynamicContext* createContext(const DynamicGlyphMatcher* ctx) const = 0;
    };

    template<class TGlyphMatcher>
    class Strategy: public StrategyBase
    {
    public:
        explicit Strategy(boost::scoped_ptr<TGlyphMatcher>& impl)
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

        virtual DynamicContext* createContext(const DynamicGlyphMatcher* ctx) const
        {
            typedef typename TGlyphMatcher::ContextT RealContextT;
            boost::scoped_ptr<RealContextT> matcher_holder(impl_->createContext());
            return new DynamicContext(ctx, matcher_holder);
        }

    private:
        boost::scoped_ptr<TGlyphMatcher> impl_;
    };

private:
    boost::scoped_ptr<StrategyBase> strategy_;
};


} } // namespace KG::Ascii

#endif // KGASCII_DYNAMIC_GLYPH_MATCHER_HPP
