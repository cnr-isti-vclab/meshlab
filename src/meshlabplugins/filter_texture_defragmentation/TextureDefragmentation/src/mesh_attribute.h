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

#ifndef MESH_ATTRIBUTE_H
#define MESH_ATTRIBUTE_H

#include "mesh.h"
#include "utils.h"


struct TexCoordStorage {
    vcg::TexCoord2d tc[3];
};

struct CoordStorage {
    vcg::Point3d P[3];
};

struct BoundaryInfo {
    std::vector<double> vBoundaryLength;
    std::vector<std::size_t> vBoundarySize;
    std::vector<std::vector<std::size_t>> vBoundaryFaces;
    std::vector<std::vector<int>> vVi; // The face boundary vertex indices

    std::size_t N();
    std::size_t LongestBoundary();
    void Clear();
};

inline std::size_t BoundaryInfo::N()
{
    ensure(vBoundaryLength.size() == vBoundarySize.size() && vBoundaryLength.size() == vBoundaryFaces.size());
    return vBoundaryLength.size();
}

inline std::size_t BoundaryInfo::LongestBoundary()
{
    ensure(N() > 0);
    return std::distance(vBoundaryLength.begin(),
                         std::max_element(vBoundaryLength.begin(), vBoundaryLength.end()));
}

inline void BoundaryInfo::Clear()
 {
    vBoundaryLength.clear();
    vBoundarySize.clear();
    vBoundaryFaces.clear();
    vVi.clear();
}

struct FF {
    int f[3]; // opposite face index
    int e[3]; // opposite edge index
};

inline Mesh::PerFaceAttributeHandle<FF> Get3DFaceAdjacencyAttribute(Mesh& m);
inline Mesh::PerFaceAttributeHandle<TexCoordStorage> GetWedgeTexCoordStorageAttribute(Mesh& m);
inline Mesh::PerMeshAttributeHandle<BoundaryInfo> GetBoundaryInfoAttribute(Mesh& m);
inline Mesh::PerFaceAttributeHandle<CoordStorage> GetTargetShapeAttribute(Mesh& shell);
inline Mesh::PerFaceAttributeHandle<int> GetFaceIndexAttribute(Mesh& shell);
inline Mesh::PerFaceAttributeHandle<CoordStorage> GetShell3DShapeAttribute(Mesh& shell);

inline bool Has3DFaceAdjacencyAttribute(Mesh& m);
inline bool HasWedgeTexCoordStorageAttribute(Mesh& m);
inline bool HasBoundaryInfoAttribute(Mesh& m);
inline bool HasTargetShapeAttribute(Mesh& shell);
inline bool HasFaceIndexAttribute(Mesh& shell);
inline bool HasShell3DShapeAttribute(Mesh& shell);

void Compute3DFaceAdjacencyAttribute(Mesh& m);
void ComputeWedgeTexCoordStorageAttribute(Mesh& m);
void ComputeBoundaryInfoAttribute(Mesh& m);


inline Mesh::PerFaceAttributeHandle<FF> Get3DFaceAdjacencyAttribute(Mesh& m)
{
    return tri::Allocator<Mesh>::GetPerFaceAttribute<FF>(m, "FaceAttribute_3DFaceAdjacency");
}

inline bool Has3DFaceAdjacencyAttribute(Mesh& m)
{
    return tri::Allocator<Mesh>::IsValidHandle<FF>(
                m, tri::Allocator<Mesh>::FindPerFaceAttribute<FF>(m, "FaceAttribute_3DFaceAdjacency"));
}

inline Mesh::PerFaceAttributeHandle<TexCoordStorage> GetWedgeTexCoordStorageAttribute(Mesh& m)
{
    return tri::Allocator<Mesh>::GetPerFaceAttribute<TexCoordStorage>(m, "WedgeTexCoordStorage");
}

inline bool HasWedgeTexCoordStorageAttribute(Mesh& m)
{
    return tri::Allocator<Mesh>::IsValidHandle<TexCoordStorage>(
                m, tri::Allocator<Mesh>::FindPerFaceAttribute<TexCoordStorage>(m, "WedgeTexCoordStorage"));
}

inline Mesh::PerMeshAttributeHandle<BoundaryInfo> GetBoundaryInfoAttribute(Mesh& m)
{
    ensure(HasBoundaryInfoAttribute(m));
    return tri::Allocator<Mesh>::GetPerMeshAttribute<BoundaryInfo>(m, "MeshAttribute_BoundaryInfo");
}

inline bool HasBoundaryInfoAttribute(Mesh& m)
{
    return tri::Allocator<Mesh>::IsValidHandle<BoundaryInfo>(
                m, tri::Allocator<Mesh>::FindPerMeshAttribute<BoundaryInfo>(m, "MeshAttribute_BoundaryInfo"));
}

inline Mesh::PerFaceAttributeHandle<CoordStorage> GetTargetShapeAttribute(Mesh& shell)
{
    return tri::Allocator<Mesh>::GetPerFaceAttribute<CoordStorage>(shell, "FaceAttribute_TargetShape");
}

inline bool HasTargetShapeAttribute(Mesh& shell)
{
    return tri::Allocator<Mesh>::IsValidHandle<CoordStorage>(
                shell, tri::Allocator<Mesh>::FindPerFaceAttribute<CoordStorage>(shell, "FaceAttribute_TargetShape"));
}

inline Mesh::PerFaceAttributeHandle<int> GetFaceIndexAttribute(Mesh& shell)
{
    return tri::Allocator<Mesh>::GetPerFaceAttribute<int>(shell, "FaceAttribute_FaceIndex");
}

inline bool HasFaceIndexAttribute(Mesh& shell)
{
    return tri::Allocator<Mesh>::IsValidHandle<int>(
                shell, tri::Allocator<Mesh>::FindPerFaceAttribute<int>(shell, "FaceAttribute_FaceIndex"));
}

inline Mesh::PerFaceAttributeHandle<CoordStorage> GetShell3DShapeAttribute(Mesh& shell)
{
    return tri::Allocator<Mesh>::GetPerFaceAttribute<CoordStorage>(shell, "FaceAttribute_Shell3DShape");
}

inline bool HasShell3DShapeAttribute(Mesh& shell)
{
    return tri::Allocator<Mesh>::IsValidHandle<CoordStorage>(
                shell, tri::Allocator<Mesh>::FindPerFaceAttribute<CoordStorage>(shell, "FaceAttribute_Shell3DShape"));
}

#endif // MESH_ATTRIBUTE_H

