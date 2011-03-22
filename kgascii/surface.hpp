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

#ifndef KGASCII_SURFACE_HPP
#define KGASCII_SURFACE_HPP

#include <cassert>
#include <utility>

namespace KG { namespace Ascii {

template<typename T>
class SurfaceBase
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

public:
    SurfaceBase()
    {
        assign(0, 0, 0, 0);
    }

    SurfaceBase(size_t w, size_t h, T* d)
    {
        assign(w, h, d, w);
    }

    SurfaceBase(size_t w, size_t h, T* d, ptrdiff_t p)
    {
        assign(w, h, d, p);
    }

    SurfaceBase(const SurfaceBase& s)
    {
        assign(s.width(), s.height(), s.data(), s.pitch());
    }

    template<typename U>
    SurfaceBase(const SurfaceBase<U>& s)
    {
        assign(s.width(), s.height(), s.data(), s.pitch());
    }

    SurfaceBase& operator =(const SurfaceBase& s)
    {
        assign(s.width(), s.height(), s.data(), s.pitch());
        return *this;
    }

    template<typename U>
    SurfaceBase& operator =(const SurfaceBase<U>& s)
    {
        assign(s.width(), s.height(), s.data(), s.pitch());
        return *this;
    }

    void assign(size_t w, size_t h, T* d)
    {
        assign(w, h, d, w);
    }

    void assign(size_t w, size_t h, T* d, ptrdiff_t p)
    {
        assert(static_cast<ptrdiff_t>(w) <= p);
        width_ = w;
        height_ = h;
        data_ = d;
        pitch_ = p;
    }

public:
    T* data() const
    {
        return data_;
    }
    
    T* dataEnd() const
    {
        return data_ + pitch_ * height_;
    }

    T* row(size_t r) const
    {
        return data_ + r * pitch_;
    }
    
    T* rowEnd(size_t r) const
    {
        return row(r) + width_;
    }

    T& operator()(size_t x, size_t y) const
    {
        return *(data_ + y * pitch_ + x);
    }
    
    size_t width() const
    {
        return width_;
    }

    size_t height() const
    {
        return height_;
    }

    std::pair<size_t, size_t> dimensions() const
    {
        return std::make_pair(width_, height_);
    }

    ptrdiff_t pitch() const
    {
        return pitch_;
    }

    size_t size() const
    {
        return width_ * height_;
    }

    bool isContinuous() const
    {
        return static_cast<ptrdiff_t>(width_) == pitch_;
    }

    SurfaceBase window(size_t x, size_t y, size_t w, size_t h) const
    {
        assert(x + w <= width_);
        assert(y + h <= height_);
        return SurfaceBase(w, h, row(y) + x, pitch_);
    }

private:
    T* data_;
    size_t width_;
    size_t height_;
    ptrdiff_t pitch_;
};

typedef SurfaceBase<unsigned char> Surface8;
typedef SurfaceBase<const unsigned char> Surface8c;
typedef SurfaceBase<float> Surface32f;
typedef SurfaceBase<const float> Surface32fc;

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_HPP

