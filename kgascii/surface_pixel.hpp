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

#ifndef KGASCII_SURFACE_PIXEL_HPP
#define KGASCII_SURFACE_PIXEL_HPP

#include <limits>
#include <kgascii/surface_fwd.hpp>

namespace KG { namespace Ascii {

namespace Internal {

template<typename TValue>
struct MinMax
{
    static TValue minValue() { return std::numeric_limits<TValue>::min(); }
    static TValue maxValue() { return std::numeric_limits<TValue>::max(); }
};

template<>
struct MinMax<float>
{
    static float minValue() { return 0; }
    static float maxValue() { return 1; }
};

} // namespace Internal

template<typename TValue, template<typename> class TMinMax>
struct PixelType
{
    typedef TValue value_type;
    typedef TValue& reference;
    typedef const TValue& const_reference;
    typedef TValue* pointer;
    typedef const TValue* const_pointer;

    static TValue minValue() { return TMinMax<TValue>::minValue(); }
    static TValue maxValue() { return TMinMax<TValue>::maxValue(); }
};

namespace Internal {

template<typename TPixel>
struct PixelTraits
{
    typedef typename TPixel::value_type value_type;
    typedef typename TPixel::reference reference;
    typedef typename TPixel::const_reference const_reference;
    typedef typename TPixel::pointer pointer;
    typedef typename TPixel::const_pointer const_pointer;

    static value_type minValue() { return TPixel::minValue(); }
    static value_type maxValue() { return TPixel::maxValue(); }
};

template<typename TPixel>
struct PixelTraits<const TPixel>: public PixelTraits<TPixel>
{
    typedef typename TPixel::const_reference reference;
    typedef typename TPixel::const_pointer pointer;
};

} // namespace Internal

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_PIXEL_HPP
