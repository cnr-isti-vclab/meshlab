#ifndef PARAM_MESH
#define PARAM_MESH

// stuff to define the mesh
#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component_rt.h>
#include <vcg/simplex/edge/base.h>
#include <vcg/complex/trimesh/base.h>
#include "iso_parametrization.h"

class BaseVertex;
class BaseEdge;    
class BaseFace;

class BaseUsedTypes: public vcg::UsedTypes < vcg::Use<BaseVertex>::AsVertexType,
                                          vcg::Use<BaseEdge   >::AsEdgeType,
                                          vcg::Use<BaseFace  >::AsFaceType >{};


///AUXILIARY STRUCTURES USED FOR PARAMETRIZATION COMPUTATION
class BaseVertex  : public vcg::Vertex< BaseUsedTypes,
	vcg::vertex::VFAdj, 
	vcg::vertex::Coord3f, 
	vcg::vertex::Normal3f, 
	vcg::vertex::Mark, 
	vcg::vertex::BitFlags,
	vcg::vertex::Color4b,
	vcg::vertex::TexCoord2f>
{
public: 

	ScalarType area;
	CoordType RPos;
	BaseVertex *brother;
	BaseFace *father;
	CoordType Bary;
	//ScalarType Damp;

	BaseVertex()
	{
		brother=NULL;
		//Damp=1;
		//num_collapse=1;
	}
	
	static const bool Has_Auxiliary(){return true;}

	vcg::Point2<ScalarType> RestUV;
	
	vcg::Color4b OriginalCol;
	//int num_collapse;

  void ImportLocal(const BaseVertex  & left )
  {
    vcg::Vertex< BaseUsedTypes,
	vcg::vertex::VFAdj, 
	vcg::vertex::Coord3f, 
	vcg::vertex::Normal3f, 
	vcg::vertex::Mark, 
	vcg::vertex::BitFlags,
	vcg::vertex::Color4b,
	vcg::vertex::TexCoord2f>::ImportLocal(left);

    this->area=left.area;
	this->RPos=left.RPos;
	this->brother=left.brother;
	this->father=left.father;
	this->Bary=left.Bary;
  }
  
   template < class LeftV>
  void ImportLocal(const LeftV  & left )
  {
    vcg::Vertex< BaseUsedTypes,
	vcg::vertex::VFAdj, 
	vcg::vertex::Coord3f, 
	vcg::vertex::Normal3f, 
	vcg::vertex::Mark, 
	vcg::vertex::BitFlags,
	vcg::vertex::Color4b,
	vcg::vertex::TexCoord2f>::ImportLocal(left);
  }

  
};

///class maintaing additional auxiliary data used during the parameterization
class AuxiliaryVertData{
public:
	BaseVertex::ScalarType area;
	BaseVertex::CoordType RPos;
	BaseVertex *brother;
	BaseFace *father;
	BaseVertex::CoordType Bary;
	
	vcg::Point2<BaseVertex::ScalarType> RestUV;
	
	vcg::Color4b OriginalCol;

	AuxiliaryVertData()
	{brother=NULL;}
};

class AuxiliaryFaceData
{
public:
	std::vector<std::pair<BaseVertex*,vcg::Point3f> > vertices_bary;
	typedef std::vector<std::pair<BaseVertex*,vcg::Point3f> >::iterator IteVBary;
	vcg::Color4b group;
	BaseVertex::ScalarType areadelta;
	vcg::Color4b colorDivision;
};

class BaseFace;

class BaseEdge : public vcg::Edge<BaseUsedTypes,vcg::edge::EVAdj> {
public:
	inline BaseEdge() {};
	inline BaseEdge( BaseVertex * v0, BaseVertex * v1)
	{
		V(0)=v0;
		V(1)=v1;
	}
	//EdgeBase<BaseVertex,BaseEdge,BaseFace>(v0,v1){};
	static inline BaseEdge OrderedEdge(BaseVertex* v0,BaseVertex* v1){
		if(v0<v1) return BaseEdge(v0,v1);
		else return BaseEdge(v1,v0);
	}
};


class BaseFace    : public vcg::Face  < BaseUsedTypes,
	vcg::face::VFAdj, 
	vcg::face::FFAdj,
	vcg::face::VertexRef, 
	vcg::face::BitFlags,
	vcg::face::EdgePlane,
	vcg::face::Mark,
	vcg::face::Normal3f,
	vcg::face::Color4b>
{
public:
	std::vector<std::pair<BaseVertex*,vcg::Point3f> > vertices_bary;
	typedef std::vector<std::pair<BaseVertex*,vcg::Point3f> >::iterator IteVBary;
	vcg::Color4b group;
	ScalarType areadelta;
	vcg::Color4b colorDivision;

   template < class LeftV>
  void ImportLocal(const LeftV  & left )
  {
    vcg::Face  < BaseUsedTypes,
	vcg::face::VFAdj, 
	vcg::face::FFAdj,
	vcg::face::VertexRef, 
	vcg::face::BitFlags,
	vcg::face::EdgePlane,
	vcg::face::Mark,
	vcg::face::Normal3f,
	vcg::face::Color4b>::ImportLocal(left);
  }
	
  void ImportLocal(const BaseFace  & left )
  {
    vcg::Face  < BaseUsedTypes,
	vcg::face::VFAdj, 
	vcg::face::FFAdj,
	vcg::face::VertexRef, 
	vcg::face::BitFlags,
	vcg::face::EdgePlane,
	vcg::face::Mark,
	vcg::face::Normal3f,
	vcg::face::Color4b>::ImportLocal(left);
    this->vertices_bary = std::vector<std::pair<BaseVertex*,vcg::Point3f> > (left.vertices_bary);
	this->group=left.group;
	this->areadelta=left.areadelta;
	this->colorDivision=left.colorDivision;
  }

 
};              

/// the main mesh class 
class BaseMesh: public vcg::tri::TriMesh<std::vector<BaseVertex>, std::vector<BaseFace> > {};

#endif
