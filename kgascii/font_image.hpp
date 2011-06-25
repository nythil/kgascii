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
#include <kgascii/surface_container.hpp>
#include <kgascii/symbol.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/serialization/nvp.hpp>


namespace KG { namespace Ascii {

class FontImageLoader;

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

    unsigned glyphSize() const;

    size_t glyphCount() const;

    std::vector<Symbol> charcodes() const;

    std::vector<Surface8c> glyphs() const;

    Surface8c glyphByIndex(size_t i) const;

    Surface8c glyphByCharcode(Symbol c) const;

    bool save(const std::string& file_path) const;

    bool load(const std::string& file_path);

    bool load(FontImageLoader& loader, Symbol ci_min, Symbol ci_max);

private:
    void prepareStorage(size_t count, unsigned w, unsigned h);

private:
    std::string familyName_;
    std::string styleName_;
    unsigned pixelSize_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    struct GlyphData
    {
        Symbol sym;
        SurfaceContainer8 data;
        template<typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(sym);
            ar & BOOST_SERIALIZATION_NVP(data);
        }
    };
    typedef boost::multi_index_container<
            GlyphData,
            boost::multi_index::indexed_by<
                    boost::multi_index::random_access<>,
                    boost::multi_index::ordered_unique<
                            boost::multi_index::member<GlyphData, Symbol, &GlyphData::sym>
                            >
                    >
            > GlyphContainer;
    GlyphContainer glyphs_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FONTIMAGE_HPP
