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

#include "asciifier.hpp"
#include "glyphmatcher.hpp"
#include "textsurface.hpp"
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace KG { namespace Ascii {

using namespace boost::gil;

class AsciifierStrategy: boost::noncopyable
{
public:
    virtual void generate(const gray8c_view_t& imgv, TextSurface& text) = 0;
    
    virtual ~AsciifierStrategy();

protected:
    AsciifierStrategy(const GlyphMatcher& m);

protected:
    const GlyphMatcher& matcher_;
};

AsciifierStrategy::AsciifierStrategy(const GlyphMatcher& m)
    :matcher_(m)
{
}

AsciifierStrategy::~AsciifierStrategy()
{
}

class SequentialStrategy: public AsciifierStrategy
{
public:
    SequentialStrategy(const GlyphMatcher& m);

    void generate(const gray8c_view_t& imgv, TextSurface& text);

private:
    boost::gil::gray8_image_t cornerImg_;    
};

SequentialStrategy::SequentialStrategy(const GlyphMatcher& m)
    :AsciifierStrategy(m)
    ,cornerImg_(m.glyphWidth(), m.glyphHeight())
{
}

void SequentialStrategy::generate(const gray8c_view_t& imgv, TextSurface& text)
{
    //single character size
    int char_w = matcher_.glyphWidth();
    int char_h = matcher_.glyphHeight();
    //text surface size
    int text_w = text.cols() * char_w;
    int text_h = text.rows() * char_h;
    //processed image region size
    int roi_w = std::min(imgv.width(), text_w);
    int roi_h = std::min(imgv.height(), text_h);

    gray8_view_t tmpview = view(cornerImg_);

    int y = 0, r = 0;
    for (; y + char_h <= roi_h; y += char_h, ++r) {
        int x = 0, c = 0;
        for (; x + char_w <= roi_w; x += char_w, ++c) {
            text(r, c) = matcher_.match(subimage_view(imgv, x, y, char_w, char_h));
        }
        if (x < roi_w) {
            int dx = roi_w - x;
            fill_pixels(tmpview, 0);
            copy_pixels(subimage_view(imgv, x, y, dx, char_h), 
                    subimage_view(tmpview, 0, 0, dx, char_h));
            text(r, c) = matcher_.match(tmpview);
        }
    }
    if (y < roi_h) {
        int dy = roi_h - y;
        int x = 0, c = 0;
        fill_pixels(tmpview, 0);
        for (; x + char_w <= roi_w; x += char_w, ++c) {
            copy_pixels(subimage_view(imgv, x, y, char_w, dy), 
                    subimage_view(tmpview, 0, 0, char_w, dy));
            text(r, c) = matcher_.match(tmpview);
        }
        if (x < roi_w) {
            int dx = roi_w - x;
            fill_pixels(tmpview, 0);
            copy_pixels(subimage_view(imgv, x, y, dx, dy), 
                    subimage_view(tmpview, 0, 0, dx, dy));
            text(r, c) = matcher_.match(tmpview);
        }
    }
}

template<typename Task>
class TaskQueue: boost::noncopyable
{
public:
    TaskQueue()
        :closing_(false)
    {
    }

    void push(const Task& t)
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        queue_.push_back(t);
        lock.unlock();
        activeCondition_.notify_one();
    }

    bool wait_pop(Task& t)
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        while (!closing_ && queue_.empty()) {
            activeCondition_.wait(lock);
        }
        if (closing_)
            return false;
        t = queue_.front();
        queue_.pop_front();
        return true;
    }

    void done()
    {
        doneCondition_.notify_one();
    }

    void wait_empty()
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        while (!queue_.empty()) {
            doneCondition_.wait(lock);
        }
    }

    void close()
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        closing_ = true;
        lock.unlock();
        activeCondition_.notify_all();
    }

private:
    std::deque<Task> queue_;
    boost::mutex mutex_;
    bool closing_;
    boost::condition_variable activeCondition_;
    boost::condition_variable doneCondition_;
};

class ParallelStrategy: public AsciifierStrategy
{
public:
    ParallelStrategy(const GlyphMatcher& m, int thr_cnt);

    ~ParallelStrategy();

    void generate(const gray8c_view_t& imgv, TextSurface& text);

private:
    void threadFunc();

    struct WorkItem
    {
        gray8c_view_t imgv;
        char* outp;
    };
    
private:
    boost::thread_group group_;
    TaskQueue<WorkItem> queue_;
};

ParallelStrategy::ParallelStrategy(const GlyphMatcher& m, int thr_cnt)
    :AsciifierStrategy(m)
{
    for (int i = 0; i < thr_cnt; ++i) {
        group_.create_thread(boost::bind(&ParallelStrategy::threadFunc, this));
    }
}

ParallelStrategy::~ParallelStrategy()
{
    queue_.close();
    group_.join_all();
}

void ParallelStrategy::generate(const gray8c_view_t& imgv, TextSurface& text)
{
    //single character size
    int char_w = matcher_.glyphWidth();
    int char_h = matcher_.glyphHeight();
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

void ParallelStrategy::threadFunc()
{
    gray8_image_t corner_img(matcher_.glyphWidth(), matcher_.glyphHeight());
    gray8_view_t corner_view = view(corner_img);

    WorkItem wi;
    while (queue_.wait_pop(wi)) {
        fill_pixels(corner_view, 0);
        copy_pixels(wi.imgv, subimage_view(corner_view, 0, 0, wi.imgv.width(), wi.imgv.height()));
        *wi.outp = matcher_.match(corner_view);
        queue_.done();
    }
}


Asciifier::Asciifier(const GlyphMatcher& m)
    :matcher_(m)
{
    setSequential();
}

Asciifier::~Asciifier()
{
}

void Asciifier::generate(const gray8c_view_t& imgv, TextSurface& text) const
{
    if (strategy_) {
        strategy_->generate(imgv, text);
    }
}

void Asciifier::setSequential()
{
    strategy_.reset(new SequentialStrategy(matcher_));
}

void Asciifier::setParallel(int cnt)
{
    strategy_.reset(new ParallelStrategy(matcher_, cnt));
}

} } // namespace KG::Ascii

