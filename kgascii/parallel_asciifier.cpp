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

#include <kgascii/parallel_asciifier.hpp>
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

class ParallelAsciifier::Impl: boost::noncopyable
{
public:
	explicit Impl(const GlyphMatcherContext* c);

	void setupThreads(unsigned thr_cnt);

	void endThreads();

	void enqueue(const Surface8c& surf, char* outp);

private:
    void threadFunc();

public:
    const GlyphMatcherContext* context_;
    boost::thread_group group_;
	struct WorkItem
	{
		Surface8c imgv;
		char* outp;
	};
    TaskQueue<WorkItem> queue_;
};

ParallelAsciifier::Impl::Impl(const GlyphMatcherContext* c)
	:context_(c)
{
}

void ParallelAsciifier::Impl::setupThreads(unsigned thr_cnt)
{
    if (thr_cnt == 0) {
        thr_cnt = boost::thread::hardware_concurrency() + 1;
    }
    for (unsigned i = 0; i < thr_cnt; ++i) {
		group_.create_thread(boost::bind(&ParallelAsciifier::Impl::threadFunc, this));
    }
}

void ParallelAsciifier::Impl::endThreads()
{
    queue_.close();
    group_.join_all();
}

void ParallelAsciifier::Impl::enqueue(const Surface8c& surf, char* outp)
{
    WorkItem wi = { surf, outp };
    queue_.push(wi);
}

void ParallelAsciifier::Impl::threadFunc()
{
    boost::scoped_ptr<GlyphMatcher> matcher(context_->createMatcher());
    //single character size
    size_t char_w = context_->cellWidth();

    WorkItem wi;
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

    
ParallelAsciifier::ParallelAsciifier(const GlyphMatcherContext* c, unsigned thr_cnt)
    :Asciifier()
	,impl_(boost::make_shared<Impl>(c))
{
	impl_->setupThreads(thr_cnt);
}

ParallelAsciifier::~ParallelAsciifier()
{
	impl_->endThreads();
}

const GlyphMatcherContext* ParallelAsciifier::context() const
{
    return impl_->context_;
}

unsigned ParallelAsciifier::threadCount() const
{
    return impl_->group_.size();
}

void ParallelAsciifier::generate(const Surface8c& imgv, TextSurface& text)
{
    //single character size
    size_t char_w = impl_->context_->cellWidth();
    size_t char_h = impl_->context_->cellHeight();
    //text surface size
    size_t text_w = text.cols() * char_w;
    size_t text_h = text.rows() * char_h;
    //processed image region size
    size_t roi_w = std::min(imgv.width(), text_w);
    size_t roi_h = std::min(imgv.height(), text_h);

    size_t y = 0, r = 0;
    for (; y + char_h <= roi_h; y += char_h, ++r) {
        impl_->enqueue(imgv.window(0, y, roi_w, char_h), text.row(r));
    }
    if (y < roi_h) {
        size_t dy = roi_h - y;
        impl_->enqueue(imgv.window(0, y, roi_w, dy), text.row(r));
    }
    impl_->queue_.wait_empty();
}

} } // namespace KG::Ascii

