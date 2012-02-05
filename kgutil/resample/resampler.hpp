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

#ifndef KGUTIL_RESAMPLE_RESAMPLER_HPP
#define KGUTIL_RESAMPLE_RESAMPLER_HPP

#include <vector>
#include <cassert>
#include <boost/gil/gil_all.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/casts.hpp>


namespace KG { namespace Util {

template<class TFilter, typename TReal=typename TFilter::result_type>
struct FilterContributions
{
    struct PixelContributions
    {
        size_t index;
        size_t size;
        TReal* weights;
    };
    std::vector<PixelContributions> pixels;
    std::vector<TReal> weights;
    unsigned ssize;
    unsigned dsize;

    static unsigned reflect(int i, unsigned size)
    {
        if (i < 0)
            return -i;
        if (static_cast<unsigned>(i) >= size)
            return size - (i - size) - 1;        
        return i;
    }

    explicit FilterContributions(unsigned ssize, unsigned dsize, const TFilter& filter=TFilter())
        :ssize(ssize), dsize(dsize)
    {
        assert(ssize > 0);
        assert(dsize > 0);
        assert(ssize <= 2 * dsize || dsize <= 2 * ssize);
        const TReal scale = static_cast<TReal>(dsize) / static_cast<TReal>(ssize);
        TReal fwidth = filter.support();
        TReal fscale = 1;
        if (scale < 1) {
            fwidth /= scale;
            fscale /= scale;
        }
        const unsigned max_weights = static_cast<unsigned>(2 * fwidth + 1);
        assert(ssize > max_weights);
        pixels.resize(dsize);
        weights.resize(dsize * max_weights);
        for (unsigned i = 0; i < dsize; ++i) {
            TReal center = (i + static_cast<TReal>(0.5)) / scale - static_cast<TReal>(0.5);
            int left = static_cast<int>(ceil(center - fwidth));
            int right = static_cast<int>(floor(center + fwidth));
            assert(static_cast<unsigned>(right - left) < max_weights);
            int left_lim = std::max<int>(0, left);
            int right_lim = std::min<int>(right, ssize - 1);
            int lim_size = right_lim - left_lim + 1;
            int lim_overflow = std::max<int>(right - right_lim, left_lim - left) + 1;
            if (lim_overflow > lim_size) {
                lim_size = lim_overflow;
                if (left_lim + lim_overflow > ssize) {
                    left_lim = ssize - lim_overflow;
                }
            }
            pixels[i].index = left_lim;
            pixels[i].size = lim_size;
            assert(pixels[i].size <= max_weights);
            pixels[i].weights = &weights[0] + i * max_weights;
            TReal total_weight = 0;
            for (int j = left; j <= right; ++j) {
                TReal weight = filter((center - j) / fscale) / fscale;
                unsigned k = reflect(j, ssize) - pixels[i].index;
                assert(k < pixels[i].size);
                pixels[i].weights[k] += weight;
                total_weight += weight;
            }
            if (total_weight > 0) {
                unsigned max_k = 0;
                TReal weight_sum = 0;
                for (unsigned k = 0; k < pixels[i].size; ++k) {
                    pixels[i].weights[k] /= total_weight;
                    weight_sum += pixels[i].weights[k];
                    if (pixels[i].weights[k] > pixels[i].weights[max_k]) {
                        max_k = k;
                    }
                }
                if (weight_sum != 1) {
                    pixels[i].weights[max_k] += 1 - weight_sum;
                }
            }
        }
    }
};

template<class TFilter, typename TReal=typename TFilter::result_type>
struct Resampler
{
    typedef FilterContributions<TFilter, TReal> Contributions;
    typedef typename Contributions::PixelContributions PixelContributions;
    Contributions h_weights;
    Contributions v_weights;

    explicit Resampler(unsigned swidth, unsigned sheight, unsigned dwidth, unsigned dheight, const TFilter& filter=TFilter())
        :h_weights(swidth, dwidth, filter)
        ,v_weights(sheight, dheight, filter)
    {
    }

    template<class TSrcView, class TDstView>
    void apply(const TSrcView& sview, const TDstView& dview) const
    {
        assert(sview.width() == h_weights.ssize);
        assert(dview.width() == h_weights.dsize);
        assert(sview.height() == v_weights.ssize);
        assert(dview.height() == v_weights.dsize);

        typedef boost::gil::layout<
                typename boost::gil::color_space_type<TSrcView>::type,
                typename boost::gil::channel_mapping_type<TSrcView>::type
                > layout_t;
        typedef typename boost::gil::channel_type<TSrcView>::type src_channel_t;
        typedef typename boost::gil::pixel_value_type<TReal, layout_t>::type real_pixel_t;

        namespace bll = boost::lambda;

        std::vector<real_pixel_t> column(v_weights.ssize);
        for (unsigned dx = 0; dx < h_weights.dsize; ++dx) {
            for (unsigned sy = 0; sy < v_weights.ssize; ++sy) {
                const PixelContributions& hcontrib = h_weights.pixels[dx];
                real_pixel_t acc_value;
                boost::gil::static_fill(acc_value, static_cast<TReal>(0));
                for (unsigned iw = 0; iw < hcontrib.size; ++iw) {
                    const TReal weight = hcontrib.weights[iw];
                    boost::gil::static_for_each(acc_value, sview(hcontrib.index + iw, sy), 
                        bll::_1 += bll::ll_static_cast<TReal>(bll::_2) * bll::constant(weight));
                }
                column[sy] = acc_value;
            }

            for (unsigned dy = 0; dy < v_weights.dsize; ++dy) {
                const PixelContributions& vcontrib = v_weights.pixels[dy];
                real_pixel_t acc_value;
                boost::gil::static_fill(acc_value, static_cast<TReal>(0));
                for (unsigned iw = 0; iw < vcontrib.size; ++iw) {
                    const TReal weight = vcontrib.weights[iw];
                    boost::gil::static_for_each(acc_value, column[vcontrib.index + iw], 
                        bll::_1 += bll::ll_static_cast<TReal>(bll::_2) * bll::constant(weight));
                }
                typename TSrcView::value_type dst_value;
                boost::gil::static_transform(acc_value, dst_value, bll::ll_static_cast<src_channel_t>(bll::_1));
                dview(dx, dy) = dst_value;
            }
        }
    }
};


} } // namespace KG::Util

#endif // KGUTIL_RESAMPLE_RESAMPLER_HPP

