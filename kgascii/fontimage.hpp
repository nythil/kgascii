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

#ifndef KGASCII_FONTIMAGE_HPP
#define KGASCII_FONTIMAGE_HPP

#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <Eigen/Dense>
#include "kgascii_api.hpp"

namespace kgAscii {

class FontLoader;

class KGASCII_API FontImage: boost::noncopyable
{
    typedef Eigen::Array<unsigned char, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> GlyphStorage;
    typedef std::map<int, int> GlyphMap;

public:
    FontImage();

public:
    const std::string& familyName() const;

    const std::string& styleName() const;

    int pixelSize() const;

    int glyphWidth() const;

    int glyphHeight() const;

    std::vector<int> charcodes() const;

    boost::gil::gray8c_view_t getGlyph(int charcode) const;

    bool save(const std::string& file_path) const;

    bool load(const std::string& file_path);

    bool load(FontLoader& loader, int ci_min, int ci_max);

private:
    std::string familyName_;
    std::string styleName_;
    int pixelSize_;
    int glyphWidth_;
    int glyphHeight_;
    GlyphStorage glyphs_;
    GlyphMap charmap_;
};

}

#endif // KGASCII_FONTIMAGE_HPP
