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

#ifndef KGASCII_FT2FONTLOADER_HPP
#define KGASCII_FT2FONTLOADER_HPP

#include <set>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/gil/gil_all.hpp>
#include <kgutil/image_io.hpp>
#include <kgascii/symbol.hpp>

namespace KG { namespace Ascii {

class ImageDirectoryFontLoader
{
public:
    typedef std::set<Symbol> SymbolCollectionT;

public:
    explicit ImageDirectoryFontLoader(const boost::filesystem::path& imgdir_path)
    {
        size_t img_cnt = 0;
        unsigned font_width = 0, font_height = 0;

        for (boost::filesystem::directory_iterator
             dir_it(imgdir_path), dir_end; dir_it != dir_end; ++dir_it) 
        {
            boost::gil::gray8_image_t glyph_image;
            if (!KG::Util::loadAndConvertImage(dir_it->path().string(), glyph_image))
                BOOST_THROW_EXCEPTION(std::runtime_error("loadAndConvertImage"));

            unsigned image_w = glyph_image.width();
            unsigned image_h = glyph_image.height();
            if (img_cnt == 0) {
                font_width = image_w;
                font_height = image_h;
            } else {
                if (image_w != font_width)
                    BOOST_THROW_EXCEPTION(std::runtime_error("width != font_width"));
                if (image_h != font_height)
                    BOOST_THROW_EXCEPTION(std::runtime_error("height != font_height"));
            }

            glyphs_.push_back(glyph_image);
            charcodes_.push_back(Symbol(32 + img_cnt));

            img_cnt++;
        }

        baseName_ = imgdir_path.stem().string();
        glyphWidth_ = font_width;
        glyphHeight_ = font_height;
    }

public:
    std::string familyName() const
    {
        return baseName_;
    }

    std::string styleName() const
    {
        return "plain";
    }

    unsigned pixelSize() const
    {
        return glyphWidth_;
    }

    unsigned glyphWidth() const
    {
        return glyphWidth_;
    }

    unsigned glyphHeight() const
    {
        return glyphHeight_;
    }

    SymbolCollectionT symbols() const
    {
        SymbolCollectionT result(charcodes_.begin(), charcodes_.end());
        return result;
    }

    bool loadGlyph(Symbol charcode, const boost::gil::gray8_view_t& glyph_surf) const
    {
        std::vector<Symbol>::const_iterator it = std::find(
                charcodes_.begin(), charcodes_.end(), charcode);
        if (it == charcodes_.end())
            return false;

        size_t index = std::distance(charcodes_.begin(), it);
        copy_pixels(const_view(glyphs_[index]), glyph_surf);

        return true;
    }

private:
    std::string baseName_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    std::vector<boost::gil::gray8_image_t> glyphs_;
    std::vector<Symbol> charcodes_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FT2FONTLOADER_HPP

