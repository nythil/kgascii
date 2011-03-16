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

#ifndef KGASCII_ASCIIFIER_HPP
#define KGASCII_ASCIIFIER_HPP

#include <boost/noncopyable.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include "kgascii_api.hpp"

namespace KG { namespace Ascii {

class GlyphMatcher;
class TextSurface;

class KGASCII_API Asciifier: boost::noncopyable
{
public:
    Asciifier(const GlyphMatcher& m);

public:
    void generate(const boost::gil::gray8c_view_t& imgv, TextSurface& text) const;

private:
    const GlyphMatcher& matcher_;
    mutable boost::gil::gray8_image_t cornerImg_;
};

} } // namespace KG::Ascii

#endif // KGASCII_ASCIIFIER_HPP

