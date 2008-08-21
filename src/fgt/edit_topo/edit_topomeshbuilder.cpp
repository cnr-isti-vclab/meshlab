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
/****************************************************************************
  History
$Log: edit_topomeshbuuilder.h,v $
****************************************************************************/


#include "edit_topomeshbuilder.h"
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/color.h>

void RetopMeshBuilder::init(MeshModel *_mm, double dist)
{
	_mm->updateDataMask(MeshModel::MM_FACEMARK);
	midSampler = new NearestMidPoint<CMeshO>();
	midSampler->init(&_mm->cm, dist);
}

void RetopMeshBuilder::createBasicMesh(MeshModel &outMesh, QList<Fce> Fstack, QList<Vtx> Vstack) 
{
	// Vertex names compact
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


	outMesh.updateDataMask(MeshModel::MM_FACETOPO);

	bool oriented,orientable;
    tri::Clean<CMeshO>::IsOrientedMesh(outMesh.cm, oriented,orientable); 
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(outMesh.cm);
	vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(outMesh.cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(outMesh.cm);

	outMesh.clearDataMask(MeshModel::MM_FACETOPO);

/*	DEBUG
	outMesh.cm.Clear();
	vcg::tri::Allocator<CMeshO>::AddVertices(outMesh.cm, 6);
	vcg::tri::Allocator<CMeshO>::AddFaces(outMesh.cm, 5);


	QVector<CMeshO::VertexPointer> ivp(6);

	CMeshO::VertexIterator vi;	

	vi=outMesh.cm.vert.begin(); 

	ivp[0] = &*vi; (*vi).P() = Point3f(-0.053,		0.104,		0.040); ++vi;
	ivp[1] = &*vi; (*vi).P() = Point3f(-0.031,		0.113,		0.034); ++vi;
	ivp[2] = &*vi; (*vi).P() = Point3f(-0.031,		0.094,		0.044); ++vi;
	ivp[3] = &*vi; (*vi).P() = Point3f(-0.012,		0.105,		0.043); ++vi;
	ivp[4] = &*vi; (*vi).P() = Point3f(-0.046,		0.082,		0.043); ++vi;
	ivp[5] = &*vi; (*vi).P() = Point3f(-0.017,		0.084,		0.057);

	CMeshO::FaceIterator fi;
	fi=outMesh.cm.face.begin();

	(*fi).V(0) = ivp[0]; (*fi).V(1) = ivp[2]; (*fi).V(2) = ivp[4]; fi++;
	(*fi).V(0) = ivp[0]; (*fi).V(1) = ivp[1]; (*fi).V(2) = ivp[2]; fi++;
	(*fi).V(0) = ivp[3]; (*fi).V(1) = ivp[2]; (*fi).V(2) = ivp[1]; fi++;
	(*fi).V(0) = ivp[3]; (*fi).V(1) = ivp[5]; (*fi).V(2) = ivp[2]; fi++;
	(*fi).V(0) = ivp[4]; (*fi).V(1) = ivp[2]; (*fi).V(2) = ivp[5]; 
*/
}



 void RetopMeshBuilder::createRefinedMesh(MeshModel &outMesh, MeshModel &in, double dist, int iterations, QList<Fce> Fstack, QList<Vtx> stack, edit_topodialog *dialog, bool DEBUG)
{
	dialog->setBarMax(iterations++);//pow((float)(Fstack.count() * 4), (float)iterations) );

	midSampler->DEBUG = DEBUG;

	midSampler->LinMid = &Lin;
	midSampler->LoutMid = &Lout;

	createBasicMesh(outMesh, Fstack, stack);

	outMesh.updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
	if(tri::Clean<CMeshO>::IsTwoManifoldFace(outMesh.cm))
		for(int i=0; i<iterations; i++)
		{
			outMesh.updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
			Refine<CMeshO,NearestMidPoint<CMeshO> >(outMesh.cm, *midSampler /*MyMidPoint<CMeshO>()*/, 0, false, 0);
	//		Refine<CMeshO,MidPoint<CMeshO>>(outMesh.cm, MidPoint<CMeshO>(), 0, false, 0);
			outMesh.clearDataMask( MeshModel::MM_VERTFACETOPO);
			dialog->setBarVal(i);
		}

	outMesh.fileName = "Retopology.ply";
	tri::UpdateBounding<CMeshO>::Box(outMesh.cm);
//	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(outMesh.cm);

	outMesh.updateDataMask(MeshModel::MM_FACETOPO);

	bool oriented,orientable;
    tri::Clean<CMeshO>::IsOrientedMesh(outMesh.cm, oriented,orientable); 
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(outMesh.cm);
	vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(outMesh.cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(outMesh.cm);

	outMesh.clearDataMask(MeshModel::MM_FACETOPO);

	/* DEBUG-OLD
	// foreach "iterations"
		// Foreach face
			// Foreach vertex: calculate mid points
			// Foreach midpoint: calculate real position over the orignial mesh (with unif grid???)
			// Create the new data structure for vtx and edges
	// At the end make a old-good createSimpleMesh call, with the new structures (Fstack and Vstack), et voilà!
	
	int vtxId = 0;

	QList<Fce> newFstack, tempFstack;
	QList<Vtx> newVstack, tempVstack;

	tempFstack = Fstack;

	for(int it=0; it<iterations; it++)
	{
		for(int f=0; f<tempFstack.count(); f++)
		{		
			Fce fc = tempFstack.at(f);

			if(fc.selected)
			{

			QList<Vtx> allVert;
			for(int e=0; e<3; e++)
				for(int v=0; v<2; v++)
					if(!allVert.contains(fc.e[e].v[v]))
						allVert.push_back(fc.e[e].v[v]);

//				
//					New vertex naming convention:
//
//						v0 
//						/\
//					   /  \
//				   nv0/----\nv2
//					 / \  / \
//					/	\/   \	
//				 v1/----------\v2
//						nv1
//				

			Point3f av0 = allVert.at(0).V;
			Point3f av1 = allVert.at(1).V;
			Point3f av2 = allVert.at(2).V;

			//if(it!=0)
			{
//				Point3f Rv0 = getClosestPoint(av0, d1, d2); av0 = Rv0;
//				Point3f Rv1 = getClosestPoint(av1, d1, d2); av1 = Rv1;
//				Point3f Rv2 = getClosestPoint(av2, d1, d2); av2 = Rv2;

				Lin.push_back(av0);
				Lin.push_back(av1);
				Lin.push_back(av2);

				sampler.sample(av0);
				Point3f Rv0 = av0;
				sampler.sample(av1);
				Point3f Rv1 = av1;
				sampler.sample(av2);
				Point3f Rv2 = av2;

				Lout.push_back(Rv0);
				Lout.push_back(Rv1);
				Lout.push_back(Rv2);
			}

			Point3f nv0P = (av0+av1)/2;
			Point3f nv1P = (av1+av2)/2;
			Point3f nv2P = (av2+av0)/2;

//			Point3f Rnv0 = getClosestPoint(nv0P, d1, d2);
//			Point3f Rnv1 = getClosestPoint(nv1P, d1, d2);
//			Point3f Rnv2 = getClosestPoint(nv2P, d1, d2);

				Lin.push_back(nv0P);
				Lin.push_back(nv1P);
				Lin.push_back(nv2P);

			sampler.sample(nv0P);
			Point3f Rnv0 = nv0P;
			sampler.sample(nv1P);
			Point3f Rnv1 = nv1P;
			sampler.sample(nv2P);
			Point3f Rnv2 = nv2P;

				Lout.push_back(Rnv0);
				Lout.push_back(Rnv1);
				Lout.push_back(Rnv2);

			Vtx v0, v1, v2;
			v0.V = av0; v0.vName = QString("%1").arg(vtxId++);
			v1.V = av1;	v1.vName = QString("%1").arg(vtxId++);
			v2.V = av2; v2.vName = QString("%1").arg(vtxId++);

			Vtx nv0, nv1, nv2;
			nv0.V = Rnv0; nv0.vName = QString("%1").arg(vtxId++);
			nv1.V = Rnv1; nv1.vName = QString("%1").arg(vtxId++);
			nv2.V = Rnv2; nv2.vName = QString("%1").arg(vtxId++);
		    
			newVstack.push_back(v0);newVstack.push_back(v1);newVstack.push_back(v2);
			newVstack.push_back(nv0);newVstack.push_back(nv1);newVstack.push_back(nv2);

			Fce fv0nv0nv2, fnv0nv1nv2, fv1nv0nv1, fnv1nv2v2;
			// face ( nv1 - nv2 - v2 )
			fnv1nv2v2.e[0].v[0] = nv1;
			fnv1nv2v2.e[0].v[1] = nv2;
			fnv1nv2v2.e[1].v[0] = nv2;
			fnv1nv2v2.e[1].v[1] = v2;
			fnv1nv2v2.e[2].v[0] = v2;
			fnv1nv2v2.e[2].v[1] = nv1;
			newFstack.push_back(fnv1nv2v2);

			// face ( v1 - nv0 - nv1 )
			fv1nv0nv1.e[0].v[0] = v1;
			fv1nv0nv1.e[0].v[1] = nv0;
			fv1nv0nv1.e[1].v[0] = nv0;
			fv1nv0nv1.e[1].v[1] = nv1;
			fv1nv0nv1.e[2].v[0] = nv1;
			fv1nv0nv1.e[2].v[1] = v1;
			newFstack.push_back(fv1nv0nv1);

			// face ( v0 - nv0 - nv2 )
			fv0nv0nv2.e[0].v[0] = v0;
			fv0nv0nv2.e[0].v[1] = nv0;
			fv0nv0nv2.e[1].v[0] = nv0;
			fv0nv0nv2.e[1].v[1] = nv2;
			fv0nv0nv2.e[2].v[0] = nv2;
			fv0nv0nv2.e[2].v[1] = v0;
			newFstack.push_back(fv0nv0nv2);

			// face ( nv0 - nv1 - nv2 )
			fnv0nv1nv2.e[0].v[0] = nv0;
			fnv0nv1nv2.e[0].v[1] = nv2;
			fnv0nv1nv2.e[1].v[0] = nv2;
			fnv0nv1nv2.e[1].v[1] = nv1;
			fnv0nv1nv2.e[2].v[0] = nv1;
			fnv0nv1nv2.e[2].v[1] = nv0;		
			newFstack.push_back(fnv0nv1nv2);

			dialog->setBarVal(vtxId);

			}
		}

		for(int j=0; j<newFstack.count(); j++)
			tempFstack.push_back(newFstack.at(j));

		for(int j=0; j<newVstack.count(); j++)
			tempVstack.push_back(newVstack.at(j));

		newVstack.clear(); newFstack.clear();
	}

	createBasicMesh(outMesh, tempFstack, tempVstack);

	dialog->setBarVal(0);
*/
}




