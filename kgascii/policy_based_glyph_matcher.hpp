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

#ifndef KGASCII_POLICYBASEDGLYPHMATCHER_HPP
#define KGASCII_POLICYBASEDGLYPHMATCHER_HPP

#include <limits>
#include <boost/gil/gil_all.hpp>
#include <kgascii/symbol.hpp>

namespace KG { namespace Ascii {

template<class TFontImage, class TDistance>
class PolicyBasedGlyphMatcherContext;
template<class TFontImage, class TDistance>
class PolicyBasedGlyphMatcher;

template<class TFontImage, class TDistance>
class PolicyBasedGlyphMatcher: boost::noncopyable
{
public:
    typedef PolicyBasedGlyphMatcherContext<TFontImage, TDistance> GlyphMatcherContextT;
    typedef TFontImage FontImageT;
    typedef typename FontImageT::PixelT PixelT;
    typedef typename FontImageT::ImageT ImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

public:
    explicit PolicyBasedGlyphMatcher(const GlyphMatcherContextT* c)
        :context_(c)
    {
        image_.recreate(context_->cellWidth(), context_->cellHeight());
    }

public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    template<class TSomeConstView>
    Symbol match(const TSomeConstView& imgv);

private:
    const GlyphMatcherContextT* context_;
    ImageT image_;
};


template<class TFontImage, class TDistance>
class PolicyBasedGlyphMatcherContext: boost::noncopyable
{
public:
    typedef PolicyBasedGlyphMatcher<TFontImage, TDistance> GlyphMatcherT;
    typedef TFontImage FontImageT;
    typedef typename FontImageT::PixelT PixelT;
    typedef typename FontImageT::ImageT ImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

public:
    explicit PolicyBasedGlyphMatcherContext(const FontImageT* f,
            const TDistance& dist=TDistance())
        :font_(f)
        ,distance_(dist)
    {
    }

    const FontImageT* font() const
    {
        return font_;
    }

    unsigned cellWidth() const
    {
        return font_->glyphWidth();
    }

    unsigned cellHeight() const
    {
        return font_->glyphHeight();
    }

    GlyphMatcherT* createMatcher() const
    {
        return new GlyphMatcherT(this);
    }

    int calculateDistance(const ConstViewT& view1, const ConstViewT& view2) const
    {
        return distance_(view1, view2);
    }

private:
    const FontImageT* font_;
    TDistance distance_;
};

template<class TConstView>
Symbol match(const TConstView& imgv);

template<class TFontImage, class TDistance>
template<class TSomeConstView>
Symbol PolicyBasedGlyphMatcher<TFontImage, TDistance>::match(const TSomeConstView& imgv)
{
    assert(imgv.width() <= image_.width());
    assert(imgv.height() <= image_.height());

    ViewT image_view = view(image_);
    fill_pixels(image_view, PixelT());
    copy_pixels(imgv, subimage_view(image_view, 0, 0, imgv.width(), imgv.height()));

    int d2_min = std::numeric_limits<int>::max();
    Symbol cc_min;
    for (size_t ci = 0; ci < context_->font()->glyphCount(); ++ci) {
        int d2 = context_->calculateDistance(image_view, context_->font()->getGlyph(ci));
        if (d2 < d2_min) {
            d2_min = d2;
            cc_min = context_->font()->getSymbol(ci);
        }
    }
    return cc_min;
}

} } // namespace KG::Ascii

#endif // KGASCII_POLICYBASEDGLYPHMATCHER_HPP


