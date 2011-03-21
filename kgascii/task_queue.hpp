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

#ifndef KGASCII_TASKQUEUE_HPP
#define KGASCII_TASKQUEUE_HPP

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <deque>

namespace KG { namespace Ascii {

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
    bool closing_;
    std::deque<Task> queue_;
    boost::mutex mutex_;
    boost::condition_variable activeCondition_;
    boost::condition_variable doneCondition_;
};

} } // namespace KG::Ascii

#endif // KGASCII_TASKQUEUE_HPP

