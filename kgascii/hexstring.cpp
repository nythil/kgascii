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

#include "hexstring.hpp"
#include <boost/range.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace kgAscii {

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;

struct out_hexchar_s: public karma::symbols<unsigned, char>
{
    out_hexchar_s()
    {
        add(0, '0')(1, '1')(2, '2')(3, '3');
        add(4, '4')(5, '5')(6, '6')(7, '7');
        add(8, '8')(9, '9')(10, 'A')(11, 'B');
        add(12, 'C')(13, 'D')(14, 'E')(15, 'F');
    }
} out_hexchar;

struct in_hexchar_s: public qi::symbols<char, unsigned>
{
    in_hexchar_s()
    {
        add("0", 0)("1", 1)("2", 2)("3", 3);
        add("4", 4)("5", 5)("6", 6)("7", 7);
        add("8", 8)("9", 9)("A", 10)("B", 11);
        add("C", 12)("D", 13)("E", 14)("F", 15);
    }
} in_hexchar;

template<typename OutputIterator, typename Container>
struct out_hexstring_grammar: public karma::grammar<OutputIterator, Container()>
{
    out_hexstring_grammar()
        :out_hexstring_grammar::base_type(hexstring)
    {
        hexstring = *hexpair;
        hexpair = out_hexchar[karma::_1 = karma::_val / 16] << out_hexchar[karma::_1 = karma::_val % 16];
    }
    karma::rule<OutputIterator, Container()> hexstring;
    karma::rule<OutputIterator, unsigned()> hexpair;

    static out_hexstring_grammar instance;
};

template<typename OutputIterator, typename Container>
out_hexstring_grammar<OutputIterator, Container>
    out_hexstring_grammar<OutputIterator, Container>::instance;

template<typename Iterator, typename OutputIterator>
struct in_hexstring_grammar: public qi::grammar<Iterator, OutputIterator()>
{
    in_hexstring_grammar()
        :in_hexstring_grammar::base_type(hexstring)
    {
        hexstring = *(hexpair[*qi::_val++ = qi::_1]);
        hexpair = in_hexchar[qi::_val = qi::_1 * 16] >> in_hexchar[qi::_val += qi::_1];
    }
    qi::rule<Iterator, OutputIterator()> hexstring;
    qi::rule<Iterator, unsigned()> hexpair;

    static in_hexstring_grammar instance;
};

template<typename Iterator, typename OutputIterator>
in_hexstring_grammar<Iterator, OutputIterator>
    in_hexstring_grammar<Iterator, OutputIterator>::instance;

template<typename OutputIterator, typename Container>
void generate_hexstring(OutputIterator& sink, const Container& v)
{
    karma::generate(sink, out_hexstring_grammar<OutputIterator, Container>::instance, v);
}

template<typename Iterator, typename OutputIterator>
void parse_hexstring(Iterator first, Iterator last, OutputIterator& v)
{
    qi::parse(first, last, in_hexstring_grammar<Iterator, OutputIterator>::instance, v);
}

std::string hexlify(const unsigned char* first, const unsigned char* last)
{
    std::string output;
    std::back_insert_iterator<std::string> sink(output);
    generate_hexstring(sink, boost::make_iterator_range(first, last));
    return output;
}

unsigned char* unhexlify(const std::string& chars, unsigned char* dst)
{
    parse_hexstring(chars.begin(), chars.end(), dst);
    return dst;
}

}

