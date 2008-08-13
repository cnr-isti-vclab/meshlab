
#ifndef edit_topomeshbuilder_H
#define edit_topomeshbuilder_H

#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/intersection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/closest.h>

using namespace std;
using namespace vcg;


#include "edit_topodialog.h"











template<class MESH_TYPE>
class NearestMidPoint : public   std::unary_function<face::Pos<typename MESH_TYPE::FaceType> ,  typename MESH_TYPE::CoordType >
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
public:

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
		Point3f       closestPt,      normf, bestq, ip;
		float dist = dist_upper_bound;
		const CMeshO::CoordType &startPt= (ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0;
		
		// compute distance between startPt and the mesh S2
		CMeshO::FaceType   *nearestF=0;
		vcg::face::PointDistanceBaseFunctor PDistFunct;
		
		dist=dist_upper_bound;
	  
		nearestF =  unifGrid.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);

		if(dist == dist_upper_bound) 
		{
			if( MESH_TYPE::HasPerVertexNormal())
				nv.N()= (ep.f->V(ep.z)->N()+ep.f->V1(ep.z)->N()).Normalize();

			if( MESH_TYPE::HasPerVertexColor())
				nv.C().lerp(ep.f->V(ep.z)->C(),ep.f->V1(ep.z)->C(),.5f);
		
			if( MESH_TYPE::HasPerVertexQuality())
				nv.Q() = ((ep.f->V(ep.z)->Q()+ep.f->V1(ep.z)->Q())) / 2.0;

			nv.P()= startPt; return; 
		}

		Point3f interp;
		if(InterpolationParameters(*nearestF, closestPt, interp[0], interp[1], interp[2]))
		//assert(ret);
			interp[2]=1.0-interp[1]-interp[0];
																			 
		nv.P()= closestPt; 

		if( MESH_TYPE::HasPerVertexNormal())
			nv.N()= (ep.f->V(ep.z)->N()+ep.f->V1(ep.z)->N()).Normalize();

		if( MESH_TYPE::HasPerVertexColor())
			nv.C().lerp(ep.f->V(ep.z)->C(),ep.f->V1(ep.z)->C(),.5f);
		
		if( MESH_TYPE::HasPerVertexQuality())
			nv.Q() = ((ep.f->V(ep.z)->Q()+ep.f->V1(ep.z)->Q())) / 2.0;
		
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

private:
 
	CMeshO *m;           /// the source mesh for which we search the closest points (e.g. the mesh from which we take colors etc). 

	MetroMeshGrid   unifGrid;

	// Parameters
	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;
	
	float dist_upper_bound;
};
























class RetopMeshBuilder
{
public:
	NearestMidPoint<CMeshO> * midSampler;

	QList<Point3f> Lin;
	QList<Point3f> Lout;

	RetopMeshBuilder() {};
	void init(MeshModel *_m, float dist);

	void createBasicMesh(MeshModel &out, QList<Fce> Fstack, QList<Vtx> Vstack);
	void createRefinedMesh(MeshModel &out, MeshModel &in, float dist, int iterations, QList<Fce> Fstack, QList<Vtx> stack, edit_topodialog *dialog);

private:
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
};


























#endif


