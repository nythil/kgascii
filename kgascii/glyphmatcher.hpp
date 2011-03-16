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

#ifndef KGASCII_GLYPHMATCHER_HPP
#define KGASCII_GLYPHMATCHER_HPP

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/gil/typedefs.hpp>
#include "kgascii_api.hpp"

namespace KG { namespace Ascii {

class FontImage;

class KGASCII_API GlyphMatcher: boost::noncopyable
{
public:
    GlyphMatcher(const FontImage& f);

public:
    int glyphWidth() const;

    int glyphHeight() const;

    char match(const boost::gil::gray8c_view_t& imgv) const;

private:
    int distance(const boost::gil::gray8c_view_t& img1, 
            const boost::gil::gray8c_view_t& img2) const;

private:
    const FontImage& font_;
    std::vector<int> charcodes_;
};

} } // namespace KG::Ascii

#endif // KGASCII_GLYPHMATCHER_HPP

