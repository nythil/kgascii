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

#ifndef KGASCII_FONT_PCA_HPP
#define KGASCII_FONT_PCA_HPP

#include <Eigen/Dense>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

class FontImage;
class FontPCAnalyzer;

class FontPCA
{
public:
    FontPCA(const FontPCAnalyzer* analyzer, size_t feat_cnt);

public:
    Eigen::VectorXf combine(const Eigen::VectorXf& vec) const;

    Eigen::VectorXf& combine(const Eigen::VectorXf& vec, Eigen::VectorXf& out) const;

    Eigen::VectorXf project(const Eigen::VectorXf& vec) const;

    Eigen::VectorXf& project(const Eigen::VectorXf& vec, Eigen::VectorXf& out) const;

    size_t findClosestGlyph(const Eigen::VectorXf& vec) const;

public:
    size_t featureCount() const;

    const FontImage* font() const;

    const Eigen::VectorXf& mean() const;

    const Eigen::VectorXf& energies() const;

    const Eigen::MatrixXf& features() const;

    const Eigen::MatrixXf& glyphs() const;

private:
    const FontImage* font_;
    Eigen::VectorXf mean_;
    Eigen::VectorXf energies_;
    Eigen::MatrixXf features_;
    Eigen::MatrixXf glyphs_;
};

} } // namespace KG::Ascii

#endif // KGASCII_FONT_PCA_HPP

