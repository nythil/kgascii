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
#include <common/cmdline_tool.hpp>
#include <common/validate_optional.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/font_io.hpp>
#include <kgascii/dynamic_asciifier.hpp>
#include <kgascii/text_surface.hpp>
#include <kgascii/glyph_matcher_context_factory.hpp>
#include <kgascii/srgb.hpp>
#include <kgascii/image_io.hpp>
#include <kgutil/resample.hpp>
#include <kgutil/resample/filter/bspline.hpp>

using namespace KG::Ascii;
using namespace KG::Util;

typedef Font<> FontT;

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
    std::string algorithm_;
    unsigned threadCount_;
    bool gamma_;
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
        ("cols,c", value(&maxCols_)->default_value(79), "suggested number of text columns")
        ("rows,r", value(&maxRows_)->default_value(49), "suggested number of text rows")
        ("output-file,o", value(&outputFile_), "output text file")
        ("algorithm,a", value(&algorithm_)->default_value("pca"), "glyph matching algorithm")
        ("threads", value(&threadCount_)->default_value(0), "worker thread count")
        ("gamma", value<bool>()->zero_tokens(), "use gamma correction")
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

    gamma_ = vm_.count("gamma") > 0;

    return true;
}

template<class ImageT>
class ConverterImpl
{
    typedef FontImage<FontT, ImageT> FontImageT;
    typedef DynamicGlyphMatcher<FontImageT> DynamicGlyphMatcherT;
    typedef DynamicAsciifier<DynamicGlyphMatcherT> DynamicAsciifierT;

public:
    explicit ConverterImpl(boost::shared_ptr<const FontT> font, const std::string& algo, size_t threads)
    {
        registerGlyphMatcherFactories<FontImageT>();
        fontImage_.reset(new FontImageT(font));
        matcher_ = GlyphMatcherFactory::create(fontImage_, algo);
        asciifier_.reset(new DynamicAsciifierT(matcher_));
        if (threads == 1) {
            asciifier_->setSequential();
        } else {
            asciifier_->setParallel(threads);
        }
    }

    template<class TView>
    void generate(const TView& view, TextSurface& text)
    {
        ImageT temp_image(view.width(), view.height());
        boost::gil::copy_and_convert_pixels(view, boost::gil::view(temp_image));
        asciifier_->generate(boost::gil::const_view(temp_image), text);
    }

private:
    boost::shared_ptr<FontImageT> fontImage_;
    boost::shared_ptr<DynamicGlyphMatcherT> matcher_;
    boost::shared_ptr<DynamicAsciifierT> asciifier_;
};

int ImageToAscii::doExecute()
{
    std::cerr << "loading font...\n";
    boost::shared_ptr<FontT> font(new FontT);
    if (!font->load(fontFile_))
        return -1;
    unsigned char_width = font->glyphWidth();
    unsigned char_height = font->glyphHeight();

    std::cerr << "loading image...\n";
    typedef boost::mpl::vector<
        boost::gil::gray8_image_t,
        boost::gil::gray16_image_t,
        boost::gil::rgb8_image_t,
        boost::gil::rgb16_image_t,
        boost::gil::rgba8_image_t,
        boost::gil::rgba16_image_t
    >::type input_image_types;
    boost::gil::any_image<input_image_types> loaded_image;
    if (!loadImage(inputFile_, loaded_image))
        return -1;

    unsigned frame_width = loaded_image.width();
    unsigned frame_height = loaded_image.height();

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

    std::cout << "image width " << frame_width << "\n";
    std::cout << "image height " << frame_height << "\n";
    std::cout << "output width " << out_width << "\n";
    std::cout << "output height " << out_height << "\n";
    std::cout << "output columns " << col_count << "\n";
    std::cout << "output rows " << row_count << "\n";

    boost::gil::rgb_lin16_image_t input_image(frame_width, frame_height);
    boost::gil::copy_and_convert_pixels(const_view(loaded_image), view(input_image));

    boost::gil::rgb_lin16_image_t scaled_image(out_width, out_height);
    resample(const_view(input_image), view(scaled_image), Filter::BSplineFilter<>());

    boost::gil::gray8_image_t grayscale_image(out_width, out_height);
    boost::gil::copy_and_convert_pixels(const_view(scaled_image), view(grayscale_image));

    TextSurface text(row_count, col_count);
    if (gamma_) {
        ConverterImpl<boost::gil::gray_lin16_image_t> converter(font, algorithm_, threadCount_);
        converter.generate(const_view(grayscale_image), text);
    } else {
        ConverterImpl<boost::gil::gray8_image_t> converter(font, algorithm_, threadCount_);
        converter.generate(const_view(grayscale_image), text);
    }

    std::ofstream fout(outputFile_.c_str());
    for (size_t r = 0; r < text.rows(); ++r) {
        for (size_t c = 0; c < text.cols(); ++c)
            fout.put(text(r, c).charValue());
        fout.put('\n');
    }
    fout.close();

    return 0;
}

