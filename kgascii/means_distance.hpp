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
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

class MeansDistance
{
public:
    template<class TView>
    int operator()(const TView& view1, const TView& view2) const
    {
        assert(view1.dimensions() == view2.dimensions());
        size_t width = view1.width();
        size_t height = view1.height();
        int sum1 = 0, sum2 = 0;
        for (size_t y = 0; y < height; ++y) {
            typename TView::x_iterator it1 = view1.row_begin(y);
            typename TView::x_iterator it2 = view2.row_begin(y);
            for (size_t x = 0; x < width; ++x) {
                sum1 += get_color(*it1++, boost::gil::gray_color_t());
                sum2 += get_color(*it2++, boost::gil::gray_color_t());
            }
        }
        return abs(sum1 - sum2);
    }
};

template<class TFontImage>
class MeansDistanceGlyphMatcherFactory
{
public:
    typedef PolicyBasedGlyphMatcher<TFontImage, MeansDistance> GlyphMatcherT;
    typedef DynamicGlyphMatcher<TFontImage> DynamicGlyphMatcherT;

    boost::shared_ptr<DynamicGlyphMatcherT> operator()(boost::shared_ptr<const TFontImage> font, const std::map<std::string, std::string>&) const
    {
        boost::shared_ptr<GlyphMatcherT> matcher(new GlyphMatcherT(font));
        boost::shared_ptr<DynamicGlyphMatcherT> dynamic_matcher(new DynamicGlyphMatcherT(matcher));
        return dynamic_matcher;
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_MEANSDISTANCE_HPP

