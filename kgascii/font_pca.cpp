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

#include <kgascii/font_pca.hpp>
#include <kgascii/font_pcanalyzer.hpp>
#include <kgascii/font_image.hpp>

namespace KG { namespace Ascii {

FontPCA::FontPCA(const FontPCAnalyzer* analyzer, size_t feat_cnt)
    :font_(analyzer->font())
{
    size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
    size_t samples_cnt = font_->glyphCount();

    mean_ = analyzer->mean().cast<float>();
    assert(static_cast<size_t>(mean_.size()) == glyph_size);

    Eigen::VectorXd energies_dbl = analyzer->energies().head(feat_cnt);
    energies_dbl /= energies_dbl.sum();
    energies_dbl *= energies_dbl.size();
    energies_ = energies_dbl.cast<float>();
    assert(static_cast<size_t>(energies_.size()) == feat_cnt);

    Eigen::MatrixXd features_dbl = analyzer->features().leftCols(feat_cnt);
    features_ = features_dbl.cast<float>();
    assert(static_cast<size_t>(features_.rows()) == glyph_size);
    assert(static_cast<size_t>(features_.cols()) == feat_cnt);

    Eigen::MatrixXd glyphs_dbl = (features_dbl * energies_dbl.asDiagonal()).transpose() * analyzer->samples();
    glyphs_ = glyphs_dbl.cast<float>();
    assert(static_cast<size_t>(glyphs_.cols()) == samples_cnt);
    assert(static_cast<size_t>(glyphs_.rows()) == feat_cnt);
}

Eigen::VectorXf FontPCA::combine(const Eigen::VectorXf& vec) const
{
    Eigen::VectorXf out;
    combine(vec, out);
    return out;
}

Eigen::VectorXf& FontPCA::combine(const Eigen::VectorXf& vec, Eigen::VectorXf& out) const
{
    out = features_ * energies_.asDiagonal().inverse() * vec + mean_;
    return out;
}

Eigen::VectorXf FontPCA::project(const Eigen::VectorXf& vec) const
{
    Eigen::VectorXf out;
    project(vec, out);
    return out;
}

Eigen::VectorXf& FontPCA::project(const Eigen::VectorXf& vec, Eigen::VectorXf& out) const
{
    assert(vec.size() == features_.rows());
    out = (features_.transpose() * (vec - mean_)).cwiseProduct(energies_);
    return out;
}

size_t FontPCA::findClosestGlyph(const Eigen::VectorXf& vec) const
{
    size_t min_index = 0;
    (glyphs_.colwise() - vec).colwise().squaredNorm().minCoeff(&min_index);
    return min_index;
}

size_t FontPCA::featureCount() const
{
    return energies_.size();
}

const FontImage* FontPCA::font() const
{
    return font_;
}

const Eigen::VectorXf& FontPCA::mean() const
{
    return mean_;
}

const Eigen::VectorXf& FontPCA::energies() const
{
    return energies_;
}

const Eigen::MatrixXf& FontPCA::features() const
{
    return features_;
}

const Eigen::MatrixXf& FontPCA::glyphs() const
{
    return glyphs_;
}

} } // namespace KG::Ascii

