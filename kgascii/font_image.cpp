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
#include <boost/iterator/transform_iterator.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <kgascii/surface_algorithm.hpp>

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
    return glyphs_.size();
}

std::vector<Symbol> FontImage::charcodes() const
{
    const GlyphContainer::nth_index<0>::type& glyph_index = glyphs_.get<0>();
    std::vector<Symbol> result(
            boost::make_transform_iterator(glyph_index.begin(), boost::bind(&GlyphData::sym, _1)),
            boost::make_transform_iterator(glyph_index.end(), boost::bind(&GlyphData::sym, _1))
            );
    return result;
}

std::vector<Surface8c> FontImage::glyphs() const
{
    const GlyphContainer::nth_index<0>::type& glyph_index = glyphs_.get<0>();
    std::vector<Surface8c> result(
            boost::make_transform_iterator(glyph_index.begin(), boost::bind(static_cast<Surface8c (SurfaceContainer8::*)() const>(&SurfaceContainer8::surface), boost::bind(&GlyphData::data, _1))),
            boost::make_transform_iterator(glyph_index.end(), boost::bind(static_cast<Surface8c (SurfaceContainer8::*)() const>(&SurfaceContainer8::surface), boost::bind(&GlyphData::data, _1)))
            );
    return result;
}

Surface8c FontImage::glyphByIndex(size_t i) const
{
    return glyphs_.get<0>().at(i).data.surface();
}

Surface8c FontImage::glyphByCharcode(Symbol c) const
{
    typedef GlyphContainer::nth_index<1>::type SymbolIndex;
    const SymbolIndex& sym_index = glyphs_.get<1>();
    SymbolIndex::const_iterator it = sym_index.find(c);
    if (it == sym_index.end())
        BOOST_THROW_EXCEPTION(std::out_of_range("Invalid symbol"));
    return it->data.surface();
}

bool FontImage::save(const std::string& file_path) const
{
    std::ofstream ofs(file_path.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!ofs.good())
        BOOST_THROW_EXCEPTION(std::runtime_error("ofstream"));

    boost::archive::text_oarchive oa(ofs);

    oa << boost::serialization::make_nvp("family-name", familyName_);
    oa << boost::serialization::make_nvp("style-name", styleName_);
    oa << boost::serialization::make_nvp("pixel-size", pixelSize_);
    oa << boost::serialization::make_nvp("glyph-width", glyphWidth_);
    oa << boost::serialization::make_nvp("glyph-height", glyphHeight_);
    oa << boost::serialization::make_nvp("glyphs", glyphs_);

    return true;
}

bool FontImage::load(const std::string& file_path)
{
    std::ifstream ifs(file_path.c_str(), std::ios_base::in);
    if (!ifs.good())
        BOOST_THROW_EXCEPTION(std::runtime_error("ifstream"));

    boost::archive::text_iarchive ia(ifs);

    ia >> boost::serialization::make_nvp("family-name", familyName_);
    ia >> boost::serialization::make_nvp("style-name", styleName_);
    ia >> boost::serialization::make_nvp("pixel-size", pixelSize_);
    ia >> boost::serialization::make_nvp("glyph-width", glyphWidth_);
    ia >> boost::serialization::make_nvp("glyph-height", glyphHeight_);
    ia >> boost::serialization::make_nvp("glyphs", glyphs_);

    return true;
}

bool FontImage::load(FontImageLoader& loader, Symbol ci_min, Symbol ci_max)
{
    familyName_ = loader.familyName();
    styleName_ = loader.styleName();
    pixelSize_ = loader.pixelSize();

    glyphWidth_ = loader.glyphWidth();
    glyphHeight_ = loader.glyphHeight();

    glyphs_.clear();
    for (Symbol sym = ci_min; sym <= ci_max; ++sym) {
        if (!loader.loadGlyph(sym))
            continue;
        GlyphData gd = GlyphData();
        gd.sym = sym;
        gd.data.resize(glyphWidth_, glyphHeight_);
        copyPixels(loader.glyph(), gd.data.surface());
        glyphs_.push_back(gd);
    }

    return true;
}

} } // namespace KG::Ascii

