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

#include "dump_font_command.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/iostreams/filter/line.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/compose.hpp>

using namespace KG::Ascii::FT2pp;
namespace io = boost::iostreams;


class prefixing_filter: public io::line_filter
{
public:
    explicit prefixing_filter(const string_type& pfx)
        :prefix_(pfx)
    {
    }

private:
    virtual string_type do_filter(const string_type& line)
    {
        return prefix_ + line;
    }

private:
    const string_type prefix_;
};


DumpFontCommand::DumpFontCommand()
{
}

void DumpFontCommand::execute(const Parameters& params)
{
    boost::for_each(params.files, 
        boost::bind(&DumpFontCommand::enumerateFaces, this, _1, boost::ref(std::cout)));
}
    
void DumpFontCommand::enumerateFaces(const std::string& fileName, std::ostream& out)
{
    out << "file: " << fileName << "\n";

    int face_idx = 0;
    int num_faces = 1;
    while (face_idx < num_faces) {
        Face ft_face(ftLibrary_, fileName, face_idx);

        if (face_idx == 0) {
            num_faces = ft_face->num_faces;
            out << "num_faces: " << num_faces << "\n\n";
        }

        out << "face #" << ft_face->face_index << "\n";
        out << "family name: " << ft_face->family_name << "\n";
        out << "style name: " << ft_face->style_name << "\n";
        out << "horizontal: " << (FT_HAS_HORIZONTAL(ft_face) != 0) << "\n";
        out << "vertical: " << (FT_HAS_VERTICAL(ft_face) != 0) << "\n";
        out << "hinter: " << ((ft_face->face_flags & FT_FACE_FLAG_HINTER) != 0) << "\n";
        out << "scalable: " << (FT_IS_SCALABLE(ft_face) != 0) << "\n";
        out << "fixed width: " << (FT_IS_FIXED_WIDTH(ft_face) != 0) << "\n";
        out << "fixed sizes: " << (FT_HAS_FIXED_SIZES(ft_face) != 0) << "\n";
        if (FT_HAS_FIXED_SIZES(ft_face)) {
            io::filtering_ostream out_indented(io::compose(prefixing_filter("  "), out));
            enumerateFaceSizes(ft_face, out_indented);
        }
        out << "\n";

        face_idx++;
    }

    out << "\n\n";
}

void DumpFontCommand::enumerateFaceSizes(KG::Ascii::FT2pp::Face& ft_face, std::ostream& out)
{
    for (int si = 0; si < ft_face->num_fixed_sizes; ++si) {
        FT_Bitmap_Size size = ft_face->available_sizes[si];
        out << "size " << si << ": ";
        out << "w=" << size.width << " ";
        out << "h=" << size.height << " ";
        out << "x_ppem=" << from26Dot6(size.x_ppem) << " ";
        out << "y_ppem=" << from26Dot6(size.y_ppem) << "\n";

        int ppem = size.y_ppem / 64;
        ft_face.setPixelSizes(ppem, ppem);

        io::filtering_ostream out_indented(io::compose(prefixing_filter("  "), out));
        printSizeMetrics(ft_face, out_indented);
    }
}

void DumpFontCommand::printSizeMetrics(const KG::Ascii::FT2pp::Face& ft_face, std::ostream& out)
{
    FT_Fixed y_scale = ft_face->size->metrics.y_scale;
    FT_Fixed x_scale = ft_face->size->metrics.x_scale;

    out << "size.metrics.x_ppem = " << ft_face->size->metrics.x_ppem << "\n";
    out << "size.metrics.y_ppem = " << ft_face->size->metrics.y_ppem << "\n";
    out << "size.metrics.x_scale = " << from16Dot16(x_scale) << "\n";
    out << "size.metrics.y_scale = " << from16Dot16(y_scale) << "\n";
    out << "size.metrics.ascender = " << from26Dot6(ft_face->size->metrics.ascender) << "\n";
    out << "size.metrics.descender = " << from26Dot6(ft_face->size->metrics.descender) << "\n";
    out << "size.metrics.height = " << from26Dot6(ft_face->size->metrics.height) << "\n";
    out << "size.metrics.max_advance = " << from26Dot6(ft_face->size->metrics.max_advance) << "\n";

    out << "ascent = " << from26Dot6(FT_MulFix(ft_face->ascender, y_scale)) << "\n";
    out << "descent = " << from26Dot6(FT_MulFix(ft_face->descender, y_scale)) << "\n";
    out << "yMax = " << from26Dot6(FT_MulFix(ft_face->bbox.yMax, y_scale)) << "\n";
    out << "yMin = " << from26Dot6(FT_MulFix(ft_face->bbox.yMin, y_scale)) << "\n";
    out << "xMax = " << from26Dot6(FT_MulFix(ft_face->bbox.xMax, x_scale)) << "\n";
    out << "xMin = " << from26Dot6(FT_MulFix(ft_face->bbox.xMin, x_scale)) << "\n";
    out << "height = " << from26Dot6(FT_MulFix(ft_face->height, y_scale)) << "\n";
    out << "max_advance_width = " << from26Dot6(FT_MulFix(ft_face->max_advance_width, x_scale)) << "\n";
}
