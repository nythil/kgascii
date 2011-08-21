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

#ifndef KGASCII_IMAGE_IO_HPP
#define KGASCII_IMAGE_IO_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/gil/extension/io_new/bmp_all.hpp>
#include <boost/gil/extension/io_new/pnm_all.hpp>
#include <boost/gil/extension/io_new/targa_all.hpp>
#include <boost/mpl/bool.hpp>

namespace KG { namespace Ascii {

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

} // namespace Internal

template<class TString, class TView>
bool saveImageView(const TString& filename, const TView& view)
{
    boost::filesystem::path file_path(filename);
    std::string ext = boost::algorithm::to_lower_copy(file_path.extension().string());

    if (ext == ".bmp") {
        if (Internal::trySaveImageView(filename, view, boost::gil::bmp_tag()))
            return true;
    }
    if (ext == ".pnm" || ext == ".pbm" || ext == ".pgm" || ext == ".ppm") {
        if (Internal::trySaveImageView(filename, view, boost::gil::pnm_tag()))
            return true;
    }
    if (ext == ".tga") {
        if (Internal::trySaveImageView(filename, view, boost::gil::targa_tag()))
            return true;
    }
    return false;
}

} } // namespace KG::Ascii

#endif // KGASCII_IMAGE_IO_HPP
