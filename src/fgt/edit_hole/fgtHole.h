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

#ifndef HOLE_H
#define HOLE_H

#include <utility>
#include <vector>
#include <float.h>
#include "vcg/simplex/face/pos.h"
#include "vcg/space/point3.h"
#include "vcg/complex/trimesh/hole.h"
//#include "holePatch.h"



/** An hole is a border which visit only 1 time each of its vertexes.
 *  To visit entire hole we need to iterate the skipping of position on next border from an init position (firstPos)
 *  until we return over this one.
 *  This data structure store, to avoid to enter in other hole, store a list of "patches". 
 *  A patch is a couple of position current position and next position. This patches in hole allow to move over right
 *  border when walking the border we find an non manifold vertex.
 */
template <class MESH>
class FgtHole
{
public:
	typedef typename MESH::FaceType FaceType;
	typedef typename MESH::FacePointer FacePointer;
    typedef typename MESH::CoordType CoordType;
	typedef typename vcg::face::Pos<FaceType> PosType;
	typedef typename vcg::tri::Hole<MESH> vcgHole;
	typedef typename vcgHole::Info HoleInfo;
	typedef typename std::vector< FgtHole<MESH> > HoleVector;

	FgtHole(HoleInfo hi)
	{
		holeInfo = hi;
		UpdateVertexCoords();
		isSelected = false;
		isAccepted = false;
	};

	~FgtHole() {};

	/* From hole Info update vector of vertex coord
	 */
	void UpdateVertexCoords()
	{
		vertexCoords.clear();
		// scorro i vertici del buco e ne salvo il puntatore in un vettore
		PosType curPos;
		PosType startPos = curPos = holeInfo.p;
		do
		{
			vertexCoords.push_back( &(curPos.v->P()) );
			curPos.NextB();
		}while(curPos != startPos);
	   
	};


	void Draw() const
	{
		glBegin(GL_LINE_LOOP);

		std::vector<CoordType*>::const_iterator it = vertexCoords.begin();
		for( ; it != vertexCoords.end() ; it++) 
			glVertex( **it );
		
		glEnd();
	};
	

	
	/* Check if face is a border face of this hole
	*/
	bool HaveBorderFace(FacePointer bFace) const
	{
		// per ogni buco della mesh scorro il bordo cercando la faccia richiesta
		PosType curPos;
		PosType startPos = curPos = holeInfo.p;
		do
		{			
			if(curPos.F() == bFace) 
				return true;
			curPos.NextB();
		}while( curPos != startPos );	
		return false;
	}



	/********* Static functions **********/

	/* Inspect a mesh to find its holes.
	*/
	static void GetMeshHoles(MESH &mesh, std::vector< FgtHole<MESH> >&ret) 
	{
		ret.clear();
		std::vector<HoleInfo> vhi;
		
		//prendo la lista di info(sugli hole) tutte le facce anche le non selezionate
		int UBIT = vcgHole::GetInfo(mesh, false, vhi);
		
		std::vector<HoleInfo>::iterator itH = vhi.begin();
		for( ; itH != vhi.end(); itH++)
		{
			FgtHole<MESH> newHole(*itH);
			ret.push_back(newHole);		
		}
	}

	/** Return index into holes vector of hole adjacent to picked face
	 */
	static int FindHoleFromBorderFace(FacePointer bFace, const HoleVector holes)
	{ 
		// BUG: se la faccia selezionata ha due edge di bordo su hole diversi, viene selezionato il primo hole
		// che si trova nella lista. Sarebbe carino che venisse selezionato l'hole relativo al bordo più vicino al click 
		if( vcg::face::BorderCount(*bFace) == 0 )
			return -1;

		int index = 0;
		HoleVector::const_iterator hit = holes.begin();
		
		//scorro i buchi della mesh
		for( ; hit != holes.end(); ++hit)
		{
			// for each hole check if face is its border face
			if(hit->HaveBorderFace(bFace))
				return index;
			index++;
		}
		return -1; // means no find hole
	}

public:
	HoleInfo holeInfo;
	std::vector<CoordType*> vertexCoords;
	bool isSelected;
	bool isAccepted;
};


#endif
