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

#ifndef KGASCII_FT2_FONT_IMAGE_LOADER_HPP
#define KGASCII_FT2_FONT_IMAGE_LOADER_HPP

#include <kgascii/kgascii_api.hpp>
#include <kgascii/font_image_loader.hpp>
#include <kgascii/surface_container.hpp>

namespace KG { namespace Ascii {

class FT2FontLoader;

class KGASCII_API FT2FontImageLoader: public FontImageLoader
{
public:
    explicit FT2FontImageLoader(FT2FontLoader& ldr);

public:
    std::string familyName() const;

    std::string styleName() const;

    unsigned pixelSize() const;

    unsigned glyphWidth() const;

    unsigned glyphHeight() const;

    std::vector<int> charcodes() const;

    bool loadGlyph(int charcode);

    Surface8c glyph() const;

private:
    FT2FontLoader& loader_;
    Surface8 glyph_;
    SurfaceContainer8 glyphData_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FT2_FONT_IMAGE_LOADER_HPP

