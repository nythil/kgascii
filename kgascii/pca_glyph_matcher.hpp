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

#ifndef KGASCII_PCAGLYPHMATCHER_HPP
#define KGASCII_PCAGLYPHMATCHER_HPP

#include <vector>
#include <boost/noncopyable.hpp>
#include <Eigen/Dense>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/font_pca.hpp>

namespace KG { namespace Ascii {

class PcaGlyphMatcherContext: public GlyphMatcherContext
{
    friend class PcaGlyphMatcher;

public:
    explicit PcaGlyphMatcherContext(const FontPCA* pca)
        :GlyphMatcherContext(pca->font())
        ,pca_(pca)
        ,charcodes_(font()->charcodes())
    {
    }

public:
    GlyphMatcher* createMatcher() const;

private:
    const FontPCA* pca_;
    std::vector<Symbol> charcodes_;
};

class KGASCII_API PcaGlyphMatcher: public GlyphMatcher
{
public:
    explicit PcaGlyphMatcher(const PcaGlyphMatcherContext* c);

public:
    const GlyphMatcherContext* context() const
    {
        return context_;
    }

    Symbol match(const Surface8c& imgv);

private:
    const PcaGlyphMatcherContext* context_;
    Eigen::VectorXf imgvec_;
    Eigen::VectorXf components_;
};

} } // namespace KG::Ascii

namespace KG { namespace Ascii {

inline GlyphMatcher* PcaGlyphMatcherContext::createMatcher() const
{
    return new PcaGlyphMatcher(this);
}

inline PcaGlyphMatcher::PcaGlyphMatcher(const PcaGlyphMatcherContext* c)
    :GlyphMatcher()
    ,context_(c)
    ,imgvec_(context_->font()->glyphSize())
    ,components_(context_->pca_->featureCount())
{
}

inline Symbol PcaGlyphMatcher::match(const Surface8c& imgv)
{
    assert(imgv.width() <= context_->cellWidth());
    assert(imgv.height() <= context_->cellHeight());

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

    context_->pca_->project(imgvec_, components_);
    return context_->charcodes_.at(context_->pca_->findClosestGlyph(components_));
}

} } // namespace KG::Ascii

#endif // KGASCII_PCAGLYPHMATCHER_HPP
