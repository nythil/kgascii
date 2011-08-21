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

#include <iostream>
#include <boost/filesystem.hpp>
#include <common/cmdline_tool.hpp>
#include "render_font_command.hpp"


class Dsc2Img: public CmdlineTool
{
public:
    Dsc2Img();

protected:
    int doExecute();    
};


int main(int argc, char* argv[])
{
    return Dsc2Img().execute(argc, argv);
}


Dsc2Img::Dsc2Img()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("max-width,w", value<unsigned>()->default_value(1024), "max output image width")
        ("font-file,f", value<std::string>()->required(), "input dsc file")
        ("output-file,o", value<std::string>(), "output file")
    ;
    posDesc_.add("font-file", 1);
}

int Dsc2Img::doExecute()
{
    RenderFontCommand::Parameters params;
    params.font_file = vm_["font-file"].as<std::string>();
    params.max_width = vm_["max-width"].as<unsigned>();
    if (vm_["output-file"].empty()) {
        boost::filesystem::path input_path(params.font_file);
        params.output_file = input_path.replace_extension(".png").filename().string();
    } else {
        params.output_file = vm_["output-file"].as<std::string>();
    }

    RenderFontCommand cmd;
    cmd.execute(params);

    return 0;
}
