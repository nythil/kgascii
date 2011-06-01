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

#include <sstream>
#include <kgascii/font_pcanalyzer.hpp>
#include <kgascii/font_image.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace KG { namespace Ascii {

FontPCAnalyzer::FontPCAnalyzer(const FontImage* f)
    :font_(f)
{
}

void FontPCAnalyzer::analyze()
{
    size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
    size_t samples_cnt = font_->glyphCount();

    typedef Eigen::Matrix<Surface8::value_type, Eigen::Dynamic, 1> VectorXuc;

    Eigen::MatrixXd input_samples(glyph_size, samples_cnt);
    for (size_t ci = 0; ci < samples_cnt; ++ci) {
        Surface8c glyph_surface = font_->glyphByIndex(ci);
        assert(glyph_surface.isContinuous());
        input_samples.col(ci) = VectorXuc::Map(glyph_surface.data(), glyph_surface.size()).cast<double>();
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

FontPCA FontPCAnalyzer::extract(size_t cnt) const
{
    return FontPCA(this, cnt);
}

bool FontPCAnalyzer::saveToCache(const std::string& filename) const
{
    size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
    size_t samples_cnt = font_->glyphCount();

    boost::property_tree::ptree pt;

    std::ostringstream ostr_mean;
    for (size_t i = 0; i < glyph_size; ++i) {
        ostr_mean << mean_(i) << " ";
    }
    pt.put("pca.mean", ostr_mean.str());

    std::ostringstream ostr_samples;
    for (size_t i = 0; i < glyph_size; ++i) {
        for (size_t j = 0; j < samples_cnt; ++j) {
            ostr_samples << samples_(i, j) << " ";
        }
    }
    pt.put("pca.samples", ostr_samples.str());

    std::ostringstream ostr_energies;
    for (size_t i = 0; i < glyph_size; ++i) {
        ostr_energies << energies_(i) << " ";
    }
    pt.put("pca.energies", ostr_energies.str());

    std::ostringstream ostr_features;
    for (size_t i = 0; i < glyph_size; ++i) {
        for (size_t j = 0; j < glyph_size; ++j) {
            ostr_features << features_(i, j) << " ";
        }
    }
    pt.put("pca.features", ostr_features.str());

    std::ofstream fstr(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
    boost::property_tree::xml_parser::write_xml(fstr, pt,
        boost::property_tree::xml_writer_make_settings(' ', 2));

    return true;
}

bool FontPCAnalyzer::loadFromCache(const std::string& filename)
{
    size_t glyph_size = font_->glyphWidth() * font_->glyphHeight();
    size_t samples_cnt = font_->glyphCount();

    boost::property_tree::ptree pt;
    boost::property_tree::xml_parser::read_xml(filename, pt);

    std::string str_mean = pt.get<std::string>("pca.mean");
    std::string str_samples = pt.get<std::string>("pca.samples");
    std::string str_energies = pt.get<std::string>("pca.energies");
    std::string str_features = pt.get<std::string>("pca.features");

    std::istringstream istr_mean(str_mean);
    mean_.setZero(glyph_size);
    for (size_t i = 0; i < glyph_size; ++i) {
        istr_mean >> mean_.coeffRef(i);
    }

    std::istringstream istr_samples(str_samples);
    samples_.setZero(glyph_size, samples_cnt);
    for (size_t i = 0; i < glyph_size; ++i) {
        for (size_t j = 0; j < samples_cnt; ++j) {
            istr_samples >> samples_.coeffRef(i, j);
        }
    }

    std::istringstream istr_energies(str_energies);
    energies_.setZero(glyph_size);
    for (size_t i = 0; i < glyph_size; ++i) {
        istr_energies >> energies_.coeffRef(i);
    }

    std::istringstream istr_features(str_features);
    features_.setZero(glyph_size, glyph_size);
    for (size_t i = 0; i < glyph_size; ++i) {
        for (size_t j = 0; j < glyph_size; ++j) {
            istr_features >> features_.coeffRef(i, j);
        }
    }

    return true;
}

const FontImage* FontPCAnalyzer::font() const
{
    return font_;
}

const Eigen::VectorXd& FontPCAnalyzer::mean() const
{
    return mean_;
}

const Eigen::MatrixXd& FontPCAnalyzer::samples() const
{
    return samples_;
}

const Eigen::VectorXd& FontPCAnalyzer::energies() const
{
    return energies_;
}

const Eigen::MatrixXd& FontPCAnalyzer::features() const
{
    return features_;
}

} } // namespace KG::Ascii

