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

#ifndef KGASCII_FONT_HPP
#define KGASCII_FONT_HPP

#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits.hpp>
#include <kgascii/symbol.hpp>
#include <kgascii/surface.hpp>
#include <kgascii/surface_container.hpp>

namespace KG { namespace Ascii {

class FontImageLoader;

class Font: boost::noncopyable
{
public:
    Font()
    {
    }

public:
    const std::string& familyName() const
    {
        return familyName_;
    }

    void setFamilyName(const std::string& value)
    {
        familyName_ = value;
    }

    const std::string& styleName() const
    {
        return styleName_;
    }

    void setStyleName(const std::string& value)
    {
        styleName_ = value;
    }

    unsigned pixelSize() const
    {
        return pixelSize_;
    }

    void setPixelSize(unsigned value)
    {
        pixelSize_ = value;
    }

    unsigned glyphWidth() const
    {
        return glyphWidth_;
    }

    void setGlyphWidth(unsigned value)
    {
        setGlyphSize(value, glyphHeight_);
    }

    unsigned glyphHeight() const
    {
        return glyphHeight_;
    }

    void setGlyphHeight(unsigned value)
    {
        setGlyphSize(glyphWidth_, value);
    }

    void setGlyphSize(unsigned w, unsigned h)
    {
        if (glyphWidth_ != w || glyphHeight_ != h) {
            glyphWidth_ = w;
            glyphHeight_ = h;
            clear();
        }
    }

    size_t glyphCount() const
    {
        return glyphs_.size();
    }

    Symbol getSymbol(size_t i) const
    {
        return glyphs_.at(i).sym;
    }

    Surface8c getGlyph(size_t i) const
    {
        return glyphs_.at(i).surf;
    }

    Surface8 getGlyph(size_t i)
    {
        return glyphs_.at(i).surf;
    }

    bool contains(Symbol sym) const
    {
        return (findGlyphRecord(sym) != 0);
    }

    Surface8c getGlyph(Symbol sym) const
    {
        if (const GlyphRecord* gr = findGlyphRecord(sym))
            return gr->surf;
        BOOST_THROW_EXCEPTION(std::out_of_range("Invalid symbol"));
    }

    Surface8 getGlyph(Symbol sym)
    {
        if (const GlyphRecord* gr = findGlyphRecord(sym))
            return gr->surf;
        BOOST_THROW_EXCEPTION(std::out_of_range("Invalid symbol"));
    }

    void clear()
    {
        glyphs_.clear();
    }

    Surface8 addGlyph(Symbol sym)
    {
        GlyphRecord gr(sym, glyphWidth_, glyphHeight_);
        std::pair<GlyphContainer::iterator, bool> result = glyphs_.push_back(gr);
        if (!result.second)
            BOOST_THROW_EXCEPTION(std::runtime_error("Insertion error"));
        return result.first->surf;
    }

    bool save(const std::string& file_path) const;

    bool load(const std::string& file_path);

    bool load(FontImageLoader& loader, Symbol ci_min, Symbol ci_max);

private:
    friend class boost::serialization::access;

    struct GlyphRecord;

    const GlyphRecord* findGlyphRecord(Symbol sym) const
    {
        typedef GlyphContainer::nth_index<1>::type SymbolIndex;
        const SymbolIndex& sym_index = glyphs_.get<1>();
        SymbolIndex::const_iterator it = sym_index.find(sym);
        if (it != sym_index.end())
            return &(*it);
        return 0;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version);

private:
    struct GlyphRecord
    {
        Symbol sym;
        SurfaceContainer8 data;
        Surface8 surf;

        GlyphRecord()
        {
        }

        GlyphRecord(Symbol s, unsigned w, unsigned h)
            :sym(s)
            ,data(w, h)
            ,surf(data.surface())
        {
        }

        GlyphRecord(const GlyphRecord& t)
            :sym(t.sym)
            ,data(t.data)
            ,surf(data.surface())
        {
        }

        template<typename Archive>
        void load(Archive& ar, const unsigned int version);

        template<typename Archive>
        void save(Archive& ar, const unsigned int version) const;

        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };

    typedef boost::multi_index_container<
            GlyphRecord,
            boost::multi_index::indexed_by<
                    boost::multi_index::random_access<>,
                    boost::multi_index::ordered_unique<
                            boost::multi_index::member<GlyphRecord, Symbol, &GlyphRecord::sym>
                            >
                    >
            > GlyphContainer;

private:
    std::string familyName_;
    std::string styleName_;
    unsigned pixelSize_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    GlyphContainer glyphs_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FONT_HPP
