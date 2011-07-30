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
#include <algorithm>
#include <vector>
#include <boost/bind.hpp>
#include <kgascii/ft2pp/library.hpp>
#include <kgascii/ft2pp/face.hpp>
#include <kgascii/ft2pp/util.hpp>
#include <common/cmdline_tool.hpp>

using namespace KG::Ascii::FT2pp;

class DumpFont: public CmdlineTool
{
public:
    DumpFont();

protected:
    bool processArgs();

    int doExecute();
    
private:
    void dumpFont(Library& ft_library, const std::string& fileName);

    void dumpFontMetrics(Face& ft_face, int pixel_size);

private:
    std::vector<std::string> fontFiles_;
};

int main(int argc, char* argv[])
{
    return DumpFont().execute(argc, argv);
}

DumpFont::DumpFont()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-file,i", value(&fontFiles_), "input font file")
    ;
    posDesc_.add("input-file", -1);
}

bool DumpFont::processArgs()
{
    requireOption("input-file");
    return true;
}

int DumpFont::doExecute()
{
    Library ft_library;
    std::for_each(fontFiles_.begin(), fontFiles_.end(),
        boost::bind(&DumpFont::dumpFont, this, boost::ref(ft_library), _1));

    return 0;
}

void DumpFont::dumpFont(Library& ft_library, const std::string& fileName)
{
    std::cout << "file: " << fileName << "\n";

    int face_idx = 0;
    int num_faces = 1;
    while (face_idx < num_faces) {
        Face ft_face(ft_library, fileName, face_idx);

        if (face_idx == 0) {
            num_faces = ft_face->num_faces;
            std::cout << "num_faces: " << num_faces << "\n\n";
        }

        std::cout << "face #" << ft_face->face_index << "\n";
        std::cout << "family name: " << ft_face->family_name << "\n";
        std::cout << "style name: " << ft_face->style_name << "\n";
        std::cout << "horizontal: " << (FT_HAS_HORIZONTAL(ft_face) != 0) << "\n";
        std::cout << "vertical: " << (FT_HAS_VERTICAL(ft_face) != 0) << "\n";
        std::cout << "hinter: " << ((ft_face->face_flags & FT_FACE_FLAG_HINTER) != 0) << "\n";
        std::cout << "scalable: " << (FT_IS_SCALABLE(ft_face) != 0) << "\n";
        std::cout << "fixed width: " << (FT_IS_FIXED_WIDTH(ft_face) != 0) << "\n";
        std::cout << "fixed sizes: " << (FT_HAS_FIXED_SIZES(ft_face) != 0) << "\n";
        if (FT_HAS_FIXED_SIZES(ft_face)) {
            for (int si = 0; si < ft_face->num_fixed_sizes; ++si) {
                FT_Bitmap_Size size = ft_face->available_sizes[si];
                std::cout << "  ";
                std::cout << "size " << si << ": ";
                std::cout << "w=" << size.width << " ";
                std::cout << "h=" << size.height << " ";
                std::cout << "x_ppem=" << from26Dot6(size.x_ppem) << " ";
                std::cout << "y_ppem=" << from26Dot6(size.y_ppem) << "\n";

                dumpFontMetrics(ft_face, size.y_ppem / 64);
            }
        }
        std::cout << "\n";

        face_idx++;
    }

    std::cout << "\n\n";
}

void DumpFont::dumpFontMetrics(Face& ft_face, int pixel_size)
{
    ft_face.setPixelSizes(pixel_size, pixel_size);

    FT_Fixed y_scale = ft_face->size->metrics.y_scale;
    FT_Fixed x_scale = ft_face->size->metrics.x_scale;

    std::cout << "    ";
    std::cout << "size.metrics.x_ppem = " << ft_face->size->metrics.x_ppem << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.y_ppem = " << ft_face->size->metrics.y_ppem << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.x_scale = " << from16Dot16(x_scale) << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.y_scale = " << from16Dot16(y_scale) << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.ascender = " << from26Dot6(ft_face->size->metrics.ascender) << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.descender = " << from26Dot6(ft_face->size->metrics.descender) << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.height = " << from26Dot6(ft_face->size->metrics.height) << "\n";
    std::cout << "    ";
    std::cout << "size.metrics.max_advance = " << from26Dot6(ft_face->size->metrics.max_advance) << "\n";

    std::cout << "    ";
    std::cout << "ascent = " << from26Dot6(FT_MulFix(ft_face->ascender, y_scale)) << "\n";
    std::cout << "    ";
    std::cout << "descent = " << from26Dot6(FT_MulFix(ft_face->descender, y_scale)) << "\n";
    std::cout << "    ";
    std::cout << "yMax = " << from26Dot6(FT_MulFix(ft_face->bbox.yMax, y_scale)) << "\n";
    std::cout << "    ";
    std::cout << "yMin = " << from26Dot6(FT_MulFix(ft_face->bbox.yMin, y_scale)) << "\n";
    std::cout << "    ";
    std::cout << "xMax = " << from26Dot6(FT_MulFix(ft_face->bbox.xMax, x_scale)) << "\n";
    std::cout << "    ";
    std::cout << "xMin = " << from26Dot6(FT_MulFix(ft_face->bbox.xMin, x_scale)) << "\n";
    std::cout << "    ";
    std::cout << "height = " << from26Dot6(FT_MulFix(ft_face->height, y_scale)) << "\n";
    std::cout << "    ";
    std::cout << "max_advance_width = " << from26Dot6(FT_MulFix(ft_face->max_advance_width, x_scale)) << "\n";
}


