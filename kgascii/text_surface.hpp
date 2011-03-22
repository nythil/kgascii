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
    TextSurface();

    TextSurface(unsigned rr, unsigned cc);

public:
    unsigned rows() const;

    unsigned cols() const;

    void resize(unsigned rr, unsigned cc);

    void clear();

    char operator()(unsigned r, unsigned c) const;

    char& operator()(unsigned r, unsigned c);

    const char* row(unsigned r) const;

private:
    unsigned rows_;
    unsigned cols_;
    std::vector<char> data_;
};

} } // namespace KG::Ascii

#endif // KGASCII_TEXTSURFACE_HPP

