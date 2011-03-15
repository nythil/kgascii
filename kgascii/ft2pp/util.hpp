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

#ifndef KGASCII_FT2PP_UTIL_HPP
#define KGASCII_FT2PP_UTIL_HPP

namespace FT2pp {

inline double from26Dot6(long x)
{
    return (x / 64) + (x % 64) / 64.0;
}

inline double from16Dot16(long x)
{
    return (x / 65536) + (x % 65536) / 65536.0;
}

}

#endif // KGASCII_FT2PP_UTIL_HPP
