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

#ifndef KGASCII_MUTUAL_INFORMATION_GLYPH_MATCHER_HPP
#define KGASCII_MUTUAL_INFORMATION_GLYPH_MATCHER_HPP

#include <vector>
#include <limits>
#include <cmath>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/surface_container.hpp>
#include <kgascii/surface_algorithm.hpp>
#include <Eigen/Dense>

namespace KG { namespace Ascii {

class MutualInformationGlyphMatcherContext;


class MutualInformationGlyphMatcher: public GlyphMatcher
{
public:
    explicit MutualInformationGlyphMatcher(const MutualInformationGlyphMatcherContext* c);

public:
    const GlyphMatcherContext* context() const;

    unsigned match(const Surface8c& imgv);

private:
    const MutualInformationGlyphMatcherContext* context_;
    Eigen::VectorXi histogram_;
    Eigen::MatrixXi jointHistogram_;
    SurfaceContainer8 surfaceData_;
};


class MutualInformationGlyphMatcherContext: public GlyphMatcherContext
{
    friend class MutualInformationGlyphMatcher;

public:
    explicit MutualInformationGlyphMatcherContext(const FontImage* f, size_t bins)
        :GlyphMatcherContext(f)
        ,charcodes_(font()->charcodes())
        ,glyphs_(font()->glyphs())
        ,histograms_(glyphs_.size())
        ,colorBins_(bins)
        ,colorBinSize_(256 / colorBins_)
    {
        //precompute glyph histograms
        for (size_t ci = 0; ci < glyphs_.size(); ++ci) {
            makeHistogram(glyphs_[ci], histograms_[ci]);
        }
    }

public:
    MutualInformationGlyphMatcher* createMatcher() const
    {
        return new MutualInformationGlyphMatcher(this);
    }

    void makeHistogram(const Surface8c& surf, Eigen::VectorXi& hist) const
    {
        assert(surf.width() == cellWidth());
        assert(surf.height() == cellHeight());

        hist.setZero(colorBins_);

        for (size_t y = 0; y < cellHeight(); ++y) {
            Surface8c::const_pointer ptr = surf.row(y);
            for (size_t x = 0; x < cellWidth(); ++x) {
                hist[ptr[x] / colorBinSize_]++;
            }
        }

        assert(static_cast<size_t>(hist.sum()) == cellWidth() * cellHeight());
    }

    void makeJointHistogram(const Surface8c& surf1, const Surface8c& surf2, Eigen::MatrixXi& hist) const
    {
        assert(surf1.width() == cellWidth());
        assert(surf2.width() == cellWidth());
        assert(surf1.height() == cellHeight());
        assert(surf2.height() == cellHeight());

        hist.setZero(colorBins_, colorBins_);

        for (size_t y = 0; y < cellHeight(); ++y) {
            Surface8c::const_pointer ptr1 = surf1.row(y);
            Surface8c::const_pointer ptr2 = surf2.row(y);
            for (size_t x = 0; x < cellWidth(); ++x) {
                hist(ptr1[x] / colorBinSize_, ptr2[x] / colorBinSize_)++;
            }
        }

        assert(static_cast<size_t>(hist.sum()) == cellWidth() * cellHeight());
    }

    template<typename Derived>
    double entropy(const Eigen::MatrixBase<Derived>& hist) const
    {
        double ent = 0;
        double volume = cellWidth() * cellHeight();
        for (typename Derived::Index r = 0; r < hist.rows(); ++r) {
            for (typename Derived::Index c = 0; c < hist.cols(); ++c) {
                double hi = hist(r, c);
                if (hi <= 0) continue;
                double pi = hi / volume;
                ent -= pi * log(pi);
            }
        }
        assert(ent > -0.001);
        return std::max(ent, 0.0);
    }

private:
    std::vector<unsigned> charcodes_;
    std::vector<Surface8c> glyphs_;
    std::vector<Eigen::VectorXi> histograms_;
    size_t colorBins_;
    size_t colorBinSize_;
};

MutualInformationGlyphMatcher::MutualInformationGlyphMatcher(const MutualInformationGlyphMatcherContext* c)
    :GlyphMatcher()
    ,context_(c)
    ,histogram_(context_->colorBins_)
    ,jointHistogram_(context_->colorBins_, context_->colorBins_)
    ,surfaceData_(context_->cellWidth(), context_->cellHeight())
{
}

const GlyphMatcherContext* MutualInformationGlyphMatcher::context() const
{
    return context_;
}

unsigned MutualInformationGlyphMatcher::match(const Surface8c& imgv)
{
    //copy imgv to tmp_surf padding with black pixels
    Surface8 tmp_surf = surfaceData_.surface();
    fillPixels(tmp_surf, 0);
    copyPixels(imgv, tmp_surf.window(0, 0, imgv.width(), imgv.height()));

    context_->makeHistogram(tmp_surf, histogram_);
    double imgv_ent = context_->entropy(histogram_);

    double nmi_max = std::numeric_limits<double>::min();
    unsigned cc_max = ' ';
    for (size_t ci = 0; ci < context_->charcodes_.size(); ++ci) {
        const Eigen::VectorXi& glyph_histogram = context_->histograms_[ci];
        double glyph_ent = context_->entropy(glyph_histogram);

        context_->makeJointHistogram(tmp_surf, context_->glyphs_[ci], jointHistogram_);
        assert(jointHistogram_.rowwise().sum() == histogram_);
        assert(jointHistogram_.colwise().sum().transpose() == glyph_histogram);
        double joint_ent = context_->entropy(jointHistogram_);

        //normalized mutual information
        double nmi = (imgv_ent + glyph_ent) / joint_ent;
        if (nmi > nmi_max) {
            nmi_max = nmi;
            cc_max = context_->charcodes_[ci];
        }
    }
    return cc_max;
}

} } // namespace KG::Ascii

#endif // KGASCII_MUTUAL_INFORMATION_GLYPH_MATCHER_HPP


