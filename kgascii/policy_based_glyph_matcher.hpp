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
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/typedefs.hpp>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/font_image.hpp>

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
        ,image_(context_.cellWidth(), context_.cellHeight())
        ,view_(boost::gil::view(image_))
    {
    }

public:
    const Context& context() const
    {
        return context_;
    }

    char match(const boost::gil::gray8c_view_t& imgv);

private:
    const Context& context_;
    boost::gil::gray8_image_t image_;
    boost::gil::gray8_view_t view_;
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
        ,glyphs_(charcodes_.size())
    {
        for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
            glyphs_[ci] = font().getGlyph(charcodes_[ci]);
        }
    }

public:
    Matcher* createMatcher() const
    {
        return new Matcher(*this);
    }

private:
    std::vector<int> charcodes_;
    std::vector<boost::gil::gray8c_view_t> glyphs_;
    DistancePolicy distance_;
};

template<typename DistancePolicy>
char PolicyBasedGlyphMatcher<DistancePolicy>::match(const boost::gil::gray8c_view_t& imgv)
{
    assert(imgv.width() <= view_.width());
    assert(imgv.height() <= view_.height());

    boost::gil::fill_pixels(view_, 0);
    boost::gil::copy_pixels(imgv, 
        boost::gil::subimage_view(view_, 0, 0, imgv.width(), imgv.height()));

    int d2_min = std::numeric_limits<int>::max();
    int cc_min = ' ';
    for (size_t ci = 0; ci < context_.charcodes_.size(); ++ci) {
        int d2 = context_.distance_(view_, context_.glyphs_[ci]);
        if (d2 < d2_min) {
            d2_min = d2;
            cc_min = context_.charcodes_[ci];
        }
    }
    return (char)cc_min;
}

} } // namespace KG::Ascii

#endif // KGASCII_POLICYBASEDGLYPHMATCHER_HPP


