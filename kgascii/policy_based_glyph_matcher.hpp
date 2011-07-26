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

#ifndef KGASCII_POLICYBASEDGLYPHMATCHER_HPP
#define KGASCII_POLICYBASEDGLYPHMATCHER_HPP

#include <limits>
#include <kgascii/font_image.hpp>
#include <kgascii/surface_container.hpp>
#include <kgascii/surface_algorithm.hpp>

namespace KG { namespace Ascii {

template<typename TDistance>
class PolicyBasedGlyphMatcherContext;
template<typename TDistance>
class PolicyBasedGlyphMatcher;

template<typename TDistance>
class PolicyBasedGlyphMatcher: boost::noncopyable
{
public:
    typedef PolicyBasedGlyphMatcherContext<TDistance> GlyphMatcherContextT;
    typedef FontImage<PixelType8> FontImageT;
    typedef Surface8 SurfaceT;
    typedef Surface8c ConstSurfaceT;
    typedef SurfaceContainer8 SurfaceContainerT;

public:
    explicit PolicyBasedGlyphMatcher(const GlyphMatcherContextT* c)
        :context_(c)
        ,container_(context_->cellWidth(), context_->cellHeight())
        ,surface_(container_.surface())
    {
    }

public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    Symbol match(const ConstSurfaceT& imgv);

private:
    const GlyphMatcherContextT* context_;
    SurfaceContainerT container_;
    SurfaceT surface_;
};


template<typename TDistance>
class PolicyBasedGlyphMatcherContext: boost::noncopyable
{
    friend class PolicyBasedGlyphMatcher<TDistance>;
public:
    typedef PolicyBasedGlyphMatcher<TDistance> GlyphMatcherT;
    typedef FontImage<PixelType8> FontImageT;
    typedef Surface8 SurfaceT;
    typedef Surface8c ConstSurfaceT;
    typedef SurfaceContainer8 SurfaceContainerT;

public:
    explicit PolicyBasedGlyphMatcherContext(const FontImageT* f,
            const TDistance& dist=TDistance())
        :font_(f)
    	,distance_(dist)
    {
    }

    const FontImageT* font() const
    {
        return font_;
    }

    unsigned cellWidth() const
    {
        return font_->glyphWidth();
    }

    unsigned cellHeight() const
    {
        return font_->glyphHeight();
    }

    GlyphMatcherT* createMatcher() const
    {
        return new GlyphMatcherT(this);
    }

private:
    const FontImageT* font_;
    TDistance distance_;
};

template<typename TDistance>
Symbol PolicyBasedGlyphMatcher<TDistance>::match(const ConstSurfaceT& imgv)
{
    assert(imgv.width() <= surface_.width());
    assert(imgv.height() <= surface_.height());

    fillPixels(surface_, 0);
    copyPixels(imgv, surface_.window(0, 0, imgv.width(), imgv.height()));

    int d2_min = std::numeric_limits<int>::max();
    Symbol cc_min;
    for (size_t ci = 0; ci < context_->font()->glyphCount(); ++ci) {
        int d2 = context_->distance_(surface_, context_->font()->getGlyph(ci));
        if (d2 < d2_min) {
            d2_min = d2;
            cc_min = context_->font()->getSymbol(ci);
        }
    }
    return cc_min;
}

} } // namespace KG::Ascii

#endif // KGASCII_POLICYBASEDGLYPHMATCHER_HPP


