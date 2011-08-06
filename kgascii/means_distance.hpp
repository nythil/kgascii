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

#ifndef KGASCII_MEANSDISTANCE_HPP
#define KGASCII_MEANSDISTANCE_HPP

#include <cstdlib>
#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

class MeansDistance
{
public:
    int operator()(const boost::gil::gray8c_view_t& view1, const boost::gil::gray8c_view_t& view2) const
    {
        assert(view1.dimensions() == view2.dimensions());
        size_t width = view1.width();
        size_t height = view1.height();
        int sum1 = 0, sum2 = 0;
        for (size_t y = 0; y < height; ++y) {
            boost::gil::gray8c_view_t::x_iterator it1 = view1.row_begin(y);
            boost::gil::gray8c_view_t::x_iterator it2 = view2.row_begin(y);
            for (size_t x = 0; x < width; ++x) {
                sum1 += get_color(*it1++, boost::gil::gray_color_t());
                sum2 += get_color(*it2++, boost::gil::gray_color_t());
            }
        }
        return abs(sum1 - sum2);
    }
};

template<class TFontImage>
class MeansDistanceGlyphMatcherContextFactory
{
public:
    typedef PolicyBasedGlyphMatcherContext<TFontImage, MeansDistance> GlyphMatcherContextT;

    DynamicGlyphMatcherContext<TFontImage>* operator()(const TFontImage* font, const std::map<std::string, std::string>&)
    {
        boost::scoped_ptr<GlyphMatcherContextT> impl_holder(new GlyphMatcherContextT(font));
        return new DynamicGlyphMatcherContext<TFontImage>(impl_holder);
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_MEANSDISTANCE_HPP

