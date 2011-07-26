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

#ifndef KGASCII_FT2PP_ERROR_HPP
#define KGASCII_FT2PP_ERROR_HPP

#include <string>
#include <stdexcept>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/throw_exception.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace KG { namespace Ascii { namespace FT2pp {

typedef boost::error_info<struct tag_fterror, FT_Error> errinfo_fterror;
typedef boost::error_info<struct tag_ftapi, std::string> errinfo_ftapi;

struct FreeTypeError: virtual std::exception, virtual boost::exception {};

#define KGASCII_FREETYPE_CALL(fun, ...)         \
        do {                                    \
            FT_Error err = (fun)(__VA_ARGS__);  \
            if (err)                            \
                BOOST_THROW_EXCEPTION(          \
                    FreeTypeError() <<          \
                    errinfo_fterror(err) <<     \
                    errinfo_ftapi(#fun)         \
                );                              \
        } while (0)

} } } // namespace KG::Ascii::FT2pp

#endif // KGASCII_FT2PP_ERROR_HPP
