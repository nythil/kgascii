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
#include <algorithm>
#include <iostream>
#include <sstream>
#include <kgascii/fontimage.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bgil = boost::gil;
using namespace KG::Ascii;

int main(int argc, char* argv[])
{
    int max_width;
    std::string input_file;
    std::string output_file;

    bpo::options_description opt_desc("Options");
    opt_desc.add_options()
        ("help", "help message")
        ("max-width,w", bpo::value(&max_width)->default_value(1024), "max output image width")
        ("input-file,i", bpo::value(&input_file), "input dsc file")
        ("output-file,o", bpo::value(&output_file), "output file")
    ;

    bpo::positional_options_description pos_opt_desc;
    pos_opt_desc.add("input-file", 1);

    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).
               options(opt_desc).positional(pos_opt_desc).run(), vm);
    bpo::notify(vm);

    if (vm.count("help") || !vm.count("input-file")) {
        std::cout << "Usage: dsc2img [options] input_file\n";
        std::cout << opt_desc;
        return 0;
    }

    bfs::path dsc_file_path(input_file);

    bfs::path output_image_path;
    if (vm.count("output-file")) {
        output_image_path = output_file;
    } else {
        output_image_path = input_file;
        output_image_path.replace_extension(".png");
    }

    FontImage image;
    if (!image.load(dsc_file_path.string())) {
        std::cout << "loading error\n";
        return -1;
    }

    int max_chars_per_row = max_width / image.glyphWidth();
    assert(max_chars_per_row > 1);

    std::vector<int> charcodes = image.charcodes();
    int row_count = (charcodes.size() + max_chars_per_row - 1) / max_chars_per_row;

    int image_width = std::min<int>(max_chars_per_row, charcodes.size()) * image.glyphWidth();
    int image_height = row_count * image.glyphHeight();

    cv::Mat output_image(image_height, image_width, CV_8UC1);
    bgil::gray8_view_t output_view = 
        bgil::interleaved_view(image_width, image_height, 
            reinterpret_cast<bgil::gray8_ptr_t>(output_image.data), output_image.step[0]);

    for (size_t ci = 0; ci < charcodes.size(); ++ci) {
        int charcode = charcodes[ci];
        bgil::gray8c_view_t glyph_view = image.getGlyph(charcode);
        int row = ci % max_chars_per_row;
        int rowX = row * image.glyphWidth();
        int col = ci / max_chars_per_row;
        int colY = col * image.glyphHeight();
        bgil::gray8_view_t output_cell = 
            bgil::subimage_view(output_view, rowX, colY, image.glyphWidth(), image.glyphHeight());
        bgil::copy_pixels(glyph_view, output_cell);
    }

    cv::imwrite(output_image_path.string(), output_image);

    return 0;
}

