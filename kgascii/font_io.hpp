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

#ifndef KGASCII_FONT_IO_HPP
#define KGASCII_FONT_IO_HPP

#include <fstream>
#include <stdexcept>
#include <vector>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/throw_exception.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/algorithm/lower_bound.hpp>
#include <boost/range/algorithm/upper_bound.hpp>
#include <kgascii/font.hpp>

namespace boost { namespace serialization {

template<class TArchive, class TPixel>
inline void save(TArchive& ar, const boost::gil::image<TPixel>& img, const unsigned int)
{
    typedef boost::gil::image<TPixel> ImageT;
    typedef typename boost::gil::channel_type<typename ImageT::value_type>::type ChannelT;
    ptrdiff_t width = img.width(), height = img.height();
    ar << make_nvp("width", width);
    ar << make_nvp("height", height);
    std::vector<typename ImageT::value_type> row_buffer(width);
    const ChannelT* row_addr = reinterpret_cast<const ChannelT*>(&row_buffer[0]);
    typename ImageT::const_view_t img_view = const_view(img);
    for (ptrdiff_t y = 0; y < height; ++y) {
        std::copy(img_view.row_begin(y), img_view.row_end(y), row_buffer.begin());
        ar << make_array(row_addr, width);
    }
}

template<class TArchive, class TPixel>
inline void load(TArchive& ar, boost::gil::image<TPixel>& img, const unsigned int)
{
    typedef boost::gil::image<TPixel> ImageT;
    typedef typename boost::gil::channel_type<typename ImageT::value_type>::type ChannelT;
    ptrdiff_t width, height;
    ar >> make_nvp("width", width);
    ar >> make_nvp("height", height);
    std::vector<typename ImageT::value_type> row_buffer(width);
    ChannelT* row_addr = reinterpret_cast<ChannelT*>(&row_buffer[0]);
    img.recreate(width, height);
    typename ImageT::view_t img_view = view(img);
    for (ptrdiff_t y = 0; y < height; ++y) {
        ar >> make_array(row_addr, width);
        std::copy(row_buffer.begin(), row_buffer.end(), img_view.row_begin(y));
    }
}

template<class TArchive, class TPixel>
inline void serialize(TArchive& ar, boost::gil::image<TPixel>& img, const unsigned int version)
{
    split_free(ar, img, version);
}


} } // namespace boost::serialization

namespace KG { namespace Ascii {

struct FontIOError: virtual std::exception, virtual boost::exception {};

template<class TImage>
template<class TArchive>
void Font<TImage>::serialize(TArchive& ar, const unsigned int version)
{
    using namespace boost::serialization;
    (void)version;
    ar & make_nvp("family-name", familyName_);
    ar & make_nvp("style-name", styleName_);
    ar & make_nvp("pixel-size", pixelSize_);
    ar & make_nvp("glyph-width", glyphWidth_);
    ar & make_nvp("glyph-height", glyphHeight_);
    ar & make_nvp("glyphs", glyphs_);
}

template<class TImage>
template<class TArchive>
void Font<TImage>::GlyphRecord::load(TArchive& ar, const unsigned int version)
{
    using namespace boost::serialization;
    (void)version;
    ar >> make_nvp("sym", sym);
    ar >> make_nvp("data", data);
    surf = view(data);
}

template<class TImage>
template<class TArchive>
void Font<TImage>::GlyphRecord::save(TArchive& ar, const unsigned int version) const
{
    using namespace boost::serialization;
    (void)version;
    ar << make_nvp("sym", sym);
    ar << make_nvp("data", data);
}

template<class TImage>
bool Font<TImage>::save(const std::string& file_path) const
{
    std::ofstream ofs(file_path.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!ofs.good())
        BOOST_THROW_EXCEPTION(FontIOError() << boost::errinfo_api_function("ofstream"));

    boost::archive::text_oarchive oa(ofs);
    oa << boost::serialization::make_nvp("font-image", *this);

    return true;
}

template<class TImage>
bool Font<TImage>::load(const std::string& file_path)
{
    std::ifstream ifs(file_path.c_str(), std::ios_base::in);
    if (!ifs.good())
        BOOST_THROW_EXCEPTION(FontIOError() << boost::errinfo_api_function("ifstream"));

    boost::archive::text_iarchive ia(ifs);
    ia >> boost::serialization::make_nvp("font-image", *this);

    return true;
}

namespace Internal {

template<class TFont, class TLoader, typename TSymbolIterator>
void doLoad(TFont& font, TLoader& loader, TSymbolIterator first, TSymbolIterator last)
{
    font.setFamilyName(loader.familyName());
    font.setStyleName(loader.styleName());
    font.setPixelSize(loader.pixelSize());
    font.setGlyphSize(loader.glyphWidth(), loader.glyphHeight());

    font.clear();
    for (TSymbolIterator sym = first; sym != last; ++sym) {
        if (!loader.loadGlyph(*sym, font.addGlyph(*sym)))
            BOOST_THROW_EXCEPTION(FontIOError() << boost::errinfo_api_function("loadGlyph"));
    }
}

} // namespace Internal

template<class TImage, class TLoader>
bool load(Font<TImage>& font, TLoader& loader)
{
    typename TLoader::SymbolCollectionT symbols = loader.symbols();
    Internal::doLoad(font, loader, symbols.begin(), symbols.end());

    return true;
}

template<class TImage, class TLoader>
bool load(Font<TImage>& font, TLoader& loader, Symbol ci_min, Symbol ci_max)
{
    typename TLoader::SymbolCollectionT symbols = loader.symbols();
    Internal::doLoad(font, loader, boost::lower_bound(symbols, ci_min), boost::upper_bound(symbols, ci_max));

    return true;
}

} } // namespace KG::Ascii

#endif // KGASCII_FONT_IO_HPP
