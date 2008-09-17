
#ifndef edit_topomeshbuilder_H
#define edit_topomeshbuilder_H

#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/intersection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/closest.h>

#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/index/aabb_binary_tree/aabb_binary_tree.h>
#include <vcg/space/index/octree.h>
#include <vcg/space/index/spatial_hashing.h>

using namespace std;
using namespace vcg;


#include "edit_topodialog.h"




template<class MESH_TYPE>
class NearestMidPoint : public   std::unary_function<face::Pos<typename MESH_TYPE::FaceType> ,  typename MESH_TYPE::CoordType >
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;

	//  typedef AABBBinaryTreeIndex<CMeshO::FaceType, typename CMeshO::ScalarType, vcg::EmptyClass> MetroMeshGrid;
	//	typedef Octree<CMeshO::FaceType, typename CMeshO::ScalarType> MetroMeshGrid;

public:
	bool DEBUG;
	QList<Point3f> * LinMid;
	QList<Point3f> * LoutMid;

 	void init(CMeshO *_m, float dist)
	{
		m=_m;
		if(m) 
		{
			unifGrid.Set(m->face.begin(),m->face.end());
			markerFunctor.SetMesh(m);
			dist_upper_bound = dist;

		}
	}

	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType>  ep)
	{
		Point3f closestPt, closestPt1, closestPt2, normf, bestq, ip;

		float dist = dist_upper_bound;
		vcg::face::PointDistanceBaseFunctor PDistFunct;

/*		const CMeshO::CoordType &p1 = ep.f->V(ep.z)->P();
		unifGrid.GetClosest(PDistFunct,markerFunctor,p1,dist_upper_bound,dist,closestPt1);
		dist=dist_upper_bound;
		const CMeshO::CoordType &p2 = ep.f->V1(ep.z)->P();
		unifGrid.GetClosest(PDistFunct,markerFunctor,p2,dist_upper_bound,dist,closestPt2);
		const CMeshO::CoordType &startPt= ( p1 + p2 ) / 2.0;	*/

		const CMeshO::CoordType &startPt= (ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0;
		CMeshO::FaceType *nearestF=0;
		
		dist=dist_upper_bound;
	  
//		GetClosest(	OBJPOINTDISTFUNCTOR & _getPointDistance, 
//					OBJMARKER & _marker, 
//					const CoordType & _p, 
//					const ScalarType & _maxDist,
//					ScalarType & _minDist, 
//					CoordType & _closestPt)

		// Incremental distance 
		Point3f p1 = ep.f->V(ep.z)->P();
		Point3f p2 = ep.f->V1(ep.z)->P();
		float incDist = sqrt(sqr(p1.X()-p2.X())+sqr(p1.Y()-p2.Y())+sqr(p1.Z()-p2.Z()));
		incDist = incDist * distPerc;

		dist_upper_bound = incDist;

		nearestF =  unifGrid.GetClosest(PDistFunct, 
										markerFunctor, 
										startPt, 
										dist_upper_bound, 
										dist, 
										closestPt);

/*		QString x1 = " Dist "+ QString("%1").arg(dist) +" Dist up "+ QString("%1").arg(dist_upper_bound);
		qDebug(x1.toLatin1());
		QString x = " x "+ QString("%1").arg(startPt.X())+ " y "+QString("%1").arg(startPt.Y()) + " z " + QString("%1").arg(startPt.Z());
		qDebug(x.toLatin1()); */
								
		// If the closest point has not been found, then, softly, use the original vertex and say: laplacian smooth is needed!
		if(dist == dist_upper_bound) 
		{
			nv.P()= startPt;
			nv.N()= ((ep.f->V(ep.z)->N() + ep.f->V(ep.z)->N())/2).Normalize();
			nv.C().lerp(ep.f->V(ep.z)->C(),ep.f->V1(ep.z)->C(),.5f);
			nv.Q() = ((ep.f->V(ep.z)->Q()+ep.f->V1(ep.z)->Q())) / 2.0;

			nv.SetS();

			qDebug(" ---> get <--- ");
			if(DEBUG)
				LoutMid->push_back(startPt);
		}
		else
		{
			nv.P()= closestPt; 

			Point3f interp;
			if(InterpolationParameters(*nearestF, closestPt, interp[0], interp[1], interp[2]))
			{
				assert(ret);
				interp[2]=1.0-interp[1]-interp[0];

				nv.P()= closestPt; 
				nv.N()= ((nearestF->V(0)->N() + nearestF->V(1)->N() + nearestF->V(2)->N())/3).Normalize();
				nv.C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),nearestF->V(2)->C(),interp);
				nv.Q() = nearestF->V(0)->Q()*interp[0] + nearestF->V(1)->Q()*interp[1] + nearestF->V(2)->Q()*interp[2];

				nv.ClearS();
			}		
		}
	}

	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TexCoord2<FL_TYPE,1> WedgeInterp(TexCoord2<FL_TYPE,1> &t0, TexCoord2<FL_TYPE,1> &t1)
	{
		TexCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}

	float dist_upper_bound; // maximum upper distance (deprecated. See below "distPerc")
	float distPerc; // distance for getClosest() is evalutated as a % of the edge's length (float from 0.01 to 0.99)

private: 
	CMeshO *m; 
	MetroMeshGrid   unifGrid;

	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;	
};
























class RetopMeshBuilder
{
public:
	NearestMidPoint<CMeshO> * midSampler;

	QList<Point3f> Lin;
	QList<Point3f> Lout;

	RetopMeshBuilder() {};
	void init(MeshModel *_m, double dist);

	void createRefinedMesh(MeshModel &out, MeshModel &in, float dist, int iterations, QList<Fce> Fstack, QList<Vtx> stack, edit_topodialog *dialog, bool DEBUG);

private:
//	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;

	void createBasicMesh(MeshModel &out, QList<Fce> Fstack, QList<Vtx> Vstack);
};


























#endif


