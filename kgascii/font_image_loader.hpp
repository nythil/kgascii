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

#ifndef KGASCII_FONT_IMAGE_LOADER_HPP
#define KGASCII_FONT_IMAGE_LOADER_HPP

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

class KGASCII_API FontImageLoader: boost::noncopyable
{
public:
    FontImageLoader();

    virtual ~FontImageLoader();

public:
    virtual std::string familyName() const = 0;

    virtual std::string styleName() const = 0;

    virtual unsigned pixelSize() const = 0;

    virtual unsigned glyphWidth() const = 0;

    virtual unsigned glyphHeight() const = 0;

    virtual std::vector<unsigned> charcodes() const = 0;

    virtual bool loadGlyph(unsigned charcode) = 0;

    virtual Surface8c glyph() const = 0;
};

} } // namespace KG::Ascii

#endif // KGASCII_FONT_IMAGE_LOADER_HPP

