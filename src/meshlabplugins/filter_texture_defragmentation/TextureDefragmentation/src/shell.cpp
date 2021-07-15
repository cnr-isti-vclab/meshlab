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

#include "shell.h"
#include "mesh.h"
#include "utils.h"
#include "logging.h"
#include "mesh_graph.h"
#include "mesh_attribute.h"

#include "timer.h"

#include <vcg/complex/algorithms/hole.h>

#include <vector>


static bool Build(Mesh& shell, FaceGroup& fg);

/*
 * Convention: s0 > s1
 * downscaleFactor determines the uv area shrinking. We discussed two different
 * possibilities:
 *   1. ds defines a `linear' downscaling on the largest singular value, the
 *      generator F = USV of the new target shape uses S={s0', s1'} such that
 *        s0' = k * s0
 *        s1' = min(s1, s0')
 *
 *   2. ds defines the scaling factor of the parameterization, and we compute
 *      the singular values accordingly. (there are some notes of mine on
 *      overleaf). Essentially, we can find a scaling threshold such that,
 *      above this threshold we shrink only the largest sing.val. s1, below
 *      this threshold we set s1' = s0' = k * s0 for a suitable k
 *      INTUITION: a square mapped to a rectangle, we can reduce the area of the
 *      rectangle by making it more and more like a square, shrinking only
 *      one dimension. At some point the rectangle becomes a square, and we
 *      start making it smaller until we reach the target area
 *
 * */
bool BuildShellWithTargetsFromUV(Mesh& shell, FaceGroup& fg, double downsamplingFactor)
{
    bool singleComponent = Build(shell, fg);

    auto ia_ = GetFaceIndexAttribute(shell);
    for (unsigned i = 0; i < fg.FN(); ++i) {
        ensure(tri::Index(fg.mesh, fg.fpVec[i]) == (unsigned) ia_[shell.face[i]]);
    }

    Mesh& m = fg.mesh;

    double targetArea = 0;

    auto sa = GetShell3DShapeAttribute(shell);
    auto ia = GetFaceIndexAttribute(shell);
    auto tsa = GetTargetShapeAttribute(shell);
    auto wtcsa = GetWedgeTexCoordStorageAttribute(m);

    for (auto& sf : shell.face) {
        CoordStorage target;
        auto& f = m.face[ia[sf]];

        // Interpolate between texture and mesh face shapes to mitigate distortion
        const Point2d& u0 = wtcsa[f].tc[0].P();
        const Point2d& u1 = wtcsa[f].tc[1].P();
        const Point2d& u2 = wtcsa[f].tc[2].P();
        Point2d u10 = u1 - u0;
        Point2d u20 = u2 - u0;
        double area = std::abs(u10 ^ u20) / 2.0;

        if (area == 0) {
            // just scale everything by the linear scaling factor
            target.P[0] = vcg::Point3d(u0.X(), u0.Y(), 0) * downsamplingFactor;
            target.P[1] = vcg::Point3d(u1.X(), u1.Y(), 0) * downsamplingFactor;
            target.P[2] = vcg::Point3d(u2.X(), u2.Y(), 0) * downsamplingFactor;
        } else {
            // Compute the matrix of the input mapping and its SVD
            Point2d x10;
            Point2d x20;
            LocalIsometry(f.P(1) - f.P(0), f.P(2) - f.P(0), x10, x20);
            Eigen::Matrix2d A = ComputeTransformationMatrix(x10, x20, u10, u20);

            Eigen::Matrix2d U;
            Eigen::Matrix2d V;
            Eigen::Vector2d s;
            Eigen::JacobiSVD<Eigen::Matrix2d> svd;
            svd.compute(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
            U = svd.matrixU();
            V = svd.matrixV();
            s = svd.singularValues();
            ensure(s[0] >= s[1]);

            // Compute the 'generator' matrix  and the target shape
            Eigen::Vector2d sNew;

            // First strategy - capping the texel-per-dim allocation
            sNew[0] = s[0] * downsamplingFactor;
            sNew[1] = std::min(sNew[0], s[1]);

            Eigen::Matrix2d gen = U * sNew.asDiagonal() * V.transpose();
            
            Eigen::Vector2d t10 = gen * Eigen::Vector2d(x10[0], x10[1]);
            Eigen::Vector2d t20 = gen * Eigen::Vector2d(x20[0], x20[1]);

            target.P[0] = Point3d(0, 0, 0);
            target.P[1] = Point3d(t10[0], t10[1], 0);
            target.P[2] = Point3d(t20[0], t20[1], 0);
        }

        tsa[sf] = target;
        targetArea += ((target.P[1] - target.P[0]) ^ (target.P[2] - target.P[0])).Norm() / 2.0;

        ensure(std::isfinite(targetArea));

        sa[sf].P[0] = sf.P(0);
        sa[sf].P[1] = sf.P(1);
        sa[sf].P[2] = sf.P(2);
    }

    return singleComponent;
}

void CloseHoles3D(Mesh& shell)
{
    Timer t;

    int startFN = shell.FN();

    auto ia = GetFaceIndexAttribute(shell);
    auto tsa = GetTargetShapeAttribute(shell);

    // Use the target area of the original faces (which should have been already computed) to
    // compute the scaling factors for the target triangles of the hole-filling faces
    double surfaceArea = 0;
    double targetArea = 0;
    for (auto& sf : shell.face) {
        targetArea += ((tsa[sf].P[1] - tsa[sf].P[0]) ^ (tsa[sf].P[2] - tsa[sf].P[0])).Norm() / 2.0;
        surfaceArea += vcg::DoubleArea<MeshFace>(sf) / 2.0;
    }

    double scale = std::sqrt(targetArea / surfaceArea);

    // Get border info
    ComputeBoundaryInfoAttribute(shell);
    BoundaryInfo& info = GetBoundaryInfoAttribute(shell)();

    // Leave only the longest boundary
    tri::UpdateFlags<Mesh>::FaceClearS(shell);
    ensure(info.vBoundaryFaces.size() > 0 && "Mesh has no boundaries");
    if (info.vBoundaryFaces.size() > 1) {
        std::size_t k = info.LongestBoundary();
        // select all the boundary faces
        for (std::size_t i = 0; i < info.vBoundaryFaces.size(); ++i) {
            if (i == k) continue;
            for (auto j : info.vBoundaryFaces[i]) {
                ensure(face::IsBorder(shell.face[j], 0) || face::IsBorder(shell.face[j], 1) || face::IsBorder(shell.face[j], 2));
                shell.face[j].SetS();
            }
        }
        tri::Hole<Mesh>::EarCuttingFill<tri::TrivialEar<Mesh>>(shell, shell.FN(), true);
    }

    tri::Allocator<Mesh>::CompactFaceVector(shell);
    ensure(shell.FN() == (int) shell.face.size());

    for (auto& sf : shell.face) {
        if (int(tri::Index(shell, sf)) >= startFN) {
            sf.SetHoleFilling();
            ia[sf] = -1;
            double area = Area3D(sf);
            for (int i = 0; i < 3; ++i) {
                vcg::Point2d wti = sf.V(i)->T().P();
                sf.WT(i).P() = wti;

                // if the hole-filling face is zero-area (which can happen if the shell has been cut along seams)
                // use the texture coordinates at the vertices to determine the target shape, otherwise
                // use the scaled 3D shape. This is just to avoid numerical issues when using the shell to solve arap instances
                if (area == 0)
                    tsa[sf].P[i] = vcg::Point3d(wti.X(), wti.Y(), 0);
                else
                    tsa[sf].P[i] = sf.P(i) * scale;
            }
        }
    }

    tri::UpdateTopology<Mesh>::FaceFace(shell);
    tri::UpdateTopology<Mesh>::VertexFace(shell);
}

void SyncShellWithUV(Mesh& shell)
{
    for (auto& v : shell.vert) {
        v.P().X() = v.T().U();
        v.P().Y() = v.T().V();
        v.P().Z() = 0.0;
    }
    tri::UpdateBounding<Mesh>::Box(shell);
}

void SyncShellWith3D(Mesh& shell)
{
    auto sa = GetShell3DShapeAttribute(shell);
    for (auto& sf : shell.face) {
        ensure(sf.IsMesh());
        for (int i = 0; i < 3; ++i)
            sf.P(i) = sa[sf].P[i];
    }
    tri::UpdateBounding<Mesh>::Box(shell);
}

void ClearHoleFillingFaces(Mesh& shell, bool holefill, bool scaffold)
{
    for (auto& f : shell.face)
        if ((holefill && f.IsHoleFilling()) || (scaffold && f.IsScaffold()))
            tri::Allocator<Mesh>::DeleteFace(shell, f);

    tri::Clean<Mesh>::RemoveUnreferencedVertex(shell);
    tri::UpdateTopology<Mesh>::FaceFace(shell);
    tri::UpdateTopology<Mesh>::VertexFace(shell);
    tri::Allocator<Mesh>::CompactEveryVector(shell);
}

static bool Build(Mesh& shell, FaceGroup& fg)
{
    CopyToMesh(fg, shell);

    tri::Clean<Mesh>::RemoveDuplicateVertex(shell);
    tri::Allocator<Mesh>::CompactEveryVector(shell);

    tri::UpdateBounding<Mesh>::Box(shell);
    tri::UpdateTopology<Mesh>::FaceFace(shell);

    return tri::Clean<Mesh>::CountConnectedComponents(shell) == 1;
}
