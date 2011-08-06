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

#if 0
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/font_io.hpp>
#include <kgascii/font_pca.hpp>
#include <kgascii/pca_reconstruction_font_loader.hpp>
#include <common/cmdline_tool.hpp>

using namespace KG::Ascii;

class PcaDump: public CmdlineTool
{
public:
    PcaDump();

protected:
    bool processArgs();

    int doExecute();
    
private:
    void dumpFeatures(const FontPCA<PixelType8>& pca);
    void dumpDsc(const FontPCA<PixelType8>& pca);

private:
    std::string inputFile_;
    unsigned featureCnt_;
    std::string outputDsc_;
    std::string outputFeatures_;
};


int main(int argc, char* argv[])
{
    return PcaDump().execute(argc, argv);
}


PcaDump::PcaDump()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-file,i", value(&inputFile_), "input dsc file")
        ("nfeatures,n", value(&featureCnt_), "number of features to extract")
        ("output-dsc", value(&outputDsc_), "output reconstructed dsc file")
        ("output-features", value(&outputFeatures_), "output extracted feature masks")
    ;
    posDesc_.add("input-file", 1);
    posDesc_.add("nfeatures", 1);
}

bool PcaDump::processArgs()
{
    requireOption("input-file");
    requireOption("nfeatures");

    return true;
}

int PcaDump::doExecute()
{
    Font font;
    if (!font.load(inputFile_)) {
        std::cout << "loading error\n";
        return -1;
    }
    FontImage<PixelType8> image(&font);

    FontPCAnalyzer<PixelType8> pcanalyzer(&image);
    FontPCA<PixelType8> pca(&pcanalyzer, featureCnt_);

    std::cout << "features: " << featureCnt_ << "\n";
    std::cout << "energy: " << pca.energies().sum() / pcanalyzer.energies().sum() << "\n";

    dumpFeatures(pca);
    dumpDsc(pca);

    return 0;
}

void PcaDump::dumpFeatures(const FontPCA<PixelType8>& pca)
{
    if (outputFeatures_.empty())
        return;

    size_t glyph_w = pca.font()->glyphWidth();
    size_t glyph_h = pca.font()->glyphHeight();

    size_t image_width = glyph_w * 2;
    size_t image_height = glyph_h * (pca.featureCount() + 1);

    cv::Mat output_image(image_height, image_width, CV_8UC1);
    Surface8 output_surface(image_width, image_height, output_image.data, output_image.step[0]);
    
    fillPixels(output_surface, 0);

    copyPixels(pca.mean(), output_surface.window(0, 0, glyph_w, glyph_h));

    for (size_t i = 0; i < pca.featureCount(); ++i) {
        Eigen::VectorXf feat = pca.features().col(i);
        float fc = 3.0f / sqrt(static_cast<float>(feat.size()));
        Eigen::VectorXf feat_plus = feat.cwiseMax(Eigen::VectorXf::Zero(feat.size())) / fc;
        feat_plus = feat_plus.cwiseMin(Eigen::VectorXf::Ones(feat.size()));
        Eigen::VectorXf feat_minus = -feat.cwiseMin(Eigen::VectorXf::Zero(feat.size())) / fc;
        feat_minus = feat_minus.cwiseMin(Eigen::VectorXf::Ones(feat.size()));
        copyPixels(255 * feat_plus, output_surface.window(0, (i + 1) * glyph_h, glyph_w, glyph_h));
        copyPixels(255 * feat_minus, output_surface.window(glyph_w, (i + 1) * glyph_h, glyph_w, glyph_h));
    }

    cv::imwrite(outputFeatures_, output_image);
}

void PcaDump::dumpDsc(const FontPCA<PixelType8>& pca)
{
    if (outputDsc_.empty())
        return;

    PcaReconstructionFontLoader pca_loader(&pca);
    Font restored_font;
    load(restored_font, pca_loader);
    restored_font.save(outputDsc_);
}
#else
int main(int argc, char* argv[])
{
    return 0;
}
#endif
