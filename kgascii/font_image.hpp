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
#include <boost/shared_ptr.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/noncopyable.hpp>
#include <kgascii/symbol.hpp>

namespace KG { namespace Ascii {

template<
    class TFont,
    class TImage=typename TFont::ImageT
    >
class FontImage: boost::noncopyable
{
public:
    typedef TFont FontT;
    typedef TImage ImageT;
    typedef typename ImageT::value_type PixelT;
    typedef typename ImageT::view_t ViewT;
    typedef typename ImageT::const_view_t ConstViewT;

public:
    explicit FontImage(boost::shared_ptr<const FontT> f)
        :font_(f)
    {
        familyName_ = font_->familyName();
        styleName_ = font_->styleName();
        pixelSize_ = font_->pixelSize();
        glyphWidth_ = font_->glyphWidth();
        glyphHeight_ = font_->glyphHeight();
        size_t glyph_count = font_->glyphCount();
        data_.recreate(glyphWidth_, glyphHeight_ * glyph_count);
        glyphs_.reserve(glyph_count);
        for (size_t i = 0; i < glyph_count; ++i) {
            ViewT glyph_surface = subimage_view(view(data_), 0, glyphHeight_ * i, glyphWidth_, glyphHeight_);
            GlyphRecord gr = { font_->getSymbol(i), glyph_surface };
            copy_and_convert_pixels(font_->getGlyph(i), glyph_surface);
            glyphs_.push_back(gr);
        }
    }

public:
    boost::shared_ptr<const FontT> font() const
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

    const ConstViewT& getGlyph(size_t i) const
    {
        return glyphs_.at(i).surf;
    }

private:
    struct GlyphRecord
    {
        Symbol sym;
        ConstViewT surf;
    };

private:
    boost::shared_ptr<const FontT> font_;
    std::string familyName_;
    std::string styleName_;
    unsigned pixelSize_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    std::vector<GlyphRecord> glyphs_;
    ImageT data_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FONTIMAGE_HPP
