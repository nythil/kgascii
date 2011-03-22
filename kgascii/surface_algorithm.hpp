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

template<typename T, typename U>
void fillPixels(const SurfaceBase<T>& surf, U val)
{
    if (surf.isContinuous()) {
        Detail::fill(surf.data(), val, surf.size());
    } else {
        typename SurfaceBase<T>::pointer rowp = surf.data();
        for (size_t y = 0; y < surf.height(); ++y) {
            Detail::fill(rowp, val, surf.width());
            rowp += surf.pitch();
        }
    }
}

template<typename T, typename U>
void copyPixels(const SurfaceBase<T>& src, const SurfaceBase<U>& dst)
{
    assert(src.dimensions() == dst.dimensions());
    if (src.isContinuous() && dst.isContinuous()) {
        assert(src.size() == dst.size());
        Detail::copy(dst.data(), src.data(), src.size());
    } else {
        typename SurfaceBase<T>::pointer src_rowp = src.data();
        typename SurfaceBase<U>::pointer dst_rowp = dst.data();
        for (size_t y = 0; y < src.height(); ++y) {
            Detail::copy(dst_rowp, src_rowp, src.width());
            src_rowp += src.pitch();
            dst_rowp += dst.pitch();
        }
    }
}

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_ALGORITHM_HPP

