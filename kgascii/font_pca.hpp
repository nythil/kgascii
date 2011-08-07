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

#include <fstream>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/gil/gil_all.hpp>
#include <Eigen/Dense>


namespace KG { namespace Ascii {

template<typename TPixel>
struct float_channel_type
{
    typedef boost::gil::bits32f type;
};

template<class TFontImage>
class FontPCA;
template<class TFontImage>
class FontPCAnalyzer;

template<class TFontImage>
class FontPCA
{
public:
    typedef TFontImage FontImageT;

public:
    FontPCA(const FontPCAnalyzer<FontImageT>* analyzer, size_t feat_cnt);

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

template<class TFontImage>
class FontPCAnalyzer
{
public:
    typedef TFontImage FontImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

public:
    explicit FontPCAnalyzer(const FontImageT* f)
        :font_(f)
    {
    }

    void analyze()
    {
        size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
        size_t samples_cnt = font_->glyphCount();

        typedef boost::gil::layout<
                typename boost::gil::color_space_type<ConstViewT>::type,
                typename boost::gil::channel_mapping_type<ConstViewT>::type
                > LayoutT;
        typedef typename float_channel_type<typename boost::gil::channel_type<ConstViewT>::type>::type FloatChannelT;
        typedef typename boost::gil::pixel_value_type<FloatChannelT, LayoutT>::type FloatPixelT;
        typedef typename boost::gil::type_from_x_iterator<FloatPixelT*>::view_t FloatViewT;

        Eigen::VectorXf tmp_glyph_data(glyph_size * boost::gil::num_channels<FloatPixelT>::value);
        FloatViewT tmp_glyph_view = boost::gil::interleaved_view(
                font_->glyphWidth(), font_->glyphHeight(),
                reinterpret_cast<FloatPixelT*>(tmp_glyph_data.data()),
                font_->glyphWidth() * sizeof(FloatPixelT));

        Eigen::MatrixXd input_samples(glyph_size, samples_cnt);
        for (size_t ci = 0; ci < samples_cnt; ++ci) {
            ConstViewT glyph_surface = font_->getGlyph(ci);
            boost::gil::copy_and_convert_pixels(glyph_surface, tmp_glyph_view);
            input_samples.col(ci) = tmp_glyph_data.template cast<double>();
        }

        mean_ = input_samples.rowwise().sum() / samples_cnt;
        assert(static_cast<size_t>(mean_.size()) == glyph_size);

        samples_ = input_samples.colwise() - mean_;
        assert(samples_.rows() == input_samples.rows());
        assert(samples_.cols() == input_samples.cols());

        Eigen::MatrixXd covariance(glyph_size, glyph_size);
        covariance.setZero();
        covariance.selfadjointView<Eigen::Lower>().rankUpdate(samples_, 1.0 / (samples_cnt - 1));
        assert(static_cast<size_t>(covariance.rows()) == glyph_size);
        assert(static_cast<size_t>(covariance.cols()) == glyph_size);

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(covariance);

        Eigen::VectorXd eigvals_tmp = eigen_solver.eigenvalues().reverse();
        energies_ = eigvals_tmp.cwiseMax(Eigen::VectorXd::Zero(glyph_size));
        assert(static_cast<size_t>(energies_.size()) == glyph_size);

        features_ = eigen_solver.eigenvectors().rowwise().reverse();
        assert(static_cast<size_t>(features_.rows()) == glyph_size);
        assert(static_cast<size_t>(features_.cols()) == glyph_size);
    }

    FontPCA<FontImageT> extract(size_t cnt) const
    {
        return FontPCA<FontImageT>(this, cnt);
    }

    bool saveToCache(const std::string& filename) const
    {
        size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
        size_t samples_cnt = font_->glyphCount();

        std::ofstream ofs(filename.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (!ofs)
            return false;

        boost::archive::binary_oarchive oa(ofs);

        using namespace boost::serialization;

        oa << BOOST_SERIALIZATION_NVP(glyph_size);
        oa << BOOST_SERIALIZATION_NVP(samples_cnt);
        oa << make_nvp("mean", make_array(mean_.data(), mean_.size()));
        oa << make_nvp("samples", make_array(samples_.data(), samples_.size()));
        oa << make_nvp("energies", make_array(energies_.data(), energies_.size()));
        oa << make_nvp("features", make_array(features_.data(), features_.size()));

        return true;
    }

    bool loadFromCache(const std::string& filename)
    {
        std::ifstream ifs(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!ifs)
            return false;

        boost::archive::binary_iarchive ia(ifs);

        using namespace boost::serialization;

        size_t glyph_size, samples_cnt;
        ia >> BOOST_SERIALIZATION_NVP(glyph_size);
        if (glyph_size != font_->glyphWidth() * font_->glyphHeight())
            return false;
        ia >> BOOST_SERIALIZATION_NVP(samples_cnt);
        if (samples_cnt != font_->glyphCount())
            return false;
        mean_.resize(glyph_size);
        ia >> make_nvp("mean", make_array(mean_.data(), mean_.size()));
        samples_.resize(glyph_size, samples_cnt);
        ia >> make_nvp("samples", make_array(samples_.data(), samples_.size()));
        energies_.resize(glyph_size);
        ia >> make_nvp("energies", make_array(energies_.data(), energies_.size()));
        features_.resize(glyph_size, glyph_size);
        ia >> make_nvp("features", make_array(features_.data(), features_.size()));

        return true;
    }

public:
    const FontImageT* font() const
    {
        return font_;
    }

    const Eigen::VectorXd& mean() const
    {
        return mean_;
    }

    const Eigen::MatrixXd& samples() const
    {
        return samples_;
    }

    const Eigen::VectorXd& energies() const
    {
        return energies_;
    }

    const Eigen::MatrixXd& features() const
    {
        return features_;
    }

private:
    const FontImageT* font_;
    Eigen::VectorXd mean_;
    Eigen::MatrixXd samples_;
    Eigen::VectorXd energies_;
    Eigen::MatrixXd features_;
};

template<class FontImageT>
FontPCA<FontImageT>::FontPCA(const FontPCAnalyzer<FontImageT>* analyzer, size_t feat_cnt)
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

