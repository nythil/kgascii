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
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <Eigen/Dense>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

template<class TFontImage>
class MutualInformationGlyphMatcher: boost::noncopyable
{
public:
    typedef TFontImage FontImageT;
    typedef typename FontImageT::PixelT PixelT;
    typedef typename FontImageT::ImageT ImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

    class MutualInformationContext
    {
        friend class MutualInformationGlyphMatcher;
    public:
        typedef MutualInformationGlyphMatcher GlyphMatcherT;

    private:
        explicit MutualInformationContext(const MutualInformationGlyphMatcher* matcher)
            :histogram_(matcher->colorBins())
            ,jointHistogram_(matcher->colorBins(), matcher->colorBins())
            ,surfaceData_(matcher->cellWidth(), matcher->cellHeight())
        {
        }

    private:
        Eigen::VectorXi histogram_;
        Eigen::MatrixXi jointHistogram_;
        ImageT surfaceData_;
    };
    typedef MutualInformationContext ContextT;

public:
    explicit MutualInformationGlyphMatcher(boost::shared_ptr<const FontImageT> f, size_t bins)
        :font_(f)
        ,histograms_(font()->glyphCount())
        ,colorBins_(bins)
        ,colorBinSize_(256 / colorBins_)
    {
        //precompute glyph histograms
        for (size_t ci = 0; ci < font()->glyphCount(); ++ci) {
            makeHistogram(font()->getGlyph(ci), histograms_[ci]);
        }
    }

public:
    boost::shared_ptr<const FontImageT> font() const
    {
        return font_;
    }

    unsigned cellWidth() const
    {
        return font_->glyphWidth();
    }

    unsigned cellHeight() const
    {
        return font_->glyphHeight();
    }

    MutualInformationContext createContext() const
    {
        return MutualInformationContext(this);
    }

    template<class TSomeView>
    Symbol match(MutualInformationContext& ctx, const TSomeView& imgv) const
    {
        //copy imgv to tmp_surf padding with black pixels
        ViewT tmp_view = view(ctx.surfaceData_);
        fill_pixels(tmp_view, PixelT());
        copy_pixels(imgv, subimage_view(tmp_view, 0, 0, imgv.width(), imgv.height()));

        makeHistogram(tmp_view, ctx.histogram_);
        double imgv_ent = entropy(ctx.histogram_);

        double nmi_max = std::numeric_limits<double>::min();
        Symbol cc_max;
        for (size_t ci = 0; ci < font()->glyphCount(); ++ci) {
            const Eigen::VectorXi& glyph_histogram = histogram(ci);
            double glyph_ent = entropy(glyph_histogram);

            makeJointHistogram(tmp_view, font()->getGlyph(ci), ctx.jointHistogram_);
            assert(ctx.jointHistogram_.rowwise().sum() == ctx.histogram_);
            assert(ctx.jointHistogram_.colwise().sum().transpose() == glyph_histogram);
            double joint_ent = entropy(ctx.jointHistogram_);

            //normalized mutual information
            double nmi = (imgv_ent + glyph_ent) / joint_ent;
            if (nmi > nmi_max) {
                nmi_max = nmi;
                cc_max = font()->getSymbol(ci);
            }
        }
        return cc_max;
    }

    size_t colorBins() const
    {
        return colorBins_;
    }

    void makeHistogram(const ConstViewT& surf, Eigen::VectorXi& hist) const
    {
        assert(static_cast<size_t>(surf.width()) == cellWidth());
        assert(static_cast<size_t>(surf.height()) == cellHeight());

        hist.setZero(colorBins_);

        for (size_t y = 0; y < cellHeight(); ++y) {
            typename ConstViewT::x_iterator ptr = surf.row_begin(y);
            for (size_t x = 0; x < cellWidth(); ++x) {
                int value = boost::gil::get_color(*ptr++, boost::gil::gray_color_t());
                hist[value / colorBinSize_]++;
            }
        }

        assert(static_cast<size_t>(hist.sum()) == cellWidth() * cellHeight());
    }

    void makeJointHistogram(const ConstViewT& surf1, const ConstViewT& surf2, Eigen::MatrixXi& hist) const
    {
        assert(static_cast<size_t>(surf1.width()) == cellWidth());
        assert(static_cast<size_t>(surf2.width()) == cellWidth());
        assert(static_cast<size_t>(surf1.height()) == cellHeight());
        assert(static_cast<size_t>(surf2.height()) == cellHeight());

        hist.setZero(colorBins_, colorBins_);

        for (size_t y = 0; y < cellHeight(); ++y) {
            typename ConstViewT::x_iterator ptr1 = surf1.row_begin(y);
            typename ConstViewT::x_iterator ptr2 = surf2.row_begin(y);
            for (size_t x = 0; x < cellWidth(); ++x) {
                int value1 = boost::gil::get_color(*ptr1++, boost::gil::gray_color_t());
                int value2 = boost::gil::get_color(*ptr2++, boost::gil::gray_color_t());
                hist(value1 / colorBinSize_, value2 / colorBinSize_)++;
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

    const Eigen::VectorXi& histogram(size_t index) const
    {
        return histograms_.at(index);
    }

private:
    boost::shared_ptr<const FontImageT> font_;
    std::vector<Eigen::VectorXi> histograms_;
    size_t colorBins_;
    size_t colorBinSize_;
};

template<class TFontImage>
class MutualInformationGlyphMatcherFactory
{
public:
    typedef MutualInformationGlyphMatcher<TFontImage> GlyphMatcherT;
    typedef DynamicGlyphMatcher<TFontImage> DynamicGlyphMatcherT;

    boost::shared_ptr<DynamicGlyphMatcherT> operator()(boost::shared_ptr<const TFontImage> font, const std::map<std::string, std::string>& options) const
    {
        size_t bins = 16;
        if (options.count("bins")) {
            try {
                bins = boost::lexical_cast<size_t>(options.find("bins")->second);
            } catch (boost::bad_lexical_cast&) { }
        }

        boost::shared_ptr<GlyphMatcherT> matcher(new GlyphMatcherT(font, bins));
        boost::shared_ptr<DynamicGlyphMatcherT> dynamic_matcher(new DynamicGlyphMatcherT(matcher));
        return dynamic_matcher;
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_MUTUAL_INFORMATION_GLYPH_MATCHER_HPP


