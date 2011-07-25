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

#include <boost/noncopyable.hpp>
#include <kgascii/surface_container.hpp>
#include <kgascii/font_pca.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/surface_algorithm.hpp>

namespace KG { namespace Ascii {

class PcaReconstructionFontLoader: boost::noncopyable
{
public:
    explicit PcaReconstructionFontLoader(const FontPCA<PixelType8>* pca)
        :pca_(pca)
    {
        glyphData_.resize(pca_->font()->glyphWidth(), pca_->font()->glyphHeight());
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

    bool loadGlyph(Symbol charcode)
    {
        for (size_t i = 0; i < pca_->font()->glyphCount(); ++i) {
            if (pca_->font()->getSymbol(i) != charcode) continue;

            Eigen::VectorXf proj_glyph_vec = pca_->glyphs().col(i);
            Eigen::VectorXf glyph_vec = pca_->combine(proj_glyph_vec);
            glyph_vec = glyph_vec.cwiseMax(Eigen::VectorXf::Zero(glyph_vec.size()));
            glyph_vec = glyph_vec.cwiseMin(255 * Eigen::VectorXf::Ones(glyph_vec.size()));
            copyPixels(glyph_vec, glyphData_.surface());
            return true;
        }

        return false;
    }

    Surface8c glyph() const
    {
        return glyphData_.surface();
    }

private:
    const FontPCA<PixelType8>* pca_;
    SurfaceContainer8 glyphData_;
};

} } // namespace KG::Ascii

#endif // KGASCII_PCARECONSTRUCTIONFONTLOADER_HPP

