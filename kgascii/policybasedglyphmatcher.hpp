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
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>
#include "glyphmatcher.hpp"
#include "fontimage.hpp"

namespace KG { namespace Ascii {

template<typename DistancePolicy>
class PolicyBasedGlyphMatcher: public GlyphMatcher
{
public:
    PolicyBasedGlyphMatcher(const FontImage& f, 
            const DistancePolicy& dist=DistancePolicy())
        :GlyphMatcher(f)
        ,charcodes_(font().charcodes())
        ,glyphs_(charcodes_.size())
    {
        for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
            glyphs_[ci] = font().getGlyph(charcodes_[ci]);
        }
    }

public:
    char match(const boost::gil::gray8c_view_t& imgv) const
    {
        assert(imgv.width() == glyphWidth());
        assert(imgv.height() == glyphHeight());
        int d2_min = std::numeric_limits<int>::max();
        int cc_min = ' ';
        for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
            int d2 = distance_(imgv, glyphs_[ci]);
            if (d2 < d2_min) {
                d2_min = d2;
                cc_min = charcodes_[ci];
            }
        }
        return (char)cc_min;
    }

private:
    std::vector<int> charcodes_;
    std::vector<boost::gil::gray8c_view_t> glyphs_;
    DistancePolicy distance_;
};

} } // namespace KG::Ascii

#endif // KGASCII_POLICYBASEDGLYPHMATCHER_HPP


