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

#include <kgascii/pcaglyphmatcher.hpp>
#include <kgascii/fontimage.hpp>
#include <iostream>
#include <boost/gil/image_view.hpp>

using std::cout;


namespace KG { namespace Ascii {

PcaGlyphMatcherContext::PcaGlyphMatcherContext(const FontImage& f)
    :GlyphMatcherContext(f)
    ,charcodes_(font().charcodes())
{
    glyphSize_ = font().glyphWidth() * font().glyphHeight();
    Eigen::MatrixXd input_samples(glyphSize_, charcodes_.size());
    for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
        boost::gil::gray8c_view_t glyph_view = font().getGlyph(charcodes_[ci]);
        for (int yy = 0; yy < font().glyphHeight(); ++yy) {
            for (int xx = 0; xx < font().glyphWidth(); ++xx) {
                input_samples(yy * font().glyphWidth() + xx, ci) = glyph_view(xx, yy);
            }
        }
    }
    cout << "input_samples\n";
    Eigen::VectorXd mean_sample = input_samples.rowwise().sum() / charcodes_.size();
    Eigen::MatrixXd normalized_samples = input_samples.colwise() - mean_sample;
    cout << "normalized_samples\n";

    Eigen::MatrixXd covariance(glyphSize_, glyphSize_);
    covariance.setZero();
    covariance.selfadjointView<Eigen::Lower>().rankUpdate(normalized_samples, 1.0 / (normalized_samples.cols() - 1));
    cout << "covariance\n";

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(covariance);
    Eigen::VectorXd eigvals_tmp = eigen_solver.eigenvalues();
    Eigen::VectorXd eigvals = eigvals_tmp.cwiseMax(Eigen::VectorXd::Zero(glyphSize_));
    Eigen::MatrixXd eigvecs = eigen_solver.eigenvectors();
    cout << "eigenvalues\n";

    Eigen::VectorXd featvals = eigvals.tail(10).reverse();
    Eigen::MatrixXd featvecs = eigvecs.rightCols(10).rowwise().reverse();
    for (int i = 0; i < 10; ++i) {
        //featvecs.col(i) *= featvals(i) / featvals.sum();
    }

    Eigen::MatrixXd glyphs = featvecs.transpose() * normalized_samples;
    cout << "glyphs\n";

    mean_ = mean_sample.cast<float>();
    energies_ = featvals.cast<float>();
    cout << "energies: " << energies_ << "\n";
    cout << "perc: " << featvals.sum() / eigvals.sum() << "\n";
    features_ = featvecs.cast<float>();
    cout << "features-min: " << features_.colwise().minCoeff() << "\n";
    cout << "features-max: " << features_.colwise().maxCoeff() << "\n";
    glyphs_ = glyphs.cast<float>();
    cout << "glyphs-min: " << glyphs_.rowwise().minCoeff() << "\n";
    cout << "glyphs-max: " << glyphs_.rowwise().maxCoeff() << "\n";
}

GlyphMatcher* PcaGlyphMatcherContext::createMatcher() const
{
    return new PcaGlyphMatcher(*this);
}


PcaGlyphMatcher::PcaGlyphMatcher(const PcaGlyphMatcherContext& c)
    :GlyphMatcher()
    ,context_(c)
    ,imgvec_(context_.glyphSize_)
    ,components_(10)
{
}

const GlyphMatcherContext& PcaGlyphMatcher::context() const
{
    return context_;
}

char PcaGlyphMatcher::match(const boost::gil::gray8c_view_t& imgv)
{
    assert(imgv.width() <= context_.cellWidth());
    assert(imgv.height() <= context_.cellHeight());

    imgvec_.setZero();
    for (size_t y = 0; y < imgv.height(); ++y) {
        for (size_t x = 0; x < imgv.width(); ++x) {
            imgvec_(x + context_.cellWidth() * y) = imgv(x, y);
        }
    }

    components_ = (imgvec_ - context_.mean_).transpose() * context_.features_;
    int cc_min = ' ';
    (context_.glyphs_.colwise() - components_).colwise().squaredNorm().minCoeff(&cc_min);
    return (char)context_.charcodes_.at(cc_min);
}

} } // namespace KG::Ascii

