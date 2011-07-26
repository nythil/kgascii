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
#include <kgascii/font_image.hpp>

namespace KG { namespace Ascii {

template<typename TPixel>
class DynamicGlyphMatcherContext;
template<typename TPixel>
class DynamicGlyphMatcher;

template<typename TPixel>
class DynamicGlyphMatcherContext: boost::noncopyable
{
    friend class DynamicGlyphMatcher<TPixel>;

public:
    typedef DynamicGlyphMatcher<TPixel> GlyphMatcherT;
    typedef FontImage<TPixel> FontImageT;
    typedef Surface<TPixel> SurfaceT;
    typedef Surface<const TPixel> ConstSurfaceT;

public:
    template<typename TImplementation>
    explicit DynamicGlyphMatcherContext(TImplementation* stgy)
    {
        setStrategy(stgy);
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

private:
    template<typename TImplementation>
    void setStrategy(TImplementation* impl)
    {
        strategy_.reset(new Strategy<TImplementation>(impl));
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

        virtual const FontImageT* font() const = 0;

        virtual unsigned cellWidth() const = 0;

        virtual unsigned cellHeight() const = 0;

        virtual GlyphMatcherT* createMatcher(const DynamicGlyphMatcherContext<TPixel>* ctx) const = 0;
    };

    template<typename TImplementation>
    class Strategy: public StrategyBase
    {
    public:
        explicit Strategy(TImplementation* impl)
            :impl_(impl)
        {
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

        virtual GlyphMatcherT* createMatcher(const DynamicGlyphMatcherContext<TPixel>* ctx) const;

    private:
        boost::scoped_ptr<TImplementation> impl_;
    };

private:
    boost::scoped_ptr<StrategyBase> strategy_;
};


template<typename TPixel>
class DynamicGlyphMatcher: boost::noncopyable
{
public:
    typedef DynamicGlyphMatcherContext<TPixel> GlyphMatcherContextT;
    typedef Surface<TPixel> SurfaceT;
    typedef Surface<const TPixel> ConstSurfaceT;

public:
    template<typename TImplementation>
    DynamicGlyphMatcher(const GlyphMatcherContextT* c, TImplementation* stgy)
        :context_(c)
    {
        setStrategy(stgy);
    }

public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    Symbol match(const ConstSurfaceT& imgv)
    {
        return strategy_->match(imgv);
    }

private:
    template<typename TImplementation>
    void setStrategy(TImplementation* impl)
    {
        strategy_.reset(new Strategy<TImplementation>(impl));
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

        virtual Symbol match(const ConstSurfaceT& imgv) = 0;
    };

    template<typename TImplementation>
    class Strategy: public StrategyBase
    {
    public:
        explicit Strategy(TImplementation* impl)
            :impl_(impl)
        {
        }

        virtual Symbol match(const ConstSurfaceT& imgv)
        {
            return impl_->match(imgv);
        }

    private:
        boost::scoped_ptr<TImplementation> impl_;
    };

private:
    const GlyphMatcherContextT* context_;
    boost::scoped_ptr<StrategyBase> strategy_;
};

template<typename TPixel>
template<typename TImplementation>
inline DynamicGlyphMatcher<TPixel>* DynamicGlyphMatcherContext<TPixel>::Strategy<TImplementation>::createMatcher(const DynamicGlyphMatcherContext<TPixel>* ctx) const
{
    return new DynamicGlyphMatcher<TPixel>(ctx, impl_->createMatcher());
}


} } // namespace KG::Ascii

#endif // KGASCII_DYNAMIC_GLYPH_MATCHER_HPP
