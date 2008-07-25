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
#include <GL/glew.h>
#include "vcg/simplex/face/pos.h"
#include "vcg/space/point3.h"
#include "vcg/complex/trimesh/hole.h"



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
	typedef typename MESH::FaceType					FaceType;
	typedef typename MESH::FacePointer				FacePointer;
	typedef typename MESH::FaceIterator				FaceIterator;
    typedef typename MESH::CoordType				CoordType;
	typedef typename vcg::face::Pos<FaceType>		PosType;
	typedef typename vcg::tri::Hole<MESH>			vcgHole;
	typedef typename vcgHole::Info					HoleInfo;
	typedef typename std::vector< FgtHole<MESH> >	HoleVector;
	

	FgtHole(HoleInfo &hi, QString holeName)
	{
		holeInfo = hi;
		UpdateVertexCoords();
		size = holeInfo.Perimeter();
		name = holeName;
		isSelected = false;
		isCompenetrating = false;
		isAccepted = true;
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

		typename std::vector<CoordType*>::const_iterator it = vertexCoords.begin();
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

	/* Restore hole, remove patch applied to mesh
	 * Le facce usate come patch sono salvate come copie, pertanto si devono cercare 
	 * le corrispettive facce nella mesh. Per fare questo si parte dal pos iniziale
	 * contenuta nelle info del "hole", che deve è posizionato su una faccia\edge\vertice
	 * sul bordo del buco; si testano le facce adiacenti al vertice cercando le facce patch.
	 * Quando si trova una faccia patch questa viene eliminata dalla mesh e poi si continua la ricerca
	 * di altre facce patch cercando tra le facce adiacenti ai suoi vertici non ancora viste; in questo 
	 * modo si scorrono solo le facce interne ed appena esterne al buco riuscendo anche a ripristinare 
	 * buchi non-manifold.
	 */
	void RestoreHole(MESH &mesh)
	{
		if(facesPatch.size() == 0 )
			return;

		std::vector<FaceType* > meshFaces;
		std::vector<FaceType* >::iterator mfit;
		PosType curPos;
		PosType startPos = curPos = holeInfo.p;

		// scorro le facce adiacenti alla faccia iniziale del buco
		do
		{
			if(!curPos.f->IsV())
			{
				meshFaces.push_back( curPos.f );
				curPos.f->SetV();
			}
			curPos.FlipF();
			curPos.FlipE();
		}while(curPos != startPos);

		// tra le facce adiacenti al buco cerco quelle usate come patch
		for(int index =0; index < meshFaces.size(); index++ )
		{
			FaceType* mf = meshFaces.at(index);
			FaceIterator hfit = facesPatch.begin();
			for( ; hfit != facesPatch.end(); ++hfit) 
			{
				if( hfit->V(0) == mf->V(0) &&
					hfit->V(1) == mf->V(1) &&
					hfit->V(2) == mf->V(2) )
				{
					// trovata faccia patch
					// inserisco tra le facce da esaminare le facce adiacenti ai vertici della faccia patch
					for(int i=0; i<3; ++i)
					{
						PosType initPos(mf, i, mf->V(i) );
						curPos = initPos;
						do
						{
							if(!curPos.f->IsV() && !curPos.f->IsD())
							{
								meshFaces.push_back( curPos.f );
								curPos.f->SetV();
							}
							curPos.FlipF();
							curPos.FlipE();
						}while(curPos != initPos); 
					}

					// si elimina la faccia patch
					tri::Allocator<MESH>::DeleteFace(mesh, *mf );
					facesPatch.erase(hfit);
					break;
				}				
			}
		}
		
		std::vector<FaceType* >::iterator it;
		for(it = meshFaces.begin() ; it != meshFaces.end(); ++it)
			(*it)->ClearV();		
	}


	/********* Static functions **********/

	/* Inspect a mesh to find its holes.
	*/
	static int GetMeshHoles(MESH &mesh, HoleVector &ret) 
	{
		ret.clear();
		std::vector<HoleInfo> vhi;
		
		//prendo la lista di info(sugli hole) tutte le facce anche le non selezionate
		int UBIT = vcgHole::GetInfo(mesh, false, vhi);
		
		typename std::vector<HoleInfo>::iterator itH = vhi.begin();
		int i=0;
		for( ; itH != vhi.end(); itH++)
		{
			FgtHole<MESH> newHole(*itH, QString("Hole_%1").arg(i));
			ret.push_back(newHole);
			i++;
		}
		return UBIT;
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
		typename HoleVector::const_iterator hit = holes.begin();
		
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
	std::vector<FaceType> facesPatch;
	float size;
	QString name;
	bool isSelected;
	bool isCompenetrating;
	bool isAccepted;
};


#endif
