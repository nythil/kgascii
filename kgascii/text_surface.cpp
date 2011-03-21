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

#include <kgascii/text_surface.hpp>

namespace KG { namespace Ascii {

TextSurface::TextSurface()
    :rows_(0)
    ,cols_(0)
{
}

TextSurface::TextSurface(int rr, int cc)
    :rows_(rr)
    ,cols_(cc)
    ,data_(rr * cc)
{
}

int TextSurface::rows() const
{
    return rows_;
}

int TextSurface::cols() const
{
    return cols_;
}

void TextSurface::resize(int rr, int cc)
{
    if (rows_ != rr || cols_ != cc) {
        std::vector<char> new_data(rr * cc);
        std::swap(rows_, rr);
        std::swap(cols_, cc);
        std::swap(data_, new_data);
    }
}

void TextSurface::clear()
{
    std::fill(data_.begin(), data_.end(), ' ');
}

char TextSurface::operator()(int r, int c) const
{
    return data_.at(r * cols_ + c);
}

char& TextSurface::operator()(int r, int c)
{
    return data_.at(r * cols_ + c);
}

const char* TextSurface::row(int r) const
{
    return &data_.at(r * cols_);
}

} } // namespace KG::Ascii

