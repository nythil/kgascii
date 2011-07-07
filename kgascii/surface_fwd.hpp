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

namespace KG { namespace Ascii {

namespace Internal {

template<typename TValue>
struct MinMax;

} // namespace Internal

template<typename TValue, template<typename> class TMinMax=Internal::MinMax>
struct PixelType;

template<typename TPixel>
class Surface;

template<typename TPixel>
class SurfaceContainer;

typedef PixelType<unsigned char> PixelType8;
typedef PixelType<unsigned short> PixelType16;
typedef PixelType<unsigned long> PixelType32;
typedef PixelType<float> PixelType32f;

typedef Surface<PixelType8> Surface8;
typedef Surface<const PixelType8> Surface8c;
typedef SurfaceContainer<PixelType8> SurfaceContainer8;

typedef Surface<PixelType16> Surface16;
typedef Surface<const PixelType16> Surface16c;
typedef SurfaceContainer<PixelType16> SurfaceContainer16;

typedef Surface<PixelType32> Surface32;
typedef Surface<const PixelType32> Surface32c;
typedef SurfaceContainer<PixelType32> SurfaceContainer32;

typedef Surface<PixelType32f> Surface32f;
typedef Surface<const PixelType32f> Surface32fc;
typedef SurfaceContainer<PixelType32f> SurfaceContainer32f;

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_FWD_HPP
