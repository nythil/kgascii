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
#include <boost/program_options.hpp>
#include <iostream>
#include <sstream>
#include <kgascii/fontimage.hpp>
#include <kgascii/fontloader.hpp>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
using namespace KG::Ascii;

int main(int argc, char* argv[])
{
    int min_char;
    int max_char;
    std::string str_hint;
    std::string str_autohint;
    std::string str_mode;
    std::string font_file;
    int font_size;
    std::string output_file;

    bpo::options_description opt_desc("Options");
    opt_desc.add_options()
        ("help", "help message")
        ("hint,h", bpo::value(&str_hint)->default_value("normal"), "hinting algorithm (normal|light|off)")
        ("autohint,a", bpo::value(&str_autohint)->default_value("on"), "auto-hinter options (on|force|off)")
        ("mode,m", bpo::value(&str_mode)->default_value("gray"), "rendering mode (gray|mono)")
        ("min-char,f", bpo::value(&min_char)->default_value(32), "first charcode")
        ("max-char,l", bpo::value(&max_char)->default_value(127), "last charcode")
        ("input-file,i", bpo::value(&font_file), "input font file")
        ("output-file,o", bpo::value(&output_file), "output file")
        ("pixel-size,s", bpo::value(&font_size), "font nominal pixel size")
    ;

    bpo::positional_options_description pos_opt_desc;
    pos_opt_desc.add("input-file", 1);
    pos_opt_desc.add("pixel-size", 1);

    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).
               options(opt_desc).positional(pos_opt_desc).run(), vm);
    bpo::notify(vm);

    if (vm.count("help") || !vm.count("input-file") || !vm.count("pixel-size")) {
        std::cout << "Usage: extractfont [options] input_file pixel_size\n";
        std::cout << opt_desc;
        return 0;
    }

    FontLoader::Hinting hint = FontLoader::HintingLight;
    if (str_hint == "normal")
        hint = FontLoader::HintingNormal;
    else if (str_hint == "light")
        hint = FontLoader::HintingLight;
    else if (str_hint == "off")
        hint = FontLoader::HintingOff;
    else
        throw std::logic_error("bad option");

    FontLoader::AutoHinter autohint = FontLoader::AutoHinterOn;
    if (str_autohint == "on")
        autohint = FontLoader::AutoHinterOn;
    else if (str_autohint == "off")
        autohint = FontLoader::AutoHinterOff;
    else if (str_autohint == "force")
        autohint = FontLoader::AutoHinterForce;
    else
        throw std::logic_error("bad option");

    FontLoader::RenderMode mode = FontLoader::RenderGrayscale;
    if (str_mode == "gray")
        mode = FontLoader::RenderGrayscale;
    else if (str_mode == "mono")
        mode = FontLoader::RenderMonochrome;
    else
        throw std::logic_error("bad option");

    bfs::path font_path(font_file);

    bfs::path output_path;
    if (vm.count("output-file")) {
        output_path = output_file;
    } else {
        std::ostringstream path_builder;
        path_builder << font_path.stem().string();
        path_builder << "_";
        path_builder << font_size;
        path_builder << ".dsc";
        output_path = path_builder.str();
    }

    FontLoader loader;
    if (!loader.loadFont(font_path.string(), font_size)) {
        std::cout << "font not found\n";
        return -1;
    }
    loader.setAutohinter(autohint);
    loader.setHinting(hint);
    loader.setRenderMode(mode);

    FontImage image;
    if (!image.load(loader, min_char, max_char)) {
        std::cout << "loading error\n";
        return -1;
    }

    image.save(output_path.string());

    return 0;
}


