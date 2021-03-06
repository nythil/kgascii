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
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <kgascii/symbol.hpp>

namespace KG { namespace Ascii {

template<class TFontImage, class TDistance>
class PolicyBasedGlyphMatcher: boost::noncopyable
{
public:
    typedef TFontImage FontImageT;
    typedef typename FontImageT::PixelT PixelT;
    typedef typename FontImageT::ImageT ImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

    class PolicyBasedContext
    {
        friend class PolicyBasedGlyphMatcher;
    public:
        typedef PolicyBasedGlyphMatcher GlyphMatcherT;

    private:
        explicit PolicyBasedContext(const PolicyBasedGlyphMatcher* matcher)
            :image_(matcher->cellWidth(), matcher->cellHeight())
        {
        }

    private:
        ImageT image_;
    };
    typedef PolicyBasedContext ContextT;

public:
    explicit PolicyBasedGlyphMatcher(boost::shared_ptr<const FontImageT> f, const TDistance& dist=TDistance())
        :font_(f)
        ,distance_(dist)
    {
    }

    boost::shared_ptr<const FontImageT> font() const
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

    PolicyBasedContext createContext() const
    {
        return PolicyBasedContext(this);
    }

    template<class TSomeView>
    Symbol match(PolicyBasedContext& ctx, const TSomeView& imgv) const
    {
        assert(imgv.width() <= ctx.image_.width());
        assert(imgv.height() <= ctx.image_.height());

        ViewT image_view = view(ctx.image_);
        fill_pixels(image_view, PixelT());
        copy_pixels(imgv, subimage_view(image_view, 0, 0, imgv.width(), imgv.height()));

        int d2_min = std::numeric_limits<int>::max();
        Symbol cc_min;
        for (size_t ci = 0; ci < font()->glyphCount(); ++ci) {
            int d2 = calculateDistance(image_view, font()->getGlyph(ci));
            if (d2 < d2_min) {
                d2_min = d2;
                cc_min = font()->getSymbol(ci);
            }
        }
        return cc_min;
    }

    int calculateDistance(const ConstViewT& view1, const ConstViewT& view2) const
    {
        return distance_(view1, view2);
    }

private:
    boost::shared_ptr<const FontImageT> font_;
    TDistance distance_;
};

} } // namespace KG::Ascii

#endif // KGASCII_POLICYBASEDGLYPHMATCHER_HPP


