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

#ifndef KGASCII_GLYPHMATCHERCONTEXT_HPP
#define KGASCII_GLYPHMATCHERCONTEXT_HPP

#include <boost/noncopyable.hpp>
#include <kgascii/kgascii_api.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

class GlyphMatcher;
class FontImage;

class KGASCII_API GlyphMatcherContext: boost::noncopyable
{
public:
    virtual ~GlyphMatcherContext();

public:
    const FontImage& font() const;

    virtual unsigned cellWidth() const;

    virtual unsigned cellHeight() const;

    virtual unsigned match(const Surface8c& imgv) const;

    virtual GlyphMatcher* createMatcher() const = 0;

protected:
    GlyphMatcherContext(const FontImage& f);

private:
    const FontImage& font_;
};

} } // namespace KG::Ascii

#endif // KGASCII_GLYPHMATCHERCONTEXT_HPP

