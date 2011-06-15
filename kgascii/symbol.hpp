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

#ifndef KGASCII_SYMBOL_HPP
#define KGASCII_SYMBOL_HPP

#include <boost/operators.hpp>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

class KGASCII_API Symbol: public boost::totally_ordered<Symbol>
{
public:
    Symbol()
        :value_(0)
    {
    }

    explicit Symbol(int val)
        :value_(val)
    {
    }

public:
    int value() const
    {
        return value_;
    }

    char charValue() const
    {
        return static_cast<char>(value_);
    }

private:
    int value_;
};

inline bool operator <(const Symbol& lh, const Symbol& rh)
{
    return lh.value() < rh.value();
}

inline bool operator ==(const Symbol& lh, const Symbol& rh)
{
    return lh.value() == rh.value();
}

} } // namespace KG::Ascii

#endif // KGASCII_SYMBOL_HPP