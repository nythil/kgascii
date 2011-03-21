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

#include <kgascii/fontloader.hpp>
#include <kgascii/ft2pp/library.hpp>
#include <kgascii/ft2pp/face.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>

namespace KG { namespace Ascii {

FontLoader::FontLoader()
    :library_(boost::make_shared<FT2pp::Library>())
    ,glyph_loaded_(false)
    ,hinting_(HintingNormal)
    ,autohint_(AutoHinterOff)
    ,mode_(RenderGrayscale)
{
}

bool FontLoader::loadFont(const std::string& file_path, int pixel_size)
{
    int face_idx = 0;
    int num_faces = 1;
    while (face_idx < num_faces) {
        boost::shared_ptr<FT2pp::Face> ft_face_ptr =
            boost::make_shared<FT2pp::Face>(boost::ref(*library_), file_path, face_idx);
        FT2pp::Face& ft_face = *ft_face_ptr;

        if (face_idx == 0) {
            num_faces = ft_face->num_faces;
        }
        face_idx++;

        if (!FT_HAS_HORIZONTAL(ft_face.handle()))
            continue;
        if (FT_IS_SCALABLE(ft_face.handle())) {
            ft_face.setPixelSizes(pixel_size, pixel_size);
            face_ = ft_face_ptr;
            return true;
        }
        if (!FT_HAS_FIXED_SIZES(ft_face.handle()))
            continue;

        for (int si = 0; si < ft_face->num_fixed_sizes; ++si) {
            FT_Bitmap_Size size = ft_face->available_sizes[si];
            if (size.y_ppem / 64 == pixel_size) {
                ft_face.setPixelSizes(pixel_size, pixel_size);
                face_ = ft_face_ptr;
                return true;
            }
        }
    }

    return false;
}

bool FontLoader::isFontOk() const
{
    return face_;
}

std::string FontLoader::familyName() const
{
    assert(isFontOk());
    return (*face_)->family_name;
}

std::string FontLoader::styleName() const
{
    assert(isFontOk());
    return (*face_)->style_name;
}

int FontLoader::pixelSize() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.y_ppem;
}

int FontLoader::ascender() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.ascender / 64;
}

int FontLoader::descender() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.descender / 64;
}

int FontLoader::maxAdvance() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.max_advance / 64;
}

bool FontLoader::fixedWidth() const
{
    assert(isFontOk());
    return FT_IS_FIXED_WIDTH(face_->handle());
}

std::vector<int> FontLoader::charcodes() const
{
    return std::vector<int>();
}

FontLoader::Hinting FontLoader::hinting() const
{
    return hinting_;
}

void FontLoader::setHinting(Hinting val)
{
    hinting_ = val;
}

FontLoader::AutoHinter FontLoader::autohinter() const
{
    return autohint_;
}

void FontLoader::setAutohinter(AutoHinter val)
{
    autohint_ = val;
}

FontLoader::RenderMode FontLoader::renderMode() const
{
    return mode_;
}

void FontLoader::setRenderMode(RenderMode val)
{
    mode_ = val;
}

bool FontLoader::loadGlyph(int charcode)
{
    assert(isFontOk());

    glyph_loaded_ = false;

    face_->loadChar(charcode, makeLoadFlags());
    face_->renderChar(static_cast<FT_Render_Mode>(makeRenderFlags()));

    FT_Bitmap bmp = (*face_)->glyph->bitmap;
    glyph_.recreate(bmp.width, bmp.rows);
    boost::gil::gray8_view_t glyph_view = boost::gil::view(glyph_);

    unsigned char* pbmp = bmp.buffer;
    if (bmp.pixel_mode == FT_PIXEL_MODE_MONO) {
        for (int y = 0; y < bmp.rows; ++y) {
            for (int x = 0; x < bmp.width; ++x) {
                int xbyte = x >> 3;
                int xbit = x & 7;
                int pix = (pbmp[xbyte] >> (7 - xbit)) & 1;
                glyph_view(x, y) = pix * 255;
            }
            pbmp += bmp.pitch;
        }
    } else if (bmp.pixel_mode == FT_PIXEL_MODE_GRAY) {
        int grays_max = bmp.num_grays - 1;
        for (int y = 0; y < bmp.rows; ++y) {
            for (int x = 0; x < bmp.width; ++x) {
                glyph_view(x, y) = (pbmp[x] * 255) / grays_max;
            }
            pbmp += bmp.pitch;
        }
    } else {
        assert(false);
        return false;
    }

    glyph_loaded_ = true;

    return true;
}

bool FontLoader::isGlyphOk() const
{
    return glyph_loaded_;
}

int FontLoader::glyphLeft() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap_left;
}

int FontLoader::glyphTop() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap_top;
}

int FontLoader::glyphWidth() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap.width;
}

int FontLoader::glyphHeight() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap.rows;
}

boost::gil::gray8c_view_t FontLoader::glyph() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return boost::gil::const_view(glyph_);
}

int FontLoader::makeLoadFlags() const
{
    int loadf = FT_LOAD_DEFAULT;

    switch (autohint_) {
    case AutoHinterForce: loadf |= FT_LOAD_FORCE_AUTOHINT; break;
    case AutoHinterOn: break;
    case AutoHinterOff: loadf |= FT_LOAD_NO_AUTOHINT; break;
    }

    switch (hinting_) {
    case HintingNormal: loadf |= FT_LOAD_TARGET_NORMAL; break;
    case HintingLight: loadf |= FT_LOAD_TARGET_LIGHT; break;
    case HintingOff: loadf |= FT_LOAD_NO_HINTING; break;
    }

    return loadf;
}

int FontLoader::makeRenderFlags() const
{
    switch (mode_) {
    case RenderGrayscale: return FT_RENDER_MODE_NORMAL;
    case RenderMonochrome: return FT_RENDER_MODE_MONO;
    }
}

} } // namespace KG::Ascii

