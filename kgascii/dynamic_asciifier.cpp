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

#include <kgascii/dynamic_asciifier.hpp>
#include <kgascii/sequential_asciifier.hpp>
#include <kgascii/parallel_asciifier.hpp>

namespace KG { namespace Ascii {

DynamicAsciifier::DynamicAsciifier(const GlyphMatcherContext* c)
    :Asciifier()
    ,context_(c)
{
    setSequential();
}

DynamicAsciifier::~DynamicAsciifier()
{
}

const GlyphMatcherContext* DynamicAsciifier::context() const
{
    return context_;
}

unsigned DynamicAsciifier::threadCount() const
{
    return strategy_->threadCount();
}

void DynamicAsciifier::generate(const Surface8c& imgv, TextSurface& text)
{
    strategy_->generate(imgv, text);
}

void DynamicAsciifier::setSequential()
{
    strategy_.reset(new SequentialAsciifier(context_));
}

void DynamicAsciifier::setParallel(unsigned cnt)
{
    strategy_.reset(new ParallelAsciifier(context_, cnt));
}

} } // namespace KG::Ascii


