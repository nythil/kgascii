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

#ifndef KGASCII_SURFACE_FWD_HPP
#define KGASCII_SURFACE_FWD_HPP

#include <limits>

namespace KG { namespace Ascii {

template<typename PixelTraits, typename AccessTag>
class SurfaceBase;

template<typename PixelTraits>
class SurfaceContainerBase;

template<typename T>
struct PixelTraitsBase
{
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
};

template<typename T>
struct IntegerPixelTraits: public PixelTraitsBase<T>
{
    static T minValue() { return std::numeric_limits<T>::min(); }
    static T maxValue() { return std::numeric_limits<T>::max(); }
};

typedef IntegerPixelTraits<unsigned char> PixelTraits8;
typedef IntegerPixelTraits<unsigned short> PixelTraits16;

struct PixelTraits32f: public PixelTraitsBase<float>
{
    static float minValue() { return 0.0f; }
    static float maxValue() { return 1.0f; }
};

struct MutableAccessTag {};
struct ConstAccessTag {};

template<typename PixelTraits, typename AccessTag>
struct PixelAccessTraits
{
    typedef typename PixelTraits::value_type value_type;
    typedef typename PixelTraits::reference reference;
    typedef typename PixelTraits::pointer pointer;
};

template<typename PixelTraits>
struct PixelAccessTraits<PixelTraits, ConstAccessTag>
{
    typedef const typename PixelTraits::value_type value_type;
    typedef typename PixelTraits::const_reference reference;
    typedef typename PixelTraits::const_pointer pointer;
};

typedef SurfaceBase<PixelTraits8, MutableAccessTag> Surface8;
typedef SurfaceBase<PixelTraits8, ConstAccessTag> Surface8c;
typedef SurfaceBase<PixelTraits16, MutableAccessTag> Surface16;
typedef SurfaceBase<PixelTraits16, ConstAccessTag> Surface16c;
typedef SurfaceBase<PixelTraits32f, MutableAccessTag> Surface32f;
typedef SurfaceBase<PixelTraits32f, ConstAccessTag> Surface32fc;

typedef SurfaceContainerBase<PixelTraits8> SurfaceContainer8;
typedef SurfaceContainerBase<PixelTraits16> SurfaceContainer16;
typedef SurfaceContainerBase<PixelTraits32f> SurfaceContainer32f;

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_FWD_HPP
