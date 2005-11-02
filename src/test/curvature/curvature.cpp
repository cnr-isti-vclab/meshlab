#include <iostream>
#include <time.h>
#include<vcg/simplex/vertexplus/base.h>
#include "curvature.h"

using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFace;
class CVertex;
class pVertex;

class CVertex : public VertexSimp2<CVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Curvaturef >{};
class pVertex : public VertexSimp2<pVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f >{};

int main(int , char **)
{
	CVertex v;
	pVertex p;

	assert(v.HasCurvature());
		cout << "Sono v: Ho la curvatura!" << endl;
	

	assert(p.HasCurvature());
		cout << "Sono p: Ho la curvatura (ma non e' vero... )!" << endl;
	
	return 1;
}
