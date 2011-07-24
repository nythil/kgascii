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
#include <kgascii/symbol.hpp>
#include <kgascii/font.hpp>
#include <kgascii/surface.hpp>
#include <kgascii/surface_container.hpp>
#include <kgascii/surface_algorithm.hpp>
//#include <kgascii/font_image_loader.hpp>
//#include <boost/multi_index_container.hpp>
//#include <boost/multi_index/random_access_index.hpp>
//#include <boost/multi_index/ordered_index.hpp>
//#include <boost/multi_index/member.hpp>
//#include <boost/serialization/access.hpp>
//#include <boost/serialization/nvp.hpp>
//#include <boost/iterator/transform_iterator.hpp>
//#include <boost/bind.hpp>
//#include <fstream>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>

namespace KG { namespace Ascii {

template<typename TPixel>
class FontImage: boost::noncopyable
{
public:
    typedef typename boost::remove_cv<TPixel>::type PixelT;
    typedef Surface<PixelT> SurfaceT;
    typedef Surface<const PixelT> ConstSurfaceT;
    typedef SurfaceContainer<PixelT> SurfaceContainerT;

public:
    explicit FontImage(const Font* f)
        :font_(f)
        ,familyName_(font_->familyName())
        ,styleName_(font_->styleName())
        ,pixelSize_(font_->pixelSize())
        ,glyphWidth_(font_->glyphWidth())
        ,glyphHeight_(font_->glyphHeight())
        ,data_(glyphWidth_, glyphHeight_ * font_->glyphCount())
    {
        glyphs_.reserve(font_->glyphCount());
        for (size_t i = 0; i < font_->glyphCount(); ++i) {
            SurfaceT glyph_surface = data_.surface().window(0, glyphHeight_ * i, glyphWidth_, glyphHeight_);
            GlyphRecord gr = { font_->getSymbol(i), glyph_surface };
            copyPixels(font_->getGlyph(i), glyph_surface);
            glyphs_.push_back(gr);
        }
    }

public:
    const Font* font() const
    {
        return font_;
    }

    const std::string& familyName() const
    {
        return familyName_;
    }

    const std::string& styleName() const
    {
        return styleName_;
    }

    unsigned pixelSize() const
    {
        return pixelSize_;
    }

    unsigned glyphWidth() const
    {
        return glyphWidth_;
    }

    unsigned glyphHeight() const
    {
        return glyphHeight_;
    }

    unsigned glyphSize() const
    {
        return glyphWidth_ * glyphHeight_;
    }

    size_t glyphCount() const
    {
        return glyphs_.size();
    }

    Symbol getSymbol(size_t i) const
    {
        return glyphs_.at(i).sym;
    }

    const ConstSurfaceT& getGlyph(size_t i) const
    {
        return glyphs_.at(i).surf;
    }

//    std::vector<Symbol> charcodes() const
//    {
//        using namespace boost;
//        const GlyphContainer::nth_index<0>::type& glyph_index = glyphs_.get<0>();
//        Symbol GlyphData::*get_sym = &GlyphData::sym;
//        std::vector<Symbol> result(
//                make_transform_iterator(glyph_index.begin(), bind(get_sym, _1)),
//                make_transform_iterator(glyph_index.end(), bind(get_sym, _1))
//                );
//        return result;
//    }
//
//    std::vector<Surface8c> glyphs() const
//    {
//        using namespace boost;
//        const GlyphContainer::nth_index<0>::type& glyph_index = glyphs_.get<0>();
//        SurfaceContainer8 GlyphData::*get_data = &GlyphData::data;
//        Surface8c (SurfaceContainer8::*get_surface)() const = &SurfaceContainer8::surface;
//        std::vector<Surface8c> result(
//                make_transform_iterator(glyph_index.begin(), bind(get_surface, bind(get_data, _1))),
//                make_transform_iterator(glyph_index.end(), bind(get_surface, bind(get_data, _1)))
//                );
//        return result;
//    }
//
//    Surface8c glyphByIndex(size_t i) const
//    {
//        return glyphs_.get<0>().at(i).data.surface();
//    }
//
//    Surface8c glyphByCharcode(Symbol c) const
//    {
//        typedef GlyphContainer::nth_index<1>::type SymbolIndex;
//        const SymbolIndex& sym_index = glyphs_.get<1>();
//        SymbolIndex::const_iterator it = sym_index.find(c);
//        if (it == sym_index.end())
//            BOOST_THROW_EXCEPTION(std::out_of_range("Invalid symbol"));
//        return it->data.surface();
//    }

//    bool save(const std::string& file_path) const
//    {
//        std::ofstream ofs(file_path.c_str(), std::ios_base::out | std::ios_base::trunc);
//        if (!ofs.good())
//            BOOST_THROW_EXCEPTION(std::runtime_error("ofstream"));
//
//        boost::archive::text_oarchive oa(ofs);
//        oa << boost::serialization::make_nvp("font-image", *this);
//
//        return true;
//    }
//
//    bool load(const std::string& file_path)
//    {
//        std::ifstream ifs(file_path.c_str(), std::ios_base::in);
//        if (!ifs.good())
//            BOOST_THROW_EXCEPTION(std::runtime_error("ifstream"));
//
//        boost::archive::text_iarchive ia(ifs);
//        ia >> boost::serialization::make_nvp("font-image", *this);
//
//        return true;
//    }
//
//    bool load(FontImageLoader& loader, Symbol ci_min, Symbol ci_max)
//    {
//        familyName_ = loader.familyName();
//        styleName_ = loader.styleName();
//        pixelSize_ = loader.pixelSize();
//
//        glyphWidth_ = loader.glyphWidth();
//        glyphHeight_ = loader.glyphHeight();
//
//        glyphs_.clear();
//        for (Symbol sym = ci_min; sym <= ci_max; ++sym) {
//            if (!loader.loadGlyph(sym))
//                continue;
//            GlyphData gd = GlyphData();
//            gd.sym = sym;
//            gd.data.resize(glyphWidth_, glyphHeight_);
//            copyPixels(loader.glyph(), gd.data.surface());
//            glyphs_.push_back(gd);
//        }
//
//        return true;
//    }

private:
//    friend class boost::serialization::access;
//
//    template<typename Archive>
//    void serialize(Archive& ar, const unsigned int version)
//    {
//    	(void)version;
//    	using namespace boost::serialization;
//        ar & make_nvp("family-name", familyName_);
//        ar & make_nvp("style-name", styleName_);
//        ar & make_nvp("pixel-size", pixelSize_);
//        ar & make_nvp("glyph-width", glyphWidth_);
//        ar & make_nvp("glyph-height", glyphHeight_);
//        ar & make_nvp("glyphs", glyphs_);
//    }

private:
    struct GlyphRecord
    {
        Symbol sym;
        ConstSurfaceT surf;
    };

private:
    const Font* font_;
    std::string familyName_;
    std::string styleName_;
    unsigned pixelSize_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    std::vector<GlyphRecord> glyphs_;
    SurfaceContainerT data_;

//    struct GlyphData
//    {
//        Symbol sym;
//        SurfaceContainer8 data;
//    };
//    template<typename Archive>
//    friend void serialize(Archive& ar, FontImage::GlyphData& gd, const unsigned int version);
//    typedef boost::multi_index_container<
//            GlyphData,
//            boost::multi_index::indexed_by<
//                    boost::multi_index::random_access<>,
//                    boost::multi_index::ordered_unique<
//                            boost::multi_index::member<GlyphData, Symbol, &GlyphData::sym>
//                            >
//                    >
//            > GlyphContainer;
//    GlyphContainer glyphs_;
};

//template<typename Archive>
//inline void serialize(Archive& ar, FontImage::GlyphData& gd, const unsigned int version)
//{
//	(void)version;
//    using namespace boost::serialization;
//    ar & make_nvp("sym", gd.sym);
//    ar & make_nvp("data", gd.data);
//}

} } // namespace KG::Ascii

#endif // KGASCII_FONTIMAGE_HPP
