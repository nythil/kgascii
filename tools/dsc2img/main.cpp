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

#include <algorithm>
#include <iostream>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <kgascii/font.hpp>
#include <kgascii/font_io.hpp>
#include <kgascii/surface_algorithm.hpp>
#include <common/cmdline_tool.hpp>

using namespace KG::Ascii;

class Dsc2Img: public CmdlineTool
{
public:
    Dsc2Img();

protected:
    bool processArgs();

    int doExecute();
    
private:
    unsigned maxWidth_;
    std::string inputFile_;
    std::string outputFile_;
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
        ("max-width,w", value(&maxWidth_)->default_value(1024), "max output image width")
        ("input-file,i", value(&inputFile_), "input dsc file")
        ("output-file,o", value(&outputFile_), "output file")
    ;
    posDesc_.add("input-file", 1);
}

bool Dsc2Img::processArgs()
{
    requireOption("input-file");

    if (!vm_.count("output-file")) {
        boost::filesystem::path input_path(inputFile_);
        outputFile_ = input_path.stem().string() + ".png";
    }

    return true;
}

int Dsc2Img::doExecute()
{
    Font font;
    if (!font.load(inputFile_)) {
        std::cout << "loading error\n";
        return -1;
    }

    unsigned max_chars_per_row = maxWidth_ / font.glyphWidth();
    assert(max_chars_per_row > 1);

    size_t row_count = (font.glyphCount() + max_chars_per_row - 1) / max_chars_per_row;

    size_t image_width = std::min<size_t>(max_chars_per_row, font.glyphCount()) * font.glyphWidth();
    size_t image_height = row_count * font.glyphHeight();

    cv::Mat output_image(image_height, image_width, CV_8UC1);
    Surface8 output_surface(image_width, image_height, output_image.data, output_image.step[0]);

    for (size_t ci = 0; ci < font.glyphCount(); ++ci) {
        Surface8c glyph_surface = font.getGlyph(ci);
        unsigned row = ci % max_chars_per_row;
        unsigned rowX = row * font.glyphWidth();
        unsigned col = ci / max_chars_per_row;
        unsigned colY = col * font.glyphHeight();
        copyPixels(glyph_surface, output_surface.window(rowX, colY, 
                font.glyphWidth(), font.glyphHeight()));
    }

    cv::imwrite(outputFile_, output_image);

    return 0;
}

