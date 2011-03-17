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

#include "dynamicasciifier.hpp"
#include "sequentialasciifier.hpp"
#include "parallelasciifier.hpp"

namespace KG { namespace Ascii {

using namespace boost::gil;

DynamicAsciifier::DynamicAsciifier(const GlyphMatcher& m)
    :Asciifier(m)
{
    setSequential();
}

DynamicAsciifier::~DynamicAsciifier()
{
}

void DynamicAsciifier::generate(const gray8c_view_t& imgv, TextSurface& text)
{
    strategy_->generate(imgv, text);
}

size_t DynamicAsciifier::threadCount() const
{
    return strategy_->threadCount();
}

void DynamicAsciifier::setSequential()
{
    strategy_.reset(new SequentialAsciifier(matcher()));
}

void DynamicAsciifier::setParallel(size_t cnt)
{
    strategy_.reset(new ParallelAsciifier(matcher(), cnt));
}

} } // namespace KG::Ascii


