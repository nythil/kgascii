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

#include <vector>
#include <boost/noncopyable.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/font_image.hpp>

namespace KG { namespace Ascii {

class DynamicGlyphMatcherContext;
class DynamicGlyphMatcher;

namespace Internal {

template<>
struct Traits<DynamicGlyphMatcherContext>
{
    typedef DynamicGlyphMatcherContext GlyphMatcherContextT;
    typedef DynamicGlyphMatcher GlyphMatcherT;
    typedef FontImage FontImageT;
    typedef Surface8 SurfaceT;
    typedef Surface8c ConstSurfaceT;
};

template<>
struct Traits<DynamicGlyphMatcher>: public Traits<DynamicGlyphMatcherContext>
{
};

} // namespace Internal

class DynamicGlyphMatcherContext: public GlyphMatcherContext<DynamicGlyphMatcherContext>
{
    friend class DynamicGlyphMatcher;

public:
    typedef GlyphMatcherContext<DynamicGlyphMatcherContext> BaseT;
    typedef typename BaseT::FontImageT FontImageT;
    typedef typename BaseT::ConstSurfaceT ConstSurfaceT;

public:
    template<typename TImplementation>
    explicit DynamicGlyphMatcherContext(GlyphMatcherContext<TImplementation>* stgy)
        :BaseT(0)
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

    Symbol match(const ConstSurfaceT& imgv) const
    {
        return strategy_->match(imgv);
    }

    DynamicGlyphMatcher* createMatcher() const
    {
        return strategy_->createMatcher(this);
    }

private:
    template<typename TImplementation>
    void setStrategy(GlyphMatcherContext<TImplementation>* impl)
    {
        strategy_.reset(new Strategy<TImplementation>(&impl->derived()));
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

        virtual Symbol match(const ConstSurfaceT& imgv) const = 0;

        virtual DynamicGlyphMatcher* createMatcher(const DynamicGlyphMatcherContext* ctx) const = 0;
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

        virtual Symbol match(const ConstSurfaceT& imgv) const
        {
            return impl_->match(imgv);
        }

        virtual DynamicGlyphMatcher* createMatcher(const DynamicGlyphMatcherContext* ctx) const;

    private:
        boost::scoped_ptr<TImplementation> impl_;
    };

private:
    boost::scoped_ptr<StrategyBase> strategy_;
};


class DynamicGlyphMatcher: public GlyphMatcher<DynamicGlyphMatcher>
{
public:
    typedef GlyphMatcher<DynamicGlyphMatcher> BaseT;
    typedef typename BaseT::ConstSurfaceT ConstSurfaceT;

public:
    template<typename TImplementation>
    DynamicGlyphMatcher(const DynamicGlyphMatcherContext* c, GlyphMatcher<TImplementation>* stgy)
        :context_(c)
    {
        setStrategy(stgy);
    }

public:
    const DynamicGlyphMatcherContext* context() const
    {
        return context_;
    }

    Symbol match(const ConstSurfaceT& imgv)
    {
        return strategy_->match(imgv);
    }

private:
    template<typename TImplementation>
    void setStrategy(GlyphMatcher<TImplementation>* impl)
    {
        strategy_.reset(new Strategy<TImplementation>(&impl->derived()));
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
    const DynamicGlyphMatcherContext* context_;
    boost::scoped_ptr<StrategyBase> strategy_;
};

template<typename TImplementation>
inline DynamicGlyphMatcher* DynamicGlyphMatcherContext::Strategy<TImplementation>::createMatcher(const DynamicGlyphMatcherContext* ctx) const
{
    return new DynamicGlyphMatcher(ctx, impl_->createMatcher());
}


} } // namespace KG::Ascii

#endif // KGASCII_DYNAMIC_GLYPH_MATCHER_HPP
