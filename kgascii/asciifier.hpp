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
#include <kgascii/internal/traits.hpp>
#include <kgascii/text_surface.hpp>

namespace KG { namespace Ascii {

template<typename TDerived>
class Asciifier: boost::noncopyable
{
public:
    typedef typename Internal::Traits<TDerived>::GlyphMatcherContextT GlyphMatcherContextT;
    typedef typename Internal::Traits<TDerived>::GlyphMatcherT GlyphMatcherT;
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

    const GlyphMatcherContextT* context() const
    {
        return derived().context();
    }
    
    unsigned threadCount() const
    {
        return derived().threadCount();
    }

    void generate(const ConstSurfaceT& imgv, TextSurface& text)
    {
        derived().generate(imgv, text);
    }

protected:
    Asciifier()
    {
    }
};

} } // namespace KG::Ascii

#endif // KGASCII_ASCIIFIER_HPP
