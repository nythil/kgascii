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
#include "generate_font_command.hpp"


class GenerateFont: public CmdlineTool
{
public:
    GenerateFont();

protected:
    int doExecute();
};

int main(int argc, char* argv[])
{
    return GenerateFont().execute(argc, argv);
}

GenerateFont::GenerateFont()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("image-path,i", value<std::string>()->required(), "input image directory")
        ("output-file,o", value<std::string>(), "output font file")
    ;
    posDesc_.add("image-path", 1);
    posDesc_.add("output-file", 1);
}

int GenerateFont::doExecute()
{
    GenerateFontCommand::Parameters params;
    params.image_path = vm_["image-path"].as<std::string>();
    if (vm_["output-file"].empty()) {
        boost::filesystem::path input_path(params.image_path);
        params.output_filename = input_path.replace_extension(".dsc").filename().string();
    } else {
        params.output_filename = vm_["output-file"].as<std::string>();
    }

    GenerateFontCommand cmd;
    cmd.execute(params);

    return 0;
}
