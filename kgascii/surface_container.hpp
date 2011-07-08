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
#include <kgascii/surface_pixel.hpp>

namespace KG { namespace Ascii {

template<typename TPixel>
class SurfaceContainer
{
public:
    typedef typename Internal::PixelTraits<TPixel>::value_type value_type;
    typedef typename Internal::PixelTraits<TPixel>::reference reference;
    typedef typename Internal::PixelTraits<TPixel>::const_reference const_reference;
    typedef typename Internal::PixelTraits<TPixel>::pointer pointer;
    typedef typename Internal::PixelTraits<TPixel>::const_pointer const_pointer;
    typedef Surface<TPixel> SurfaceT;
    typedef Surface<const TPixel> ConstSurfaceT;

public:
    SurfaceContainer()
    {
        resize(0, 0);
    }

    SurfaceContainer(size_t w, size_t h)
    {
        resize(w, h);
    }

    SurfaceContainer(const SurfaceContainer& s)
    {
        assign(s);
    }

    template<typename TOther>
    SurfaceContainer(const SurfaceContainer<TOther>& s)
    {
        assign(s);
    }

    SurfaceContainer& operator =(const SurfaceContainer& s)
    {
        assign(s);
        return *this;
    }

    template<typename TOther>
    SurfaceContainer& operator =(const SurfaceContainer<TOther>& s)
    {
        assign(s);
        return *this;
    }

    template<typename TOther>
    void assign(const SurfaceContainer<TOther>& s)
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

    SurfaceT surface()
    {
        return SurfaceT(width_, height_, &data_[0]);
    }

    ConstSurfaceT surface() const
    {
        return ConstSurfaceT(width_, height_, &data_[0]);
    }

private:
    friend class boost::serialization::access;

    template<typename Archive>
    void save(Archive& ar, const unsigned int version) const
    {
    	(void)version;
        using namespace boost::serialization;
        ar << make_nvp("width", width_);
        ar << make_nvp("height", height_);
        ar << make_nvp("data", make_array(&data_[0], width_ * height_));
    }

    template<typename Archive>
    void load(Archive& ar, const unsigned int version)
    {
    	(void)version;
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

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_CONTAINER_HPP

