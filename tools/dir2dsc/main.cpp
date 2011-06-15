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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/ft2_font_loader.hpp>
#include <kgascii/ft2_font_image_loader.hpp>
#include <common/cmdline_tool.hpp>


class GenerateFont: public CmdlineTool
{
public:
    GenerateFont();

protected:
    bool processArgs();

    int doExecute();
    
private:
    std::string imageDir_;
    std::string outputFile_;
};

int main(int argc, char* argv[])
{
    return GenerateFont().execute(argc, argv);
}

GenerateFont::GenerateFont()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-dir,i", value(&imageDir_), "input image directory")
        ("output-file,o", value(&outputFile_), "output file")
    ;
    posDesc_.add("input-dir", 1);
    posDesc_.add("output-file", 1);
}

bool GenerateFont::processArgs()
{
    using namespace KG::Ascii;
    
    requireOption("input-dir");

    if (!vm_.count("output-file")) {
        boost::filesystem::path input_path(imageDir_);
        std::ostringstream path_builder;
        path_builder << input_path.stem().string();
        path_builder << ".dsc";
        outputFile_ = path_builder.str();
    }

    return true;
}

class FromImageFontImageLoader: public KG::Ascii::FontImageLoader
{
public:
    explicit FromImageFontImageLoader()
    {
    }

public:
    std::string familyName() const
    {
        return imageDir_.stem().string();
    }

    std::string styleName() const
    {
        return "plain";
    }

    unsigned pixelSize() const
    {
        return glyphWidth_;
    }

    unsigned glyphWidth() const
    {
        return glyphWidth_;
    }

    unsigned glyphHeight() const
    {
        return glyphHeight_;
    }

    std::vector<KG::Ascii::Symbol> charcodes() const
    {
        return charcodes_;
    }

    bool loadGlyph(KG::Ascii::Symbol charcode)
    {
        std::vector<KG::Ascii::Symbol>::iterator it = std::find(
                charcodes_.begin(), charcodes_.end(), charcode);
        if (it == charcodes_.end())
            return false;

        loadedIndex_ = std::distance(charcodes_.begin(), it);

        return true;
    }

    KG::Ascii::Surface8c glyph() const
    {
        return glyphs_[loadedIndex_].surface();
    }

public:
    bool readDir(const boost::filesystem::path& imgdir_path)
    {
        charcodes_.clear();
        glyphs_.clear();

        size_t img_cnt = 0;
        unsigned font_width = 0, font_height = 0;
        boost::filesystem::directory_iterator dir_it(imgdir_path);
        boost::filesystem::directory_iterator dir_end;
        for (; dir_it != dir_end; ++dir_it) {
            cv::Mat glyph_image = cv::imread(dir_it->path().string(), 0);
            if (glyph_image.empty())
                BOOST_THROW_EXCEPTION(std::runtime_error("cv::imread"));

            cv::Mat gray_glyph_image;
//            cv::cvtColor(glyph_image, gray_glyph_image, CV_BGR2GRAY);
//            assert(gray_glyph_image.type() == CV_8UC1);
            gray_glyph_image = glyph_image;

            if (gray_glyph_image.dims != 2)
                BOOST_THROW_EXCEPTION(std::runtime_error("dims != 2"));
            if (gray_glyph_image.type() != CV_8UC1)
                BOOST_THROW_EXCEPTION(std::runtime_error("type != CV_8UC1"));
            if (img_cnt == 0) {
                font_width = gray_glyph_image.cols;
                font_height = gray_glyph_image.rows;
            } else {
                if (static_cast<unsigned>(gray_glyph_image.cols) != font_width)
                    BOOST_THROW_EXCEPTION(std::runtime_error("cols != font_width"));
                if (static_cast<unsigned>(gray_glyph_image.rows) != font_height)
                    BOOST_THROW_EXCEPTION(std::runtime_error("rows != font_height"));
            }

            KG::Ascii::Surface8c glyph_image_surface(font_width, font_height,
                    gray_glyph_image.data, gray_glyph_image.step[0]);

            KG::Ascii::SurfaceContainer8 glyph_container(font_width, font_height);
            KG::Ascii::copyPixels(glyph_image_surface, glyph_container.surface());

            glyphs_.push_back(glyph_container);
            charcodes_.push_back(KG::Ascii::Symbol(32 + img_cnt));

            img_cnt++;
        }

        imageDir_ = imgdir_path;
        glyphWidth_ = font_width;
        glyphHeight_ = font_height;

        return true;
    }

private:
    boost::filesystem::path imageDir_;
    unsigned glyphWidth_;
    unsigned glyphHeight_;
    std::vector<KG::Ascii::SurfaceContainer8> glyphs_;
    std::vector<KG::Ascii::Symbol> charcodes_;
    size_t loadedIndex_;
};


int GenerateFont::doExecute()
{
    using namespace KG::Ascii;

    if (!boost::filesystem::exists(imageDir_)) {
        std::cout << "image dir does not exist\n";
        return -1;
    }
    if (!boost::filesystem::is_directory(imageDir_)) {
        std::cout << "image dir is not a directory\n";
        return -1;
    }
    FromImageFontImageLoader loader;
    if (!loader.readDir(imageDir_)) {
        std::cout << "font not found\n";
        return -1;
    }

    FontImage image;
    if (!image.load(loader, Symbol(32), Symbol(127))) {
        std::cout << "loading error\n";
        return -1;
    }

    image.save(outputFile_);

    return 0;
}

