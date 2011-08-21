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

#include "extract_font_command.hpp"
#include <boost/throw_exception.hpp>
#include <kgascii/font_io.hpp>

using namespace KG::Ascii;


ExtractFontCommand::ExtractFontCommand()
{
}

void ExtractFontCommand::execute(const Parameters& params)
{
    FT2FontLoader loader;
    if (!loader.loadFont(params.font_file, params.font_size)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("font not found"));
    }

    if (params.autohint)
        loader.setAutohinter(params.autohint.get());
    if (params.hinting)
        loader.setHinting(params.hinting.get());
    if (params.render_mode)
        loader.setRenderMode(params.render_mode.get());

    Font<> font;
    if (params.min_char && params.max_char) {
        if (!load(font, loader, params.min_char.get(), params.max_char.get())) {
            BOOST_THROW_EXCEPTION(std::runtime_error("font loading error"));
        }
    } else if (!params.min_char && !params.max_char) {
        if (!load(font, loader)) {
            BOOST_THROW_EXCEPTION(std::runtime_error("font loading error"));
        }
    } else {
        BOOST_THROW_EXCEPTION(std::exception("not yet implemented"));
    }

    font.save(params.output_file);
}
