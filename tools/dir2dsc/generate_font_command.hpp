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

#ifndef KGASCII_TOOLS_GENERATE_FONT_COMMAND_HPP
#define KGASCII_TOOLS_GENERATE_FONT_COMMAND_HPP

#include <string>
#include <boost/noncopyable.hpp>


class GenerateFontCommand: boost::noncopyable
{
public:
    struct Parameters
    {
        std::string image_path;
        std::string output_filename;
    };

public:
    explicit GenerateFontCommand();

    void execute(const Parameters& params);
};


#endif /* KGASCII_TOOLS_GENERATE_FONT_COMMAND_HPP */
