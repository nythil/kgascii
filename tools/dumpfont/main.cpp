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

#include <string>
#include <vector>
#include <common/cmdline_tool.hpp>
#include "dump_font_command.hpp"


class DumpFont: public CmdlineTool
{
    typedef std::vector<std::string> StringVectorT;
public:
    DumpFont();

protected:
    bool processArgs();
    int doExecute();
};

int main(int argc, char* argv[])
{
    return DumpFont().execute(argc, argv);
}

DumpFont::DumpFont()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-file,i", value<StringVectorT>(), "input font file")
    ;
    posDesc_.add("input-file", -1);
}

bool DumpFont::processArgs()
{
    requireOption("input-file");
    return true;
}

int DumpFont::doExecute()
{
    DumpFontCommand::Parameters params;
    params.files = vm_["input-file"].as<StringVectorT>();

    DumpFontCommand cmd;
    cmd.execute(params);

    return 0;
}
