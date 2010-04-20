/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


#ifndef edit_topomeshbuilder_H
#define edit_topomeshbuilder_H

#include <common/interfaces.h>

#include <vcg/complex/intersection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/closest.h>

#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/index/aabb_binary_tree/aabb_binary_tree.h>
#include <vcg/space/index/octree.h>
#include <vcg/space/index/spatial_hashing.h>

#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/smooth.h>

#include "edit_topodialog.h"




//**************************************************************
//	template class NearestMidPoint
//		This class is used by the retopology algorithm
//		to obtain the closest point to each "ideal" new
//		mesh vertex
//
//	The operator () is called by the standard vcg "Refine<,>"
//	method to obtain the new vertices coordinates
//
//	Because of the great reuse of that operator, this class
//  needs to be initialized with the original "source" model.
//	This can be done by calling the "init" method
//
template<class MESH_TYPE>
class NearestMidPoint : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType>, typename MESH_TYPE::CoordType>
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;

public:
	// if DEBUG value is true, the class will fill
	// the LoutMid QList with ALL the "failed" vertices.
	// So, LoutMid will contain all vertices that have not
	// been found by the UnifGrid.GetClosest() function
	bool DEBUG;
	QList<Point3f> * LoutMid;

	// Uniform grid, init by the "init()" method
	MetroMeshGrid   unifGrid;

	// Marker
	typedef tri::FaceTmark<MESH_TYPE> MarkerFace;
	MarkerFace markerFunctor;

	// All of the data structures used by the retopology algo have
	// to be initialized *before* the function call.
	//
	// This is necessary in order to reduce comput time
	void init(MESH_TYPE *_m, float dist)
	{
		m=_m;
		if(m)
		{
			// Set up uniform grid
			unifGrid.Set(m->face.begin(),m->face.end());
			markerFunctor.SetMesh(m);
			dist_upper_bound = dist;
		}
	}

	// Standard operator called by Refine<>
	//
	// If you want to change the way new vertices are generated
	// you have to modify the "VertexTypr nv" output coords
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType>  ep)
	{
		Point3f closestPt, normf, bestq, ip;

		float dist = dist_upper_bound;
		vcg::face::PointDistanceBaseFunctor<float> PDistFunct;

		// startPt is the point from where the "GetClosest" query will start
		const typename MESH_TYPE::CoordType &startPt= (ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0;
		CMeshO::FaceType *nearestF=0;

		// in "dist" will be returned the closest point distance from startPt
		dist=dist_upper_bound;

		Point3f p1 = ep.f->V(ep.z)->P();
		Point3f p2 = ep.f->V1(ep.z)->P();
		float incDist = sqrt(math::Sqr(p1.X()-p2.X())+math::Sqr(p1.Y()-p2.Y())+math::Sqr(p1.Z()-p2.Z()));

		// distPerc is the % distance used to evaluate the maximum query distance
		incDist = incDist * distPerc;

		// dist_ upper_bound is the maximum query distance, evaluated
		// with a % factor given by the user
		dist_upper_bound = incDist;

		// Query the uniform grid and get the original mesh's point nearest to startPt
		nearestF =  unifGrid.GetClosest(PDistFunct,
										markerFunctor,
										startPt,
										dist_upper_bound,
										dist,
										closestPt);

		// Output distance has not changed: no closest point found.
		// The original "ideal" point will be used, and then will be
		// smoothed with laplacian smooth algorithm
		if(dist == dist_upper_bound)
		{
			nv.P()= startPt;
			nv.N()= ((ep.f->V(ep.z)->N() + ep.f->V(ep.z)->N())/2).normalized();
			nv.C().lerp(ep.f->V(ep.z)->C(),ep.f->V1(ep.z)->C(),.5f);
			nv.Q() = ((ep.f->V(ep.z)->Q()+ep.f->V1(ep.z)->Q())) / 2.0;

			// Mark it as selected to smooth it
			nv.SetS();

			qDebug("Unable to find closest point. Marked for smoothing");

			// If debug mode is on, the point will be drawn in meshlab
			if(DEBUG) LoutMid->push_back(startPt);
		}
		// distance has changed: got the closest point
		else
		{
			nv.P()= closestPt;

			Point3f interp;
			// Try to interpolate vertex colors and normals
			if(InterpolationParameters(*nearestF, closestPt, interp[0], interp[1], interp[2]))
			{
				interp[2]=1.0-interp[1]-interp[0];

				nv.P()= closestPt;
				nv.N()= ((nearestF->V(0)->N() + nearestF->V(1)->N() + nearestF->V(2)->N())/3).normalized();
				nv.C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),nearestF->V(2)->C(),interp);
				nv.Q() = nearestF->V(0)->Q()*interp[0] + nearestF->V(1)->Q()*interp[1] + nearestF->V(2)->Q()*interp[2];

				nv.ClearS();
			}
		}
	}

	// Color interpolation called by Refine<,>
	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	// Color interpolation called by Refine<,>
	template<class FL_TYPE>
	TexCoord2<FL_TYPE,1> WedgeInterp(TexCoord2<FL_TYPE,1> &t0, TexCoord2<FL_TYPE,1> &t1)
	{
		TexCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n();
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}

	float dist_upper_bound; // maximum upper distance (See below "distPerc")
	float distPerc; // distance for getClosest() is evalutated as a % of the edge's length (float from 0.01 to 0.99)

private:
	// Internal mesh model
	CMeshO *m;
};











//**************************************************************
//	class RetopMeshBuilder
//		This class contains the retopology algorithm,
//		and is used for "edit" and "filter" plugin modes
//
class RetopMeshBuilder
{
public:
	// Mid point sampler object, used by "Refine" in the retopo process
	NearestMidPoint<CMeshO> * midSampler;

	// This list will be filled (in debug mode) with all the "not found" closest vertices
	QList<Point3f> Lout;

	RetopMeshBuilder() {};

	// init mid point sampler object
	void init(MeshModel *_mm, double dist)
	{
		_mm->updateDataMask(MeshModel::MM_FACEMARK);
		midSampler = new NearestMidPoint<CMeshO>();
		midSampler->init(&_mm->cm, dist);
	}

	//
	// Creates the retopology mesh in edit mode (edit_topo plugin)
	//
	void createRefinedMesh(MeshModel &outMesh, /*MeshModel &in,*/ float dist, int iterations, QList<Fce> Fstack, QList<Vtx> stack, edit_topodialog *dialog, bool DEBUG)
	{
		dialog->setBarMax(iterations);
		dialog->setStatusLabel("Init topo");

		midSampler->DEBUG = DEBUG;
		midSampler->distPerc = dist;
		midSampler->LoutMid = &Lout;

		// Create a "flat" mesh from the user defined topology defined
		// in faces stack "Fstack" and vertices stack "stack"
		createBasicMesh(outMesh, Fstack, stack);

		dialog->setStatusLabel("Done");

		CMeshO::FaceIterator fi;
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
		{(*fi).ClearS(); for(int i=0; i<3; i++) (*fi).V(i)->ClearS(); }

		outMesh.updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    if(tri::Clean<CMeshO>::CountNonManifoldEdgeFF(outMesh.cm)==0)
			for(int i=0; i<iterations; i++)
			{
				dialog->setStatusLabel("Iter: "+QString("%1").arg(i+1));

				// This is che core point of the retopology plugin:
				//	here "Refine" is called with "NearestMidPoint" and "midSampler"
				//		- midSampler uses an uniform grid to get the closest point
				//		  for each given vertex, and "builds" the new mesh by adapting
				//		  it over the existing "in" meshmodel
				//	If the midSampler fails (for example if a hole is found), each
				//  vertex is marked with SetS() and will be smoothed later
				//
				outMesh.updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				Refine<CMeshO, NearestMidPoint<CMeshO> >(outMesh.cm, *midSampler, 0, false, 0);
				outMesh.clearDataMask( MeshModel::MM_VERTFACETOPO);
				dialog->setBarVal(i+1);
			}

        outMesh.setFileName("Retopology.ply");
		tri::UpdateBounding<CMeshO>::Box(outMesh.cm);

		dialog->setStatusLabel("Normals");

		// Recalculate new model's face normals and select faces with selected vertices
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
		{
			(*fi).N()=((fi->V(0)->N() + fi->V(1)->N() + fi->V(2)->N())/3);
			(*fi).ClearS();

			for(int i=0; i<3; i++)
				if((*fi).V(i)->IsS())
					(*fi).SetS();
		}

		// Expand selected faces to obtain a more refined smooth
		for(int i=0; i<(1+(int)(iterations/4)); i++)
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
			if((*fi).IsS())
				for(int i=0; i<3; i++)
					(*fi).FFp(i)->SetS();

		dialog->setStatusLabel("Lapl smooth");

		// Laplacian smooth for selected faces
		tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(outMesh.cm);
		tri::Smooth<CMeshO>::VertexCoordLaplacian(outMesh.cm,3,true,0);
		dialog->setStatusLabel("Done");
	}





	//********************************************************************************************//
	//
	//    Creates a retop mesh for the "filter" mode (not used in edit_topo)
	//		  -	userMesh = The low level mesh selected by the user as new basic topology
	//		  -	inMesh = The original mesh from where the userMesh has been created
	//		  - it = Number of iterations (from 0 to 10)
	//		  -	dist = % for incremental distance (from 0.01 to 0.99)
	//		  -	outMesh = The output retopology mesh
	//
	//
	//********************************************************************************************//
	bool applyTopoMesh(MeshModel &userTopoMesh, MeshModel &inModel, int it, float dist, MeshModel &outMesh)
	{
		// turn off debug mode
		midSampler->DEBUG = false;

		midSampler->distPerc = dist;
		outMesh.updateDataMask(MeshModel::MM_FACEFACETOPO);

		// Update topology for in mesh, to be sure that the model can be refined
		bool oriented,orientable;
		tri::Clean<CMeshO>::IsOrientedMesh(outMesh.cm, oriented,orientable);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(outMesh.cm);
		vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(outMesh.cm);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(outMesh.cm);
		outMesh.clearDataMask(MeshModel::MM_FACEFACETOPO);

		// Clear faces and vertices selection
		CMeshO::FaceIterator fi;
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
		{(*fi).ClearS(); for(int i=0; i<3; i++) (*fi).V(i)->ClearS(); }

		outMesh.updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    if(tri::Clean<CMeshO>::CountNonManifoldEdgeFF(outMesh.cm)==0)
		{
			for(int i=0; i<it; i++)
			{
				outMesh.updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);

				// Call refine to build the retopologized mesh. Important note: "midSampler" needs to be initialized before
				Refine<CMeshO,NearestMidPoint<CMeshO> >(outMesh.cm, *midSampler, 0, false, 0);
				outMesh.clearDataMask( MeshModel::MM_VERTFACETOPO);
			}
		}
		else return false;

        outMesh.setFileName("Retopology.ply");
		tri::UpdateBounding<CMeshO>::Box(outMesh.cm);

		// compute face normals, and select faces for not found vertices (to smooth them)
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
		{
			(*fi).N()=((fi->V(0)->N() + fi->V(1)->N() + fi->V(2)->N())/3);
			(*fi).ClearS();

			for(int i=0; i<3; i++)
				if((*fi).V(i)->IsS())
					(*fi).SetS();
		}

		for(int i=0; i<(1+(int)(it/4)); i++)
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
			if((*fi).IsS())
				for(int i=0; i<3; i++)
					(*fi).FFp(i)->SetS();

		// Laplacian smooth for not-found vertices
		tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(outMesh.cm);
		tri::Smooth<CMeshO>::VertexCoordLaplacian(outMesh.cm,3,true,0);

		return true;
	}



private:
	// Creates the flat initial mesh from the user defined topology
	void createBasicMesh(MeshModel &outMesh, QList<Fce> Fstack, QList<Vtx> Vstack)
	{
		// Elaborate topology relations
		QVector<Vtx> nStack(Vstack.count());
		QVector<Fce> nFstack(Fstack.count()); nFstack = Fstack.toVector();
		for(int i=0; i<Vstack.count(); i++)
		{
			Vtx v = Vstack.at(i);

			for(int j=0; j<Fstack.count(); j++)
			{
				Fce f = nFstack[j];
				for(int n=0; n<3; n++)
					for(int m=0; m<2; m++)
						if(f.e[n].v[m].vName == v.vName)
							f.e[n].v[m].vName = QString("%1").arg(i);

				nFstack[j]=f;
			}
			v.vName = QString("%1").arg(i);
			nStack[i]=v;
		}

		int allFce = 0;
		for(int i=0; i<nFstack.count(); i++)
			if(nFstack.at(i).selected)
				allFce++;

		// Allocate the new mesh
		outMesh.cm.Clear();
		vcg::tri::Allocator<CMeshO>::AddVertices(outMesh.cm, nStack.count());
		vcg::tri::Allocator<CMeshO>::AddFaces(outMesh.cm, allFce);

		QVector<CMeshO::VertexPointer> ivp(Vstack.count());

		int v =0;
		CMeshO::VertexIterator vi;
		for(vi=outMesh.cm.vert.begin(); vi!=outMesh.cm.vert.end(); vi++)
		{
			ivp[v] = &*vi;
			(*vi).P() = Point3f(nStack[v].V.X(), nStack[v].V.Y(), nStack[v].V.Z());

			Point3f closestPt;
			vcg::face::PointDistanceBaseFunctor<float> PDistFunct;

			const CMeshO::CoordType &startPt = (*vi).P();
			CMeshO::FaceType *nearestF=0;

			float d1,d2; d1 = d2 = 1000;

			// Use the sampler to get original vertices normals
			nearestF =  midSampler->unifGrid.GetClosest(PDistFunct,
											midSampler->markerFunctor,
											startPt,
											d1,
											d2,
											closestPt);

			(*vi).C().lerp(nearestF->V(0)->C(),nearestF->V(1)->C(),.5f);
			(*vi).N() = ((nearestF->V(0)->N() + nearestF->V(1)->N() + nearestF->V(2)->N())/3).normalized();

			++v;
		}

		int f = 0;
		CMeshO::FaceIterator fi;
		for(fi=outMesh.cm.face.begin(); fi!=outMesh.cm.face.end(); fi++)
		{
			Fce fce = nFstack[f];

			if(fce.selected)
			{
				QList<Vtx> allV;
				for(int i=0; i<3; i++)
					for(int j=0; j<2; j++)
						if(!allV.contains(fce.e[i].v[j]))
							allV.push_back(fce.e[i].v[j]);

				int ivpId0 = allV.at(0).vName.toInt();
				int ivpId1 = allV.at(1).vName.toInt();
				int ivpId2 = allV.at(2).vName.toInt();

				(*fi).V(0) = ivp.at(ivpId0);
				(*fi).V(1) = ivp.at(ivpId1);
				(*fi).V(2) = ivp.at(ivpId2);
				f++;
			}
		}
		outMesh.updateDataMask(MeshModel::MM_FACEFACETOPO);

		// Re-orient new mesh
		bool oriented,orientable;
		tri::Clean<CMeshO>::IsOrientedMesh(outMesh.cm, oriented,orientable);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(outMesh.cm);
		vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(outMesh.cm);

		outMesh.clearDataMask(MeshModel::MM_FACEFACETOPO);
	}
};


























#endif


