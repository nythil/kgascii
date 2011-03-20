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

#include "glyphmatchercontext.hpp"
#include "glyphmatcher.hpp"
#include "fontimage.hpp"
#include <boost/scoped_ptr.hpp>

namespace KG { namespace Ascii {

GlyphMatcherContext::GlyphMatcherContext(const FontImage& f)
    :font_(f)
{
}

GlyphMatcherContext::~GlyphMatcherContext()
{
}

const FontImage& GlyphMatcherContext::font() const
{
    return font_;
}

int GlyphMatcherContext::cellWidth() const
{
    return font_.glyphWidth();
}

int GlyphMatcherContext::cellHeight() const
{
    return font_.glyphHeight();
}

char GlyphMatcherContext::match(const boost::gil::gray8c_view_t& imgv) const
{
    boost::scoped_ptr<GlyphMatcher> matcher(createMatcher());
    return matcher->match(imgv);
}

} } // namespace KG::Ascii

