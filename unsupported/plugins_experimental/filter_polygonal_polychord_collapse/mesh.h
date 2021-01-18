#ifndef MESH_H
#define MESH_H

#include <vcg/complex/complex.h>

using namespace vcg;

class PolyVertex;
class PolyFace;

struct PolyUsedTypes : public vcg::UsedTypes<
        vcg::Use<PolyVertex>::AsVertexType,
        vcg::Use<PolyFace>::AsFaceType
        > {};

class PolyVertex : public vcg::Vertex<
        PolyUsedTypes,
        vcg::vertex::Coord3f,
        vcg::vertex::Normal3f,
        vcg::vertex::BitFlags
        > {};

class PolyFace : public vcg::Face<
        PolyUsedTypes,
        vcg::face::PolyInfo,
        vcg::face::Normal3f,
        vcg::face::BitFlags,
        vcg::face::PFVAdj,
        vcg::face::PFFAdj
        > {};

class PolyMesh : public vcg::tri::TriMesh<
        std::vector<PolyVertex>,
        std::vector<PolyFace>
        > {};

#endif // MESH_H
