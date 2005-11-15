#include <iostream>
#include <time.h>
#include<vcg/simplex/vertexplus/base.h>
#include "curvature.h"
#include <vcg/complex/trimesh/update/curvature.h>

#include <vcg/simplex/vertex/with/vn.h>
#include <vcg/simplex/face/with/af.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/refine.h>

#include<vcg/complex/trimesh/create/platonic.h>



using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFace;
class CVertex;
class pVertex;

class CVertex : public VertexSimp2<CVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Curvaturef >{};
class pVertex : public VertexSimp2<pVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f >{};

class qVertex : public VertexSimp2<pVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Qualityf >{};

struct MyFace: public FaceAF<qVertex,CEdge,MyFace>{};
struct MyMesh: public tri::TriMesh< vector<qVertex>, vector<MyFace> >{};

int main(int , char **)
{
/*	CVertex v;
	pVertex p;
	assert(v.HasCurvature());
		cout << "Sono v: Ho la curvatura!" << endl;
	assert(p.HasCurvature());
		cout << "Sono p: Ho la curvatura (ma non e' vero... )!" << endl;
	*/

	MyMesh mesh;
	MyMesh::VertexIterator vi;
	int i = 0;
	tri::Tetrahedron(mesh);

	tri::UpdateCurvature<MyMesh>::Gaussian(mesh);


	for(vi=mesh.vert.begin();vi!=mesh.vert.end();++vi) {
		cout << "Vertice " << i++ << " : " << (*vi).Q() << endl;
	}


	return 1;
}
