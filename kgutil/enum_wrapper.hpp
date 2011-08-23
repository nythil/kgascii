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

#ifndef KGUTIL_ENUM_WRAPPER_HPP
#define KGUTIL_ENUM_WRAPPER_HPP

#include <string>
#include <istream>
#include <ostream>
#include <iomanip>
#include <boost/bimap.hpp>
#include <boost/operators.hpp>

namespace KG { namespace Util {

template<class TEnumClass, typename TInnerValue=typename TEnumClass::Value>
class EnumWrapper
    : public TEnumClass
    , boost::totally_ordered< EnumWrapper<TEnumClass, TInnerValue> >
{
    typedef typename TEnumClass::Value Value;
    typedef boost::bimaps::bimap<
        boost::bimaps::set_of<Value>, 
        boost::bimaps::set_of<std::string>
    > ValueNameMap;

    TInnerValue value_;

public:
    EnumWrapper(): value_(Value())
    {
    }

    EnumWrapper(Value val): value_(val)
    {
    }

    TInnerValue innerValue() const
    {
        return value_;
    }

    Value value() const
    {
        return static_cast<Value>(value_);
    }

    friend bool operator ==(const EnumWrapper& lh, const EnumWrapper& rh)
    {
        return lh.value() == rh.value();
    }

    friend bool operator <(const EnumWrapper& lh, const EnumWrapper& rh)
    {
        return lh.value() < rh.value();
    }

    static const std::string& asString(const EnumWrapper& val)
    {
        return getValueNameMap().left.at(val.value());
    }

    static EnumWrapper parse(const std::string& name)
    {
        return EnumWrapper::getValueNameMap().right.at(name);
    }

    friend std::ostream& operator <<(std::ostream& ostr, const EnumWrapper& val)
    {
        try {
            ostr << EnumWrapper::asString(val);
        } catch (const std::out_of_range&) {
            ostr.setstate(std::ios::failbit);
        }
        return ostr;
    }

    friend std::istream& operator >>(std::istream& istr, EnumWrapper& val)
    {
        std::string buf;
        istr >> std::setw(EnumWrapper::getMaxNameLength()) >> buf;
        try {
            val = EnumWrapper::parse(buf);
        } catch (const std::out_of_range&) {
            istr.setstate(std::ios::failbit);
        }
        return istr;
    }

private:
    class ValueNameMapInserter
    {
        ValueNameMap& map_;
    public:
        explicit ValueNameMapInserter(ValueNameMap& m): map_(m)
        {
        }

        ValueNameMapInserter& insert(const Value& val, const std::string& name)
        {
            map_.insert(typename ValueNameMap::value_type(val, name));
            return *this;
        }

        ValueNameMapInserter& operator()(const Value& val, const std::string& name)
        {
            return insert(val, name);
        }
    };

    static const ValueNameMap& getValueNameMap()
    {
        static ValueNameMap map;
        if (map.empty()) {
            TEnumClass::fillValueNameMap(ValueNameMapInserter(map));
        }
        return map;
    }

    static unsigned getMaxNameLength()
    {
        static unsigned max_length = 0;
        if (max_length == 0) {
            max_length = 1;
            const ValueNameMap& vn_map = getValueNameMap();
            for (typename ValueNameMap::const_iterator 
                 it = vn_map.begin(), it_end = vn_map.end(); 
                 it != it_end; ++it) 
            {
                max_length = std::max(max_length, it->right.size());
            }
        }
        return max_length;
    }
};

} } // namespace KG::Util

#endif // KGUTIL_ENUM_WRAPPER_HPP
