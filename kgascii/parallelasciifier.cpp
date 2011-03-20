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

#include "parallelasciifier.hpp"
#include "glyphmatchercontext.hpp"
#include "glyphmatcher.hpp"
#include "textsurface.hpp"
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

namespace KG { namespace Ascii {

using namespace boost::gil;

ParallelAsciifier::ParallelAsciifier(const GlyphMatcherContext& c, size_t thr_cnt)
    :Asciifier()
    ,context_(c)
{
    if (thr_cnt == 0) {
        thr_cnt = boost::thread::hardware_concurrency() + 1;
    }
    for (size_t i = 0; i < thr_cnt; ++i) {
        group_.create_thread(boost::bind(&ParallelAsciifier::threadFunc, this));
    }
}

ParallelAsciifier::~ParallelAsciifier()
{
    queue_.close();
    group_.join_all();
}

const GlyphMatcherContext& ParallelAsciifier::context() const
{
    return context_;
}

size_t ParallelAsciifier::threadCount() const
{
    return group_.size();
}

void ParallelAsciifier::generate(const gray8c_view_t& imgv, TextSurface& text)
{
    //single character size
    int char_w = context_.cellWidth();
    int char_h = context_.cellHeight();
    //text surface size
    int text_w = text.cols() * char_w;
    int text_h = text.rows() * char_h;
    //processed image region size
    int roi_w = std::min(imgv.width(), text_w);
    int roi_h = std::min(imgv.height(), text_h);

    int y = 0, r = 0;
    for (; y + char_h <= roi_h; y += char_h, ++r) {
        int x = 0, c = 0;
        for (; x + char_w <= roi_w; x += char_w, ++c) {
            WorkItem wi = { subimage_view(imgv, x, y, char_w, char_h), &text(r, c) };
            queue_.push(wi);
        }
        if (x < roi_w) {
            int dx = roi_w - x;
            WorkItem wi = { subimage_view(imgv, x, y, dx, char_h), &text(r, c) };
            queue_.push(wi);
        }
    }
    if (y < roi_h) {
        int dy = roi_h - y;
        int x = 0, c = 0;
        for (; x + char_w <= roi_w; x += char_w, ++c) {
            WorkItem wi = { subimage_view(imgv, x, y, char_w, dy), &text(r, c) };
            queue_.push(wi);
        }
        if (x < roi_w) {
            int dx = roi_w - x;
            WorkItem wi = { subimage_view(imgv, x, y, dx, dy), &text(r, c) };
            queue_.push(wi);
        }
    }
    queue_.wait_empty();
}

void ParallelAsciifier::threadFunc()
{
    boost::scoped_ptr<GlyphMatcher> matcher(context_.createMatcher());

    WorkItem wi;
    while (queue_.wait_pop(wi)) {
        *wi.outp = matcher->match(wi.imgv);
        queue_.done();
    }
}

} } // namespace KG::Ascii

