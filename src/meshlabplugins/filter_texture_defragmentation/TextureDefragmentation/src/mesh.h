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

#ifndef MESH_H
#define MESH_H

#include <vcg/complex/complex.h>

#include <vector>
#include <memory>

#include <QImage>

#include "types.h"


typedef vcg::face::Pos<MeshFace> PosF;


using namespace vcg;


/* per face extra flags, defined according to the vcg style */
template <class T>
class FaceQualifier : public T {

public:

    FaceQualifier() : _qualifier(0) {}

    void SetMesh()        { _qualifier = MESH; }
    void SetHoleFilling() { _qualifier = HOLE_FILLING; }
    void SetScaffold()    { _qualifier = SCAFFOLD; }

    bool IsMesh()        const { return _qualifier == MESH; }
    bool IsHoleFilling() const { return _qualifier == HOLE_FILLING; }
    bool IsScaffold()    const { return _qualifier == SCAFFOLD; }

    template <class RType>
    void ImportData(const RType& rhs) {
        assert(rhs.HasQualifier() && "RHS NOT COMPATIBLE");
        _qualifier = rhs._qualifier;
        T::ImportData(rhs);
    }
    void Alloc(const int & ns) { T::Alloc(ns); }
    void Dealloc() { T::Dealloc(); }
    static bool HasQualifier() { return true; }
    static void Name(std::vector<std::string> & name) {
        name.push_back(std::string("FaceQualifier"));
        T::Name(name);
    }

private:
    static constexpr unsigned char MESH = 1;
    static constexpr unsigned char HOLE_FILLING = 2;
    static constexpr unsigned char SCAFFOLD = 3;

    unsigned char _qualifier;
};

struct MeshUsedTypes : public UsedTypes<Use<MeshVertex>::AsVertexType, Use<MeshFace>::AsFaceType, Use<MeshEdge>::AsEdgeType> {};

class MeshVertex : public Vertex<MeshUsedTypes, vertex::Coord3d, vertex::TexCoord2d, vertex::Normal3d, vertex::VEAdj, vertex::VFAdj, vertex::Color4b, vertex::Qualityd, vertex::Mark, vertex::BitFlags> {};
class MeshFace : public Face<MeshUsedTypes, FaceQualifier, face::VertexRef, face::FFAdj, face::VFAdj, face::Mark, face::WedgeTexCoord2d, face::Normal3d, face::Color4b, face::Qualityf, face::BitFlags>
{
public:
    RegionID id = INVALID_ID;
    RegionID initialId = INVALID_ID;
};

class MeshEdge : public Edge<MeshUsedTypes, edge::VertexRef, edge::VEAdj, edge::EEAdj, edge::BitFlags> {};
class Mesh : public tri::TriMesh<std::vector<MeshVertex>, std::vector<MeshFace>>{
public:
    std::string name{"mesh"};

    ~Mesh()
    {
        ClearAttributes();
    }
};

struct SeamUsedTypes : public UsedTypes<Use<SeamVertex>::AsVertexType, Use<SeamEdge>::AsEdgeType/*, Use<SeamFace>::AsFaceType*/>{};

class SeamVertex  : public Vertex< SeamUsedTypes, vertex::Coord3d, vertex::Normal3d, vertex::Color4b, vertex::VEAdj, vertex::VFAdj,vertex::BitFlags  >{};
class SeamEdge    : public Edge<   SeamUsedTypes, edge::VertexRef, edge::VEAdj, edge::EEAdj, edge::Color4b, edge::BitFlags> {
public:
    Mesh::FacePointer fa;
    Mesh::FacePointer fb;
    int ea;
    int eb;
};
//class SeamFace    : public Face  < SeamUsedTypes, face::VertexRef, face::VFAdj, face::FFAdj, face::Mark, face::Color4b, face::BitFlags > {};
class SeamMesh : public tri::TriMesh< std::vector<SeamVertex>, std::vector<SeamEdge>/*, std::vector<SeamFace> */>{};

bool LoadMesh(const char *fileName, Mesh& m, TextureObjectHandle& textureObject, int &loadMask);
bool SaveMesh(const char *fileName, Mesh& m, const std::vector<std::shared_ptr<QImage>>& textureImages, bool color);

void ScaleTextureCoordinatesToImage(Mesh& m, TextureObjectHandle textureObject);
void ScaleTextureCoordinatesToParameterArea(Mesh& m, TextureObjectHandle textureObject);

vcg::Box2d UVBox(const Mesh& m);
vcg::Box2d UVBoxVertex(const Mesh& m);

/* Duplicates vertices at seams */
void CutAlongSeams(Mesh& m);

/* Builds a mesh from a given vector of face pointers. The order of the faces
 * is guaranteed to be preserved in the face container of the mesh. */
void MeshFromFacePointers(const std::vector<Mesh::FacePointer>& vfp, Mesh& out);

inline double AreaUV(const Mesh::FaceType& f)
{
    vcg::Point2d u0 = f.cWT(0).P();
    vcg::Point2d u1 = f.cWT(1).P();
    vcg::Point2d u2 = f.cWT(2).P();
    return ((u1 - u0) ^ (u2 - u0)) / 2.0;
}


inline double Area3D(const Mesh::FaceType& f)
{
    return ((f.cP(1) - f.cP(0)) ^ (f.cP(2) - f.cP(0))).Norm() / 2.0;
}


#endif // MESH_H
