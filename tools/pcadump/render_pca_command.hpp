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

#ifndef KGASCII_TOOLS_RENDER_PCA_COMMAND_HPP
#define KGASCII_TOOLS_RENDER_PCA_COMMAND_HPP

#include <string>
#include <ostream>
#include <boost/noncopyable.hpp>


class RenderPcaCommand: boost::noncopyable
{
public:
    struct Parameters
    {
        std::string font_file;
        unsigned feature_cnt;
        std::string reconstructed_font_file;
        std::string features_file;
    };

public:
    explicit RenderPcaCommand(std::ostream& ostr);

    void execute(const Parameters& params);

private:
    std::ostream& log_;
};


#endif /* KGASCII_TOOLS_RENDER_PCA_COMMAND_HPP */
