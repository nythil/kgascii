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
#include <vector>
#include <boost/noncopyable.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

class FontLoader;

class KGASCII_API FontImage: boost::noncopyable
{
public:
    FontImage();

public:
    const std::string& familyName() const;

    const std::string& styleName() const;

    unsigned pixelSize() const;

    unsigned glyphWidth() const;

    unsigned glyphHeight() const;

    size_t glyphCount() const;

    std::vector<unsigned> charcodes() const;

    std::vector<Surface8c> glyphs() const;

    Surface8c glyphByIndex(size_t i) const;

    Surface8c glyphByCharcode(unsigned c) const;

    bool save(const std::string& file_path) const;

    bool load(const std::string& file_path);

    bool load(FontLoader& loader, unsigned ci_min, unsigned ci_max);

private:
    void prepareStorage(size_t count, unsigned w, unsigned h);

private:
    std::string familyName_;
    std::string styleName_;
    unsigned pixelSize_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    std::vector<unsigned> charcodes_;
    std::vector<Surface8> glyphs_;
    std::vector<Surface8::value_type> glyphStorage_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FONTIMAGE_HPP
