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
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <Eigen/Dense>
#include <kgascii/font_image.hpp>
#include <kgascii/font_pca.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

class PcaGlyphMatcherContext;
class PcaGlyphMatcher;

class PcaGlyphMatcherContext: boost::noncopyable
{
    friend class PcaGlyphMatcher;

public:
    typedef PcaGlyphMatcher GlyphMatcherT;
    typedef FontImage<PixelType8> FontImageT;
    typedef Surface8 SurfaceT;
    typedef Surface8c ConstSurfaceT;

public:
    explicit PcaGlyphMatcherContext(const FontPCA<PixelType8>* pca)
        :font_(pca->font())
        ,pca_(pca)
    {
    }

public:
    const FontImageT* font() const
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

    PcaGlyphMatcher* createMatcher() const;

private:
    const FontImageT* font_;
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

template<typename TPixel>
class PcaGlyphMatcherContextFactory
{
};

template<>
class PcaGlyphMatcherContextFactory<PixelType8>
{
public:
    typedef PcaGlyphMatcherContext GlyphMatcherContextT;

    DynamicGlyphMatcherContext<PixelType8>* operator()(const FontImage<PixelType8>* font, const std::map<std::string, std::string>& options)
    {
        size_t nfeatures = 10;
        if (options.count("nf")) {
            try {
                nfeatures = boost::lexical_cast<size_t>(options.find("nf")->second);
            } catch (boost::bad_lexical_cast&) { }
        }

        FontPCAnalyzer<PixelType8>* pcanalyzer = new FontPCAnalyzer<PixelType8>(font);
        if (options.count("cache") && !options.find("cache")->second.empty()) {
            pcanalyzer->loadFromCache(options.find("cache")->second);
        } else {
            pcanalyzer->analyze();
        }
        if (options.count("makecache") && !options.find("makecache")->second.empty()) {
            pcanalyzer->saveToCache(options.find("makecache")->second);
        }

        FontPCA<PixelType8>* pca = new FontPCA<PixelType8>(pcanalyzer, nfeatures);

        boost::scoped_ptr<GlyphMatcherContextT> impl_holder(new GlyphMatcherContextT(pca));
        return new DynamicGlyphMatcherContext<PixelType8>(impl_holder);
    }
};


} } // namespace KG::Ascii


#endif // KGASCII_PCAGLYPHMATCHER_HPP
