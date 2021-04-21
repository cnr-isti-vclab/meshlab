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

#ifndef TYPES_H
#define TYPES_H

#include <memory>
#include <vector>
#include <vcg/space/point2.h>

class Mesh;
class MeshVertex;
class MeshFace;
class MeshEdge;

class SeamMesh;
class SeamEdge;
class SeamVertex;

typedef int RegionID;
constexpr RegionID INVALID_ID = 0xffffffff;

class MeshGraph;
class FaceGroup;
class TextureObject;

class AlgoState;
class Seam;
class ClusteredSeam;

typedef std::shared_ptr<MeshGraph>       GraphHandle;
typedef std::shared_ptr<FaceGroup>       ChartHandle;
typedef std::shared_ptr<TextureObject>   TextureObjectHandle;
typedef std::shared_ptr<Seam>            SeamHandle;
typedef std::shared_ptr<ClusteredSeam>   ClusteredSeamHandle;
typedef std::shared_ptr<AlgoState>       AlgoStateHandle;
typedef std::shared_ptr<const AlgoState> ConstAlgoStateHandle;

typedef std::vector<vcg::Point2f> Outline2f;
typedef std::vector<vcg::Point2d> Outline2d;

typedef std::pair<ClusteredSeamHandle, double> WeightedSeam;

struct TextureSize;

#endif // TYPES_H
