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

#ifndef KGASCII_PCARECONSTRUCTIONFONTLOADER_HPP
#define KGASCII_PCARECONSTRUCTIONFONTLOADER_HPP

#include <iterator>
#include <set>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/shared_ptr.hpp>

namespace KG { namespace Ascii {

template<class TPrincipalComponents, class TImage=typename TPrincipalComponents::FontImageT::ImageT>
class PcaReconstructionFontLoader: boost::noncopyable
{
public:
    typedef TImage ImageT;
    typedef typename ImageT::view_t ViewT;
    typedef typename ImageT::const_view_t ConstViewT;
    typedef typename TPrincipalComponents::FontImageT FontImageT;
    typedef std::set<Symbol> SymbolCollectionT;

public:
    explicit PcaReconstructionFontLoader(boost::shared_ptr<const TPrincipalComponents> pca)
        :pca_(pca)
    {
        glyphData_.recreate(pca_->font()->glyphWidth(), pca_->font()->glyphHeight());
    }

public:
    std::string familyName() const
    {
        return pca_->font()->familyName();
    }

    std::string styleName() const
    {
        return pca_->font()->styleName();
    }

    unsigned pixelSize() const
    {
        return pca_->font()->pixelSize();
    }

    unsigned glyphWidth() const
    {
        return pca_->font()->glyphWidth();
    }

    unsigned glyphHeight() const
    {
        return pca_->font()->glyphHeight();
    }

    SymbolCollectionT symbols() const
    {
        using namespace boost;
        SymbolCollectionT result;
        transform(irange<size_t>(0, pca_->font()->glyphCount()),
            std::inserter(result, result.begin()),
            boost::bind(&FontImageT::getSymbol, pca_->font(), _1));
        return result;
    }

    bool loadGlyph(Symbol charcode)
    {
        for (size_t i = 0; i < pca_->font()->glyphCount(); ++i) {
            if (pca_->font()->getSymbol(i) != charcode) continue;

            Eigen::VectorXf proj_glyph_vec = pca_->glyphs().col(i);
            Eigen::VectorXf glyph_vec = pca_->combine(proj_glyph_vec);
            glyph_vec = glyph_vec.cwiseMax(Eigen::VectorXf::Zero(glyph_vec.size()));
            glyph_vec = glyph_vec.cwiseMin(Eigen::VectorXf::Ones(glyph_vec.size()));
            boost::gil::copy_and_convert_pixels(
                    boost::gil::interleaved_view(
                            pca_->font()->glyphWidth(), pca_->font()->glyphHeight(),
                            reinterpret_cast<const boost::gil::gray32f_pixel_t*>(glyph_vec.data()),
                            pca_->font()->glyphWidth() * sizeof(float)),
                    boost::gil::view(glyphData_));
            return true;
        }

        return false;
    }

    ConstViewT glyph() const
    {
        return boost::gil::const_view(glyphData_);
    }

private:
    boost::shared_ptr<const TPrincipalComponents> pca_;
    ImageT glyphData_;
};

} } // namespace KG::Ascii

#endif // KGASCII_PCARECONSTRUCTIONFONTLOADER_HPP

