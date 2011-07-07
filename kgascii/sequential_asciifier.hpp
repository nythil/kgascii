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

#ifndef KGASCII_SEQUENTIALASCIIFIER_HPP
#define KGASCII_SEQUENTIALASCIIFIER_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <kgascii/asciifier.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/text_surface.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

class SequentialAsciifier;

namespace Internal {

template<>
struct Traits<SequentialAsciifier>
{
    typedef GlyphMatcherContext GlyphMatcherContextT;
    typedef GlyphMatcher GlyphMatcherT;
    typedef Surface8c ConstSurfaceT;
};

} // namespace Internal

class SequentialAsciifier: public Asciifier<SequentialAsciifier>
{
public:
    typedef Asciifier<SequentialAsciifier> BaseT;
    typedef typename BaseT::GlyphMatcherContextT GlyphMatcherContextT;
    typedef typename BaseT::GlyphMatcherT GlyphMatcherT;
    typedef typename BaseT::ConstSurfaceT ConstSurfaceT;

public:
    SequentialAsciifier(const GlyphMatcherContextT* c)
        :context_(c)
        ,matcher_(context_->createMatcher())
    {
    }

public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    unsigned threadCount() const
    {
        return 1;
    }

public:
    void generate(const ConstSurfaceT& imgv, TextSurface& text)
    {
        //single character size
        size_t char_w = context_->cellWidth();
        size_t char_h = context_->cellHeight();
        //text surface size
        size_t text_w = text.cols() * char_w;
        size_t text_h = text.rows() * char_h;
        //processed image region size
        size_t roi_w = std::min(imgv.width(), text_w);
        size_t roi_h = std::min(imgv.height(), text_h);

        size_t y = 0, r = 0;
        for (; y + char_h <= roi_h; y += char_h, ++r) {
            size_t x = 0, c = 0;
            for (; x + char_w <= roi_w; x += char_w, ++c) {
                text(r, c) = matcher_->match(imgv.window(x, y, char_w, char_h));
            }
            if (x < roi_w) {
                size_t dx = roi_w - x;
                text(r, c) = matcher_->match(imgv.window(x, y, dx, char_h));
            }
        }
        if (y < roi_h) {
            size_t dy = roi_h - y;
            size_t x = 0, c = 0;
            for (; x + char_w <= roi_w; x += char_w, ++c) {
                text(r, c) = matcher_->match(imgv.window(x, y, char_w, dy));
            }
            if (x < roi_w) {
                size_t dx = roi_w - x;
                text(r, c) = matcher_->match(imgv.window(x, y, dx, dy));
            }
        }
    }

private:
    const GlyphMatcherContextT* context_;
    boost::scoped_ptr<GlyphMatcherT> matcher_;
};

} } // namespace KG::Ascii

#endif // KGASCII_SEQUENTIALASCIIFIER_HPP

