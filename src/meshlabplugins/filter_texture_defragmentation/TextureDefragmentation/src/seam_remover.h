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

#ifndef SEAM_REMOVER_H
#define SEAM_REMOVER_H

#include <vector>
#include <memory>

#include <vcg/space/point3.h>
#include <vcg/space/point2.h>

#include "types.h"
#include "mesh.h"
#include "mesh_graph.h"
#include "matching.h"
#include "arap.h"

#include "seams.h"
#include "intersection.h"

typedef std::unordered_map<Mesh::VertexPointer, double> OffsetMap;

struct AlgoParameters {
    double matchingThreshold         = 2.0;
    double offsetFactor              = 5.0;
    double boundaryTolerance         = 0.2;
    double distortionTolerance       = 0.5;
    double globalDistortionThreshold = 0.025;
    double reductionFactor           = 0.8;
    bool   reduce                    = false;
    double timelimit                 = 0;
    bool   visitComponents           = true;
    double expb                      = 1.0;
    double UVBorderLengthReduction   = 0.0;
    bool   ignoreOnReject            = false;
};

struct SeamData {
    ClusteredSeamHandle csh;

    ChartHandle a;
    ChartHandle b;

    std::vector<vcg::Point2d> texcoorda;
    std::vector<vcg::Point2d> texcoordb;
    std::vector<int> vertexinda;
    std::vector<int> vertexindb;

    std::map<Mesh::VertexPointer, Mesh::VertexPointer> mrep;
    std::map<SeamMesh::VertexPointer, std::vector<Mesh::VertexPointer>> evec;

    typedef std::pair<std::vector<Mesh::FacePointer>, std::vector<int>> FanInfo;
    std::map<Mesh::VertexPointer, FanInfo> vfmap;

    std::unordered_set<Mesh::VertexPointer> verticesWithinThreshold;
    std::unordered_set<Mesh::FacePointer> optimizationArea;
    std::vector<vcg::Point2d> texcoordoptVert;
    std::vector<vcg::Point2d> texcoordoptWedge;

    double inputNegativeArea;
    double inputAbsoluteArea;

    double inputUVBorderLength;

    double inputArapNum;
    double inputArapDenom;

    double outputArapNum;
    double outputArapDenom;

    ARAPSolveInfo si;

    Mesh shell;

    std::vector<HalfEdgePair> intersectionOpt;
    std::vector<HalfEdgePair> intersectionBoundary;
    std::vector<HalfEdgePair> intersectionInternal;

    std::unordered_set<Mesh::VertexPointer> fixedVerticesFromIntersectingEdges;

    SeamData() : a{nullptr}, b{nullptr}, inputNegativeArea{0}, inputAbsoluteArea{0} {}
};

// enum of the possible outcomes for safety checks when performing merge operations
enum CheckStatus {
    PASS=0,
    FAIL_LOCAL_OVERLAP,
    FAIL_GLOBAL_OVERLAP_BEFORE,
    FAIL_GLOBAL_OVERLAP_AFTER_OPT, // border of the optimization area self-intersects
    FAIL_GLOBAL_OVERLAP_AFTER_BND, // border of the optimzation area hit the fixed border
    FAIL_DISTORTION_LOCAL,
    FAIL_DISTORTION_GLOBAL,
    FAIL_TOPOLOGY,  // shell genus is > 0 or shell is closed
    FAIL_NUMERICAL_ERROR,
    UNKNOWN,
    FAIL_GLOBAL_OVERLAP_UNFIXABLE,
    _END
};

struct CostInfo {
    enum MatchingValue {
        FEASIBLE=0,
        ZERO_AREA,
        UNFEASIBLE_BOUNDARY,
        UNFEASIBLE_MATCHING,
        REJECTED,
        _END
    };

    double cost;
    MatchingTransform matching;
    MatchingValue mvalue;
};

struct AlgoState {

    struct WeightedSeamCmp {
        bool operator()(const WeightedSeam& a, const WeightedSeam& b)
        {
            return a.second > b.second;
        }
    };

    std::priority_queue<WeightedSeam, std::vector<WeightedSeam>, WeightedSeamCmp> queue;
    std::unordered_map<ClusteredSeamHandle, double> cost;
    std::unordered_map<ClusteredSeamHandle, double> penalty;
    std::unordered_map<RegionID, std::set<ClusteredSeamHandle>> chartSeamMap;

    std::map<ClusteredSeamHandle, CheckStatus> status;

    std::map<int, std::set<ClusteredSeamHandle>> emap; // endpoint -> seams map

    std::unordered_map<ClusteredSeamHandle, MatchingTransform> transform; // the rigid matching computed for each currently active move
    std::unordered_map<ClusteredSeamHandle, CostInfo::MatchingValue> mvalue;

    std::unordered_map<RegionID, std::set<RegionID>> failed;

    SeamMesh sm;
    std::set<Mesh::FacePointer> changeSet;

    double arapNum;
    double arapDenom;

    double inputUVBorderLength;
    double currentUVBorderLength;
};

void PrepareMesh(Mesh& m, int *vndup);
AlgoStateHandle InitializeState(GraphHandle graph, const AlgoParameters& algoParameters);
void GreedyOptimization(GraphHandle graph, AlgoStateHandle state, const AlgoParameters& params);
void Finalize(GraphHandle graph, int *vndup);


#endif // SEAM_REMOVER_H
