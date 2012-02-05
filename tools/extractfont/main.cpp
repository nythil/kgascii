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

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <common/cmdline_tool.hpp>
#include "extract_font_command.hpp"

using namespace KG::Ascii;

class ExtractFont: public CmdlineTool
{
public:
    ExtractFont();

protected:
    bool processArgs();
    int doExecute();
};

int main(int argc, char* argv[])
{
    return ExtractFont().execute(argc, argv);
}

ExtractFont::ExtractFont()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("hint,h", value<FT2FontLoader::Hinting>()->default_value(FT2FontLoader::Hinting::Normal), "hinting algorithm (normal|light|off)")
        ("autohint,a", value<FT2FontLoader::AutoHinter>()->default_value(FT2FontLoader::AutoHinter::On), "auto-hinter options (on|force|off)")
        ("mode,m", value<FT2FontLoader::RenderMode>()->default_value(FT2FontLoader::RenderMode::Grayscale), "rendering mode (gray|mono)")
        ("min-char,f", value<unsigned>()->default_value(32), "first charcode")
        ("max-char,l", value<unsigned>()->default_value(126), "last charcode")
        ("output-file,o", value<std::string>(), "output file")
        ("input-file,i", value<std::string>(), "input font file")
        ("pixel-size,s", value<unsigned>(), "font nominal pixel size")
    ;
    posDesc_.add("input-file", 1);
    posDesc_.add("pixel-size", 1);
}

bool ExtractFont::processArgs()
{
    requireOption("input-file");
    requireOption("pixel-size");
    return true;
}

int ExtractFont::doExecute()
{
    ExtractFontCommand::Parameters params;
    params.font_file = vm_["input-file"].as<std::string>();
    params.font_size = vm_["pixel-size"].as<unsigned>();
    params.autohint = vm_["autohint"].as<FT2FontLoader::AutoHinter>();
    params.hinting = vm_["hint"].as<FT2FontLoader::Hinting>();
    params.render_mode = vm_["mode"].as<FT2FontLoader::RenderMode>();
    params.min_char = Symbol(vm_["min-char"].as<unsigned>());
    params.max_char = Symbol(vm_["max-char"].as<unsigned>());
    if (vm_["output-file"].empty()) {
        boost::filesystem::path input_path(params.font_file);
        const std::string& basename = input_path.stem().string();
        params.output_file = str(boost::format("%1%_%2%.dsc") % basename % params.font_size);
    } else {
        params.output_file = vm_["output-file"].as<std::string>();
    }

    ExtractFontCommand cmd;
    cmd.execute(params);

    return 0;
}

