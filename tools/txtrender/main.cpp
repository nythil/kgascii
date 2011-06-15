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
#include <fstream>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/surface_algorithm.hpp>
#include <common/cmdline_tool.hpp>


class RenderText: public CmdlineTool
{
public:
    RenderText();

protected:
    bool processArgs();

    int doExecute();
    
private:
    std::string inputFile_;
    std::string fontFile_;
    std::string outputFile_;
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
        ("input-file,i", value(&inputFile_), "input text file")
        ("font-file,f", value(&fontFile_), "font file")
        ("output-file,o", value(&outputFile_), "output file")
    ;
    posDesc_.add("input-file", 1);
}

bool RenderText::processArgs()
{
    requireOption("input-file");
    requireOption("font-file");

    if (!vm_.count("output-file")) {
        boost::filesystem::path input_path(inputFile_);
        outputFile_ = input_path.stem().string() + "_out.png";
    }

    return true;
}

int RenderText::doExecute()
{
    using namespace KG::Ascii;
    
    FontImage image;
    if (!image.load(fontFile_)) {
        std::cerr << "font loading error\n";
        return -1;
    }

    std::ifstream fin(inputFile_.c_str());
    if (!fin) {
        std::cerr << "input file loading error\n";
        return -1;
    }

    std::vector<std::string> input_lines;
    size_t line_length = 0;
    while (fin) {
        std::string current_line;
        std::getline(fin, current_line);
        if (current_line.size() > 0) {
            if (input_lines.empty()) {
                line_length = current_line.size();
            } else if (current_line.size() != line_length) {
                BOOST_THROW_EXCEPTION(std::runtime_error("invalid line length"));
            }
            input_lines.push_back(current_line);
        }
    }

    size_t image_width = line_length * image.glyphWidth();
    size_t image_height = input_lines.size() * image.glyphHeight();

    std::cerr << "image_width = " << image_width << "\n";
    std::cerr << "image_height = " << image_height << "\n";

    cv::Mat output_image(image_height, image_width, CV_8UC1);
    Surface8 output_surface(image_width, image_height, output_image.data, output_image.step[0]);

    for (size_t rr = 0; rr < input_lines.size(); ++rr) {
        std::string line = input_lines[rr];
        for (size_t cc = 0; cc < line_length; ++cc) {
            Surface8c glyph_surface = image.glyphByCharcode(Symbol(line[cc]));
            unsigned colX = cc * image.glyphWidth();
            unsigned rowY = rr * image.glyphHeight();
            copyPixels(glyph_surface, output_surface.window(colX, rowY,
                    image.glyphWidth(), image.glyphHeight()));
        }
    }

    cv::imwrite(outputFile_, output_image);

    return 0;
}

