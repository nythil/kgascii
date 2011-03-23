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

#include <kgascii/pca_reconstruction_font_image_loader.hpp>
#include <kgascii/font_pca.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/surface_algorithm.hpp>

namespace KG { namespace Ascii {

PcaReconstructionFontImageLoader::PcaReconstructionFontImageLoader(const FontPCA& pca)
    :pca_(pca)
    ,charcodes_(pca_.font().charcodes())
{
    glyphData_.resize(pca_.font().glyphWidth(), pca_.font().glyphHeight());
}

std::string PcaReconstructionFontImageLoader::familyName() const
{
    return pca_.font().familyName();
}

std::string PcaReconstructionFontImageLoader::styleName() const
{
    return pca_.font().styleName();
}        

unsigned PcaReconstructionFontImageLoader::pixelSize() const
{
    return pca_.font().pixelSize();
}

unsigned PcaReconstructionFontImageLoader::glyphWidth() const
{
    return pca_.font().glyphWidth();
}

unsigned PcaReconstructionFontImageLoader::glyphHeight() const
{
    return pca_.font().glyphHeight();
}

std::vector<unsigned> PcaReconstructionFontImageLoader::charcodes() const
{
    return charcodes_;
}

bool PcaReconstructionFontImageLoader::loadGlyph(unsigned charcode)
{
    std::vector<unsigned>::iterator it = std::find(charcodes_.begin(), charcodes_.end(), charcode);
    if (it == charcodes_.end())
        return false;
    size_t it_idx = std::distance(charcodes_.begin(), it);
    Eigen::VectorXf proj_glyph_vec = pca_.glyphs().col(it_idx);
    Eigen::VectorXf glyph_vec = pca_.combine(proj_glyph_vec);
    glyph_vec = glyph_vec.cwiseMax(Eigen::VectorXf::Zero(glyph_vec.size()));
    glyph_vec = glyph_vec.cwiseMin(255 * Eigen::VectorXf::Ones(glyph_vec.size()));
    copyPixels(glyph_vec, glyphData_.surface());
    return true;
}

Surface8c PcaReconstructionFontImageLoader::glyph() const
{
    return glyphData_.surface();
}

} } // namespace KG::Ascii


