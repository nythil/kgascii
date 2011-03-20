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
#include <sstream>
#include <boost/filesystem.hpp>
#include <kgascii/fontimage.hpp>
#include <kgascii/fontloader.hpp>
#include <common/cmdlinetool.hpp>


class ExtractFont: public CmdlineTool
{
public:
    ExtractFont();

protected:
    bool processArgs();

    int doExecute();
    
private:
    int minChar_;
    int maxChar_;
    std::string strHint_;
    KG::Ascii::FontLoader::Hinting hint_;
    std::string strAutohint_;
    KG::Ascii::FontLoader::AutoHinter autohint_;
    std::string strMode_;
    KG::Ascii::FontLoader::RenderMode mode_;
    std::string fontFile_;
    int fontSize_;
    std::string outputFile_;
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
        ("hint,h", value(&strHint_)->default_value("normal"), "hinting algorithm (normal|light|off)")
        ("autohint,a", value(&strAutohint_)->default_value("on"), "auto-hinter options (on|force|off)")
        ("mode,m", value(&strMode_)->default_value("gray"), "rendering mode (gray|mono)")
        ("min-char,f", value(&minChar_)->default_value(32), "first charcode")
        ("max-char,l", value(&maxChar_)->default_value(126), "last charcode")
        ("output-file,o", value(&outputFile_), "output file")
        ("input-file,i", value(&fontFile_), "input font file")
        ("pixel-size,s", value(&fontSize_), "font nominal pixel size")
    ;
    posDesc_.add("input-file", 1);
    posDesc_.add("pixel-size", 1);
}

bool ExtractFont::processArgs()
{
    using namespace KG::Ascii;
    
    requireOption("input-file");
    requireOption("pixel-size");

    if (strHint_ == "normal")
        hint_ = FontLoader::HintingNormal;
    else if (strHint_ == "light")
        hint_ = FontLoader::HintingLight;
    else if (strHint_ == "off")
        hint_ = FontLoader::HintingOff;
    else
        throw std::logic_error("bad option");

    if (strAutohint_ == "on")
        autohint_ = FontLoader::AutoHinterOn;
    else if (strAutohint_ == "off")
        autohint_ = FontLoader::AutoHinterOff;
    else if (strAutohint_ == "force")
        autohint_ = FontLoader::AutoHinterForce;
    else
        throw std::logic_error("bad option");

    if (strMode_ == "gray")
        mode_ = FontLoader::RenderGrayscale;
    else if (strMode_ == "mono")
        mode_ = FontLoader::RenderMonochrome;
    else
        throw std::logic_error("bad option");

    if (!vm_.count("output-file")) {
        boost::filesystem::path input_path(fontFile_);
        std::ostringstream path_builder;
        path_builder << input_path.stem().string();
        path_builder << "_";
        path_builder << fontSize_;
        path_builder << ".dsc";
        outputFile_ = path_builder.str();
    }

    return true;
}

int ExtractFont::doExecute()
{
    using namespace KG::Ascii;

    FontLoader loader;
    if (!loader.loadFont(fontFile_, fontSize_)) {
        std::cout << "font not found\n";
        return -1;
    }
    loader.setAutohinter(autohint_);
    loader.setHinting(hint_);
    loader.setRenderMode(mode_);

    FontImage image;
    if (!image.load(loader, minChar_, maxChar_)) {
        std::cout << "loading error\n";
        return -1;
    }

    image.save(outputFile_);

    return 0;
}
