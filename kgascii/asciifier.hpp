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
#include <kgascii/kgascii_api.hpp>
#include <kgascii/surface.hpp>

namespace KG { namespace Ascii {

class GlyphMatcherContext;
class TextSurface;

class KGASCII_API Asciifier: boost::noncopyable
{
public:
    virtual ~Asciifier();

public:
    virtual const GlyphMatcherContext& context() const = 0;
    
    virtual unsigned threadCount() const = 0;

public:
    virtual void generate(const Surface8c& imgv, 
            TextSurface& text) = 0;

protected:
    Asciifier();
};

} } // namespace KG::Ascii

#endif // KGASCII_ASCIIFIER_HPP

