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

#ifndef KGASCII_PARALLELASCIIFIER_HPP
#define KGASCII_PARALLELASCIIFIER_HPP

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <kgascii/asciifier.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/text_surface.hpp>
#include <kgascii/surface.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include <kgascii/task_queue.hpp>

namespace KG { namespace Ascii {

class ParallelAsciifier;

namespace Internal {

template<>
struct Traits<ParallelAsciifier>
{
    typedef GlyphMatcherContext GlyphMatcherContextT;
    typedef GlyphMatcher GlyphMatcherT;
    typedef Surface8c ConstSurfaceT;
};

} // namespace Internal

class ParallelAsciifier: public Asciifier<ParallelAsciifier>
{
public:
    typedef Asciifier<ParallelAsciifier> BaseT;
    typedef typename BaseT::GlyphMatcherContextT GlyphMatcherContextT;
    typedef typename BaseT::GlyphMatcherT GlyphMatcherT;
    typedef typename BaseT::ConstSurfaceT ConstSurfaceT;

public:
    ParallelAsciifier(const GlyphMatcherContextT* c, unsigned thr_cnt)
        :context_(c)
    {
        setupThreads(thr_cnt);
    }

    ~ParallelAsciifier()
    {
        endThreads();
    }
    
public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    unsigned threadCount() const
    {
        return group_.size();
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
            enqueue(imgv.window(0, y, roi_w, char_h), text.row(r));
        }
        if (y < roi_h) {
            size_t dy = roi_h - y;
            enqueue(imgv.window(0, y, roi_w, dy), text.row(r));
        }
        queue_.wait_empty();
    }

private:
    void setupThreads(unsigned thr_cnt)
    {
        if (thr_cnt == 0) {
            thr_cnt = boost::thread::hardware_concurrency() + 1;
        }
        for (unsigned i = 0; i < thr_cnt; ++i) {
            group_.create_thread(boost::bind(&ParallelAsciifier::threadFunc, this));
        }
    }

    void endThreads()
    {
        queue_.close();
        group_.join_all();
    }

    void enqueue(const ConstSurfaceT& surf, Symbol* outp)
    {
        WorkItem wi = { surf, outp };
        queue_.push(wi);
    }

private:
    void threadFunc()
    {
        boost::scoped_ptr<GlyphMatcherT> matcher(context_->createMatcher());
        //single character size
        size_t char_w = context_->cellWidth();

        WorkItem wi = WorkItem();
        while (queue_.wait_pop(wi)) {
            //processed image region size
            size_t roi_w = wi.imgv.width();
            size_t roi_h = wi.imgv.height();
            size_t x = 0, c = 0;
            for (; x + char_w <= roi_w; x += char_w, ++c) {
                wi.outp[c] = matcher->match(wi.imgv.window(x, 0, char_w, roi_h));
            }
            if (x < roi_w) {
                size_t dx = roi_w - x;
                wi.outp[c] = matcher->match(wi.imgv.window(x, 0, dx, roi_h));
            }
            queue_.done();
        }
    }

private:
    const GlyphMatcherContextT* context_;
    boost::thread_group group_;
    struct WorkItem
    {
        ConstSurfaceT imgv;
        Symbol* outp;
    };
    TaskQueue<WorkItem> queue_;
};

} } // namespace KG::Ascii

#endif // KGASCII_PARALLELASCIIFIER_HPP

