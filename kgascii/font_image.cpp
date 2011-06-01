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

#include <kgascii/font_image.hpp>
#include <kgascii/font_image_loader.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <kgascii/hexstring.hpp>
#include <kgascii/surface_algorithm.hpp>

namespace KG { namespace Ascii {

FontImage::FontImage()
    :charCount_(0)
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

unsigned FontImage::pixelSize() const
{
    return pixelSize_;
}

unsigned FontImage::glyphWidth() const
{
    return glyphWidth_;
}

unsigned FontImage::glyphHeight() const
{
    return glyphHeight_;
}

unsigned FontImage::glyphSize() const
{
    return glyphWidth_ * glyphHeight_;
}

size_t FontImage::glyphCount() const
{
    return charCount_;
}

std::vector<unsigned> FontImage::charcodes() const
{
    return charcodes_;
}

std::vector<Surface8c> FontImage::glyphs() const
{
    return std::vector<Surface8c>(glyphs_.begin(), glyphs_.end());
}

Surface8c FontImage::glyphByIndex(size_t i) const
{
    assert(i < charCount_);
    return glyphs_[i];
}

Surface8c FontImage::glyphByCharcode(unsigned c) const
{
    std::vector<unsigned>::const_iterator it = std::find(charcodes_.begin(), 
            charcodes_.end(), c);
    assert(it != charcodes_.end());
    return glyphByIndex(std::distance(charcodes_.begin(), it));
}

bool FontImage::save(const std::string& file_path) const
{
    boost::property_tree::ptree pt;
    pt.put("font.family_name", familyName_);
    pt.put("font.style_name", styleName_);
    pt.put("font.pixel_size", pixelSize_);
    pt.put("font.glyph_width", glyphWidth_);
    pt.put("font.glyph_height", glyphHeight_);

    for (size_t ci = 0; ci < charCount_; ++ci) {
        const Surface8& glyph_surf = glyphs_[ci];
        boost::property_tree::ptree pt_glyph;
        pt_glyph.put("charcode", charcodes_[ci]);
        pt_glyph.put("data", hexlify(glyph_surf.data(), glyph_surf.dataEnd()));
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
    pixelSize_ = pt.get<unsigned>("font.pixel_size");
    glyphWidth_ = pt.get<unsigned>("font.glyph_width");
    glyphHeight_ = pt.get<unsigned>("font.glyph_height");

    const boost::property_tree::ptree& pt_glyphs = pt.get_child("font.glyphs");
    size_t ci_count = pt_glyphs.count("glyph");
    size_t ci = 0;

    prepareStorage(ci_count, glyphWidth_, glyphHeight_);

    charCount_ = 0;

    boost::property_tree::ptree::const_iterator it_end = pt_glyphs.end();
    boost::property_tree::ptree::const_iterator it;
    for (it = pt_glyphs.begin(); it != it_end; ++it, ++ci) {
        charcodes_[ci] = it->second.get<unsigned>("charcode");

        std::string data = it->second.get<std::string>("data");
        unhexlify(data, glyphs_[ci].data());

        charCount_++;
    }

    return true;
}

bool FontImage::load(FontImageLoader& loader, unsigned ci_min, unsigned ci_max)
{
    familyName_ = loader.familyName();
    styleName_ = loader.styleName();
    pixelSize_ = loader.pixelSize();

    glyphWidth_ = loader.glyphWidth();
    glyphHeight_ = loader.glyphHeight();

    unsigned ci_count = ci_max - ci_min + 1;

    prepareStorage(ci_count, glyphWidth_, glyphHeight_);

    charCount_ = 0;

    for (unsigned ci = 0; ci < ci_count; ++ci) {
        if (!loader.loadGlyph(ci + ci_min))
            continue;

        charcodes_[ci] = ci + ci_min;
        copyPixels(loader.glyph(), glyphs_[ci]);

        charCount_++;
    }

    return true;
}

void FontImage::prepareStorage(size_t count, unsigned w, unsigned h)
{
    unsigned glyphSize = w * h;
    glyphStorage_.resize(count * glyphSize);
    charcodes_.resize(count);
    glyphs_.resize(count);

    for (size_t ci = 0; ci < count; ++ci) {
        unsigned char* glyph_data = &glyphStorage_[ci * glyphSize];
        glyphs_[ci].assign(w, h, glyph_data, w);
    }
}

} } // namespace KG::Ascii

