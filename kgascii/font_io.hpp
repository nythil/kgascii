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

#ifndef KGASCII_FONT_IO_HPP
#define KGASCII_FONT_IO_HPP

#include <fstream>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <kgascii/font.hpp>
#include <kgascii/surface.hpp>
#include <kgascii/surface_algorithm.hpp>


namespace KG { namespace Ascii {

template<typename TArchive>
void Font::serialize(TArchive& ar, const unsigned int version)
{
    using namespace boost::serialization;
    (void)version;
    ar & make_nvp("family-name", familyName_);
    ar & make_nvp("style-name", styleName_);
    ar & make_nvp("pixel-size", pixelSize_);
    ar & make_nvp("glyph-width", glyphWidth_);
    ar & make_nvp("glyph-height", glyphHeight_);
    ar & make_nvp("glyphs", glyphs_);
}

template<typename TArchive>
void Font::GlyphRecord::load(TArchive& ar, const unsigned int version)
{
    using namespace boost::serialization;
    (void)version;
    ar >> make_nvp("sym", sym);
    ar >> make_nvp("data", data);
    surf = data.surface();
}

template<typename TArchive>
void Font::GlyphRecord::save(TArchive& ar, const unsigned int version) const
{
    using namespace boost::serialization;
    (void)version;
    ar << make_nvp("sym", sym);
    ar << make_nvp("data", data);
}

bool Font::save(const std::string& file_path) const
{
    std::ofstream ofs(file_path.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!ofs.good())
        BOOST_THROW_EXCEPTION(std::runtime_error("ofstream"));

    boost::archive::text_oarchive oa(ofs);
    oa << boost::serialization::make_nvp("font-image", *this);

    return true;
}

bool Font::load(const std::string& file_path)
{
    std::ifstream ifs(file_path.c_str(), std::ios_base::in);
    if (!ifs.good())
        BOOST_THROW_EXCEPTION(std::runtime_error("ifstream"));

    boost::archive::text_iarchive ia(ifs);
    ia >> boost::serialization::make_nvp("font-image", *this);

    return true;
}

template<typename TLoader>
bool Font::load(TLoader& loader, Symbol ci_min, Symbol ci_max)
{
    setFamilyName(loader.familyName());
    setStyleName(loader.styleName());
    setPixelSize(loader.pixelSize());
    setGlyphSize(loader.glyphWidth(), loader.glyphHeight());

    clear();
    for (Symbol sym = ci_min; sym <= ci_max; ++sym) {
        if (!loader.loadGlyph(sym))
            continue;
        copyPixels(loader.glyph(), addGlyph(sym));
    }

    return true;
}

} } // namespace KG::Ascii

#endif // KGASCII_FONT_IO_HPP
