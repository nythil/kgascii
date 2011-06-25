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

#ifndef KGASCII_SURFACE_ALGORITHM_HPP
#define KGASCII_SURFACE_ALGORITHM_HPP

#include <cassert>
#include <cstring>
#include <algorithm>
#include <utility>
#include <boost/type_traits.hpp>
#include <Eigen/Dense>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

namespace Detail {

template<typename T, typename U, bool b>
static void doFill(T* dst, U val, size_t cnt, const boost::integral_constant<bool, b>&)
{
    std::fill(dst, dst + cnt, val);
}

template<typename T>
static void doFill(T* dst, T val, size_t cnt, const boost::true_type&)
{
    std::memset(dst, val, cnt);
}

template<typename T, typename U>
static void fill(T* dst, U val, size_t cnt)
{
    typedef boost::integral_constant<bool,
            boost::has_trivial_assign<T>::value && 
            (sizeof(T) == 1) &&
            boost::is_convertible<U, T>::value> truth_type;
    doFill(dst, val, cnt, truth_type());
}

template<typename T, typename U, bool b>
static void doCopy(U* dst, const T* src, size_t cnt, const boost::integral_constant<bool, b>&)
{
    std::copy(src, src + cnt, dst);
}

template<typename T>
static void doCopy(T* dst, const T* src, size_t cnt, const boost::true_type&)
{
    std::memmove(dst, src, cnt * sizeof(T));
}

template<typename T, typename U>
static void copy(U* dst, const T* src, size_t cnt)
{
    doCopy(dst, src, cnt, boost::has_trivial_assign<T>());
}

} // namespace Detail

template<typename PT, typename AT>
void fillPixels(const SurfaceBase<PT, AT>& surf, typename PT::value_type val)
{
    if (surf.isContinuous()) {
        Detail::fill(surf.data(), val, surf.size());
    } else {
        typename SurfaceBase<PT, AT>::pointer rowp = surf.data();
        for (size_t y = 0; y < surf.height(); ++y) {
            Detail::fill(rowp, val, surf.width());
            rowp += surf.pitch();
        }
    }
}

template<typename PT, typename AT1, typename AT2>
void copyPixels(const SurfaceBase<PT, AT1>& src, const SurfaceBase<PT, AT2>& dst)
{
    if (src.dimensions() != dst.dimensions())
        BOOST_THROW_EXCEPTION(std::logic_error("src.dimensions() != dst.dimensions()"));
    if (src.isContinuous() && dst.isContinuous()) {
        assert(src.size() == dst.size());
        Detail::copy(dst.data(), src.data(), src.size());
    } else {
        typename SurfaceBase<PT, AT1>::pointer src_rowp = src.data();
        typename SurfaceBase<PT, AT2>::pointer dst_rowp = dst.data();
        for (size_t y = 0; y < src.height(); ++y) {
            Detail::copy(dst_rowp, src_rowp, src.width());
            src_rowp += src.pitch();
            dst_rowp += dst.pitch();
        }
    }
}

template<typename PT, typename AT>
void copyPixels(const Eigen::VectorXf& src, const SurfaceBase<PT, AT>& dst)
{
    if (static_cast<size_t>(src.size()) != dst.size())
        BOOST_THROW_EXCEPTION(std::logic_error("src.size() != dst.size()"));
    typedef typename SurfaceBase<PT, AT>::value_type value_type;
    typedef Eigen::Matrix<value_type, Eigen::Dynamic, 1> VectorXuc;
    if (dst.isContinuous()) {
        VectorXuc::Map(dst.data(), dst.size()) = src.cast<value_type>();
    } else {
        size_t img_w = dst.width();
        for (size_t y = 0; y < dst.height(); ++y) {
            VectorXuc::Map(dst.row(y), img_w) = src.segment(y * img_w, img_w).cast<value_type>();
        }
    }
}

template<typename PT, typename AT>
void copyPixels(const SurfaceBase<PT, AT>& src, Eigen::VectorXf& dst)
{
    if (src.size() != static_cast<size_t>(dst.size()))
        BOOST_THROW_EXCEPTION(std::logic_error("src.size() != dst.size()"));
    typedef typename SurfaceBase<PT, AT>::value_type value_type;
    typedef Eigen::Matrix<value_type, Eigen::Dynamic, 1> VectorXuc;
    if (src.isContinuous()) {
        dst = (VectorXuc::Map(src.data(), src.size()));
    } else {
        size_t img_w = src.width();
        for (size_t y = 0; y < src.height(); ++y) {
            dst.segment(y * img_w, img_w) = VectorXuc::Map(src.row(y), img_w);
        }
    }
}

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_ALGORITHM_HPP

