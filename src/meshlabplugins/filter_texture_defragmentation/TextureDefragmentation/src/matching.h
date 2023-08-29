/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef MATCHING_H
#define MATCHING_H

#include <vcg/space/point2.h>

#include <vector>

/* Class that encodes a matching transform. The method Apply transforms the
 * given point according to the matching encoded in the object */
struct MatchingTransform {
    vcg::Point2d t; // the translation of the matching transform
    double matCoeff[4]; // the linear portion of the matching transform, row-maj 2x2

    inline vcg::Point2d Apply(const vcg::Point2d &q) const
    {
        return vcg::Point2d(matCoeff[0] * q.X() + matCoeff[1] * q.Y() + t.X(),
                            matCoeff[2] * q.X() + matCoeff[3] * q.Y() + t.Y());
    }

    static inline MatchingTransform Identity()
    {
        return {vcg::Point2d::Zero(), { 1.0, 0.0, 0.0, 1.0 }};
    }

};

/* Computes the least squares affine transform of the matchingVector points to
 * the targetVector points */
MatchingTransform ComputeMatchingMatrix(const std::vector<vcg::Point2d>& targetVector, const std::vector<vcg::Point2d>& matchingVector);

/* Computes the least squares similarity transform of the matchingVector points
 * to the targetVector points */
MatchingTransform ComputeMatchingSimilarityMatrix(const std::vector<vcg::Point2d>& targetVector, const std::vector<vcg::Point2d>& matchingVector);

/* Computes the least squares rigid (non-scaled) transform of the matchingVector
 * points to the targetVector points */
MatchingTransform ComputeMatchingRigidMatrix(const std::vector<vcg::Point2d>& targetVector, const std::vector<vcg::Point2d>& matchingVector);

/* Computes the average matching error applied to the given point sequences */
double MatchingError(const MatchingTransform& matching, const std::vector<vcg::Point2d>& points1, const std::vector<vcg::Point2d>& points2);
double MatchingErrorAverage(const MatchingTransform& matching, const std::vector<vcg::Point2d>& points1, const std::vector<vcg::Point2d>& points2);

/* Computes the absolute matching error applied to the given point sequences */
double MatchingErrorTotal(const MatchingTransform& matching, const std::vector<vcg::Point2d>& points1, const std::vector<vcg::Point2d>& points2);

#endif // MATCHING_H
