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

#ifndef KGASCII_MEANSDISTANCE_HPP
#define KGASCII_MEANSDISTANCE_HPP

#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>
#include <cstdlib>

namespace KG { namespace Ascii {

class MeansDistance
{
public:
    int operator()(const boost::gil::gray8c_view_t& img1, 
            const boost::gil::gray8c_view_t& img2) const
    {
        assert(img1.size() == img2.size());
        int sum1 = 0, sum2 = 0;
        for (int y = 0; y < img1.height(); ++y) {
            boost::gil::gray8c_view_t::x_iterator img1_it = img1.row_begin(y);
            boost::gil::gray8c_view_t::x_iterator img2_it = img2.row_begin(y);
            for (int x = 0; x < img1.width(); ++x, ++img1_it, ++img2_it) {
                sum1 += *img1_it;
                sum2 += *img2_it;
            }
        }
        return abs(sum1 - sum2);
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_MEANSDISTANCE_HPP

