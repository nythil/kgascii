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
#include <fstream>
#include <limits>
#include <cmath>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <common/cmdline_tool.hpp>
#include <common/validate_optional.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/sequential_asciifier.hpp>
#include <kgascii/text_surface.hpp>
#include <kgascii/glyph_matcher_context_factory.hpp>

using std::cout;

class ImageToAscii: public CmdlineTool
{
public:
    ImageToAscii();

protected:
    bool processArgs();

    int doExecute();
    
private:
    std::string inputFile_;
    std::string outputFile_;
    std::string fontFile_;
    unsigned maxCols_;
    unsigned maxRows_;
    unsigned nfeatures_;
    std::string algorithm_;
};

int main(int argc, char* argv[])
{
    return ImageToAscii().execute(argc, argv);
}


ImageToAscii::ImageToAscii()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-file,i", value(&inputFile_), "input image file")
        ("font-file,f", value(&fontFile_), "font file")
        ("cols", value(&maxCols_)->default_value(79), "suggested number of text columns")
        ("rows", value(&maxRows_)->default_value(49), "suggested number of text rows")
        ("nfeatures", value(&nfeatures_)->default_value(15), "number of pca features to extract")
        ("output-file,o", value(&outputFile_), "output text file")
        ("algorithm,a", value(&algorithm_)->default_value("pca"), "glyph matching algorithm")
    ;
    posDesc_.add("input-file", 1);
}

bool ImageToAscii::processArgs()
{
    requireOption("input-file");
    requireOption("font-file");
    requireOption("algorithm");
    
    if (!vm_.count("output-file")) {
        boost::filesystem::path input_path(inputFile_);
        outputFile_ = input_path.stem().string() + ".txt";
    }

    return true;
}


int ImageToAscii::doExecute()
{
    KG::Ascii::FontImage font;
    if (!font.load(fontFile_))
        return -1;
    unsigned char_width = font.glyphWidth();
    unsigned char_height = font.glyphHeight();

    cv::Mat capture_frame = cv::imread(inputFile_);
    if (capture_frame.empty())
        return -1;

    unsigned frame_width = static_cast<unsigned>(capture_frame.cols);
    unsigned frame_height = static_cast<unsigned>(capture_frame.rows);

    unsigned hint_width = maxCols_ * char_width;
    unsigned hint_height = maxRows_ * char_height;
    unsigned out_width, out_height;
    if (hint_width * frame_height / frame_width < hint_height) {
        out_width = hint_width;
        out_height = out_width * frame_height / frame_width;
    } else {
        out_height = hint_height;
        out_width = out_height * frame_width / frame_height;
    }

    unsigned col_count = (out_width + char_width - 1) / char_width;
    unsigned row_count = (out_height + char_height - 1) / char_height;

    KG::Ascii::TextSurface text(row_count, col_count);
    KG::Ascii::GlyphMatcherContextFactory matcher_ctx_factory;
    KG::Ascii::GlyphMatcherContext* matcher_ctx = matcher_ctx_factory.create(&font, algorithm_);
    KG::Ascii::SequentialAsciifier asciifier(matcher_ctx);

    cout << "image width " << frame_width << "\n";
    cout << "image height " << frame_height << "\n";
    cout << "output width " << out_width << "\n";
    cout << "output height " << out_height << "\n";
    cout << "output columns " << col_count << "\n";
    cout << "output rows " << row_count << "\n";

    cv::Mat scaled_frame;
    if (frame_width == out_width && frame_height == out_height) {
        scaled_frame = capture_frame;
    } else {
        cv::resize(capture_frame, scaled_frame, cv::Size(out_width, out_height));
    }

    //cv::GaussianBlur(scaled_frame, scaled_frame, cv::Size(7,7), 1.5, 1.5);

    cv::Mat gray_frame;
    cv::cvtColor(scaled_frame, gray_frame, CV_BGR2GRAY);
    //cv::equalizeHist(gray_frame, gray_frame);

    assert(gray_frame.dims == 2);
    assert(static_cast<unsigned>(gray_frame.cols) == out_width);
    assert(static_cast<unsigned>(gray_frame.rows) == out_height);
    assert(gray_frame.type() == CV_8UC1);

    KG::Ascii::Surface8c gray_surface(out_width, out_height, 
            gray_frame.data, gray_frame.step[0]);

    text.clear();
    asciifier.generate(gray_surface, text);

    std::ofstream fout(outputFile_.c_str());
    for (size_t r = 0; r < text.rows(); ++r) {
        fout.write(text.row(r), text.cols());
        fout.put('\n');
    }
    fout.close();

    return 0;
}

