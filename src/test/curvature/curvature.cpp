#include <iostream>
#include <time.h>
#include <vcg/simplex/vertexplus/base.h>
#include "curvature.h"

#include <vcg/simplex/vertex/with/vn.h>
#include <vcg/simplex/face/with/af.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/create/platonic.h>

using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFace;
class CVertex;
class qVertex;

class CVertex : public VertexSimp2<CVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Curvaturef >{};
class qVertex : public VertexSimp2<qVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Qualityf >{};

class CFace: public FaceAF<qVertex,CEdge,CFace>{};
class MyMesh: public tri::TriMesh< vector<qVertex>, vector<CFace> >{};

int main(int , char **)
{
	MyMesh mesh;
	MyMesh::VertexIterator vi;
	int i = 0;

	tri::Sphere(mesh);
	tri::UpdateCurvature<MyMesh>::Gaussian(mesh);


	for(vi=mesh.vert.begin();vi!=mesh.vert.end();++vi) {
		cout << "Vertice " << i++ << " : " << (*vi).Q() << endl;
	}

	return 1;
}
