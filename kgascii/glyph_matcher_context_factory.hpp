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
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/squared_euclidean_distance.hpp>
#include <kgascii/means_distance.hpp>
#include <kgascii/mutual_information_glyph_matcher.hpp>
#include <kgascii/pca_glyph_matcher.hpp>
#include <kgascii/internal/glyph_matcher_registration.hpp>

namespace KG { namespace Ascii {

template<class TFontImage>
inline void registerGlyphMatcherFactories()
{
    static Internal::GlyphMatcherRegistration<TFontImage, SquaredEuclideanDistanceGlyphMatcherFactory> reg_sed("sed");
    static Internal::GlyphMatcherRegistration<TFontImage, MeansDistanceGlyphMatcherFactory> reg_md("md");
    static Internal::GlyphMatcherRegistration<TFontImage, MutualInformationGlyphMatcherFactory> reg_mi("mi");
    static Internal::GlyphMatcherRegistration<TFontImage, PcaGlyphMatcherFactory> reg_pca("pca");
}

class GlyphMatcherFactory
{
public:
    template<class TFontImage>
    static boost::shared_ptr<typename Internal::GlyphMatcherRegistry<TFontImage>::DynamicGlyphMatcherT> 
    create(boost::shared_ptr<TFontImage> font, const std::string& options)
    {
        std::string algo_name = "pca";
        std::map<std::string, std::string> options_map;
        parseOptions(options, algo_name, options_map);

        typedef Internal::GlyphMatcherRegistry<TFontImage> GlyphMatcherRegistryT;
        typedef typename GlyphMatcherRegistryT::CreatorFuncT CreatorFuncT;
        if (const CreatorFuncT* func = GlyphMatcherRegistryT::findFactory(algo_name)) {
            return (*func)(font, options_map);
        }
        throw std::runtime_error("unknown algo name");
    }

private:
    static void parseOptions(const std::string& options, std::string& algo_name, std::map<std::string, std::string>& options_map)
    {
        using namespace boost::algorithm;

        std::vector<std::string> options_tokens;
        split(options_tokens, options, is_any_of(":"), token_compress_on);

        if (!options_tokens.empty()) {
            algo_name = options_tokens.front();
        }

        for (size_t i = 1; i < options_tokens.size(); ++i) {
            std::vector<std::string> opt_tokens;
            split(opt_tokens, options_tokens[i], is_any_of("="), token_compress_on);
            std::string opt_name = opt_tokens[0];
            std::string opt_value = opt_tokens.size() > 1 ? opt_tokens[1] : "";
            options_map[opt_name] = opt_value;
        }
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_GLYPH_MATCHER_CONTEXT_FACTORY_HPP

