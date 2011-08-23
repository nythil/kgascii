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

#ifndef KGUTIL_IMAGE_IO_HPP
#define KGUTIL_IMAGE_IO_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/gil/extension/io_new/bmp_all.hpp>
#include <boost/gil/extension/io_new/jpeg_all.hpp>
#include <boost/gil/extension/io_new/png_all.hpp>
#include <boost/gil/extension/io_new/pnm_all.hpp>
#include <boost/gil/extension/io_new/targa_all.hpp>
#include <boost/gil/extension/io_new/tiff_all.hpp>
#include <boost/mpl/bool.hpp>

namespace KG { namespace Util {

namespace Internal {

template<class TString, class TView, class TInfo, bool B>
bool trySaveImageViewImpl(const TString& filename, const TView& view, 
    const TInfo& info, const boost::mpl::bool_<B>&)
{
    (void)filename;
    (void)view;
    (void)info;
    return false;
}

template<class TString, class TView, class TInfo>
bool trySaveImageViewImpl(const TString& filename, const TView& view, 
    const TInfo& info, const boost::mpl::true_&)
{
    boost::gil::write_view(filename, view, info);
    return true;
}

template<class TString, class TView, class TTag>
bool trySaveImageView(const TString& filename, const TView& view, 
    const boost::gil::image_write_info<TTag>& info)
{
    typedef typename TView::value_type pixel_type;
    return trySaveImageViewImpl(filename, view, info, 
        boost::gil::is_write_supported<pixel_type, TTag>());
}

template<class TString, class TView, class TTag>
bool trySaveImageView(const TString& filename, const TView& view, const TTag&)
{
    return trySaveImageView(filename, view, boost::gil::image_write_info<TTag>());
}

template<class TString, class TView>
bool saveImageView(const TString& filename, const TView& view)
{
    boost::filesystem::path file_path(filename);
    std::string ext = boost::algorithm::to_lower_copy(file_path.extension().string());

    if (ext == ".bmp") {
        if (trySaveImageView(filename, view, boost::gil::bmp_tag()))
            return true;
    }
    if (ext == ".jpg" || ext == ".jpeg") {
        if (trySaveImageView(filename, view, boost::gil::jpeg_tag()))
            return true;
    }
    if (ext == ".png") {
        if (trySaveImageView(filename, view, boost::gil::png_tag()))
            return true;
    }
    if (ext == ".pnm" || ext == ".pbm" || ext == ".pgm" || ext == ".ppm") {
        if (trySaveImageView(filename, view, boost::gil::pnm_tag()))
            return true;
    }
    if (ext == ".tga") {
        if (trySaveImageView(filename, view, boost::gil::targa_tag()))
            return true;
    }
    if (ext == ".tif" || ext == ".tiff") {
        if (trySaveImageView(filename, view, boost::gil::tiff_tag()))
            return true;
    }
    return false;
}

} // namespace Internal

template<class TString, class TView>
bool saveImageView(const TString& filename, const TView& view)
{
    return Internal::saveImageView(filename, view);
}


namespace Internal {

template<class TString, class TImage, class TTag, bool Convert>
bool tryLoadImage(const TString& filename, TImage& image, const TTag& tag, 
    const boost::mpl::bool_<Convert>& conv)
{
    boost::gil::read_image(filename, image, tag);
    return true;
}

template<class TString, class TImage, class TTag>
bool tryLoadImage(const TString& filename, TImage& image, const TTag& tag, 
    const boost::mpl::true_&)
{
    boost::gil::read_and_convert_image(filename, image, tag);
    return true;
}

template<class TString, class TImage, bool Convert>
bool loadImage(const TString& filename, TImage& image, 
    const boost::mpl::bool_<Convert>& conv)
{
    boost::filesystem::path file_path(filename);
    std::string ext = boost::algorithm::to_lower_copy(file_path.extension().string());

    if (ext == ".bmp") {
        if (tryLoadImage(filename, image, boost::gil::bmp_tag(), conv))
            return true;
    }
    if (ext == ".jpg" || ext == ".jpeg") {
        if (tryLoadImage(filename, image, boost::gil::jpeg_tag(), conv))
            return true;
    }
    if (ext == ".png") {
        if (tryLoadImage(filename, image, boost::gil::png_tag(), conv))
            return true;
    }
    if (ext == ".pnm" || ext == ".pbm" || ext == ".pgm" || ext == ".ppm") {
        if (tryLoadImage(filename, image, boost::gil::pnm_tag(), conv))
            return true;
    }
    if (ext == ".tga") {
        if (tryLoadImage(filename, image, boost::gil::targa_tag(), conv))
            return true;
    }
    if (ext == ".tif" || ext == ".tiff") {
        if (tryLoadImage(filename, image, boost::gil::tiff_tag(), conv))
            return true;
    }
    return false;
}

} // namespace Internal

template<class TString, class TImage>
bool loadImage(const TString& filename, TImage& image)
{
    return Internal::loadImage(filename, image, boost::mpl::false_());
}

template<class TString, class TImage>
bool loadAndConvertImage(const TString& filename, TImage& image)
{
    return Internal::loadImage(filename, image, boost::mpl::true_());
}

} } // namespace KG::Util

#endif // KGUTIL_IMAGE_IO_HPP
