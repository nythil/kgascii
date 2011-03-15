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

#ifndef KGASCII_FONTLOADER_HPP
#define KGASCII_FONTLOADER_HPP

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include "kgascii_api.hpp"

namespace FT2pp {
class Library;
class Face;
}

namespace kgAscii {

class KGASCII_API FontLoader: boost::noncopyable
{
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
    
public:
    FontLoader();

public:
    bool loadFont(const std::string& file_path, int pixel_size);

    bool isFontOk() const;

    std::string familyName() const;

    std::string styleName() const;

    int pixelSize() const;

    int ascender() const;

    int descender() const;

    int maxAdvance() const;

    bool fixedWidth() const;

    std::vector<int> charcodes() const;

    Hinting hinting() const;

    void setHinting(Hinting val);

    AutoHinter autohinter() const;

    void setAutohinter(AutoHinter val);

    RenderMode renderMode() const;

    void setRenderMode(RenderMode val);

    bool loadGlyph(int charcode);

    bool isGlyphOk() const;

    int glyphLeft() const;
    
    int glyphTop() const;

    int glyphWidth() const;

    int glyphHeight() const;

    boost::gil::gray8c_view_t glyph() const;

private:
    int makeLoadFlags() const;

    int makeRenderFlags() const;

private:
    boost::shared_ptr<FT2pp::Library> library_;
    boost::shared_ptr<FT2pp::Face> face_;
    bool glyph_loaded_;
    boost::gil::gray8_image_t glyph_;
    Hinting hinting_;
    AutoHinter autohint_;
    RenderMode mode_;
};

}

#endif // KGASCII_FONTLOADER_HPP

