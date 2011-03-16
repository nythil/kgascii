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

#include "fontimage.hpp"
#include "fontloader.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "hexstring.hpp"

namespace KG { namespace Ascii {

FontImage::FontImage()
{
}

const std::string& FontImage::familyName() const
{
    return familyName_;
}

const std::string& FontImage::styleName() const
{
    return styleName_;
}

int FontImage::pixelSize() const
{
    return pixelSize_;
}

int FontImage::glyphWidth() const
{
    return glyphWidth_;
}

int FontImage::glyphHeight() const
{
    return glyphHeight_;
}

std::vector<int> FontImage::charcodes() const
{
    std::vector<int> chcodes;
    GlyphMap::const_iterator chmap_it;
    for (chmap_it = charmap_.begin(); chmap_it != charmap_.end(); ++chmap_it) {
        chcodes.push_back(chmap_it->first);
    }
    return chcodes;
}

boost::gil::gray8c_view_t FontImage::getGlyph(int charcode) const
{
    GlyphMap::const_iterator it = charmap_.find(charcode);
    assert(it != charmap_.end());
    size_t row_offset = it->second * glyphs_.cols();
    const unsigned char* glyph_data = glyphs_.data() + row_offset;
    return boost::gil::interleaved_view(glyphWidth_, glyphHeight_, 
        reinterpret_cast<const boost::gil::gray8c_pixel_t*>(glyph_data),
        glyphWidth_);
}

bool FontImage::save(const std::string& file_path) const
{
    boost::property_tree::ptree pt;
    pt.put("font.family_name", familyName_);
    pt.put("font.style_name", styleName_);
    pt.put("font.pixel_size", pixelSize_);
    pt.put("font.glyph_width", glyphWidth_);
    pt.put("font.glyph_height", glyphHeight_);

    int glyphSize = glyphWidth_ * glyphHeight_;

    GlyphMap::const_iterator it_end = charmap_.end();
    for (GlyphMap::const_iterator it = charmap_.begin(); it != it_end; ++it) {
        size_t row_offset = it->second * glyphSize;
        const unsigned char* glyph_bytes_begin = glyphs_.data() + row_offset;
        const unsigned char* glyph_bytes_end = glyph_bytes_begin + glyphSize;
        boost::property_tree::ptree pt_glyph;
        pt_glyph.put("charcode", it->first);
        pt_glyph.put("data", hexlify(glyph_bytes_begin, glyph_bytes_end));
        pt.add_child("font.glyphs.glyph", pt_glyph);
    }

    std::ofstream fstr(file_path.c_str(), std::ios_base::out | std::ios_base::trunc);
    boost::property_tree::xml_parser::write_xml(fstr, pt, 
        boost::property_tree::xml_writer_make_settings(' ', 2));

    return true;
}

bool FontImage::load(const std::string& file_path)
{
    boost::property_tree::ptree pt;
    boost::property_tree::xml_parser::read_xml(file_path, pt);
    
    familyName_ = pt.get<std::string>("font.family_name");
    styleName_ = pt.get<std::string>("font.style_name");
    pixelSize_ = pt.get<int>("font.pixel_size");
    glyphWidth_ = pt.get<int>("font.glyph_width");
    glyphHeight_ = pt.get<int>("font.glyph_height");

    int glyphSize = glyphWidth_ * glyphHeight_;

    const boost::property_tree::ptree& pt_glyphs = pt.get_child("font.glyphs");
    size_t ci_count = pt_glyphs.count("glyph");
    size_t ci = 0;

    charmap_.clear();
    glyphs_.resize(ci_count, glyphSize);

    boost::property_tree::ptree::const_iterator it_end = pt_glyphs.end();
    for (boost::property_tree::ptree::const_iterator it = pt_glyphs.begin(); it != it_end; ++it) {
        int charcode = it->second.get<int>("charcode");
        std::string data = it->second.get<std::string>("data");

        size_t row_offset = ci * glyphSize;
        unsigned char* glyph_bytes_begin = glyphs_.data() + row_offset;
        unhexlify(data, glyph_bytes_begin);

        charmap_[charcode] = ci++;
    }

    return true;
}

bool FontImage::load(FontLoader& loader, int ci_min, int ci_max)
{
    if (!loader.isFontOk())
        return false;
    if (!loader.fixedWidth())
        return false;

    familyName_ = loader.familyName();
    styleName_ = loader.styleName();
    pixelSize_ = loader.pixelSize();

    glyphWidth_ = loader.maxAdvance();
    glyphHeight_ = loader.ascender() - loader.descender();
    int glyphSize = glyphWidth_ * glyphHeight_;

    int ci_count = ci_max - ci_min + 1;

    charmap_.clear();
    glyphs_.resize(ci_count, glyphSize);

    for (int ci = 0; ci < ci_count; ++ci) {
        if (!loader.loadGlyph(ci + ci_min))
            continue;

        size_t row_offset = ci * glyphs_.cols();
        unsigned char* glyph_data = glyphs_.data() + row_offset;
        boost::gil::gray8_view_t glyph_image_view = 
            boost::gil::interleaved_view(glyphWidth_, glyphHeight_, 
                reinterpret_cast<boost::gil::gray8_pixel_t*>(glyph_data),
                glyphWidth_);
        boost::gil::fill_pixels(glyph_image_view, 0);

        int bmp_off_x = std::max(-loader.glyphLeft(), 0);
        int bmp_off_y = std::max(loader.glyphTop() - loader.ascender(), 0);
        int bmp_width = std::min(loader.glyphWidth(), glyphWidth_ - bmp_off_x);
        int bmp_height = std::min(loader.glyphHeight(), glyphHeight_ - bmp_off_y);

        int img_off_x = std::max(loader.glyphLeft(), 0);
        int img_off_y = std::max(loader.ascender() - loader.glyphTop(), 0);
        int common_width = std::min(bmp_width, glyphWidth_ - img_off_x);
        int common_height = std::min(bmp_height, glyphHeight_ - img_off_y);

        //std::cout << "ci: " << ci << " ";
        //std::cout << bmp_off_x << "," << bmp_off_y << " - ";
        //std::cout << common_width << "x" << common_height << " -- ";
        //std::cout << loader.glyphLeft() << "," << loader.glyphTop() << " - ";
        //std::cout << loader.glyphWidth() << "x" << loader.glyphHeight() << "\n";

        assert(img_off_x + common_width <= glyphWidth_);
        assert(img_off_y + common_height <= glyphHeight_);

        boost::gil::gray8_view_t glyph_subimage_view = boost::gil::subimage_view(
            glyph_image_view, img_off_x, img_off_y, common_width, common_height);
        boost::gil::gray8c_view_t bmp_subimage_view = boost::gil::subimage_view(
            loader.glyph(), bmp_off_x, bmp_off_y, common_width, common_height);
        boost::gil::copy_pixels(bmp_subimage_view, glyph_subimage_view);

        charmap_[ci + ci_min] = ci;
    }

    return true;
}

} } // namespace KG::Ascii

