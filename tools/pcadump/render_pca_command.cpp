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

#include "render_pca_command.hpp"
#include <boost/throw_exception.hpp>
#include <kgascii/image_io.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/font_io.hpp>
#include <kgascii/font_pca.hpp>
#include <kgascii/pca_reconstruction_font_loader.hpp>

using namespace KG::Ascii;


RenderPcaCommand::RenderPcaCommand(std::ostream& ostr)
    :log_(ostr)
{
}

template<class TFontPrincipalComponents>
void dumpFeatures(boost::shared_ptr<TFontPrincipalComponents> pca, 
    const std::string& output_file)
{
    size_t glyph_w = pca->font()->glyphWidth();
    size_t glyph_h = pca->font()->glyphHeight();

    size_t image_width = glyph_w * 2;
    size_t image_height = glyph_h * (pca->featureCount() + 1);

    boost::gil::gray8_image_t output_image(image_width, image_height);
    
    boost::gil::fill_pixels(view(output_image), boost::gil::gray8_pixel_t());

    boost::gil::copy_and_convert_pixels(
            boost::gil::interleaved_view(
                    glyph_w, glyph_h,
                    reinterpret_cast<const boost::gil::gray32f_pixel_t*>(pca->mean().data()),
                    glyph_w * sizeof(float)),
            subimage_view(view(output_image), 0, 0, glyph_w, glyph_h));

    for (size_t i = 0; i < pca->featureCount(); ++i) {
        Eigen::VectorXf feat = pca->features().col(i);
        float fc = 3.0f / sqrt(static_cast<float>(feat.size()));
        Eigen::VectorXf feat_plus = feat.cwiseMax(Eigen::VectorXf::Zero(feat.size())) / fc;
        feat_plus = feat_plus.cwiseMin(Eigen::VectorXf::Ones(feat.size()));
        Eigen::VectorXf feat_minus = -feat.cwiseMin(Eigen::VectorXf::Zero(feat.size())) / fc;
        feat_minus = feat_minus.cwiseMin(Eigen::VectorXf::Ones(feat.size()));
        boost::gil::copy_and_convert_pixels(
                boost::gil::interleaved_view(
                        glyph_w, glyph_h,
                        reinterpret_cast<const boost::gil::gray32f_pixel_t*>(feat_plus.data()),
                        glyph_w * sizeof(float)),
                subimage_view(view(output_image), 0, (i + 1) * glyph_h, glyph_w, glyph_h));
        boost::gil::copy_and_convert_pixels(
                boost::gil::interleaved_view(
                        glyph_w, glyph_h,
                        reinterpret_cast<const boost::gil::gray32f_pixel_t*>(feat_minus.data()),
                        glyph_w * sizeof(float)),
                subimage_view(view(output_image), glyph_w, (i + 1) * glyph_h, glyph_w, glyph_h));
    }

    if (!saveImageView(output_file, const_view(output_image))) {
        BOOST_THROW_EXCEPTION(std::runtime_error("unknown output file format"));
    }
}

template<class TFontPrincipalComponents>
void dumpDsc(boost::shared_ptr<TFontPrincipalComponents> pca,
    const std::string& output_file)
{
    PcaReconstructionFontLoader<TFontPrincipalComponents> pca_loader(pca);
    Font<> restored_font;
    load(restored_font, pca_loader);
    restored_font.save(output_file);
}

void RenderPcaCommand::execute(const Parameters& params)
{
    typedef KG::Ascii::Font<> FontT;
    typedef KG::Ascii::FontImage<FontT> FontImageT;
    typedef KG::Ascii::FontEigendecomposition<FontImageT> FontEigendecompositionT;
    typedef KG::Ascii::FontPrincipalComponents<FontEigendecompositionT> FontPrincipalComponentsT;

    boost::shared_ptr<FontT> font(new FontT);
    if (!font->load(params.font_file)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("font loading error"));
    }
    boost::shared_ptr<FontImageT> image(new FontImageT(font));

    boost::shared_ptr<FontEigendecompositionT> decomposition(new FontEigendecompositionT(image));
    boost::shared_ptr<FontPrincipalComponentsT> pca(new FontPrincipalComponentsT(decomposition, params.feature_cnt));

    log_ << "features: " << pca->featureCount() << "\n";
    log_ << "energy: " << pca->energies().sum() / decomposition->energies().sum() << "\n";

    if (!params.features_file.empty()) {
        dumpFeatures(pca, params.features_file);
    }
    if (!params.reconstructed_font_file.empty()) {
        dumpDsc(pca, params.reconstructed_font_file);
    }
}
