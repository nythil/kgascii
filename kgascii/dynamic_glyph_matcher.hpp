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
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

namespace KG { namespace Ascii {

template<class TFontImage, class TView=typename TFontImage::ConstViewT>
class DynamicGlyphMatcher: boost::noncopyable
{
public:
    typedef TFontImage FontImageT;
    typedef TView ViewT;

    class DynamicContext
    {
        friend class DynamicGlyphMatcher;
    public:
        typedef DynamicGlyphMatcher GlyphMatcherT;

    public:
        template<class TContext>
        TContext& cast()
        {
            return boost::any_cast<TContext&>(strategy_);
        }

    private:
        template<class TContext>
        explicit DynamicContext(const TContext& impl)
            :strategy_(impl)
        {
        }

    private:
        boost::any strategy_;
    };
    typedef DynamicContext ContextT;

public:
    template<class TGlyphMatcher>
    explicit DynamicGlyphMatcher(boost::shared_ptr<TGlyphMatcher> impl)
    {
        setStrategy(impl);
    }

public:
    boost::shared_ptr<const FontImageT> font() const
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

    DynamicContext createContext() const
    {
        return strategy_->createContext();
    }

    Symbol match(DynamicContext& ctx, const ViewT& imgv) const
    {
        return strategy_->match(ctx, imgv);
    }

    template<class TGlyphMatcher>
    void setStrategy(boost::shared_ptr<TGlyphMatcher> impl)
    {
        strategy_ = Strategy<TGlyphMatcher>::create(impl);
    }

private:
    class StrategyBase: boost::noncopyable
    {
    public:
        virtual ~StrategyBase() { }

        virtual boost::shared_ptr<const FontImageT> font() const = 0;

        virtual unsigned cellWidth() const = 0;

        virtual unsigned cellHeight() const = 0;

        virtual DynamicContext createContext() const = 0;

        virtual Symbol match(DynamicContext& ctx, const ViewT& imgv) const = 0;
    };

    template<class TGlyphMatcher>
    class Strategy: public StrategyBase
    {
        explicit Strategy(boost::shared_ptr<const TGlyphMatcher> impl)
            :impl_(impl)
        {
        }

    public:
        static boost::shared_ptr<Strategy> create(boost::shared_ptr<const TGlyphMatcher> impl)
        {
            boost::shared_ptr<Strategy> ptr(new Strategy(impl));
            return ptr;
        }

        virtual boost::shared_ptr<const FontImageT> font() const
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

        virtual DynamicContext createContext() const
        {
            return DynamicContext(impl_->createContext());
        }

        virtual Symbol match(DynamicContext& ctx, const ViewT& imgv) const
        {
            typedef typename TGlyphMatcher::ContextT RealContextT;
            return impl_->match(ctx.template cast<RealContextT>(), imgv);
        }

    private:
        boost::shared_ptr<const TGlyphMatcher> impl_;
    };

private:
    boost::shared_ptr<const StrategyBase> strategy_;
};


} } // namespace KG::Ascii

#endif // KGASCII_DYNAMIC_GLYPH_MATCHER_HPP
