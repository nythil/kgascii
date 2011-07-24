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

#ifndef KGASCII_FONT_PCA_HPP
#define KGASCII_FONT_PCA_HPP

#include <Eigen/Dense>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

template<typename TPixel>
class FontImage;
template<typename TPixel>
class FontPCAnalyzer;

template<typename TPixel>
class FontPCA
{
public:
    typedef FontImage<TPixel> FontImageT;

public:
    FontPCA(const FontPCAnalyzer<TPixel>* analyzer, size_t feat_cnt);

public:
    Eigen::VectorXf combine(const Eigen::VectorXf& vec) const
    {
        Eigen::VectorXf out;
        combine(vec, out);
        return out;
    }

    Eigen::VectorXf& combine(const Eigen::VectorXf& vec, Eigen::VectorXf& out) const
    {
        out = features_ * energies_.asDiagonal().inverse() * vec + mean_;
        return out;
    }

    Eigen::VectorXf project(const Eigen::VectorXf& vec) const
    {
        Eigen::VectorXf out;
        project(vec, out);
        return out;
    }

    Eigen::VectorXf& project(const Eigen::VectorXf& vec, Eigen::VectorXf& out) const
    {
        assert(vec.size() == features_.rows());
        out = (features_.transpose() * (vec - mean_)).cwiseProduct(energies_);
        return out;
    }

    size_t findClosestGlyph(const Eigen::VectorXf& vec) const
    {
        size_t min_index = 0;
        (glyphs_.colwise() - vec).colwise().squaredNorm().minCoeff(&min_index);
        return min_index;
    }

public:
    size_t featureCount() const
    {
        return energies_.size();
    }

    const FontImageT* font() const
    {
        return font_;
    }

    const Eigen::VectorXf& mean() const
    {
        return mean_;
    }

    const Eigen::VectorXf& energies() const
    {
        return energies_;
    }

    const Eigen::MatrixXf& features() const
    {
        return features_;
    }

    const Eigen::MatrixXf& glyphs() const
    {
        return glyphs_;
    }

private:
    const FontImageT* font_;
    Eigen::VectorXf mean_;
    Eigen::VectorXf energies_;
    Eigen::MatrixXf features_;
    Eigen::MatrixXf glyphs_;
};

} } // namespace KG::Ascii

#include <kgascii/font_pcanalyzer.hpp>
#include <kgascii/font_image.hpp>

namespace KG { namespace Ascii {

template<typename TPixel>
FontPCA<TPixel>::FontPCA(const FontPCAnalyzer<TPixel>* analyzer, size_t feat_cnt)
    :font_(analyzer->font())
{
    size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
    size_t samples_cnt = font_->glyphCount();

    mean_ = analyzer->mean().template cast<float>();
    assert(static_cast<size_t>(mean_.size()) == glyph_size);

    Eigen::VectorXd energies_dbl = analyzer->energies().head(feat_cnt);
    energies_dbl /= energies_dbl.sum();
    energies_dbl *= energies_dbl.size();
    energies_ = energies_dbl.template cast<float>();
    assert(static_cast<size_t>(energies_.size()) == feat_cnt);

    Eigen::MatrixXd features_dbl = analyzer->features().leftCols(feat_cnt);
    features_ = features_dbl.template cast<float>();
    assert(static_cast<size_t>(features_.rows()) == glyph_size);
    assert(static_cast<size_t>(features_.cols()) == feat_cnt);

    Eigen::MatrixXd glyphs_dbl = (features_dbl * energies_dbl.asDiagonal()).transpose() * analyzer->samples();
    glyphs_ = glyphs_dbl.template cast<float>();
    assert(static_cast<size_t>(glyphs_.cols()) == samples_cnt);
    assert(static_cast<size_t>(glyphs_.rows()) == feat_cnt);
}

} } // namespace KG::Ascii

#endif // KGASCII_FONT_PCA_HPP

