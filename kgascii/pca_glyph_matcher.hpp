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
#include <kgascii/kgascii_api.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/font_pca.hpp>

namespace KG { namespace Ascii {

class PcaGlyphMatcherContext;
class PcaGlyphMatcher;

namespace Internal {

template<>
struct Traits<PcaGlyphMatcherContext>
{
    typedef PcaGlyphMatcherContext GlyphMatcherContextT;
    typedef PcaGlyphMatcher GlyphMatcherT;
    typedef FontImage<PixelType8> FontImageT;
    typedef Surface8 SurfaceT;
    typedef Surface8c ConstSurfaceT;
};

template<>
struct Traits<PcaGlyphMatcher>: public Traits<PcaGlyphMatcherContext>
{
};

} // namespace Internal

class PcaGlyphMatcherContext: public GlyphMatcherContext<PcaGlyphMatcherContext>
{
    friend class PcaGlyphMatcher;

public:
    typedef GlyphMatcherContext<PcaGlyphMatcherContext> BaseT;
    typedef BaseT::ConstSurfaceT ConstSurfaceT;

    using BaseT::font;

public:
    explicit PcaGlyphMatcherContext(const FontPCA<PixelType8>* pca)
        :BaseT(pca->font())
        ,pca_(pca)
    {
    }

public:
    PcaGlyphMatcher* createMatcher() const;

private:
    const FontPCA<PixelType8>* pca_;
};


class PcaGlyphMatcher: boost::noncopyable
{
public:
    typedef PcaGlyphMatcherContext GlyphMatcherContextT;
    typedef FontImage<PixelType8> FontImageT;
    typedef Surface8 SurfaceT;
    typedef Surface8c ConstSurfaceT;

public:
    explicit PcaGlyphMatcher(const GlyphMatcherContextT* c)
        :context_(c)
        ,imgvec_(context_->font()->glyphSize())
        ,components_(context_->pca_->featureCount())
    {
    }

public:
    const GlyphMatcherContextT* context() const
    {
        return context_;
    }

    Symbol match(const ConstSurfaceT& imgv)
    {
        assert(imgv.width() <= context_->cellWidth());
        assert(imgv.height() <= context_->cellHeight());

        typedef Eigen::Matrix<typename ConstSurfaceT::value_type, Eigen::Dynamic, 1> VectorXuc;
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
        return context_->font()->getSymbol(context_->pca_->findClosestGlyph(components_));
    }

private:
    const GlyphMatcherContextT* context_;
    Eigen::VectorXf imgvec_;
    Eigen::VectorXf components_;
};

inline PcaGlyphMatcher* PcaGlyphMatcherContext::createMatcher() const
{
    return new PcaGlyphMatcher(this);
}

} } // namespace KG::Ascii


#endif // KGASCII_PCAGLYPHMATCHER_HPP
