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

#ifndef MESH_GRAPH_H
#define MESH_GRAPH_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include <QImage>

#include "types.h"
#include "mesh.h"
#include "math_utils.h"

class Mesh;

typedef std::pair<ChartHandle, ChartHandle> ChartPair;

/* FaceGroup class
 * Used to store a mesh chart as an array of Face pointers */
struct FaceGroup {

    struct Hasher {
        std::size_t operator()(const ChartHandle& ch) const
        {
            return std::hash<RegionID>()(ch->id);
        }
    };

    struct Cache {
        double areaUV;
        double area3D;
        double borderUV;
        double border3D;
        vcg::Point3d weightedSumNormal;
        bool uvFlipped;
    };

    void UpdateCache() const;

    Mesh& mesh;
    RegionID id;
    std::vector<Mesh::FacePointer> fpVec;
    std::unordered_set<ChartHandle, Hasher> adj;

    int numMerges;

    float minMappedFaceValue;
    float maxMappedFaceValue;

    double error;

    mutable bool dirty;
    mutable Cache cache;

    FaceGroup(Mesh& m, const RegionID id_);

    void Clear();
    void AddFace(const Mesh::FacePointer fptr);
    void ParameterizationChanged();
    Mesh::FacePointer Fp();

    vcg::Point3d AverageNormal() const;

    std::size_t FN() const;
    std::size_t NumAdj() const;
    double OriginalAreaUV() const;
    double AreaUV() const;
    double Area3D() const;
    double BorderUV() const;
    double Border3D() const;
    bool UVFlipped() const;
    vcg::Box2d UVBox() const;

    bool UVFlipped();

    void UpdateBorder() const;
};

/* Constructs a mesh from a FaceGroup, the created mesh has the FaceIndex
 * attribute defined (see mesh_attribute.h) */
void CopyToMesh(FaceGroup& fg, Mesh& m);

/* Computes the mesh graph relying on the pre-computed FF adjacency attribute
 * to determine chart adjacency relations */
GraphHandle ComputeGraph(Mesh &m, TextureObjectHandle textureObject);

/* This function ensures that the vertices referenced by each chart are unique
 * to the chart. Necessary because non-manifold vertices adjacent to
 * non-manifold edges cannot be split by the VCG's SplitNonManifoldVertices() */
void DisconnectCharts(GraphHandle graph);

/*
 * MeshGraph class
 *
 * The graph is actually stored as an associative array mapping each Region id to the relative FaceGroup, the adjacencies
 * are recorded inside each FaceGroup
 */
struct MeshGraph {

    Mesh& mesh;

    std::unordered_map<RegionID, ChartHandle> charts;
    TextureObjectHandle textureObject;

    MeshGraph(Mesh& m);
    ~MeshGraph();

    /* compute the minmax distortion of the graph */
    std::pair<float,float> DistortionRange() const;

    /* Retrieve region i (ensure if not found) */
    std::shared_ptr<FaceGroup> GetChart(RegionID i);

    /* Retrieve region i, creating if it is not found */
    std::shared_ptr<FaceGroup> GetChart_Insert(RegionID i);

    /* Number of regions */
    std::size_t Count() const;

    /* Number of merges performed on the graph (is it used?) */
    int MergeCount() const;

    double Area3D() const;
    double AreaUV() const;
    double SignedAreaUV() const;

    double MappedFraction() const;

    double BorderUV() const;

};

#endif // MESH_GRAPH_H
