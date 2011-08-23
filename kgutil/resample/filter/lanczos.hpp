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

#ifndef KGUTIL_RESAMPLE_FILTER_LANCZOS_HPP
#define KGUTIL_RESAMPLE_FILTER_LANCZOS_HPP

#include <functional>
#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/sinc.hpp>


namespace KG { namespace Util { namespace Filter {

template<typename TReal=float>
struct Lanczos3Filter: public std::unary_function<TReal, TReal>
{
    TReal operator ()(TReal t) const
    {
        if (t < 0) t = -t;
        if (t < 3) {
            t *= boost::math::constants::pi<TReal>();
            return boost::math::sinc_pi(t) * boost::math::sinc_pi(t / static_cast<TReal>(3.0));
        }
        return static_cast<TReal>(0.0);
    }

    TReal support() const
    {
        return static_cast<TReal>(3.0);
    }
};

} } } // namespace KG::Util::Filter

#endif // KGUTIL_RESAMPLE_FILTER_LANCZOS_HPP

