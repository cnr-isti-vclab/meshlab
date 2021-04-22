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

#ifndef SEAMS_H
#define SEAMS_H

#include <vector>
#include <memory>

#include "types.h"
#include "mesh_graph.h"

struct Seam {
    SeamMesh& sm;
    std::vector<int> edges; // the list of seam segment edges
    std::vector<int> endpoints; // the two endpoint vertices

    Seam(SeamMesh& m) : sm{m} {}
};

struct ClusteredSeam {
    SeamMesh& sm;
    std::vector<SeamHandle> seams;

    ClusteredSeam(SeamMesh& m) : sm{m} {}
    std::size_t size() { return seams.size(); }
    SeamHandle at(int i) { return seams.at(i); }
};

ChartPair GetCharts(ClusteredSeamHandle csh, GraphHandle graph, bool *swapped = nullptr);
std::set<int> GetEndpoints(ClusteredSeamHandle csh);

void ColorizeSeam(ClusteredSeamHandle csh, const vcg::Color4b& color);
void ColorizeSeam(SeamHandle sh, const vcg::Color4b& color);

double ComputeSeamLength3D(ClusteredSeamHandle csh);
double ComputeSeamLength3D(SeamHandle sh);

// a is a set of ids that logically describe one side of the seam (whose coordinates are inserted in uva)
void ExtractUVCoordinates(ClusteredSeamHandle csh, std::vector<Point2d>& uva, std::vector<Point2d>& uvb, const std::unordered_set<RegionID>& a);

void BuildSeamMesh(Mesh& m, SeamMesh& seamMesh);
std::vector<SeamHandle> GenerateSeams(SeamMesh& seamMesh);
std::vector<ClusteredSeamHandle> ClusterSeamsByChartId(const std::vector<SeamHandle>& seams);
ClusteredSeamHandle Flatten(const std::vector<ClusteredSeamHandle>& cshVec);

#endif // SEAMS_H
