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

#ifndef KGASCII_TEXTSURFACE_HPP
#define KGASCII_TEXTSURFACE_HPP

#include <vector>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

class KGASCII_API TextSurface
{
public:
    TextSurface()
        :rows_(0)
        ,cols_(0)
    {
    }

    TextSurface(unsigned rr, unsigned cc)
        :rows_(rr)
        ,cols_(cc)
        ,data_(rr * cc)
    {
    }

public:
    unsigned rows() const
    {
        return rows_;
    }

    unsigned cols() const
    {
        return cols_;
    }

    void resize(unsigned rr, unsigned cc)
    {
        if (rows_ != rr || cols_ != cc) {
            std::vector<char> new_data(rr * cc);
            std::swap(rows_, rr);
            std::swap(cols_, cc);
            std::swap(data_, new_data);
        }
    }

    void clear()
    {
        std::fill(data_.begin(), data_.end(), ' ');
    }

    char operator()(unsigned r, unsigned c) const
    {
        return data_.at(r * cols_ + c);
    }

    char& operator()(unsigned r, unsigned c)
    {
        return data_.at(r * cols_ + c);
    }

    const char* row(unsigned r) const
    {
        return &data_.at(r * cols_);
    }

    char* row(unsigned r)
    {
        return &data_.at(r * cols_);
    }

private:
    unsigned rows_;
    unsigned cols_;
    std::vector<char> data_;
};

} } // namespace KG::Ascii

#endif // KGASCII_TEXTSURFACE_HPP

