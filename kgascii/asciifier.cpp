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

#include "asciifier.hpp"
#include "glyphmatcher.hpp"
#include "textsurface.hpp"
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

namespace KG { namespace Ascii {

using namespace boost::gil;

Asciifier::Asciifier(const GlyphMatcher& m)
    :matcher_(m)
{
}

void Asciifier::generate(const gray8c_view_t& imgv, TextSurface& text) const
{
    int char_w = matcher_.glyphWidth();
    int char_h = matcher_.glyphHeight();

    int img_w = imgv.width();
    int img_h = imgv.height();

    gray8_image_t tmpimg(char_w, char_h);
    gray8_view_t tmpview = view(tmpimg);

    for (int r = 0; r < text.rows(); ++r) {
        int y = r * char_h;
        int dy = std::min(char_h, img_h - y);
        for (int c = 0; c < text.cols(); ++c) {
            int x = c * char_w;
            int dx = std::min(char_w, img_w - x);

            fill_pixels(tmpview, 0);
            copy_pixels(subimage_view(imgv, x, y, dx, dy), 
                    subimage_view(tmpview, 0, 0, dx, dy));

            text(r, c) = matcher_.match(tmpview);
        }
    }

}

} } // namespace KG::Ascii

