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

#ifndef KGASCII_SQUAREDEUCLIDEANDISTANCE_HPP
#define KGASCII_SQUAREDEUCLIDEANDISTANCE_HPP

#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <kgascii/surface.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

class SquaredEuclideanDistance
{
public:
    int operator()(const Surface8c& img1, const Surface8c& img2) const
    {
        assert(img1.dimensions() == img2.dimensions());
        int result = 0;
        for (size_t y = 0; y < img1.height(); ++y) {
            Surface8c::pointer img1_it = img1.row(y);
            Surface8c::pointer img2_it = img2.row(y);
            for (size_t x = 0; x < img1.width(); ++x, ++img1_it, ++img2_it) {
                int df = *img1_it - *img2_it;
                result += df * df;
            }
        }
        return result;
    }
};

template<typename TPixel>
class SquaredEuclideanDistanceGlyphMatcherContextFactory
{
};

template<>
class SquaredEuclideanDistanceGlyphMatcherContextFactory<PixelType8>
{
public:
    typedef PolicyBasedGlyphMatcherContext<SquaredEuclideanDistance> GlyphMatcherContextT;

    DynamicGlyphMatcherContext<PixelType8>* operator()(const FontImage<PixelType8>* font, const std::map<std::string, std::string>& options)
    {
        (void)options;
        boost::scoped_ptr<GlyphMatcherContextT> impl_holder(new GlyphMatcherContextT(font));
        return new DynamicGlyphMatcherContext<PixelType8>(impl_holder);
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_SQUAREDEUCLIDEANDISTANCE_HPP



