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

#ifndef KGASCII_SURFACE_CONTAINER_HPP
#define KGASCII_SURFACE_CONTAINER_HPP

#include <cassert>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

template<typename PixelTraits>
class SurfaceContainerBase
{
public:
    typedef SurfaceBase<PixelTraits, MutableAccessTag> Surface;
    typedef SurfaceBase<PixelTraits, ConstAccessTag> ConstSurface;
    typedef typename PixelTraits::value_type value_type;
    typedef typename PixelTraits::reference reference;
    typedef typename PixelTraits::const_reference const_reference;
    typedef typename PixelTraits::pointer pointer;
    typedef typename PixelTraits::const_pointer const_pointer;

public:
    SurfaceContainerBase()
    {
        resize(0, 0);
    }

    SurfaceContainerBase(size_t w, size_t h)
    {
        resize(w, h);
    }

    SurfaceContainerBase(const SurfaceContainerBase& s)
    {
        assign(s);
    }

    template<typename U>
    SurfaceContainerBase(const SurfaceContainerBase<U>& s)
    {
        assign(s);
    }

    SurfaceContainerBase& operator =(const SurfaceContainerBase& s)
    {
        assign(s);
        return *this;
    }

    template<typename U>
    SurfaceContainerBase& operator =(const SurfaceContainerBase<U>& s)
    {
        assign(s);
        return *this;
    }

    template<typename U>
    void assign(const SurfaceContainerBase<U>& s)
    {
        resize(s.width(), s.height());
        copyPixels(s.surface(), surface());
    }

    void resize(size_t w, size_t h)
    {
        width_ = w;
        height_ = h;
        data_.resize(w * h);
    }

public:
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

    Surface surface()
    {
        return Surface(width_, height_, &data_[0]);
    }

    ConstSurface surface() const
    {
        return ConstSurface(width_, height_, &data_[0]);
    }

private:
    friend class boost::serialization::access;

    template<typename Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        using namespace boost::serialization;
        ar << make_nvp("width", width_);
        ar << make_nvp("height", height_);
        ar << make_nvp("data", make_array(&data_[0], width_ * height_));
    }

    template<typename Archive>
    void load(Archive& ar, const unsigned int version)
    {
        using namespace boost::serialization;
        size_t w, h;
        ar >> make_nvp("width", w);
        ar >> make_nvp("height", h);
        resize(w, h);
        ar >> make_nvp("data", make_array(&data_[0], width_ * height_));
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    std::vector<value_type> data_;
    size_t width_;
    size_t height_;
};

//typedef SurfaceContainerBase<unsigned char> SurfaceContainer8;
//typedef SurfaceContainerBase<float> SurfaceContainer32f;

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_CONTAINER_HPP

