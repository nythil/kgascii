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

#ifndef KGASCII_FT2PP_LIBRARY_HPP
#define KGASCII_FT2PP_LIBRARY_HPP

#include <boost/noncopyable.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "error.hpp"

namespace FT2pp {

class Library: boost::noncopyable
{
public:
    Library()
        :handle_(0)
    {
        checkCall(FT_Init_FreeType(&handle_));
    }

    ~Library()
    {
        FT_Done_FreeType(handle_);
        handle_ = 0;
    }

public:
    FT_Library handle()
    {
        return handle_;
    }

private:
    FT_Library handle_;
};

}

#endif // KGASCII_FT2PP_LIBRARY_HPP
