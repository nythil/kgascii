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

#ifndef KGASCII_TOOLS_COMMON_CONSOLE_HPP
#define KGASCII_TOOLS_COMMON_CONSOLE_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


namespace KG { namespace Ascii {
class TextSurface;
} } // namespace KG::Ascii


class Console: boost::noncopyable
{
public:
    Console();

    ~Console();

public:
    void setup(int rows, int cols);

    void display(const KG::Ascii::TextSurface& text);

private:
    boost::scoped_ptr<class ConsoleImpl> impl_;
};


#endif // KGASCII_TOOLS_COMMON_CONSOLE_HPP

