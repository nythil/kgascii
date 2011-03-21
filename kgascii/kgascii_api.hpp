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

#ifndef KGASCII_API_HPP
#define KGASCII_API_HPP

#include <kgascii/kgascii_config.hpp>

#if defined _WIN32 || defined __CYGWIN__
    #define KGASCII_DLL_DECL_IMPORT __declspec(dllimport)
    #define KGASCII_DLL_DECL_EXPORT __declspec(dllexport)
#else
    #if __GNUC__ >= 4
        #define KGASCII_DLL_DECL_IMPORT __attribute__ ((visibility("default")))
        #define KGASCII_DLL_DECL_EXPORT __attribute__ ((visibility("default")))
    #else
        #define KGASCII_DLL_DECL_IMPORT
        #define KGASCII_DLL_DECL_EXPORT
    #endif
#endif

#ifdef KGASCII_DLL
    #ifdef KGASCII_DLL_EXPORTS
        #define KGASCII_API KGASCII_DLL_DECL_EXPORT
    #else
        #define KGASCII_API KGASCII_DLL_DECL_IMPORT
    #endif
#else
    #define KGASCII_API
#endif

#endif // KGASCII_API_HPP

