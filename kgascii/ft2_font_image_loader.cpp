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

#include <kgascii/ft2_font_image_loader.hpp>
#include <kgascii/ft2_font_loader.hpp>
#include <kgascii/surface_algorithm.hpp>

namespace KG { namespace Ascii {

FT2FontImageLoader::FT2FontImageLoader(FT2FontLoader& ldr)
    :loader_(ldr)
{
}

std::string FT2FontImageLoader::familyName() const
{
    return loader_.familyName();
}

std::string FT2FontImageLoader::styleName() const
{
    return loader_.styleName();
}

unsigned FT2FontImageLoader::pixelSize() const
{
    return loader_.pixelSize();
}

unsigned FT2FontImageLoader::glyphWidth() const
{
    return loader_.maxAdvance();
}

unsigned FT2FontImageLoader::glyphHeight() const
{
    return loader_.ascender() + loader_.descender();
}

std::vector<int> FT2FontImageLoader::charcodes() const
{
    return loader_.charcodes();
}

bool FT2FontImageLoader::loadGlyph(int charcode)
{
    if (!loader_.loadGlyph(charcode))
        return false;

    unsigned bmp_off_x = std::max<int>(-loader_.glyphLeft(), 0);
    unsigned bmp_off_y = std::max<int>(loader_.glyphTop() - loader_.ascender(), 0);
    unsigned bmp_width = std::min<unsigned>(loader_.glyphWidth(), glyphWidth() - bmp_off_x);
    unsigned bmp_height = std::min<unsigned>(loader_.glyphHeight(), glyphHeight() - bmp_off_y);

    unsigned img_off_x = std::max<int>(loader_.glyphLeft(), 0);
    unsigned img_off_y = std::max<int>(loader_.ascender() - loader_.glyphTop(), 0);
    unsigned common_width = std::min<unsigned>(bmp_width, glyphWidth() - img_off_x);
    unsigned common_height = std::min<unsigned>(bmp_height, glyphHeight() - img_off_y);

    assert(img_off_x + common_width <= glyphWidth());
    assert(img_off_y + common_height <= glyphHeight());

    glyphData_.resize(glyphWidth(), glyphHeight());
    const Surface8& glyph_surf = glyphData_.surface();
    fillPixels(glyph_surf, 0);
    copyPixels(loader_.glyph().window(bmp_off_x, bmp_off_y, common_width, common_height),
            glyph_surf.window(img_off_x, img_off_x, common_width, common_height));

    return true;
}

Surface8c FT2FontImageLoader::glyph() const
{
    return glyphData_.surface();
}

} } // namespace KG::Ascii

