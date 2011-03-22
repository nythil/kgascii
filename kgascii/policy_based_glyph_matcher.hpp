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

#include <vector>
#include <limits>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/surface_container.hpp>
#include <kgascii/surface_algorithm.hpp>

namespace KG { namespace Ascii {

template<typename DistancePolicy>
class PolicyBasedGlyphMatcherContext;


template<typename DistancePolicy>
class PolicyBasedGlyphMatcher: public GlyphMatcher
{
public:
    typedef PolicyBasedGlyphMatcherContext<DistancePolicy> Context;

public:
    explicit PolicyBasedGlyphMatcher(const Context& c)
        :GlyphMatcher()
        ,context_(c)
        ,container_(context_.cellWidth() * context_.cellHeight())
        ,surface_(container_.surface())
    {
    }

public:
    const Context& context() const
    {
        return context_;
    }

    unsigned match(const Surface8c& imgv);

private:
    const Context& context_;
    SurfaceContainer8 container_;
    Surface8 surface_;
};


template<typename DistancePolicy>
class PolicyBasedGlyphMatcherContext: public GlyphMatcherContext
{
    friend class PolicyBasedGlyphMatcher<DistancePolicy>;

public:
    typedef PolicyBasedGlyphMatcher<DistancePolicy> Matcher;

public:
    explicit PolicyBasedGlyphMatcherContext(const FontImage& f, 
            const DistancePolicy& dist=DistancePolicy())
        :GlyphMatcherContext(f)
        ,charcodes_(font().charcodes())
        ,glyphs_(font().glyphs())
    {
    }

public:
    Matcher* createMatcher() const
    {
        return new Matcher(*this);
    }

private:
    std::vector<unsigned> charcodes_;
    std::vector<Surface8c> glyphs_;
    DistancePolicy distance_;
};

template<typename DistancePolicy>
unsigned PolicyBasedGlyphMatcher<DistancePolicy>::match(const Surface8c& imgv)
{
    assert(imgv.width() <= surface_.width());
    assert(imgv.height() <= surface_.height());

    fillPixels(surface_, 0);
    copyPixels(imgv, surface_.window(0, 0, imgv.width(), imgv.height()));

    int d2_min = std::numeric_limits<int>::max();
    unsigned cc_min = ' ';
    for (size_t ci = 0; ci < context_.charcodes_.size(); ++ci) {
        int d2 = context_.distance_(surface_, context_.glyphs_[ci]);
        if (d2 < d2_min) {
            d2_min = d2;
            cc_min = context_.charcodes_[ci];
        }
    }
    return cc_min;
}

} } // namespace KG::Ascii

#endif // KGASCII_POLICYBASEDGLYPHMATCHER_HPP


