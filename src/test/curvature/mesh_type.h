#ifndef __MYMESH
#define __MYMESH

#include <vector>

// stuff to define the mesh
#include <vcg/simplex/vertex/with/afvmvnvq.h>
#include <vcg/simplex/face/with/afavfnfq.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/space/point2.h>
// the trackball
#include <wrap/gui/trackball.h>

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

#endif