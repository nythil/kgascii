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

#ifndef KGUTIL_RESAMPLE_HPP
#define KGUTIL_RESAMPLE_HPP

#include "resample/resampler.hpp"


namespace KG { namespace Util {

template<class TFilter, class TSrcView, class TDstView>
void resample(const TSrcView& sview, const TDstView& dview, const TFilter& filter=TFilter())
{
    Resampler<TFilter> resampler(sview.width(), sview.height(), dview.width(), dview.height(), filter);
    resampler.apply(sview, dview);
}

} } // namespace KG::Util

#endif // KGUTIL_RESAMPLE_HPP

