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

#ifndef KGASCII_GLYPHMATCHERREGISTRATION_HPP
#define KGASCII_GLYPHMATCHERREGISTRATION_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>
#include <boost/function.hpp>
#include <boost/intrusive/list.hpp>

namespace KG { namespace Ascii { namespace Internal {

template<class TFontImage>
class GlyphMatcherRegistry
{
public:
    typedef TFontImage FontImageT;
    typedef DynamicGlyphMatcher<FontImageT> DynamicGlyphMatcherT;
    typedef boost::function<
            boost::shared_ptr<DynamicGlyphMatcherT> (boost::shared_ptr<const FontImageT>, const std::map<std::string, std::string>&)
            > CreatorFuncT;

    class Entry: public boost::intrusive::list_base_hook<>
    {
    public:
        Entry(const std::string& n, const CreatorFuncT& f);

        ~Entry();

    private:
        friend class GlyphMatcherRegistry;

        const std::string name;
        const CreatorFuncT func;
    };

    typedef boost::intrusive::list<Entry> EntryListT;

    static const CreatorFuncT* findFactory(const std::string& name)
    {
        for (typename EntryListT::iterator i = registrations().begin(),
                                           e = registrations().end(); i != e; ++i) {
            if (i->name == name) {
                return &i->func;
            }
        }
        return 0;
    }

private:
    friend class Entry;

    static void addEntry(Entry& entry)
    {
        registrations().push_back(entry);
    }

    static void removeEntry(Entry& entry)
    {
        registrations().erase(EntryListT::s_iterator_to(entry));
    }

    static EntryListT& registrations()
    {
        static EntryListT registry;
        return registry;
    }
};

template<class TFontImage>
GlyphMatcherRegistry<TFontImage>::Entry::Entry(const std::string& n, const CreatorFuncT& f)
    :name(n), func(f)
{
    addEntry(*this);
}

template<class TFontImage>
GlyphMatcherRegistry<TFontImage>::Entry::~Entry()
{
    removeEntry(*this);
}

template<class TFontImage>
class GlyphMatcherRegistry<const TFontImage>: public GlyphMatcherRegistry<TFontImage>
{
};

template<class TFontImage, template<class> class TFactory>
class GlyphMatcherRegistration
{
public:
    typedef TFactory<TFontImage> FactoryT;

    explicit GlyphMatcherRegistration(const std::string& name, const FactoryT& f=FactoryT())
        :entry_(name, f)
    {
    }

private:
    typename GlyphMatcherRegistry<TFontImage>::Entry entry_;
};

} } } // namespace KG::Ascii::Internal

#endif // KGASCII_GLYPHMATCHERREGISTRATION_HPP

