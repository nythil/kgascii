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

#ifndef KGASCII_INTERNAL_FT2FONTLOADER_HPP
#define KGASCII_INTERNAL_FT2FONTLOADER_HPP

#include <string>
#include <set>
#include <boost/gil/gil_all.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <kgascii/ft2pp/library.hpp>
#include <kgascii/ft2pp/face.hpp>

namespace KG { namespace Ascii { namespace Internal {

class FT2FontLoaderBase: boost::noncopyable
{
public:
    typedef boost::gil::gray8_view_t ViewT;
    typedef boost::gil::gray8c_view_t ConstViewT;
    typedef boost::gil::gray8_image_t ImageT;

public:
    enum Hinting
    {
        HintingNormal,
        HintingLight,
        HintingOff
    };
    enum AutoHinter
    {
        AutoHinterForce,
        AutoHinterOn,
        AutoHinterOff
    };
    enum RenderMode
    {
        RenderGrayscale,
        RenderMonochrome
    };    
};

class FT2FontLoader: public FT2FontLoaderBase
{
public:
    FT2FontLoader()
        :library_(new FT2pp::Library)
        ,glyph_loaded_(false)
        ,hinting_(HintingNormal)
        ,autohint_(AutoHinterOff)
        ,mode_(RenderGrayscale)
    {
    }

public:
    bool loadFont(const std::string& file_path, unsigned pixel_size)
    {
        int face_idx = 0;
        int num_faces = 1;
        while (face_idx < num_faces) {
            boost::shared_ptr<FT2pp::Face> ft_face_ptr(new FT2pp::Face(*library_, file_path, face_idx));
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

    bool isFontOk() const
    {
        return face_;
    }

    std::string familyName() const
    {
        assert(isFontOk());
        return (*face_)->family_name;
    }

    std::string styleName() const
    {
        assert(isFontOk());
        return (*face_)->style_name;
    }

    unsigned pixelSize() const
    {
        assert(isFontOk());
        return (*face_)->size->metrics.y_ppem;
    }

    unsigned ascender() const
    {
        assert(isFontOk());
        return (*face_)->size->metrics.ascender / 64;
    }

    unsigned descender() const
    {
        assert(isFontOk());
        return -(*face_)->size->metrics.descender / 64;
    }

    unsigned maxAdvance() const
    {
        assert(isFontOk());
        return (*face_)->size->metrics.max_advance / 64;
    }

    bool fixedWidth() const
    {
        assert(isFontOk());
        return (FT_IS_FIXED_WIDTH(face_->handle()) != 0);
    }

    Hinting hinting() const
    {
        return hinting_;
    }

    void setHinting(Hinting val)
    {
        hinting_ = val;
    }

    AutoHinter autohinter() const
    {
        return autohint_;
    }

    void setAutohinter(AutoHinter val)
    {
        autohint_ = val;
    }

    RenderMode renderMode() const
    {
        return mode_;
    }

    void setRenderMode(RenderMode val)
    {
        mode_ = val;
    }

    std::set<unsigned> charcodes()
    {
        assert(isFontOk());

        std::set<unsigned> result;
        boost::optional<unsigned> charcode = face_->getFirstChar();
        while (charcode) {
            result.insert(charcode.get());
            charcode = face_->getNextChar(charcode.get());
        }
        return result;
    }

    bool loadGlyph(unsigned charcode)
    {
        assert(isFontOk());

        glyph_loaded_ = false;

        face_->loadChar(charcode, makeLoadFlags());
        face_->renderChar(static_cast<FT_Render_Mode>(makeRenderFlags()));

        FT_Bitmap bmp = (*face_)->glyph->bitmap;
        glyphData_.recreate(bmp.width, bmp.rows);
        glyph_ = view(glyphData_);

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

    bool isGlyphOk() const
    {
        return glyph_loaded_;
    }

    int glyphLeft() const
    {
        assert(isFontOk());
        assert(isGlyphOk());
        return (*face_)->glyph->bitmap_left;
    }
    
    int glyphTop() const
    {
        assert(isFontOk());
        assert(isGlyphOk());
        return (*face_)->glyph->bitmap_top;
    }

    unsigned glyphWidth() const
    {
        assert(isFontOk());
        assert(isGlyphOk());
        return (*face_)->glyph->bitmap.width;
    }

    unsigned glyphHeight() const
    {
        assert(isFontOk());
        assert(isGlyphOk());
        return (*face_)->glyph->bitmap.rows;
    }

    ConstViewT glyph() const
    {
        assert(isFontOk());
        assert(isGlyphOk());
        return glyph_;
    }

private:
    int makeLoadFlags() const
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

    int makeRenderFlags() const
    {
        switch (mode_) {
        case RenderGrayscale: return FT_RENDER_MODE_NORMAL;
        case RenderMonochrome: return FT_RENDER_MODE_MONO;
        }
        return 0;
    }

private:
    boost::shared_ptr<FT2pp::Library> library_;
    boost::shared_ptr<FT2pp::Face> face_;
    bool glyph_loaded_;
    ViewT glyph_;
    ImageT glyphData_;
    Hinting hinting_;
    AutoHinter autohint_;
    RenderMode mode_;
};

} } } // namespace KG::Ascii::Internal

#endif // KGASCII_INTERNAL_FT2FONTLOADER_HPP
