#ifndef __VCG_MESH__
#define __VCG_MESH__

#include<vcg/simplex/vertex/base.h>//
#include<vcg/simplex/vertex/component.h>
#include<vcg/simplex/vertex/component_ocf.h>
#include<vcg/simplex/face/base.h>//
#include<vcg/simplex/face/component.h>
//#include<vcg/simplex/face/topology.h>//
#include<vcg/complex/complex.h>//

// input output
#include<wrap/io_trimesh/import.h>
#include<wrap/io_trimesh/export.h>//just in case

// topology computation
//#include<vcg/complex/algorithms/update/topology.h>//
//#include<vcg/complex/algorithms/update/flag.h>//

// half edge iterators
//#include<vcg/simplex/face/pos.h>

// normals and curvature
#include<vcg/complex/algorithms/update/bounding.h> //class UpdateNormals 
#include<vcg/complex/algorithms/update/normal.h> //class UpdateNormals 
#include<vcg/complex/algorithms/update/curvature.h> //class curvature

#include "../utils/release_assert.h"

extern Logging * lgn;

class vcgFace; // dummy prototype
class vcgVertex;

struct vcgUsedType : public vcg::UsedTypes< vcg::Use<vcgVertex>::AsVertexType,vcg::Use<vcgFace>::AsFaceType>{};

class vcgVertex  : public vcg::Vertex< vcgUsedType, vcg::vertex::InfoOcf, vcg::vertex::Coord3f,  vcg::vertex::Color4bOcf, vcg::vertex::Normal3f,vcg::vertex::BitFlags >{};
class vcgFace    : public vcg::Face  < vcgUsedType, vcg::face::VertexRef,  vcg::face::Normal3f, vcg::face::BitFlags> {};
class vcgMesh    : public vcg::tri::TriMesh< vcg::vertex::vector_ocf<vcgVertex>, std::vector<vcgFace> > {};


struct OFace{
	OFace(){}
	OFace(const int & i0,const int & i1,const int & i2){
														v[0] = i0;
														v[1] = i1;
														v[2] = i2;
														}
	unsigned int v[3];
	unsigned int & operator[](const int & i)  { assert(i>=0); assert ( i <3); return v[i];}
	unsigned int VN(){return 3;}
};


struct OVertex{
        OVertex( ) {}

	template <class VertexType>
        OVertex(const float & x,const float & y,const float & z){
	 p[0]=x;p[1]=y;p[2]=z;}
        OVertex(const vcg::Point3f _p){ p = _p; }
	
	template <class VertexType>
        OVertex(const VertexType & v){ p =v.P();}

	vcg::Point3f & P(){return p;}

private:
	vcg::Point3f p;
};




#endif // __VCG_MESH__
