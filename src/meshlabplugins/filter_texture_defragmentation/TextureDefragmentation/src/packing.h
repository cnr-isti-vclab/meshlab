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

#ifndef PACKING_H
#define PACKING_H

#include "types.h"

#include <vector>
#include <map>


/* Pack the texture atlas encoded in the graph. Assumes the segmentation
 * correctly reflects the texture coordinates.
 * Returns the actual number of charts packed */
int Pack(const std::vector<ChartHandle>& charts, TextureObjectHandle textureObject, std::vector<TextureSize>& texszVec);

/* Computes the UV outline(s) of the given chart. If the chart has no outlines,
 * which can happen for some inputs on small closed components that are ignored
 * by the reparameterization procedure, it returns as outline the bounding box
 * of the chart texture coordinates.
 * NOTE: It assumes the face-face topology is computed according to the wedge
 * texture coordinates of the chart/mesh */
Outline2f ExtractOutline2f(FaceGroup& chart);
Outline2d ExtractOutline2d(FaceGroup& chart);


void IntegerShift(Mesh& m,
                  const std::vector<ChartHandle>& chartsToPack,
                  const std::vector<TextureSize>& texszVec,
                  const std::map<ChartHandle, int>& anchorMap,
                  const std::map<RegionID, bool>& flippedInput);


#endif // PACKING_H
