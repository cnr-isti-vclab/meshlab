#ifndef __VCG_MESH__
#define __VCG_MESH__

#include<vcg/simplex/vertex/base.h>//
#include<vcg/simplex/vertex/component.h>
#include<vcg/simplex/face/base.h>//
#include<vcg/simplex/face/component.h>
//#include<vcg/simplex/face/topology.h>//
#include<vcg/complex/trimesh/base.h>//

// input output
#include<wrap/io_trimesh/import.h>
#include<wrap/io_trimesh/export.h>//just in case

// topology computation
//#include<vcg/complex/trimesh/update/topology.h>//
//#include<vcg/complex/trimesh/update/flag.h>//

// half edge iterators
//#include<vcg/simplex/face/pos.h>

// normals and curvature
#include<vcg/complex/trimesh/update/bounding.h> //class UpdateNormals 
#include<vcg/complex/trimesh/update/normal.h> //class UpdateNormals 
#include<vcg/complex/trimesh/update/curvature.h> //class curvature

#include "../utils/release_assert.h"

extern Logging * lgn;

class vcgFace; // dummy prototype
class vcgVertex;

struct vcgUsedType : public vcg::UsedTypes< vcg::Use<vcgVertex>::AsVertexType,vcg::Use<vcgFace>::AsFaceType>{};

class vcgVertex  : public vcg::Vertex< vcgUsedType, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Normal3f,vcg::vertex::BitFlags ,vcg::vertex::Color4b >{};
class vcgFace    : public vcg::Face  < vcgUsedType, vcg::face::VertexRef,  vcg::face::Normal3f,  vcg::face::Color4b,vcg::face::BitFlags> {};
class vcgMesh    : public vcg::tri::TriMesh< std::vector<vcgVertex>, std::vector<vcgFace> > {};


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
	OVertex(bool _isExternal = false):isExternal(_isExternal){}

	template <class VertexType>
	OVertex(const float & x,const float & y,const float & z, bool _isExternal = false):isExternal(_isExternal){
	 p[0]=x;p[1]=y;p[2]=z;}
	OVertex(const vcg::Point3f _p, bool _isExternal = false):isExternal(_isExternal){ p = _p; }
	
	template <class VertexType>
	OVertex(const VertexType & v){
	 p =v.P();
	 isExternal = false;
	}
	/*
	TODO, take these booleans out of the way. Flags will be stored separately
	*/
	// isExternal is true if the vertex is not in this cell
	bool isExternal;


	unsigned int GetIndexToExternal()const{ 
											RAssert(isExternal);
											RAssert(p[0]>=0);
											RAssert( (p[0]-floor(p[0]))==0.0);
										return (unsigned int) p[0];
									}
	void SetIndexToExternal(unsigned int v){p[0] = static_cast<float>( v);p[1]= p[2]=0.0;isExternal = true;};
	vcg::Point3f & P(){return p;}

private:
	// if isExternal == false this is the position of the vertex
	// otherwise p[0] indicates the position in Cell::externalReferences
	vcg::Point3f p;
};




#endif // __VCG_MESH__
