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
#include <kgascii/font_pca.hpp>

namespace KG { namespace Ascii {

PcaGlyphMatcherContext::PcaGlyphMatcherContext(const FontPCA* pca)
    :GlyphMatcherContext(pca->font())
    ,pca_(pca)
    ,charcodes_(font()->charcodes())
{
}

GlyphMatcher* PcaGlyphMatcherContext::createMatcher() const
{
    return new PcaGlyphMatcher(this);
}


PcaGlyphMatcher::PcaGlyphMatcher(const PcaGlyphMatcherContext* c)
    :GlyphMatcher()
    ,context_(c)
    ,imgvec_(context_->font()->glyphSize())
    ,components_(context_->pca_->featureCount())
{
}

const GlyphMatcherContext* PcaGlyphMatcher::context() const
{
    return context_;
}

Symbol PcaGlyphMatcher::match(const Surface8c& imgv)
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

