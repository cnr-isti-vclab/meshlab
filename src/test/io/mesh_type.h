#ifndef __MYMESH
#define __MYMESH

#include<vcg/simplex/vertexplus/base.h>
#include<vcg/simplex/faceplus/base.h>
#include<vcg/simplex/face/topology.h>
#include<vcg/simplex/vertexplus/component_ocf.h>
#include<vcg/simplex/faceplus/component_ocf.h>
#include<vcg/complex/trimesh/base.h>
#include <wrap/gl/trimesh.h>


// vecchia versione (molto piu' veloce ma meno elegante)
/*#include <vcg/simplex/vertex/with/afvmvnvq.h>
#include <vcg/simplex/face/with/afavfnfq.h>

class MyFace;
class MyEdge;
class MyVertex:public vcg::VertexAFVMVNVQ<float,MyEdge , MyFace, MyEdge>{
public: vcg::Point3f tx,ty;
} ;
class MyFace:public vcg::FaceAFAVFNFQ<MyVertex,MyEdge , MyFace> {
public:
	int * r; // belongs to region r
	int ip; // which plane of r
};
class MyMesh: public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace > >{};
*/// fine vecchia versione 


class CEdge;    // dummy prototype never used
class CFaceO;
class CVertexO;

// Opt stuff
//class CVertexO  : public vcg::VertexSimp2< CVertexO, CEdge, CFaceO, vcg::vert::Coord3f, vcg::vert::Normal3f, vcg::vert::BitFlags >{};
//class CFaceO    : public vcg::FaceSimp2< CVertexO, CEdge, CFaceO, vcg::face::InfoOcf, vcg::face::FFAdjOcf, vcg::face::WedgeTexture2f, vcg::face::VertexRef, vcg::face::BitFlags, vcg::face::Normal3fOcf > {};
class CVertexO  : public vcg::VertexSimp2< CVertexO, CEdge, CFaceO, vcg::vert::Coord3f, /*vcg::vert::Mark, *//*vcg::vert::VFAdj, */vcg::vert::Normal3f/*, vcg::vert::Qualityf*/>{};
class CFaceO    : public vcg::FaceSimp2< CVertexO, CEdge, CFaceO, vcg::face::VertexRef, vcg::face::InfoOcf/*, vcg::face::VFAdj, *//*vcg::face::FFAdj,*/ /*vcg::face::Qualityf*/, vcg::face::WedgeTexture2f, vcg::face::Normal3fOcf> {};
class MyMesh : public vcg::tri::TriMesh< std::vector<CVertexO>, vcg::face::vector_ocf<CFaceO> > {};

#endif