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

#include <kgascii/pca_glyph_matcher.hpp>
#include <kgascii/font_image.hpp>
#include <iostream>

using std::cout;

namespace KG { namespace Ascii {

PcaGlyphMatcherContext::PcaGlyphMatcherContext(const FontImage& f)
    :GlyphMatcherContext(f)
    ,charcodes_(font().charcodes())
{
    glyphSize_ = font().glyphWidth() * font().glyphHeight();
    Eigen::MatrixXd input_samples(glyphSize_, charcodes_.size());
    for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
        Surface8c glyph_view = font().glyphByIndex(ci);
        for (unsigned yy = 0; yy < font().glyphHeight(); ++yy) {
            for (unsigned xx = 0; xx < font().glyphWidth(); ++xx) {
                input_samples(yy * font().glyphWidth() + xx, ci) = glyph_view(xx, yy);
            }
        }
    }
    Eigen::VectorXd mean_sample = input_samples.rowwise().sum() / charcodes_.size();
    Eigen::MatrixXd normalized_samples = input_samples.colwise() - mean_sample;

    Eigen::MatrixXd covariance(glyphSize_, glyphSize_);
    covariance.setZero();
    covariance.selfadjointView<Eigen::Lower>().rankUpdate(normalized_samples, 1.0 / (normalized_samples.cols() - 1));

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(covariance);
    Eigen::VectorXd eigvals_tmp = eigen_solver.eigenvalues();
    Eigen::VectorXd eigvals = eigvals_tmp.cwiseMax(Eigen::VectorXd::Zero(glyphSize_));
    Eigen::MatrixXd eigvecs = eigen_solver.eigenvectors();

    Eigen::VectorXd featvals = eigvals.tail(10).reverse();
    Eigen::MatrixXd featvecs = eigvecs.rightCols(10).rowwise().reverse();
    for (int i = 0; i < 10; ++i) {
        //featvecs.col(i) *= featvals(i) / featvals.sum();
    }

    Eigen::MatrixXd glyphs = featvecs.transpose() * normalized_samples;

    mean_ = mean_sample.cast<float>();
    energies_ = featvals.cast<float>();
    features_ = featvecs.cast<float>();
    glyphs_ = glyphs.cast<float>();
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

unsigned PcaGlyphMatcher::match(const Surface8c& imgv)
{
    assert(imgv.width() <= context_.cellWidth());
    assert(imgv.height() <= context_.cellHeight());

    typedef Eigen::Matrix<Surface8::value_type, Eigen::Dynamic, 1> VectorXuc;
    if (imgv.isContinuous() && imgv.size() == static_cast<size_t>(imgvec_.size())) {
        imgvec_ = VectorXuc::Map(imgv.data(), imgv.size()).cast<float>();
    } else {
        imgvec_.setZero();
        size_t img_w = imgv.width();
        for (size_t y = 0; y < imgv.height(); ++y) {
            imgvec_.segment(y * img_w, img_w) = VectorXuc::Map(imgv.row(y), img_w).cast<float>();
        }
    }

    components_ = (imgvec_ - context_.mean_).transpose() * context_.features_;
    unsigned cc_min = ' ';
    (context_.glyphs_.colwise() - components_).colwise().squaredNorm().minCoeff(&cc_min);
    return context_.charcodes_.at(cc_min);
}

} } // namespace KG::Ascii

