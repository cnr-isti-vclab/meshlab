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
$Log: edit_retoptoolmeshbuuilder.h,v $
****************************************************************************/


#include "edit_retoptoolmeshbuilder.h"


RetopMeshBuilder::RetopMeshBuilder(MeshModel *originalMeshModel)
{
	Lin.clear();
	Lout.clear();

	m2 = originalMeshModel;

	m = &m2->cm;

	m2->updateDataMask(MeshModel::MM_FACEMARK);
	unifGrid.Set(m->face.begin(), m->face.end());
	unifGrid.ComputeDimAndVoxel();
	markerFunctor.SetMesh(m);

};


Point3f RetopMeshBuilder::getClosestPoint(vcg::Point3f toCheck, float dist1, float dist2)
{
	Point3f closestPt;
//	CMeshO *m = &m2.cm;
//	m2.updateDataMask(MeshModel::MM_FACEMARK);

	Lin.push_back(toCheck);

//	unifGrid.Set(m->face.begin(), m->face.end());
//	markerFunctor.SetMesh(m);

	float dist_upper_bound =m->bbox.Diag()/50;
	float dist = dist_upper_bound;
	const CMeshO::CoordType &startPt= toCheck;
    
	CMeshO::FaceType *nearestF=0;	
	vcg::face::PointDistanceBaseFunctor PDistFunct;

	nearestF = unifGrid.GetClosest(PDistFunct, markerFunctor, startPt, dist_upper_bound, dist, closestPt);

	
	/*
	unifGrid.GetClosest(OBJPOINTDISTFUNCTOR &_getPointDistance,
						OBJMARKER &_marker,
						GridStaticPtr<OBJTYPE,FLT>::CoordType &_p,
						GridStaticPtr<OBJTYPE,FLT>::ScalarType &_maxDist,
						GridStaticPtr<OBJTYPE,FLT>::ScalarType &_minDist,
						GridStaticPtr<OBJTYPE,FLT>::CoordType &_closestPt );


	unifGrid.DoRay( OBJRAYSECTFUNCTOR &_rayintersector,
					OBJMARKER &_marker,
					vcg::Ray3<scalar> &_ray, 
					GridStaticPtr<OBJTYPE,FLT>::ScalarType &_maxDist,
					GridStaticPtr<OBJTYPE,FLT>::ScalarType &_t ); */
						
					

//	closestPt = startPt;
//	nearestF = unifGrid.GetClosest(PDistFunct,markerFunctor,startPt,dist1,dist2,closestPt);
/*	
	QList<CFaceO*> fce;
	QList<Point3f> punti;
	QList<float> dizt;
*/
//	unifGrid.GetKClosest(PDistFunct, markerFunctor, 100, startPt, 1000, fce, dizt, punti);


//	for(int i=0; i <punti.count(); i++)
//		Lout.push_back(punti.at(i));



	//	Point3f interp;
	//	bool ret = InterpolationParameters(*nearestF,closestPt, interp[0], interp[1], interp[2]);

	//	interp[2]=1.0-interp[1]-interp[0];
	//	Lout.push_back(closestPt);												 

//	Lout.push_back(closestPt);
if(nearestF!=0){
	Lout.push_back(nearestF->V(0)->P());
		Lout.push_back(nearestF->V(1)->P());
			Lout.push_back(nearestF->V(2)->P()); 
	}
	return closestPt;
}

 void RetopMeshBuilder::createBasicMesh(MeshModel &out, QList<Fce> Fstack, QList<Vtx> Vstack) 
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

	out.cm.Clear();
	vcg::tri::Allocator<CMeshO>::AddVertices(out.cm, nStack.count());
	vcg::tri::Allocator<CMeshO>::AddFaces(out.cm, nFstack.count());
  
	QVector<CMeshO::VertexPointer> ivp(Vstack.count());

	int v =0;
	CMeshO::VertexIterator vi;	
	for(vi=out.cm.vert.begin(); vi!=out.cm.vert.end(); vi++)
	{
		ivp[v] = &*vi;
		(*vi).P() = Point3f(nStack[v].V.X(), nStack[v].V.Y(), nStack[v].V.Z());
		++v;
	}

	int f = 0;
	CMeshO::FaceIterator fi;
	for(fi=out.cm.face.begin(); fi!=out.cm.face.end(); fi++)
	{
		Fce fce = nFstack[f];
		
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

 void RetopMeshBuilder::createRefinedMesh(MeshModel &out, int iterations, QList<Fce> Fstack, edit_retoptooldialog *dialog, int d1, int d2)
{
	dialog->setBarMax(pow((float)(Fstack.count() * 4), (float)iterations) );
	
	
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

			QList<Vtx> allVert;
			for(int e=0; e<3; e++)
				for(int v=0; v<2; v++)
					if(!allVert.contains(fc.e[e].v[v]))
						allVert.push_back(fc.e[e].v[v]);

				/*
					New vertex naming convention:

						v0 
						/\
					   /  \
				   nv0/----\nv2
					 / \  / \
					/	\/   \	
				 v1/----------\v2
						nv1
				*/

			Point3f av0 = allVert.at(0).V;
			Point3f av1 = allVert.at(1).V;
			Point3f av2 = allVert.at(2).V;

			if(it!=0)
			{
				Point3f Rv0 = getClosestPoint(av0, d1, d2); av0 = Rv0;
				Point3f Rv1 = getClosestPoint(av1, d1, d2); av1 = Rv1;
				Point3f Rv2 = getClosestPoint(av2, d1, d2); av2 = Rv2;
			}

			Point3f nv0P = (av0+av1)/2;
			Point3f nv1P = (av1+av2)/2;
			Point3f nv2P = (av2+av0)/2;

			Point3f Rnv0 = getClosestPoint(nv0P, d1, d2);
			Point3f Rnv1 = getClosestPoint(nv1P, d1, d2);
			Point3f Rnv2 = getClosestPoint(nv2P, d1, d2);

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

		for(int j=0; j<newFstack.count(); j++)
			tempFstack.push_back(newFstack.at(j));

		for(int j=0; j<newVstack.count(); j++)
			tempVstack.push_back(newVstack.at(j));

		newVstack.clear(); newFstack.clear();
	}

	createBasicMesh(out, tempFstack, tempVstack);

	dialog->setBarVal(0);

}




void RetopMeshBuilder::draww(QList<Vtx> Vstack)
{


	Point3f	p0 = Vstack.at(0).V;
	Point3f p1 = Vstack.at(1).V;
//	rds->Set(p0, p1);

	Lin.push_back(Vstack.at(0).V);
	Lin.push_back(Vstack.at(1).V);

	/*
	CMeshO::FaceType *nearestF=0;

    nearestF =	unifGrid.DoRay(fff, markerFunctor, *rds, 1000, t); */


	Point3f punto;
//	rds->ClosestPoint(punto);

Point3f mid = (p0+p1)/2;

	Line3f *line;
	line->Set(mid,p1);

//	line->Normalize();

	if(vcg::IntersectionRayMesh<CMeshO, float>(m, *line, punto))




/*
	Lout.push_back(nearestF->V(0)->P());
	Lout.push_back(nearestF->V(1)->P());
	Lout.push_back(nearestF->V(2)->P());
*/
	Lout.push_back(punto);


}