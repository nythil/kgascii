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

#ifndef KGASCII_SEQUENTIALASCIIFIER_HPP
#define KGASCII_SEQUENTIALASCIIFIER_HPP

#include <boost/noncopyable.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/scoped_ptr.hpp>
#include "asciifier.hpp"
#include "kgascii_api.hpp"

namespace KG { namespace Ascii {

class GlyphMatcher;

class KGASCII_API SequentialAsciifier: public Asciifier
{
public:
    SequentialAsciifier(const GlyphMatcherContext& c);
    
public:
    const GlyphMatcherContext& context() const;

    size_t threadCount() const;

public:
    void generate(const boost::gil::gray8c_view_t& imgv, TextSurface& text);

private:
    const GlyphMatcherContext& context_;
    boost::scoped_ptr<GlyphMatcher> matcher_;
};

} } // namespace KG::Ascii

#endif // KGASCII_SEQUENTIALASCIIFIER_HPP

