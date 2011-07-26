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

#ifndef KGASCII_FT2PP_FACE_HPP
#define KGASCII_FT2PP_FACE_HPP

#include <string>
#include <cassert>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <kgascii/ft2pp/error.hpp>

namespace KG { namespace Ascii { namespace FT2pp {

class Library;

class Face: boost::noncopyable
{
public:
    Face(Library& lib, const std::string& filename, int face_index)
        :library_(lib)
        ,handle_(0)
    {
        KGASCII_FREETYPE_CALL(FT_New_Face, lib.handle(), filename.c_str(), face_index, &handle_);
    }

    ~Face()
    {
        FT_Done_Face(handle_);
        handle_ = 0;
    }

public:
    FT_Face handle()
    {
        return handle_;
    }

    FT_Face operator->() const
    {
        return handle_;
    }

public:
    void setPixelSizes(int x_ppem, int y_ppem)
    {
        KGASCII_FREETYPE_CALL(FT_Set_Pixel_Sizes, handle_, x_ppem, y_ppem);
    }

    boost::optional<unsigned> getFirstChar()
    {
        unsigned agindex = 0;
        unsigned charcode = FT_Get_First_Char(handle_, &agindex);
        return boost::make_optional(agindex != 0, charcode);
    }

    boost::optional<unsigned> getNextChar(unsigned charcode)
    {
        unsigned agindex = 0;
        unsigned next_charcode = FT_Get_Next_Char(handle_, charcode, &agindex);
        return boost::make_optional(agindex != 0, next_charcode);
    }

    void loadChar(unsigned charcode, int flags)
    {
        KGASCII_FREETYPE_CALL(FT_Load_Char, handle_, charcode, flags);
    }

    void renderChar(FT_Render_Mode flags)
    {
        assert(handle_->glyph);
        KGASCII_FREETYPE_CALL(FT_Render_Glyph, handle_->glyph, flags);
    }

private:
    Library& library_;
    FT_Face handle_;
};

} } } // namespace KG::Ascii::FT2pp

#endif // KGASCII_FT2PP_FACE_HPP
