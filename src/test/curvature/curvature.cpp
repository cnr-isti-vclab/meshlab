#include <iostream>
#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertex/with/vn.h>
#include <vcg/simplex/face/with/af.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/math/base.h>
#include <vcg/complex/trimesh/update/curvature.h>

//#include "curvature.h"

using namespace vcg;
using namespace std;

class CEdge;
class CFace;

//class CVertex : public VertexSimp2<CVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Curvaturef >{};
class qVertex : public VertexSimp2<qVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Qualityf >{};

class CFace: public FaceAF<qVertex,CEdge,CFace>{};
class MyMesh: public tri::TriMesh< vector<qVertex>, vector<CFace> >{};

static void Gaussian(MyMesh &m){
	
	assert(m.HasPerVertexQuality());

	MyMesh::VertexIterator vi;   // iteratore vertice
	MyMesh::FaceIterator fi;     // iteratore facce
	double *area;                  // areamix vector
	int i;												 // index
	double area0, area1, area2;
	double angle0, angle1, angle2; 
	
	//--- Initialization
	area = new double[m.vn];

	//reset the values to 0
	for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
		(*vi).Q() = 0.0;

	//--- compute Areamix
	for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
	{
		
		// angles
			 angle0 = math::Abs(Angle(	(*fi).V(1)->P()-(*fi).V(0)->P(),(*fi).V(2)->P()-(*fi).V(0)->P() ));
			 angle1 = math::Abs(Angle(	(*fi).V(0)->P()-(*fi).V(1)->P(),(*fi).V(2)->P()-(*fi).V(1)->P() ));
 			 angle2 = M_PI-(angle0+angle1);
		
		if((angle0 < M_PI/2) || (angle1 < M_PI/2) || (angle2 < M_PI/2))  // triangolo non ottuso
		{ 
			float e01 = SquaredDistance( (*fi).V(1)->P() , (*fi).V(0)->P() );
			float e12 = SquaredDistance( (*fi).V(2)->P() , (*fi).V(1)->P() );
			float e20 = SquaredDistance( (*fi).V(0)->P() , (*fi).V(2)->P() );
			
			// voronoi area v[0]
			area0 = ( e01*(1/tan(angle2)) + e20*(1/tan(angle1)) ) /8;
			// voronoi area v[1]
			area1 = ( e01*(1/tan(angle2)) + e12*(1/tan(angle0)) ) /8;
			// voronoi area v[2]
			area2 = ( e20*(1/tan(angle1)) + e20*(1/tan(angle0)) ) /8;
			
			(*fi).V(0)->Q()  += area0;
			(*fi).V(1)->Q()  += area1;
			(*fi).V(2)->Q()  += area2;
		}
		else // triangolo ottuso
		{  
			(*fi).V(0)->Q() += (*fi).Area() / 3;
			(*fi).V(1)->Q() += (*fi).Area() / 3;
			(*fi).V(2)->Q() += (*fi).Area() / 3;            
		}
	}

	i = 0;
	for(vi=m.vert.begin();vi!=m.vert.end();++vi,++i) if(!(*vi).IsD())
	{
		area[i] = (*vi).Q();
		(*vi).Q() = (float)(2.0 * M_PI);
	}

	for(fi=m.face.begin();fi!=m.face.end();++fi)  if(!(*fi).IsD())
	{
		float angle0 = math::Abs(Angle(
			(*fi).V(1)->P()-(*fi).V(0)->P(),(*fi).V(2)->P()-(*fi).V(0)->P() ));
		float angle1 = math::Abs(Angle(
			(*fi).V(0)->P()-(*fi).V(1)->P(),(*fi).V(2)->P()-(*fi).V(1)->P() ));
		float angle2 = M_PI-(angle0+angle1);
		
		(*fi).V(0)->Q() -= angle0;
		(*fi).V(1)->Q() -= angle1;
		(*fi).V(2)->Q() -= angle2;
	}
	i=0;
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
	{
		(*vi).Q() /= area[i];
		(*vi).Q()=math::Clamp((*vi).Q(),-0.050f,0.050f);
		
	}
	
	//--- DeInit
	
	delete[] area;

}


int main(int , char **)
{
	MyMesh mesh_old,mesh_new;
	MyMesh::VertexIterator vi_old,vi_new;
	int i = 0;

	tri::Dodecahedron(mesh_old);
	tri::Dodecahedron(mesh_new);
	
	Gaussian(mesh_new);
	tri::UpdateCurvature<MyMesh>::Gaussian(mesh_old);

	for(vi_old=mesh_old.vert.begin(),vi_new=mesh_new.vert.begin(); vi_old!=mesh_old.vert.end(); ++vi_old,++vi_new) {
		cout << "VERT:" << i++ << " NEW:" << (*vi_old).Q() << " OLD:" << (*vi_new).Q() << endl;
	}

	char r;
	cin >> r;

	return 1;
}
