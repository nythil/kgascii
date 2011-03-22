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

#ifndef KGASCII_DYNAMICASCIIFIER_HPP
#define KGASCII_DYNAMICASCIIFIER_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <kgascii/asciifier.hpp>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

class KGASCII_API DynamicAsciifier: public Asciifier
{
public:
    DynamicAsciifier(const GlyphMatcherContext& c);

    ~DynamicAsciifier();
    
public:
    const GlyphMatcherContext& context() const;

    unsigned threadCount() const;

public:
    void generate(const Surface8c& imgv, TextSurface& text);

    void setSequential();

    void setParallel(unsigned cnt);

private:
    const GlyphMatcherContext& context_;
    boost::scoped_ptr<Asciifier> strategy_;
};

} } // namespace KG::Ascii

#endif // KGASCII_DYNAMICASCIIFIER_HPP


