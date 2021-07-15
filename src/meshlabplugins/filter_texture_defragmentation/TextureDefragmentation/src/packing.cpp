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

#include "packing.h"
#include "texture_object.h"
#include "mesh_graph.h"
#include "logging.h"
#include "utils.h"
#include "mesh_attribute.h"

#include <vcg/complex/algorithms/outline_support.h>
#include <vcg/space/rasterized_outline2_packer.h>
#include <wrap/qt/outline2_rasterizer.h>
//#include <wrap/qt/Outline2ToQImage.h>

typedef vcg::RasterizedOutline2Packer<float, QtOutline2Rasterizer> RasterizationBasedPacker;


int Pack(const std::vector<ChartHandle>& charts, TextureObjectHandle textureObject, std::vector<TextureSize>& texszVec)
{
    // Pack the atlas

    texszVec.clear();

    std::vector<Outline2f> outlines;

    for (auto c : charts) {
        // Save the outline of the parameterization for this portion of the mesh
        Outline2f outline = ExtractOutline2f(*c);
        outlines.push_back(outline);
    }

    int packingSize = 4096;
    std::vector<std::pair<double,double>> trs = textureObject->ComputeRelativeSizes();

    std::vector<Point2i> containerVec;
    for (auto rs : trs) {
        vcg::Point2i container(packingSize * rs.first, packingSize * rs.second);
        containerVec.push_back(container);
    }

    // compute the scale factor for the packing
    int packingArea = 0;
    int textureArea = 0;
    for (unsigned i = 0; i < containerVec.size(); ++i) {
        packingArea += containerVec[i].X() * containerVec[i].Y();
        textureArea += textureObject->TextureWidth(i) * textureObject->TextureHeight(i);
    }
    double packingScale = std::sqrt(packingArea / (double) textureArea);

    RasterizationBasedPacker::Parameters packingParams;
    packingParams.costFunction = RasterizationBasedPacker::Parameters::LowestHorizon;
    packingParams.doubleHorizon = false;
    packingParams.innerHorizon = true;
    //packingParams.permutations = false;
    packingParams.permutations = (charts.size() < 50);
    packingParams.rotationNum = 4;
    packingParams.gutterWidth = 4;
    packingParams.minmax = false; // not used

    int totPacked = 0;

    std::vector<int> containerIndices(outlines.size(), -1); // -1 means not packed to any container

    std::vector<vcg::Similarity2f> packingTransforms(outlines.size(), vcg::Similarity2f{});

    unsigned nc = 0; // current container index
    while (totPacked < (int) charts.size()) {
        if (nc >= containerVec.size())
            containerVec.push_back(vcg::Point2i(packingSize, packingSize));

        std::vector<unsigned> outlineIndex_iter;
        std::vector<Outline2f> outlines_iter;
        for (unsigned i = 0; i < containerIndices.size(); ++i) {
            if (containerIndices[i] == -1) {
                outlineIndex_iter.push_back(i);
                outlines_iter.push_back(outlines[i]);
            }
        }

        const int MAX_SIZE = 20000;
        std::vector<vcg::Similarity2f> transforms;
        std::vector<int> polyToContainer;
        int n = 0;
        do {
            transforms.clear();
            polyToContainer.clear();
            LOG_INFO << "Packing into grid of size " << containerVec[nc].X() << " " << containerVec[nc].Y();
            n = RasterizationBasedPacker::PackBestEffortAtScale(outlines_iter, {containerVec[nc]}, transforms, polyToContainer, packingParams, packingScale);
            if (n == 0) {
                containerVec[nc].X() *= 1.1;
                containerVec[nc].Y() *= 1.1;
            }
        } while (n == 0 && containerVec[nc].X() <= MAX_SIZE && containerVec[nc].Y() <= MAX_SIZE);

        totPacked += n;

        if (n == 0) // no charts were packed, stop
            break;
        else {
            double textureScale = 1.0 / packingScale;
            texszVec.push_back({(int) (containerVec[nc].X() * textureScale), (int) (containerVec[nc].Y() * textureScale)});
            for (unsigned i = 0; i < outlines_iter.size(); ++i) {
                if (polyToContainer[i] != -1) {
                    ensure(polyToContainer[i] == 0); // We only use a single container
                    int outlineInd = outlineIndex_iter[i];
                    ensure(containerIndices[outlineInd] == -1);
                    containerIndices[outlineInd] = nc;
                    packingTransforms[outlineInd] = transforms[i];
                }
            }
        }
        nc++;
    }

    for (unsigned i = 0; i < charts.size(); ++i) {
        for (auto fptr : charts[i]->fpVec) {
            int ic = containerIndices[i];
            if (ic < 0) {
                for (int j = 0; j < fptr->VN(); ++j) {
                    fptr->V(j)->T().P() = Point2d::Zero();
                    fptr->V(j)->T().N() = 0;
                    fptr->WT(j).P() = Point2d::Zero();
                    fptr->WT(j).N() = 0;
                }
            }
            else {
                Point2i gridSize = containerVec[ic];
                for (int j = 0; j < fptr->VN(); ++j) {
                    Point2d uv = fptr->WT(j).P();
                    Point2f p = packingTransforms[i] * (Point2f(uv[0], uv[1]));
                    p.X() /= (double) gridSize.X();
                    p.Y() /= (double) gridSize.Y();
                    fptr->V(j)->T().P() = Point2d(p.X(), p.Y());
                    fptr->V(j)->T().N() = ic;
                    fptr->WT(j).P() = fptr->V(j)->T().P();
                    fptr->WT(j).N() = fptr->V(j)->T().N();
                }
            }
        }
    }

    for (auto c : charts)
        c->ParameterizationChanged();

    return totPacked;
}

Outline2f ExtractOutline2f(FaceGroup& chart)
{
    Outline2d outline2d = ExtractOutline2d(chart);
    Outline2f outline2f;
    for (auto& p : outline2d) {
        outline2f.push_back(vcg::Point2f(p.X(), p.Y()));
    }
    return outline2f;
}

Outline2d ExtractOutline2d(FaceGroup& chart)
{
    //ensure(chart.numMerges == 0);

    std::vector<Outline2d> outline2Vec;
    Outline2d outline;

    for (auto fptr : chart.fpVec)
        fptr->ClearV();

    for (auto fptr : chart.fpVec) {
        for (int i = 0; i < 3; ++i) {
            if (!fptr->IsV() && face::IsBorder(*fptr, i)) {
                face::Pos<Mesh::FaceType> p(fptr, i);
                face::Pos<Mesh::FaceType> startPos = p;
                ensure(p.IsBorder());
                do {
                    ensure(p.IsManifold());
                    p.F()->SetV();
                    vcg::Point2d uv = p.F()->WT(p.VInd()).P();
                    outline.push_back(uv);
                    p.NextB();
                }
                while (p != startPos);
                outline2Vec.push_back(outline);
                outline.clear();
            }
        }
    }

    vcg::Box2d box = chart.UVBox();

    int outlineIndex = -1;
    double largestArea = 0;

    for (int i = 0; i < outline2Vec.size(); ++i) {
        double outlineArea = tri::OutlineUtil<double>::Outline2Area(outline2Vec[i]);
        if (outlineArea < 0)
            tri::OutlineUtil<double>::ReverseOutline2(outline2Vec[i]);
        if (std::abs(outlineArea) >= largestArea) {
            vcg::Box2d outlineBox;
            for (const auto& p : outline2Vec[i])
                outlineBox.Add(p);
            if (outlineBox.DimX() >= box.DimX() && outlineBox.DimY() >= box.DimY()) {
                outlineIndex = i;
                largestArea = std::abs(outlineArea);
            }
        }
    }

    if (outlineIndex == -1) {
        LOG_WARN << "Outline not bounding, falling back to UV bounding box for chart " << chart.id;
        outline.clear();
        outline.push_back(Point2d(box.min.X(), box.min.Y()));
        outline.push_back(Point2d(box.max.X(), box.min.Y()));
        outline.push_back(Point2d(box.max.X(), box.max.Y()));
        outline.push_back(Point2d(box.min.X(), box.max.Y()));
        return outline;
    } else {
        return outline2Vec[outlineIndex];
    }

}

void IntegerShift(Mesh& m, const std::vector<ChartHandle>& chartsToPack, const std::vector<TextureSize>& texszVec, const std::map<ChartHandle, int>& anchorMap, const std::map<RegionID, bool>& flippedInput)
{
    // compute grid-preserving translation
    // for each chart
    //   - find an anchor vertex (i.e. find a vertex that belonged to the
    //     source chart that determined the integer translation of the final chart.
    //   - compute the displacement of this anchor vertex wrt the integer pixel coordinates
    //     both in its original configuration (t0) and in the final, packed chart (t1)
    //   - compute the translation vector t = t0 - t1
    //   - apply the translation t to the entire chart

    ensure(HasWedgeTexCoordStorageAttribute(m));
    auto wtcsh = GetWedgeTexCoordStorageAttribute(m);

    std::vector<double> angle = { 0, M_PI_2, M_PI, (M_PI_2 + M_PI) };

    auto Rotate = [] (vcg::Point2d p, double theta) -> vcg::Point2d { return p.Rotate(theta); };

    for (auto c : chartsToPack) {
        auto it = anchorMap.find(c);
        if (it != anchorMap.end()) {
            Mesh::FacePointer fptr = &(m.face[it->second]);
            bool flipped = flippedInput.at(fptr->initialId);

            vcg::Point2d d0 = wtcsh[fptr].tc[1].P() - wtcsh[fptr].tc[0].P();
            vcg::Point2d d1 = fptr->cWT(1).P() - fptr->cWT(0).P();

            if (flipped)
                d0.X() *= -1;

            double minResidual = 2 * M_PI;
            int minResidualIndex = -1;
            for (int i = 0; i < 4; ++i) {
                double residual = VecAngle(Rotate(d0, angle[i]), d1);
                if (residual < minResidual) {
                    minResidual = residual;
                    minResidualIndex = i;
                }
            }

            int ti = fptr->cWT(0).N();
            ensure(ti < (int) texszVec.size());
            vcg::Point2d textureSize(texszVec[ti].w, texszVec[ti].h);

            vcg::Point2d u0 = wtcsh[fptr].tc[0].P();
            vcg::Point2d u1 = fptr->cWT(0).P();

            double unused;
            double dx = std::modf(u0.X(), &unused);
            double dy = std::modf(u0.Y(), &unused);

            if (flipped)
                dx = 1 - dx;

            switch(minResidualIndex) {
            case 0:
                break;
            case 1:
                std::swap(dx, dy);
                dx = 1 - dx;
                break;
            case 2:
                dx = 1 - dx;
                dy = 1 - dy;
                break;
            case 3:
                std::swap(dx, dy);
                dy = 1 - dy;
                break;
            default:
                ensure(0 && "VERY BAD");
            }

            double dx1 = std::modf(u1.X() * textureSize.X(), &unused);
            double dy1 = std::modf(u1.Y() * textureSize.Y(), &unused);
            vcg::Point2d t(0, 0);
            t.X() = (dx - dx1) / textureSize.X();
            t.Y() = (dy - dy1) / textureSize.Y();

            for (auto fptr : c->fpVec) {
                for (int i = 0; i < 3; ++i) {
                    fptr->WT(i).P() += t;
                    fptr->V(i)->T().P() = fptr->WT(i).P();
                }
            }
        }
    }
}
