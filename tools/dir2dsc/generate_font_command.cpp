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

#include "generate_font_command.hpp"
#include <boost/throw_exception.hpp>
#include <boost/filesystem.hpp>
#include <kgascii/image_dir_font_loader.hpp>
#include <kgascii/font_io.hpp>
#include <kgutil/image_io.hpp>

using namespace KG::Ascii;
using namespace KG::Util;


GenerateFontCommand::GenerateFontCommand()
{
}

void GenerateFontCommand::execute(const Parameters& params)
{
    if (!boost::filesystem::exists(params.image_path)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("image path does not exist"));
    }
    if (!boost::filesystem::is_directory(params.image_path)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("image path is not a directory"));
    }

    ImageDirectoryFontLoader loader(params.image_path);

    Font<> font;
    if (!load(font, loader)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("font loading error"));
    }

    font.save(params.output_filename);
}
