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

#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <Eigen/Dense>
#include <kgascii/font_pca.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

template<class TPrincipalComponents>
class PcaGlyphMatcher: boost::noncopyable
{
public:
    typedef TPrincipalComponents PrincipalComponentsT;
    typedef typename PrincipalComponentsT::FontImageT FontImageT;
    typedef typename FontImageT::PixelT PixelT;
    typedef typename FontImageT::ImageT ImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

    class PcaContext
    {
        friend class PcaGlyphMatcher;
    public:
        typedef PcaGlyphMatcher GlyphMatcherT;

    private:
        explicit PcaContext(const PcaGlyphMatcher* matcher)
            :components_(matcher->features()->featureCount())
            ,imageData_(matcher->font()->glyphSize() * boost::gil::num_channels<ViewT>::value)
        {
        }

    private:
        Eigen::VectorXf components_;
        Eigen::VectorXf imageData_;
    };
    typedef PcaContext ContextT;

public:
    explicit PcaGlyphMatcher(boost::shared_ptr<const PrincipalComponentsT> feat)
        :features_(feat)
    {
    }

public:
    boost::shared_ptr<const PrincipalComponentsT> features() const
    {
        return features_;
    }

    boost::shared_ptr<const FontImageT> font() const
    {
        return features()->font();
    }

    unsigned cellWidth() const
    {
        return font()->glyphWidth();
    }

    unsigned cellHeight() const
    {
        return font()->glyphHeight();
    }

    PcaContext createContext() const
    {
        return PcaContext(this);
    }

    template<typename TSomeView>
    Symbol match(PcaContext& ctx, const TSomeView& imgv) const
    {
        boost::gil::gil_function_requires<boost::gil::ImageViewConcept<TSomeView> >();
        boost::gil::gil_function_requires<boost::gil::ColorSpacesCompatibleConcept<
                                    typename boost::gil::color_space_type<TSomeView>::type,
                                    typename boost::gil::color_space_type<ConstViewT>::type> >();
        boost::gil::gil_function_requires<boost::gil::ChannelsCompatibleConcept<
                                    typename boost::gil::channel_type<TSomeView>::type,
                                    typename boost::gil::channel_type<ConstViewT>::type> >();

        assert(static_cast<size_t>(imgv.width()) <= cellWidth());
        assert(static_cast<size_t>(imgv.height()) <= cellHeight());

        typedef boost::gil::layout<
                typename boost::gil::color_space_type<ConstViewT>::type,
                typename boost::gil::channel_mapping_type<TSomeView>::type
                > LayoutT;
        typedef typename float_channel_type<typename boost::gil::channel_type<ConstViewT>::type>::type FloatChannelT;
        typedef typename boost::gil::pixel_value_type<FloatChannelT, LayoutT>::type FloatPixelT;
        typedef typename boost::gil::type_from_x_iterator<FloatPixelT*>::view_t FloatViewT;

        FloatViewT tmp_glyph_view = boost::gil::interleaved_view(
                cellWidth(), cellHeight(),
                reinterpret_cast<FloatPixelT*>(ctx.imageData_.data()),
                cellWidth() * sizeof(FloatPixelT));

        boost::gil::fill_pixels(tmp_glyph_view, FloatPixelT());
        boost::gil::copy_and_convert_pixels(imgv, subimage_view(
                tmp_glyph_view, 0, 0, imgv.width(), imgv.height()));

        features()->project(ctx.imageData_, ctx.components_);
        return font()->getSymbol(features()->findClosestGlyph(ctx.components_));
    }

private:
    boost::shared_ptr<const PrincipalComponentsT> features_;
};

template<class TFontImage>
class PcaGlyphMatcherFactory
{
public:
    typedef FontEigendecomposition<TFontImage> EigendecompositionT;
    typedef FontPrincipalComponents<EigendecompositionT> PrincipalComponentsT;
    typedef PcaGlyphMatcher<PrincipalComponentsT> PcaGlyphMatcherT;
    typedef DynamicGlyphMatcher<TFontImage> DynamicGlyphMatcherT;

    boost::shared_ptr<DynamicGlyphMatcherT> operator()(boost::shared_ptr<const TFontImage> font, const std::map<std::string, std::string>& options) const
    {
        size_t nfeatures = 10;
        if (options.count("nf")) {
            try {
                nfeatures = boost::lexical_cast<size_t>(options.find("nf")->second);
            } catch (boost::bad_lexical_cast&) { }
        }

        boost::shared_ptr<EigendecompositionT> decomposition(new EigendecompositionT(font));
        if (options.count("cache") && !options.find("cache")->second.empty()) {
            decomposition->loadFromCache(options.find("cache")->second);
        } else {
            decomposition->analyze();
        }
        if (options.count("makecache") && !options.find("makecache")->second.empty()) {
            decomposition->saveToCache(options.find("makecache")->second);
        }

        boost::shared_ptr<PrincipalComponentsT> components(new PrincipalComponentsT(decomposition, nfeatures));
        boost::shared_ptr<PcaGlyphMatcherT> matcher(new PcaGlyphMatcherT(components));
        boost::shared_ptr<DynamicGlyphMatcherT> dynamic_matcher(new DynamicGlyphMatcherT(matcher));
        return dynamic_matcher;
    }
};


} } // namespace KG::Ascii


#endif // KGASCII_PCAGLYPHMATCHER_HPP
