/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#ifndef BRIDGE_H
#define BRIDGE_H

#include <utility>
#include <meshlab/meshmodel.h>
#include <vcg/simplex/face/topology.h>
#include "vcg/space/line2.h"
#include "vcg/space/triangle3.h"
#include "fgtHole.h"

/** Object rapresent connection between two border edges of different faces. 
 */
template <class MESH>
class FgtBridge
{
public:
	typedef typename MESH::FaceType					FaceType;
	typedef typename MESH::FacePointer				FacePointer;
	typedef typename MESH::FaceIterator				FaceIterator;
	typedef typename vcg::face::Pos<FaceType>		PosType;

	typedef typename FgtHole<MESH>					HoleType;
	typedef typename std::vector< FgtHole<MESH> >	HoleVector;
	
	typedef typename MESH::VertexType				VertexType;
	typedef typename MESH::CoordType				CoordType;
	typedef typename MESH::ScalarType				ScalarType;

	/*typedef typename vcg::tri::Hole<MESH>			vcgHole;
	typedef typename vcgHole::Info					HoleInfo;	
	*/

	FgtBridge(const HoleType *hole_A, PosType pos_A, const HoleType *hole_B, PosType pos_B)
	{
		assert(pos_A.IsBorder());
		assert(pos_B.IsBorder());
		holeA = hole_A;
		posA = pos_A;
		holeB = hole_B;
		posB = pos_B;
		bridgeFace[0] = bridgeFace[1] = 0;
	};
 

	/**
	 *  Insert faces rapresenting the bridge into mesh
	 *  app is the vector of pointer at faces, it is needed because ading faces can resize 
	 *  and so reallocate the faces' vector
	 *
	 *	Connect 2 different face adding 2 face between its edge.
	 *					 
	 *		/|    |\				 /|¯¯¯/|\			 /|\¯¯¯|\
	 *	   / |    | \		--\		/ |	 / | \	  or	/ |	\  | \
	 *	   \ |    | /		--/		\ |	/  | /			\ |  \ | /
	 *		\|    |/				 \|/___|/			 \|___\|/
	*/
	bool create(MeshModel *mesh, std::vector<FacePointer *> &app)
	{	
		// prima faccia del bridge
		VertexType* vA0 = posA.f->V0( posA.z ); // first vertex of pos' 1-edge 
		VertexType* vA1 = posA.f->V1( posA.z ); // second vertex of pos' 1-edge
		VertexType* vB0 = posB.f->V0( posB.z ); // first vertex of pos' 2-edge 
		VertexType* vB1 = posB.f->V1( posB.z ); // second vertex of pos' 2-edge 

		if( vA0 == vB0 || vA0 == vB1 || vA1 == vB0 || vA1 == vB1 )		// connect adjacent border edge
			return false;

		// solution A 
		Triangle3<ScalarType> bfA0(vA1->P(), vA0->P(), vB0->P());
		Triangle3<ScalarType> bfA1(vB1->P(), vB0->P(), vA0->P());

		// solution B
		Triangle3<ScalarType> bfB0(vA1->P(), vA0->P(), vB1->P());
		Triangle3<ScalarType> bfB1(vB1->P(), vB0->P(), vA1->P());

		FaceIterator f0 = vcg::tri::Allocator<MESH>::AddFaces(mesh->cm, 2, app);
		FaceIterator f1 = f0 + 1;

		// the index of edge adjacent between new 2 face, is the same for both new faces
		int adjEdgeIndex = -1;		
		
		// the index of edge adjacent between new 2 face, is the same for both new faces
		int sideEdgeIndex = -1;

		if( bfA0.QualityFace()+ bfA1.QualityFace() > bfB0.QualityFace()+ bfB1.QualityFace() )
		{
			f0->V(0) = vA1;
			f0->V(1) = vA0;
			f0->V(2) = vB0;
						
			f1->V(0) = vB1;
			f1->V(1) = vB0;
			f1->V(2) = vA0;
			
			adjEdgeIndex = 1;
			sideEdgeIndex = 2;
		}
		else
		{
			f0->V(0) = vA1;
			f0->V(1) = vA0;
			f0->V(2) = vB1;
			
			f1->V(0) = vB1;
			f1->V(1) = vB0;
			f1->V(2) = vA1;
			
			adjEdgeIndex = 2;
			sideEdgeIndex = 1;
		}

		ComputeNormal(*f0);
		ComputeNormal(*f1);

		f0->FFp(0) = posA.f;
		f0->FFi(0) = posA.z;

		f1->FFp(0) = posB.f;
		f1->FFi(0) = posB.z;
		
		// adjacence with other new face, in this case edge 1 is the adjacent for both new 2 faces
		f0->FFp(adjEdgeIndex) = &*f1;	
		f0->FFi(adjEdgeIndex) = adjEdgeIndex;

		f1->FFp(adjEdgeIndex) = &*f0;
		f1->FFi(adjEdgeIndex) = adjEdgeIndex;

		// set adjacence of bridges on its side (left/right)
		PosType leftFaceAdj;
		leftFaceAdj.SetNull();
		PosType rightFaceAdj;
		rightFaceAdj.SetNull();

		if(holeA == holeB)
		{
			leftFaceAdj = findLeftSideFace(posA, posB);
			rightFaceAdj = findRightSideFace(posA, posB);
		}

		if(rightFaceAdj.IsNull())
		{
			f0->FFp(sideEdgeIndex) = &*f0; 
			f0->FFi(sideEdgeIndex) = 2;
		}
		else
		{
			f0->FFp(sideEdgeIndex) = rightFaceAdj.f; 
			f0->FFi(sideEdgeIndex) = rightFaceAdj.z;

			rightFaceAdj.f->FFp(rightFaceAdj.z) = &*f0;
			rightFaceAdj.f->FFi(rightFaceAdj.z) = sideEdgeIndex;
		}

		if(leftFaceAdj.IsNull())
		{
			f1->FFp(sideEdgeIndex) = &*f1;
			f1->FFi(sideEdgeIndex) = sideEdgeIndex;
		}
		else
		{
			f1->FFp(sideEdgeIndex) = leftFaceAdj.f;
			f1->FFi(sideEdgeIndex) = leftFaceAdj.z;

			leftFaceAdj.f->FFp(leftFaceAdj.z) = &*f1;
			leftFaceAdj.f->FFi(leftFaceAdj.z) = sideEdgeIndex;
		}	

		posA.f->FFp(posA.z) = &*f0;
		posA.f->FFi(posA.z) = 0;
	
		posB.f->FFp(posB.z) = &*f1;
		posB.f->FFi(posB.z) = 0;	

		bridgeFace[0] = &*f0;
		bridgeFace[1] = &*f1;

		mesh->clearDataMask(MeshModel::MM_FACETOPO);
		mesh->clearDataMask(MeshModel::MM_BORDERFLAG);
		mesh->updateDataMask(MeshModel::MM_FACETOPO);
		mesh->updateDataMask(MeshModel::MM_BORDERFLAG);

		return true;
	};

	void remove(MESH &mesh)
	{
		// ripristino bordi
		FacePointer adjF;
		for(int f=0; f<2; f++)
		{
			for(int l=0; l<3; l++)
			{
				adjF = bridgeFace[f]->FFp(l);
				if(adjF != bridgeFace[f])
				{
					adjF->FFp( bridgeFace[f]->FFi(l) ) = adjF;
					adjF->FFi( bridgeFace[f]->FFi(l) ) = bridgeFace[f]->FFi(l);
				}
			}
		}
		vcg::tri::Allocator<MESH>::DeleteFace(mesh, *bridgeFace[0]);
		vcg::tri::Allocator<MESH>::DeleteFace(mesh, *bridgeFace[1]);
	};

/****	Static functions	******/

	/** Return boolean indicatind if face is a face of an hole of the list.
	 *  Also return a pointer to appartenent hole and pos finded
	 */
	static bool FindBridgeSideFromFace(FacePointer bFace, int pickedX, int pickedY, const HoleVector &holes, const HoleType **retHole, PosType &retPos) 
	{ 
		if( vcg::face::BorderCount(*bFace) == 0 )
			return false;

		assert(false); // Da Rivedere
/*
		typename HoleVector::const_iterator hit = holes.begin();
		*retHole = 0;
		
		//scorro i buchi della mesh
		for( ; hit != holes.end() && *retHole == 0; ++hit)
		{
			// for each hole check if face is its border face
			// per ogni buco della mesh scorro il bordo cercando la faccia richiesta
			PosType curPos = hit->StartPos();

			// if a bridge is previously build from/to startPos edge, it don't know where start
			// walking over hole border
			//if(!hit->StartPos().IsBorder())
			if(!curPos.IsBorder())
				continue;

			do
			{			
				if(curPos.f == bFace) 
				{
					*retHole = &*hit;
					if( vcg::face::BorderCount(*bFace) == 1 )
						retPos = curPos;
					else
						retPos = getClosestPos(bFace, pickedX, pickedY);
					return true;
				}
				curPos.NextB();
			}while( curPos != hit->StartPos() );
		}
*/
		return false; // means no find hole
	};

private:

	static PosType getClosestPos(FaceType* face, int x, int y)
	{
		double mvmatrix[16], projmatrix[16];
		int viewport[4];
		double rx, ry, rz;
		vcg::Point2d vertex[3];
		vcg::Point2d point((double)x, (double)y);

		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		for(int i=0; i<3; i++)
		{
			gluProject(face->V(i)->P()[0],face->V(i)->P()[1],face->V(i)->P()[2],mvmatrix,projmatrix,viewport,&rx,&ry,&rz);
			vertex[i] = vcg::Point2d(rx,ry);
		}

		double dist = DBL_MAX;
		int nearest = 0; 
		for(int i=0; i<3; i++)
		{
			if(!vcg::face::IsBorder<FaceType>(*face, i))
				continue;
			
			vcg::Line2d el(vertex[i], vertex[(i+1)%3]-vertex[i]);
			double tmp = vcg::Distance(el, point);
			
			if(dist > tmp)
			{
				dist = tmp;
				nearest = i;
			}
		}
		return PosType(face, nearest, face->V(nearest) );
	};

	/* This function navigate the hole finding if bridge is adjacent on its left/right side to existing face.
	 * PosA and posB must be on the same side.
	 *
	 *			\ / B \	/			\ / B \ /
	 *			 +-----+---			 +-----+---
	 *			/|					/| f1 /|			f1 is adjacent in its "left" side with face C
	 *		   / |				   / |   / |			f0 is adjacent in its "right" side no faces
	 *		   C |  hole		   C |  /  | hole
	 *		   \ |				   \ | /   |
	 *			\|					\|/ f0 |
	 *			 +-----+---		 	 +-----+---
	 *			/ \ A /	\			/ \ A / \
	 */
	PosType findSideFace(PosType startPos, PosType endPos)
	{
		PosType result;
		int dist = 0;
		PosType cur = startPos;
		while( cur.v != endPos.v)
		{
			cur.NextB();
			dist++;
			if( cur.v == startPos.v )
				dist = 0;
		}

		if(dist == 1)
			result = cur;		
		else
			result.SetNull();

		return result;
	};

	PosType findLeftSideFace(PosType startPos, PosType endPos)
	{
		startPos.v = startPos.f->V0(startPos.z);
		endPos.v = endPos.f->V1(endPos.z);
		return findSideFace(startPos, endPos);
	};

	PosType findRightSideFace(PosType startPos, PosType endPos)
	{
		startPos.v = startPos.f->V1(startPos.z);
		endPos.v = endPos.f->V0(endPos.z);
		return findSideFace(startPos, endPos);
	};


private:
	const HoleType *holeA;		// info about left side of bridge	
	const HoleType *holeB;		// info about right side of bridge
		
public:
	FaceType* bridgeFace[2];	// 2 faces rapresenting bridge
	
	PosType posA;
	PosType posB;

};

#endif