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
#include <boost/format.hpp>
#include <common/cmdline_tool.hpp>
#include "render_text_command.hpp"


class RenderText: public CmdlineTool
{
public:
    RenderText();

protected:
    int doExecute();
};


int main(int argc, char* argv[])
{
    return RenderText().execute(argc, argv);
}


RenderText::RenderText()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-file,i", value<std::string>()->required(), "input text file")
        ("font-file,f", value<std::string>()->required(), "font file")
        ("output-file,o", value<std::string>(), "output file")
    ;
    posDesc_.add("input-file", 1);
}

int RenderText::doExecute()
{
    RenderTextCommand::Parameters params;
    params.input_file = vm_["input-file"].as<std::string>();
    params.font_file = vm_["font-file"].as<std::string>();
    if (vm_["output-file"].empty()) {
        boost::filesystem::path input_path(params.input_file);
        const std::string& basename = input_path.stem().string();
        params.output_file = str(boost::format("%1%_out.pnm") % basename);
    } else {
        params.output_file = vm_["output-file"].as<std::string>();
    }

    RenderTextCommand cmd(std::clog);
    cmd.execute(params);

    return 0;
}

