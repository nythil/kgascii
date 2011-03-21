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
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/thread.hpp>
#include <kgascii/asciifier.hpp>
#include <kgascii/task_queue.hpp>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

class KGASCII_API ParallelAsciifier: public Asciifier
{
public:
    ParallelAsciifier(const GlyphMatcherContext& c, size_t thr_cnt);

    ~ParallelAsciifier();
    
public:
    const GlyphMatcherContext& context() const;

    size_t threadCount() const;

public:
    void generate(const boost::gil::gray8c_view_t& imgv, TextSurface& text);

private:
    void threadFunc();

    struct WorkItem
    {
        boost::gil::gray8c_view_t imgv;
        char* outp;
    };
    
private:
    const GlyphMatcherContext& context_;
    boost::thread_group group_;
    TaskQueue<WorkItem> queue_;
};

} } // namespace KG::Ascii

#endif // KGASCII_PARALLELASCIIFIER_HPP

