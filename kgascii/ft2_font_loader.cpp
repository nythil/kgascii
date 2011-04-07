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

#include <kgascii/ft2_font_loader.hpp>
#include <kgascii/ft2pp/library.hpp>
#include <kgascii/ft2pp/face.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>

namespace KG { namespace Ascii {

FT2FontLoader::FT2FontLoader()
    :library_(boost::make_shared<FT2pp::Library>())
    ,glyph_loaded_(false)
    ,hinting_(HintingNormal)
    ,autohint_(AutoHinterOff)
    ,mode_(RenderGrayscale)
{
}

bool FT2FontLoader::loadFont(const std::string& file_path, unsigned pixel_size)
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
            if (static_cast<unsigned>(size.y_ppem / 64) == pixel_size) {
                ft_face.setPixelSizes(pixel_size, pixel_size);
                face_ = ft_face_ptr;
                return true;
            }
        }
    }

    return false;
}

bool FT2FontLoader::isFontOk() const
{
    return face_;
}

std::string FT2FontLoader::familyName() const
{
    assert(isFontOk());
    return (*face_)->family_name;
}

std::string FT2FontLoader::styleName() const
{
    assert(isFontOk());
    return (*face_)->style_name;
}

unsigned FT2FontLoader::pixelSize() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.y_ppem;
}

unsigned FT2FontLoader::ascender() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.ascender / 64;
}

unsigned FT2FontLoader::descender() const
{
    assert(isFontOk());
    return -(*face_)->size->metrics.descender / 64;
}

unsigned FT2FontLoader::maxAdvance() const
{
    assert(isFontOk());
    return (*face_)->size->metrics.max_advance / 64;
}

bool FT2FontLoader::fixedWidth() const
{
    assert(isFontOk());
    return (FT_IS_FIXED_WIDTH(face_->handle()) != 0);
}

std::vector<unsigned> FT2FontLoader::charcodes() const
{
    return std::vector<unsigned>();
}

FT2FontLoader::Hinting FT2FontLoader::hinting() const
{
    return hinting_;
}

void FT2FontLoader::setHinting(Hinting val)
{
    hinting_ = val;
}

FT2FontLoader::AutoHinter FT2FontLoader::autohinter() const
{
    return autohint_;
}

void FT2FontLoader::setAutohinter(AutoHinter val)
{
    autohint_ = val;
}

FT2FontLoader::RenderMode FT2FontLoader::renderMode() const
{
    return mode_;
}

void FT2FontLoader::setRenderMode(RenderMode val)
{
    mode_ = val;
}

bool FT2FontLoader::loadGlyph(unsigned charcode)
{
    assert(isFontOk());

    glyph_loaded_ = false;

    face_->loadChar(charcode, makeLoadFlags());
    face_->renderChar(static_cast<FT_Render_Mode>(makeRenderFlags()));

    FT_Bitmap bmp = (*face_)->glyph->bitmap;
    glyphData_.resize(bmp.width * bmp.rows);
    glyph_.assign(bmp.width, bmp.rows, &glyphData_[0]);

    unsigned char* pbmp = bmp.buffer;
    if (bmp.pixel_mode == FT_PIXEL_MODE_MONO) {
        for (int y = 0; y < bmp.rows; ++y) {
            for (int x = 0; x < bmp.width; ++x) {
                int xbyte = x >> 3;
                int xbit = x & 7;
                int pix = (pbmp[xbyte] >> (7 - xbit)) & 1;
                glyph_(x, y) = pix * 255;
            }
            pbmp += bmp.pitch;
        }
    } else if (bmp.pixel_mode == FT_PIXEL_MODE_GRAY) {
        int grays_max = bmp.num_grays - 1;
        for (int y = 0; y < bmp.rows; ++y) {
            for (int x = 0; x < bmp.width; ++x) {
                glyph_(x, y) = (pbmp[x] * 255) / grays_max;
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

bool FT2FontLoader::isGlyphOk() const
{
    return glyph_loaded_;
}

int FT2FontLoader::glyphLeft() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap_left;
}

int FT2FontLoader::glyphTop() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap_top;
}

unsigned FT2FontLoader::glyphWidth() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap.width;
}

unsigned FT2FontLoader::glyphHeight() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return (*face_)->glyph->bitmap.rows;
}

Surface8c FT2FontLoader::glyph() const
{
    assert(isFontOk());
    assert(isGlyphOk());
    return glyph_;
}

int FT2FontLoader::makeLoadFlags() const
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

int FT2FontLoader::makeRenderFlags() const
{
    switch (mode_) {
    case RenderGrayscale: return FT_RENDER_MODE_NORMAL;
    case RenderMonochrome: return FT_RENDER_MODE_MONO;
    }
    return 0;
}

} } // namespace KG::Ascii

