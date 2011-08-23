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

#include "render_text_command.hpp"
#include <boost/throw_exception.hpp>
#include <kgascii/font_io.hpp>
#include <kgutil/image_io.hpp>

using namespace KG::Ascii;
using namespace KG::Util;


RenderTextCommand::RenderTextCommand(std::ostream& ostr)
    :log_(ostr)
{
}

void RenderTextCommand::execute(const Parameters& params)
{
    Font<> font;
    if (!font.load(params.font_file)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("font loading error"));
    }

    std::ifstream fin(params.input_file.c_str());
    if (!fin) {
        BOOST_THROW_EXCEPTION(std::runtime_error("input file loading error"));
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

    size_t image_width = line_length * font.glyphWidth();
    size_t image_height = input_lines.size() * font.glyphHeight();

    log_ << "image_width = " << image_width << "\n";
    log_ << "image_height = " << image_height << "\n";

    boost::gil::gray8_image_t output_image(image_width, image_height);

    for (size_t rr = 0; rr < input_lines.size(); ++rr) {
        std::string line = input_lines[rr];
        for (size_t cc = 0; cc < line_length; ++cc) {
            const boost::gil::gray8c_view_t& glyph_surface = font.getGlyph(Symbol(line[cc]));
            unsigned colX = cc * font.glyphWidth();
            unsigned rowY = rr * font.glyphHeight();
            copy_pixels(glyph_surface, 
                subimage_view(view(output_image), 
                    colX, rowY, font.glyphWidth(), font.glyphHeight()));
        }
    }

    if (!saveImageView(params.output_file, const_view(output_image))) {
        BOOST_THROW_EXCEPTION(std::runtime_error("unknown output file format"));
    }
}
