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

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <kgascii/internal/traits.hpp>
#include <kgascii/surface_fwd.hpp>
#include <kgascii/symbol.hpp>

namespace KG { namespace Ascii {

template<typename TDerived>
class GlyphMatcherContext: boost::noncopyable
{
public:
    typedef typename Internal::Traits<TDerived>::GlyphMatcherT GlyphMatcherT;
    typedef typename Internal::Traits<TDerived>::FontImageT FontImageT;
    typedef typename Internal::Traits<TDerived>::SurfaceT SurfaceT;
    typedef typename Internal::Traits<TDerived>::ConstSurfaceT ConstSurfaceT;

public:
    TDerived& derived()
    {
        return static_cast<TDerived&>(*this);
    }

    const TDerived& derived() const
    {
        return static_cast<const TDerived&>(*this);
    }

    const FontImageT* font() const
    {
        return font_;
    }

    unsigned cellWidth() const
    {
        return font_->glyphWidth();
    }

    unsigned cellHeight() const
    {
        return font_->glyphHeight();
    }

    Symbol match(const ConstSurfaceT& imgv) const
    {
        boost::scoped_ptr<GlyphMatcherT> matcher(createMatcher());
        return matcher->match(imgv);
    }

    GlyphMatcherT* createMatcher() const
    {
        return derived().createMatcher();
    }

protected:
    GlyphMatcherContext(const FontImageT* f)
        :font_(f)
    {
    }

private:
    const FontImageT* font_;
};

} } // namespace KG::Ascii

#endif // KGASCII_GLYPHMATCHER_HPP

