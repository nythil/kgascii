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

#ifndef KGASCII_TOOLS_DUMP_FONT_COMMAND_HPP
#define KGASCII_TOOLS_DUMP_FONT_COMMAND_HPP

#include <string>
#include <vector>
#include <iosfwd>
#include <boost/noncopyable.hpp>
#include <kgascii/ft2pp/library.hpp>
#include <kgascii/ft2pp/face.hpp>
#include <kgascii/ft2pp/util.hpp>


class DumpFontCommand: boost::noncopyable
{
public:
    struct Parameters
    {
        std::vector<std::string> files;
    };

public:
    explicit DumpFontCommand();

    void execute(const Parameters& params);
    
private:
    void enumerateFaces(const std::string& fileName, std::ostream& out);

    void enumerateFaceSizes(KG::Ascii::FT2pp::Face& ft_face, std::ostream& out);

    void printSizeMetrics(const KG::Ascii::FT2pp::Face& ft_face, std::ostream& out);

private:
    KG::Ascii::FT2pp::Library ftLibrary_;
};


#endif /* KGASCII_TOOLS_DUMP_FONT_COMMAND_HPP */
