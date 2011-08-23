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

#include "render_font_command.hpp"
#include <boost/throw_exception.hpp>
#include <kgascii/font_io.hpp>
#include <kgutil/image_io.hpp>

using namespace KG::Ascii;
using namespace KG::Util;


RenderFontCommand::RenderFontCommand()
{
}

void RenderFontCommand::execute(const Parameters& params)
{
    Font<> font;
    if (!font.load(params.font_file)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("font loading error"));
    }

    unsigned max_chars_per_row;
    if (params.max_width) {
        max_chars_per_row = params.max_width.get() / font.glyphWidth();
    }
    max_chars_per_row = std::max<unsigned>(max_chars_per_row, 1);

    size_t row_count = (font.glyphCount() + max_chars_per_row - 1) / max_chars_per_row;

    size_t image_width = std::min<size_t>(max_chars_per_row, font.glyphCount()) * font.glyphWidth();
    size_t image_height = row_count * font.glyphHeight();

    boost::gil::gray8_image_t output_image(image_width, image_height);

    for (size_t ci = 0; ci < font.glyphCount(); ++ci) {
        boost::gil::gray8c_view_t glyph_surface = font.getGlyph(ci);
        unsigned row = ci % max_chars_per_row;
        unsigned rowX = row * font.glyphWidth();
        unsigned col = ci / max_chars_per_row;
        unsigned colY = col * font.glyphHeight();
        copy_pixels(glyph_surface, 
            subimage_view(view(output_image), 
                rowX, colY, font.glyphWidth(), font.glyphHeight()));
    }

    if (!saveImageView(params.output_file, const_view(output_image))) {
        BOOST_THROW_EXCEPTION(std::runtime_error("unknown output file format"));
    }
}
