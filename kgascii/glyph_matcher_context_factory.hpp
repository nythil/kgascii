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

#ifndef KGASCII_GLYPH_MATCHER_CONTEXT_FACTORY_HPP
#define KGASCII_GLYPH_MATCHER_CONTEXT_FACTORY_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/mutual_information_glyph_matcher.hpp>
#include <kgascii/squared_euclidean_distance.hpp>
#include <kgascii/means_distance.hpp>
#include <kgascii/pca_glyph_matcher.hpp>
#include <kgascii/font_pcanalyzer.hpp>
#include <kgascii/font_pca.hpp>
#include <map>
#include <deque>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace KG { namespace Ascii {

template<typename TPixel>
class DynamicGlyphMatcherContext;
template<typename TPixel>
class FontImage;

class KGASCII_API GlyphMatcherContextFactory: boost::noncopyable
{
public:
    GlyphMatcherContextFactory()
    {
    }

    virtual ~GlyphMatcherContextFactory()
    {
    }

public:
    virtual DynamicGlyphMatcherContext<PixelType8>* create(const FontImage<PixelType8>* font,
            const std::string& options) const
    {
        using namespace boost::algorithm;
        std::vector<std::string> options_tokens;
        split(options_tokens, options, is_any_of(":"), token_compress_on);
        std::string algo_name = "pca";
        if (!options_tokens.empty()) {
            algo_name = options_tokens.front();
        }
        std::map<std::string, std::string> options_map;
        for (size_t i = 1; i < options_tokens.size(); ++i) {
            std::vector<std::string> opt_tokens;
            split(opt_tokens, options_tokens[i], is_any_of("="), token_compress_on);
            std::string opt_name = opt_tokens[0];
            std::string opt_value = opt_tokens.size() > 1 ? opt_tokens[1] : "";
            options_map[opt_name] = opt_value;
        }
        if (algo_name == "pca") {
            size_t nfeatures = 10;
            try {
                nfeatures = boost::lexical_cast<size_t>(options_map["nf"]);
            } catch (boost::bad_lexical_cast&) { }
            FontPCAnalyzer<PixelType8>* pcanalyzer = new FontPCAnalyzer<PixelType8>(font);
            if (options_map.count("cache") && !options_map["cache"].empty()) {
                pcanalyzer->loadFromCache(options_map["cache"]);
            } else {
                pcanalyzer->analyze();
            }
            if (options_map.count("makecache") && !options_map["makecache"].empty()) {
                pcanalyzer->saveToCache(options_map["makecache"]);
            }
            FontPCA<PixelType8>* pca = new FontPCA<PixelType8>(pcanalyzer, nfeatures);
            return new DynamicGlyphMatcherContext<PixelType8>(new PcaGlyphMatcherContext(pca));
        } else if (algo_name == "sed") {
            return new DynamicGlyphMatcherContext<PixelType8>(new PolicyBasedGlyphMatcherContext<SquaredEuclideanDistance>(font));
        } else if (algo_name == "md") {
            return new DynamicGlyphMatcherContext<PixelType8>(new PolicyBasedGlyphMatcherContext<MeansDistance>(font));
        } else if (algo_name == "mi") {
            size_t bins = 16;
            try {
                bins = boost::lexical_cast<size_t>(options_map["bins"]);
            } catch (boost::bad_lexical_cast&) { }
            return new DynamicGlyphMatcherContext<PixelType8>(new MutualInformationGlyphMatcherContext(font, bins));
        } else {
            throw std::runtime_error("unknown algo name");
        }
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_GLYPH_MATCHER_CONTEXT_FACTORY_HPP

