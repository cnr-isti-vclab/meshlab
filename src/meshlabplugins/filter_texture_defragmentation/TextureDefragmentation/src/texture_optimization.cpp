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

#include "texture_optimization.h"

#include "mesh.h"
#include "mesh_graph.h"
#include "timer.h"
#include "texture_rendering.h"
#include "arap.h"
#include "mesh_attribute.h"
#include "logging.h"

#include "shell.h"

#include <vcg/math/histogram.h>

#include <vector>
#include <algorithm>


static void MirrorU(ChartHandle chart);

void ReorientCharts(GraphHandle graph)
{
    for (auto entry : graph->charts) {
        ChartHandle chart = entry.second;
        if (chart->UVFlipped())
            MirrorU(chart);
    }
}

int RotateChartForResampling(ChartHandle chart, const std::set<Mesh::FacePointer>& changeSet, const std::map<RegionID, bool> &flippedInput, bool colorize, double *zeroResamplingArea)
{
    Mesh& m = chart->mesh;
    auto wtcsh = GetWedgeTexCoordStorageAttribute(m);
    *zeroResamplingArea = 0;

    // compute the area contribution of each initial region restricted to
    // faces that have _NOT_ been changed by the optimization

    std::unordered_map<RegionID, double> areaMap;
    std::unordered_map<RegionID, Mesh::FacePointer> idfp;

    for (auto fptr : chart->fpVec) {
        double areaUV = AreaUV(*fptr);
        double area3D = Area3D(*fptr);
        if ((changeSet.find(fptr) == changeSet.end()) && (areaUV != 0)) {
            areaMap[fptr->initialId] += area3D;
            idfp[fptr->initialId] = fptr;
        }
    }

    // if all the faces changed coordinates there is nothing to do
    if (areaMap.size() == 0) {
        return -1;
    }

    // compute the unchanged region with the largest area contribution

    Mesh::FacePointer zeroResamplingAreaFp = nullptr;
    for (auto& entry : areaMap) {
        if (entry.second > *zeroResamplingArea) {
            *zeroResamplingArea = entry.second;
            zeroResamplingAreaFp = idfp[entry.first];
        }
    }

    // compute the rotation angle
    TexCoordStorage tcs = wtcsh[zeroResamplingAreaFp];
    vcg::Point2d d0 = tcs.tc[1].P() - tcs.tc[0].P();
    vcg::Point2d d1 = zeroResamplingAreaFp->WT(1).P() - zeroResamplingAreaFp->WT(0).P();

    if (flippedInput.at(zeroResamplingAreaFp->initialId)) {
        d0.X() *= -1;
    }

    //double rotAngle = ((signedArea > 0) ? -1 : 1) * VecAngle(d0, d1);
    double rotAngle = VecAngle(d0, d1);

    // rotate the uvs
    for (auto fptr : chart->fpVec) {
        for (int i = 0; i < 3; ++i) {
            fptr->WT(i).P().Rotate(rotAngle);
            fptr->V(i)->T().P() = fptr->WT(i).P();
        }
        if (colorize) {
            if ((fptr->initialId == zeroResamplingAreaFp->initialId) && (changeSet.find(fptr) == changeSet.end()))
                fptr->C() = vcg::Color4b(85, 246, 85, 255);
        }
    }

    return tri::Index(chart->mesh, zeroResamplingAreaFp);

}

void TrimTexture(Mesh& m, std::vector<TextureSize>& texszVec, bool unsafeMip)
{
    std::vector<std::vector<Mesh::FacePointer>> facesByTexture;
    unsigned ntex = FacesByTextureIndex(m, facesByTexture);

    auto IsZero = [] (const Mesh::FacePointer fptr) {
        return fptr->WT(0).P() == vcg::Point2d::Zero() && fptr->WT(1).P() == vcg::Point2d::Zero() && fptr->WT(2).P() == vcg::Point2d::Zero();
    };

    for (unsigned ti = 0; ti < ntex; ++ti) {
        vcg::Box2d uvBox;
        for (auto fptr : facesByTexture[ti]) {
            if (!IsZero(fptr)) {
                for (int i = 0; i < 3; ++i) {
                    uvBox.Add(fptr->WT(i).P());
                }
            }
        }

        if (std::min(uvBox.DimX(), uvBox.DimY()) > 0.95)
            continue;

        uvBox.min.Scale(texszVec[ti].w, texszVec[ti].h);
        uvBox.max.Scale(texszVec[ti].w, texszVec[ti].h);
        uvBox.min.X() = std::max(0, int(uvBox.min.X()) - 2);
        uvBox.min.Y() = std::max(0, int(uvBox.min.Y()) - 2);
        uvBox.max.X() = std::min(texszVec[ti].w, int(uvBox.max.X()) + 2);
        uvBox.max.Y() = std::min(texszVec[ti].h, int(uvBox.max.Y()) + 2);

        if (!unsafeMip) {
            // pad the bbox so that MIP artifacts only occur at level above
            const int MAX_SAFE_MIP_LEVEL = 5;
            const int MOD_VAL = (1 << MAX_SAFE_MIP_LEVEL);

            int bboxw = uvBox.max.X() - uvBox.min.X();
            int bboxh = uvBox.max.Y() - uvBox.min.Y();

            int incw = MOD_VAL - (bboxw % MOD_VAL);
            int inch = MOD_VAL - (bboxh % MOD_VAL);

            uvBox.max.X() += incw;
            uvBox.max.Y() += inch;
        }

        double uscale = texszVec[ti].w / uvBox.DimX();
        double vscale = texszVec[ti].h / uvBox.DimY();

        vcg::Point2d t(uvBox.min.X() / texszVec[ti].w, uvBox.min.Y() / texszVec[ti].h);

        for (auto fptr : facesByTexture[ti]) {
            if (!IsZero(fptr)) {
                for (int i = 0; i < 3; ++i) {
                    fptr->WT(i).P() -= t;
                    fptr->WT(i).P().Scale(uscale, vscale);
                    fptr->V(i)->T().P() = fptr->WT(i).P();
                }
            }
        }

        // sanity check
        {
            vcg::Box2d uvBoxCheck;
            for (auto fptr : facesByTexture[ti]) {
                if (!IsZero(fptr)) {
                    for (int i = 0; i < 3; ++i) {
                        uvBoxCheck.Add(fptr->WT(i).P());
                    }
                }
            }

            ensure(uvBoxCheck.min.X() > 0);
            ensure(uvBoxCheck.min.Y() > 0);
            ensure(uvBoxCheck.max.X() < 1);
            ensure(uvBoxCheck.max.X() < 1);
        }

        // resize
        texszVec[ti].w = (int) uvBox.DimX();
        texszVec[ti].h = (int) uvBox.DimY();
    }
}

// -- static functions ---------------------------------------------------------

static void MirrorU(ChartHandle chart)
{
    double u_old = chart->UVBox().min.X();
    for (auto fptr : chart->fpVec) {
        for (int i = 0; i < 3; ++i)
            fptr->WT(i).U() *= -1;
    }
    chart->ParameterizationChanged();
    double u_new = chart->UVBox().min.X();
    for (auto fptr : chart->fpVec) {
        for (int i = 0; i < 3; ++i) {
            fptr->WT(i).U() += (u_old - u_new);
            fptr->V(i)->T().U() = fptr->WT(i).U();
        }
    }
    chart->ParameterizationChanged();
}

