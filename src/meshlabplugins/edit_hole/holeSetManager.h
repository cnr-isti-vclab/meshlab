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

#ifndef HOLESETMANAGER_H
#define HOLESETMANAGER_H

#include "fgtHole.h"
#include "fgtBridge.h"
#include "vcg/complex/algorithms/hole.h"
#include <vcg/container/simple_temporary_data.h>


/*  HoleSetManager class rappresent an entity which manages the holes and bridges
 *  founded into the same MESH.
 *  It allows to invoke some functionality for each (may be the selected ones)
 *  holes as filling and bridging.
 *  His presence is necessary because it connect holes to a mesh with the
 *  additional data used by holes to mark its "characteristic" faces.
 */
template <class MESH>
class HoleSetManager
{
	enum FaceAttribute
  {
    NONE      = 0x0000,
    BORDER		= 0x0001,
    PATCH			= 0x0002,
    COMPENET  = 0x0004,
    BRIDGE		= 0x0020
  };

	typedef FgtHole< MESH >                                   HoleType;
	typedef typename std::vector< HoleType >    							HoleVector;
	typedef typename HoleVector::iterator											HoleIterator;
	typedef typename std::vector< FgtBridgeBase<MESH>* >			BridgeVector;
	typedef typename BridgeVector::iterator										BridgeIterator;
	typedef typename MESH::FaceType														FaceType;
	typedef typename MESH::FacePointer												FacePointer;
	typedef typename MESH::FaceContainer											FaceContainer;
	typedef typename vcg::tri::Hole<MESH>											vcgHole;
	typedef typename vcgHole::Info														HoleInfo;
	typedef typename vcg::face::Pos<FaceType>									PosType;
	typedef typename std::vector< PosType >										PosVector;
	typedef typename vcg::SimpleTempData<FaceContainer, int>	SimpleData;
	typedef typename HoleType::FillerMode											FillerMode;

public:

	HoleSetManager()
	{
		autoBridgeCB = 0;
	};

	void Init(MESH* m)
	{
		nAccepted = nSelected = 0;
		mesh = m;
		faceAttr = new SimpleData (m->face, NONE);
		getMeshHoles();
	};

	void Clear()
	{
		typename HoleVector::iterator it;
		for(it=holes.begin(); it!=holes.end(); it++)
			it->ResetFlag();

		holes.clear();
	};

 /***************** PerFace additional data ********************/

	inline bool IsHoleBorderFace(FacePointer f) const { return ( (*faceAttr)[f] & BORDER) != 0; };
	inline bool IsPatchFace(FacePointer f)			const { return ((*faceAttr)[f] & PATCH) != 0; };
	inline bool IsCompFace(FacePointer f)				const { return ((*faceAttr)[f] & COMPENET) != 0; };
	inline bool IsBridgeFace(FacePointer f)			const { return ((*faceAttr)[f] & BRIDGE) != 0; };

	inline void SetHoleBorderAttr(FacePointer f) {	(*faceAttr)[f] |= BORDER; };
	inline void ClearHoleBorderAttr(FacePointer f) { (*faceAttr)[f] &= (~BORDER); };

	inline void SetPatchAttr(FacePointer f) { (*faceAttr)[f] |= PATCH; };
	inline void ClearPatchAttr(FacePointer f) { (*faceAttr)[f] &= (~PATCH);	}

	inline void SetCompAttr(FacePointer f) { (*faceAttr)[f] |= COMPENET; };
	inline void ClearCompAttr(FacePointer f) { (*faceAttr)[f] &= (~COMPENET); };

	inline void SetBridgeAttr(FacePointer f){ (*faceAttr)[f] |= BRIDGE; };
	inline void ClearBridgeAttr(FacePointer f){ (*faceAttr)[f] &= (~BRIDGE);	};

	/**************** Statistical info *****************/
  inline int SelectionCount() const { return nSelected; };
  inline int HolesCount()			const { return holes.size(); };
  inline int AcceptedCount()	const { return nAccepted; };

	/**************** Holes editing ********************/

	bool Fill(FillerMode mode)
	{
		if(nSelected==0)
			return false;

		std::vector<FacePointer *> local_facePointer;
		AddFaceReference(local_facePointer);

		HoleIterator hit = holes.begin();
		for( ; hit != holes.end(); hit++ )
			if( hit->IsSelected() )
			{
				hit->Fill(mode, *mesh, local_facePointer);

				// there are new face reference to update in next filling
				typename std::vector<FacePointer>::iterator fit;
				for(fit=hit->patches.begin(); fit!=hit->patches.end(); fit++)
					local_facePointer.push_back(&*fit);	
			}

		nAccepted=nSelected;
		return true;
	};

	/*	For accepted holes: patch faces and its adjacent bridge faces become
	 *  faces of this mesh.
	 *		- reset additional data for its faces
	 *		- remove hole from list
	 *		- accept bridge adjacent to hole
	 *
	 *	For not accepted holes: patch faces are removed from mesh
	 *		- remove hole patch and restore border
	 */
	void ConfirmFilling(bool accept)
	{
		typename std::vector<FacePointer > bridgeF;
		typename std::vector<FacePointer >::iterator fpit;

		HoleIterator it = holes.begin();
		while( it != holes.end() )
		{
			if( it->IsFilled() )
			{

				if( ( it->IsSelected() && !it->IsAccepted() ) || !accept)
				{
					it->RestoreHole();
				}
				else if( it->IsSelected() && it->IsAccepted() )
				{
					if(it->IsBridged())
					{
						for(fpit = it->patches.begin(); fpit != it->patches.end(); fpit++)
						{
							if( IsBridgeFace( *fpit ) )
								bridgeF.push_back( *fpit );

							for(int i=0; i<3; i++)
							{
								if( IsBridgeFace( (*fpit)->FFp(i) ) )
									bridgeF.push_back( (*fpit)->FFp(i) );
							}
						}
					}
					it->ResetFlag();
					it = holes.erase(it++);
					continue;
				}
			}
			else it++;
		}

		for(fpit = bridgeF.begin(); fpit != bridgeF.end(); fpit++)
		{
			BridgeIterator bit = bridges.begin();
			while( bit != bridges.end() )
			{
				PosType a = (*bit)->GetAbutmentA();
				PosType b = (*bit)->GetAbutmentB();

				if( *fpit == a.f->FFp(a.z) || *fpit == b.f->FFp(b.z) )
				{
					(*bit)->ResetFlag();
					delete *bit;
					bit = bridges.erase(bit++);
				}
				else
					 bit++;
			}
		}

		// update bridging status for holes remaining.
		// some hole marked as "bridged" can be adjacent to bridge which is accepted
		// because it is adjacent to hole filled and accepted, so they arent "bridged" now.
		for( it= holes.begin(); it != holes.end(); it++)
		{
			assert(!it->IsFilled());
			if( it->IsBridged() )
				it->UpdateBridgingStatus();
		}

		countSelected();
	};

	/*  Bridges became face of this mesh
	 */
	inline void ConfirmBridges()
	{
		BridgeIterator bit = bridges.begin();
		for( ; bit != bridges.end(); bit++ )
		{
			(*bit)->ResetFlag();
			delete *bit;
		}
		bridges.clear();

		typename HoleVector::iterator hit = holes.begin();
		for( ; hit!=holes.end(); hit++ )
			hit->SetBridged(false);
	};

	/*  Bridges are removed from bridges vector and mesh is restored,
	 *  bridge faces are removed from mesh.
	 */
	inline void DiscardBridges()
	{
		removeBridges();
		countSelected();
	};

	void CloseNonManifoldHoles()
	{
		FgtNMBridge<CMeshO>::CloseNonManifoldVertex(this);
	};

	void AutoSelfBridging(double distCoeff, std::vector<FacePointer*> *facesRef)
	{
		FgtBridge<CMeshO>::AutoSelfBridging(this, distCoeff, facesRef);
		countSelected();
	};

	void AutoMultiBridging(std::vector<FacePointer*> *facesRef)
	{
		FgtBridge<CMeshO>::AutoMultiBridging(this, facesRef);
		countSelected();
	};

	/** Return index of hole adjacent to picked face into holes vector.
	 *  Also return the iterator on correct position in holes list.
	 */
	int FindHoleFromFace(FacePointer pFace, HoleIterator &it)
	{
		int index = 0;
		HoleIterator hit = holes.begin();

		// it know if bFace is adjacent to patchFace
		if(IsPatchFace(pFace))
		{
			for( ; hit != holes.end(); ++hit)
			{
				if(hit->HavePatchFace(pFace))
				{
					it = hit;
					return index;
				}
				index++;
			}
		}
		else if(IsHoleBorderFace(pFace))
		{
			for( ; hit != holes.end(); ++hit)
			{
				if(hit->HaveBorderFace(pFace))
				{
					it = hit;
					return index;
				}
				index++;
			}
		}

		it = holes.end();	// invalid iterator
		return -1;
	};

	/** Return boolean indicatind if the picking have select a border face which can be used
	 *  as abutment for a bridge. If true it also return BridgeAbutment allowing to know
	 *  border edge and its relative hole.
	 */
	bool FindBridgeAbutmentFromPick( FacePointer bFace, int pickedX, int pickedY,
																					BridgeAbutment<MESH> &pickedResult)
	{
		if( vcg::face::BorderCount(*bFace) == 0 )
			return false;

		HoleIterator hit;
		if( FindHoleFromFace(bFace, hit) < 0 )
		{
			pickedResult.SetNull();
			return false;
		}

		pickedResult.h = &*hit;
		pickedResult.f = bFace;
		if( vcg::face::BorderCount(*bFace) == 1 )
		{
			// it choose the only one border edge
			for(int i=0; i<3; i++)
				if(vcg::face::IsBorder<FaceType>(*bFace, i))
					pickedResult.z = i;
		}
		else
		{
			// looking for the closest border edge to pick point
			PosType retPos = getClosestPos(bFace, pickedX, pickedY);
			pickedResult.f = retPos.f;
			pickedResult.z = retPos.z;
		}

		return true;
	};


	/*  Appends all its face reference to a vector, all face reference of its hole type
	 *  and its bridge.
	 */
	void AddFaceReference(std::vector<FacePointer*> &facesReferences)
	{
		typename HoleVector::iterator hit = holes.begin();
		for( ; hit!=holes.end(); hit++)
			hit->AddFaceReference(facesReferences);

		BridgeIterator bit = bridges.begin();
		for( ; bit != bridges.end(); bit++ )
			(*bit)->AddFaceReference(facesReferences);
  }


private:

	/* Inspect a mesh to find its holes. */
	void getMeshHoles()
	{
		holes.clear();
		std::vector<HoleInfo> vhi;

		//prendo la lista di info(sugli hole) tutte le facce anche le non selezionate
		vcgHole::GetInfo(*mesh, false, vhi);
		HoleType::ResetHoleId();
		typename std::vector<HoleInfo>::iterator itH = vhi.begin();
		for( ; itH != vhi.end(); itH++)
		{
			holes.push_back(HoleType(*itH, QString("Hole_%1").arg(HoleType::GetHoleId(),3,10,QChar('0')), this));

			// reset flag and setting of
			PosType curPos = itH->p;
			do{
				SetHoleBorderAttr(curPos.f);
				curPos.NextB();
				assert(curPos.IsBorder());
			}while( curPos != itH->p );
		}
	}

	/*  Return border half-edge of the same face which is nearest to point(x, y) of glArea.
	 */
	static PosType getClosestPos(FaceType* face, int x, int y)
	{
		double mvmatrix[16], projmatrix[16];
		GLint viewport[4];
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

	/*  Remove all bridges updating holes vector and its selection
	 */
	void removeBridges()
	{
		assert( holes.size()>0 );
		// contains all half-edge located over non-bridge face and over edge shared with bridge face.
		// these half-edges will become border edge when bridge faces are removed.
		PosVector adjBorderPos;

		PosType pos;
		BridgeIterator bit = bridges.begin();
		for( ; bit != bridges.end(); bit++ )
		{
			adjBorderPos.push_back( (*bit)->GetAbutmentA() );
			adjBorderPos.push_back( (*bit)->GetAbutmentB() );
		}

		// remove holes adjacent to bridge
		HoleIterator hit = holes.begin();
		while(hit != holes.end() )
		{
			if( hit->IsBridged() )
			{
				if( hit->IsSelected() )
				{
					PosType curPos = hit->p;
					do
					{
						curPos.f->SetS();
						curPos.NextB();
					}while( curPos != hit->p );
				}
				hit = holes.erase(hit++);
			}
			else
				hit++;
		}

		for( bit = bridges.begin(); bit != bridges.end(); bit++ )
		{
			(*bit)->DeleteFromMesh();
			delete *bit;
		}
		bridges.clear();

		// update hole list inserting holes touched by bridge
		// use adjBorderPos element as start pos to walk over the border, if walking doesn't
		// visit some adjBorderPos element means this belongo to other hole.
		PosType initPos, curPos;
		typename PosVector::iterator it;
		for( it=adjBorderPos.begin(); it!=adjBorderPos.end(); it++)
		{
			// bridge abutment can be placed over face of another bridge...
			// this abutments must be ignored
			if(it->f->IsD())
				continue;

			assert( it->IsBorder() );
			bool sel=it->f->IsS();
			it->f->ClearS();

			if(it->f->IsV() || it->f->IsD())
				continue;

			curPos = initPos = *it;
			do{
				curPos.f->SetV();
				sel = sel || curPos.f->IsS();
				curPos.f->ClearS();
				curPos.NextB();
				assert(curPos.IsBorder());
			}while(curPos != initPos);

			FgtHole<MESH> newHole(initPos, QString("Hole_%1").arg(HoleType::GetHoleId(),3,10,QChar('0')), this);
			newHole.SetSelect(sel);
			holes.push_back( newHole );
		}

		// resetto flag visited sulle facce degli hole interessati
		for(it=adjBorderPos.begin(); it!=adjBorderPos.end(); it++)
		{
			if(!it->f->IsV())
				continue;
			curPos = initPos = *it;
			do{
				curPos.f->ClearV();
				curPos.NextB();
				assert(curPos.IsBorder());
			}while(curPos != initPos);
		}
	};

	// update statistical info
	void countSelected()
	{
		nSelected = 0;
		HoleIterator hit = holes.begin();
		for( ; hit!=holes.end(); hit++)
			if(hit->IsSelected())
				nSelected++;
	};


public:
	int nSelected;
	int nAccepted;
	MESH* mesh;
	HoleVector holes;
	BridgeVector bridges;
	SimpleData* faceAttr;

	AutoBridgingCallback* autoBridgeCB;
};

#endif
