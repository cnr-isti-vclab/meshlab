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
//#include <vector>
//#include <float.h>
//#include <GL/glew.h>
//#include "vcg/simplex/face/pos.h"
//#include "vcg/space/point3.h"
//#include "vcg/complex/trimesh/hole.h"
//#include <vcg/complex/trimesh/closest.h>
//#include <vcg/space/index/grid_static_ptr.h>
//#include "vcg/space/color4.h"
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

	typedef FgtHole<MESH>					HoleType;
	typedef typename std::vector< FgtHole<MESH> >	HoleVector;
	/*
    typedef typename MESH::CoordType				CoordType;
	typedef typename MESH::ScalarType				ScalarType;
	typedef typename vcg::tri::Hole<MESH>			vcgHole;
	typedef typename vcgHole::Info					HoleInfo;
	
	*/

	FgtBridge(const HoleType *hole_A, PosType pos_A)
	{
		assert(pos_A.IsBorder());
		holeA = hole_A;
		posA = pos_A;
		holeB = 0;
		face1 = face2 = 0;
	};

	void setDestination(const HoleType *hole_B, PosType pos_B)
	{
		assert(pos_B.IsBorder());
		holeB = hole_B;
		posB = pos_B;
	};


	/**
	 *  Insertfaces rapresenting the bridge into mesh
	 *  app is the vector of pointer at faces, it is needed because ading faces can resize 
	 *  and so reallocate the faces' vector
	 */
	void create(MESH &mesh, std::vector<FacePointer *> &app)
	{
		// prima faccia del bridge
		//FaceIterator f = tri::Allocator<MESH>::AddFaces(mesh, 2, app);
		FaceIterator f = vcg::tri::Allocator<MESH>::AddFaces(mesh, 1, app);
		(*f).V(0) = posA.f->V1( posA.z );
		(*f).V(1) = posA.f->V0( posA.z );
		(*f).V(2) = posB.f->V0( posB.z );
		ComputeNormal(*f);
		
		(*f).FFp(0) = posA.f;
		(*f).FFi(0) = posA.z;
		(*f).FFp(1) = &*f;
		(*f).FFi(1) = 1;
		(*f).FFp(2) = &*f;
		(*f).FFi(2) = 2;

		posA.f->FFp(posA.z) = &*f;
		posA.f->FFi(posA.z) = 0;

		//e1.f->FFp(e1.z)=f;
		//e1.f->FFi(e1.z)=1;	

		/*
		// seconda faccia del bridge
		f++;
		f->V(0) = posB.f->V( posB.z );
		f->V(1) = posB.f->V( (posB.z+1)%3 );
		f->V(2) = posA.f->V( posA.z );
        ComputeNormal(*f);
		*/
	};

	void remove(MESH &mesh)
	{
		vcg::tri::Allocator<MESH>::DeleteFace(mesh, *face1);
		vcg::tri::Allocator<MESH>::DeleteFace(mesh, *face2);
	};

/****	Static functions	******/

	/** Return boolean indicatind if face is a face of an hole of the list.
	 *  Also return a pointer to appartenent hole and pos find
	 */
	static bool FindHoleFaceFrom(FacePointer bFace, const HoleVector &holes, const HoleType **retHole, PosType &retPos) 
	{ 
		// BUG: se la faccia selezionata ha due edge di bordo su hole diversi, viene selezionato il primo hole
		// che si trova nella lista. Sarebbe carino che venisse selezionato l'hole relativo al bordo più vicino al click 
		if( vcg::face::BorderCount(*bFace) == 0 )
			return false;

		typename HoleVector::const_iterator hit = holes.begin();
		*retHole = 0;
		
		//scorro i buchi della mesh
		for( ; hit != holes.end() && *retHole == 0; ++hit)
		{
			// for each hole check if face is its border face
			// per ogni buco della mesh scorro il bordo cercando la faccia richiesta
			PosType curPos;
			PosType startPos = curPos = hit->holeInfo.p;
			do
			{			
				if(curPos.F() == bFace) 
				{
					*retHole = &*hit;
					retPos = curPos;
					return true;
				}
				curPos.NextB();
			}while( curPos != startPos );
		}
		return false; // means no find hole
	}


private:
	const HoleType *holeA;		// info about left side of bridge
	
	const HoleType *holeB;		// info about right side of bridge
	
	FaceType *face1;	// 2 faces rapresenting bridge
	FaceType *face2;

public:
	PosType posA;
	PosType posB;

};

#endif