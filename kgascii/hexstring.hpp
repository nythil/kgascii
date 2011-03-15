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

#ifndef KGASCII_HEXSTRING_HPP
#define KGASCII_HEXSTRING_HPP

#include <string>
#include <vector>

#include "kgascii_api.hpp"

namespace KG { namespace Ascii {

std::string KGASCII_API hexlify(const unsigned char* first, const unsigned char* last);

unsigned char* KGASCII_API unhexlify(const std::string& chars, unsigned char* dst);

} } // namespace KG::Ascii

#endif // KGASCII_HEXSTRING_HPP

