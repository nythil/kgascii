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

#ifndef KGASCII_TOOLS_COMMON_CASTSURFACE_HPP
#define KGASCII_TOOLS_COMMON_CASTSURFACE_HPP

#include <cassert>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/gil/gil_all.hpp>

template<typename TPixel>
inline
typename boost::gil::type_from_x_iterator<TPixel*>::view_t castSurface(cv::Mat& mat)
{
    assert(mat.dims == 2);
    TPixel* data = reinterpret_cast<TPixel*>(mat.data);
    return boost::gil::interleaved_view(mat.cols, mat.rows, data, mat.step[0]);
}


#endif // KGASCII_TOOLS_COMMON_CASTSURFACE_HPP

