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

#ifndef TEXTURE_OPTIMIZATION_H
#define TEXTURE_OPTIMIZATION_H

#include "mesh.h"

#include <utility>
#include <vcg/space/point2.h>


struct Point2iHasher {
    std::size_t operator()(const vcg::Point2i& p) const noexcept
    {
        std::size_t seed = 0;
        seed ^= std::hash<int>()(p[0]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(p[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

void ReorientCharts(GraphHandle graph);


/* Given a chart, compute both the set of 2D orientation of each initial component
 * and its 3D area. Then rotate the chart according to the largest surface area contribution
 * of the initial components that have been clustered in the chart.
 * Returns the index of an anchor face, i.e. a face that does not belong to the
 * change set and is inside the largest initial component that induced the rotation
 */
int RotateChartForResampling(ChartHandle chart, const std::set<Mesh::FacePointer> &changeSet, const std::map<RegionID, bool>& flippedInput, bool colorize, double *zeroResamplingArea);

/* Texture trimming to remove unused space */
void TrimTexture(Mesh& m, std::vector<TextureSize>& texszVec, bool unsafeMip);

#endif // TEXTURE_OPTIMIZATION_H
