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

#include "intersection.h"
#include "utils.h"

#include <vcg/space/index/grid_util2d.h>
#include <vcg/space/index/grid_util.h>
#include <vcg/space/intersection2.h>

#include <unordered_map>


struct Point2iHasher {
    std::size_t operator()(const vcg::Point2i& p) const noexcept
    {
        std::size_t seed = 0;
        seed ^= std::hash<int>()(p[0]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(p[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};


static vcg::Box2d ComputeBox(const std::vector<HalfEdge>& hev);


bool SegmentBoxIntersection(const Segment& seg, const vcg::Box2d& box)
{
    vcg::Point2d isec;
    vcg::Point2d c1{box.min};
    vcg::Point2d c2{box.max[0], box.min[1]};
    vcg::Point2d c3{box.max};
    vcg::Point2d c4{box.min[0], box.max[1]};

    if (vcg::SegmentSegmentIntersection(seg, vcg::Segment2<double>{c1, c2}, isec)) return true;
    if (vcg::SegmentSegmentIntersection(seg, vcg::Segment2<double>{c2, c3}, isec)) return true;
    if (vcg::SegmentSegmentIntersection(seg, vcg::Segment2<double>{c3, c4}, isec)) return true;
    if (vcg::SegmentSegmentIntersection(seg, vcg::Segment2<double>{c4, c1}, isec)) return true;

    // if the segment does not intersect the sides, check if it is fully contained in the box
    return (box.min[0] <= std::min(seg.P0()[0], seg.P1()[0]) &&
            box.min[1] <= std::min(seg.P0()[1], seg.P1()[1]) &&
            box.max[0] >= std::max(seg.P0()[0], seg.P1()[0]) &&
            box.max[1] >= std::max(seg.P0()[1], seg.P1()[1]));
}

std::vector<HalfEdgePair> CrossIntersection(const std::vector<HalfEdge>& heVec1, const std::vector<HalfEdge>& heVec2)
{
    using HalfEdgeID = std::pair<int, int>;

    std::vector<HalfEdgePair> isects;

    std::unordered_map<vcg::Point2i, std::vector<HalfEdgeID>, Point2iHasher> grid;
    unsigned elems = heVec1.size() + heVec2.size();

    // init grid helper
    vcg::BasicGrid2D<double> gh;
    vcg::Box2d bbox1 = ComputeBox(heVec1);
    vcg::Box2d bbox2 = ComputeBox(heVec2);
    gh.bbox = bbox1;
    gh.bbox.Add(bbox2.min);
    gh.bbox.Add(bbox2.max);
    vcg::BestDim2D<double>(elems, gh.bbox.Dim(), gh.siz);
    gh.ComputeDimAndVoxel();

    const std::vector<HalfEdge> *vp[] = {&heVec1, &heVec2};

    for (int i = 0; i < 2; ++i) {
        for (unsigned j = 0; j < vp[i]->size(); ++j) {
            const HalfEdge& he = vp[i]->at(j);

            vcg::Box2d segmentBox;
            segmentBox.Add(he.P0());
            segmentBox.Add(he.P1());
            vcg::Box2i gridCover;
            gh.BoxToIBox(segmentBox, gridCover);
            for (int h = gridCover.min[0]; h <= gridCover.max[0]; h++) {
                for (int k = gridCover.min[1]; k <= gridCover.max[1]; k++) {
                    vcg::Box2d cell;
                    vcg::Point2i voxel(h, k);
                    gh.IPiToBox(voxel, cell);
                    if (SegmentBoxIntersection(Segment(he.P0(), he.P1()), cell)) {
                        grid[voxel].push_back(std::make_pair(i, j));
                    }
                }
            }
        }
    }

    for (auto& entry : grid) {
        for (unsigned j = 0; j < entry.second.size(); ++j) {
            for (unsigned k = j+1; k < entry.second.size(); ++k) {
                HalfEdgeID i1 = entry.second[j];
                HalfEdgeID i2 = entry.second[k];

                if (i1.first > i2.first)
                    std::swap(i1, i2);

                if (i1.first != i2.first) {
                    ensure(i1.first == 0);
                    // test for intersection only if the segments have no common endpoints (this improves robustness)
                    HalfEdge he1 = vp[i1.first]->at(i1.second);
                    HalfEdge he2 = vp[i2.first]->at(i2.second);
                    vcg::Point2d intersectionPoint;
                    if (he1.P0() != he2.P0() && he1.P1() != he2.P1() && he1.P0() != he2.P1() && he1.P1() != he2.P0()
                            && SegmentSegmentIntersection(Segment(he1.P0(), he1.P1()), Segment(he2.P0(), he2.P1()), intersectionPoint)) {
                        isects.push_back(std::make_pair(he1, he2));
                    }
                }
            }
        }
    }

    return isects;
}

std::vector<HalfEdgePair> Intersection(const std::vector<HalfEdge>& heVec)
{
    std::vector<HalfEdgePair> isects;

    std::unordered_map<vcg::Point2i, std::vector<int>, Point2iHasher> grid;
    unsigned elems = heVec.size();

    // init grid helper
    vcg::BasicGrid2D<double> gh;
    gh.bbox = ComputeBox(heVec);
    vcg::BestDim2D<double>(elems, gh.bbox.Dim(), gh.siz);
    gh.ComputeDimAndVoxel();

    for (unsigned j = 0; j < heVec.size(); ++j) {
        const HalfEdge& he = heVec[j];

        vcg::Box2d segmentBox;
        segmentBox.Add(he.P0());
        segmentBox.Add(he.P1());
        vcg::Box2i gridCover;
        gh.BoxToIBox(segmentBox, gridCover);
        for (int h = gridCover.min[0]; h <= gridCover.max[0]; h++) {
            for (int k = gridCover.min[1]; k <= gridCover.max[1]; k++) {
                vcg::Box2d cell;
                vcg::Point2i voxel(h, k);
                gh.IPiToBox(voxel, cell);
                if (SegmentBoxIntersection(Segment(he.P0(), he.P1()), cell)) {
                    grid[voxel].push_back(j);
                }
            }
        }
    }

    for (auto& entry : grid) {
        for (unsigned j = 0; j < entry.second.size(); ++j) {
            for (unsigned k = j+1; k < entry.second.size(); ++k) {
                int i1 = entry.second[j];
                int i2 = entry.second[k];
                Segment s1 = Segment(heVec[i1].P0(), heVec[i1].P1());
                Segment s2 = Segment(heVec[i2].P0(), heVec[i2].P1());
                vcg::Point2d intersectionPoint;

                if (s1.P0() != s2.P0() && s1.P1() != s2.P1() && s1.P0() != s2.P1() && s1.P1() != s2.P0()
                        && SegmentSegmentIntersection(s1, s2, intersectionPoint)) {
                    isects.push_back(std::make_pair(heVec[i1], heVec[i2]));
                }
            }
        }
    }


    return isects;
}

static vcg::Box2d ComputeBox(const std::vector<HalfEdge>& hev)
{
    vcg::Box2d box;
    for (auto& he : hev) {
        box.Add(he.P0());
        box.Add(he.P1());
    }
    return box;
}
