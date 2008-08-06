
#ifndef edit_topomeshbuilder_H
#define edit_topomeshbuilder_H

#include <QList>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/closest.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/intersection.h>

#include "edit_topo.h"



#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/simplex/faceplus/component.h>
#include <vcg/simplex/faceplus/component_rt.h>

using namespace std;
using namespace vcg;







class Sampler
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
public:
  
	Sampler()
	{
		m=0;		
	};
 
	CMeshO *m;           /// the source mesh for which we search the closest points (e.g. the mesh from which we take colors etc). 

	int sampleNum;  // the expected number of samples. Used only for the callback
	int sampleCnt;
	MetroMeshGrid   unifGrid;

	// Parameters
	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;
	
	float dist_upper_bound;
 	void init(CMeshO *_m)
	{
		m=_m;
		if(m) 
		{
			unifGrid.Set(m->face.begin(),m->face.end());
			markerFunctor.SetMesh(m);
		}
	}

	// this function is called for each vertex of the target mesh.
	// and retrieve the closest point on the source mesh.
	void sample(Point3f &p) 
	{
		assert(m);
		// the results
		Point3f       closestPt,      normf, bestq, ip;
		float dist = dist_upper_bound;
		const CMeshO::CoordType &startPt= p;
		
		// compute distance between startPt and the mesh S2
		CMeshO::FaceType   *nearestF=0;
		vcg::face::PointDistanceBaseFunctor PDistFunct;
		
		dist=dist_upper_bound;
	  
		nearestF =  unifGrid.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);

		if(dist == dist_upper_bound) return ;																				

		Point3f interp;
		bool ret = InterpolationParameters(*nearestF, closestPt, interp[0], interp[1], interp[2]);
		assert(ret);
		interp[2]=1.0-interp[1]-interp[0];
																			 
		p = closestPt; // (nearestF->V(0)->P() + nearestF->V(1)->P() + nearestF->V(2)->P()) /3; //->V(0)->P();//closestPt;
		/*if(colorFlag)*/ 
	//		p.C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),nearestF->V(2)->C(),interp);
//		if(qualityFlag) p.Q()= nearestF->V(0)->Q()*interp[0] + nearestF->V(1)->Q()*interp[1] + nearestF->V(2)->Q()*interp[2];
	}

};














class RetopMeshBuilder
{
public:

	Sampler sampler;

	QList<Point3f> Lin;
	QList<Point3f> Lout;

	RetopMeshBuilder(MeshModel *originalMeshModel);
	void init(CMeshO *_m, double dist);

	Point3f getClosestPoint(vcg::Point3f toCheck, float dist1, float dist2);//(MeshModel &m2, vcg::Point3f toCheck, QList<Fce> Flist);

	void createBasicMesh(MeshModel &out, QList<Fce> Fstack, QList<Vtx> Vstack);
	void createRefinedMesh(MeshModel &out, int iterations, QList<Fce> Fstack, edit_topodialog *dialog, int d1, int d2);

	void draww(QList<Vtx> Vstack);

private:
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
//	typedef GridStaticPtr<CFace, CMesh::ScalarType > MetroMeshGrid;
	MetroMeshGrid   unifGrid;
	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;



MeshModel *m2;
CMeshO *m;
 };


























#endif


