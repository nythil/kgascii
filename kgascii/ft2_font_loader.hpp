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

#ifndef KGASCII_FT2_FONTLOADER_HPP
#define KGASCII_FT2_FONTLOADER_HPP

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

namespace FT2pp {
class Library;
class Face;
} // namespace FT2pp

class KGASCII_API FT2FontLoader: boost::noncopyable
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
    FT2FontLoader();

public:
    bool loadFont(const std::string& file_path, unsigned pixel_size);

    bool isFontOk() const;

    std::string familyName() const;

    std::string styleName() const;

    unsigned pixelSize() const;

    unsigned ascender() const;

    unsigned descender() const;

    unsigned maxAdvance() const;

    bool fixedWidth() const;

    std::vector<unsigned> charcodes() const;

    Hinting hinting() const;

    void setHinting(Hinting val);

    AutoHinter autohinter() const;

    void setAutohinter(AutoHinter val);

    RenderMode renderMode() const;

    void setRenderMode(RenderMode val);

    bool loadGlyph(unsigned charcode);

    bool isGlyphOk() const;

    int glyphLeft() const;
    
    int glyphTop() const;

    unsigned glyphWidth() const;

    unsigned glyphHeight() const;

    Surface8c glyph() const;

private:
    int makeLoadFlags() const;

    int makeRenderFlags() const;

private:
    boost::shared_ptr<FT2pp::Library> library_;
    boost::shared_ptr<FT2pp::Face> face_;
    bool glyph_loaded_;
    Surface8 glyph_;
    std::vector<Surface8::value_type> glyphData_;
    Hinting hinting_;
    AutoHinter autohint_;
    RenderMode mode_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FT2_FONTLOADER_HPP

