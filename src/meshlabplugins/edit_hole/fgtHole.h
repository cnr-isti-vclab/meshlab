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
#include <qstring.h>
#include <float.h>
#include <GL/glew.h>
#include "vcg/simplex/face/pos.h"
#include "vcg/space/point3.h"
#include "vcg/complex/algorithms/clean.h"
#include "vcg/complex/algorithms/hole.h"
#include <vcg/complex/algorithms/closest.h>
#include <vcg/space/index/grid_static_ptr.h>
#include "vcg/space/color4.h"
#include "holeSetManager.h"

/** An hole type, extends vcg::tri::Hole<MESH>::Info adding more information
 *  as filling, selection, filing autocompenetration and non manifoldness. 
 *  This class also allow to fill (and restore) an hole using different criteria.
 *
 *  FgtHole uses meshe's additional data to mark interesting face, so surfing 
 *  the hole faces is driven by face-face ajacency and mark meaning.
 *  Additional data are accesible for an hole by parentManager (HoleSetManager)
 *  which links hole to a mesh.
 *  Characteristic faces for an hole are marked as:
 *    - HoleBorderFace: face which initially (before filling) have 1 or 2 border edge.
 *    - HolePatchFace: faces added to fill the hole
 *    - PatchCompFace: patch faces which are selfintersected with mesh.
 *    - BridgeFace: faces added to edit hole (unify 2 holes, subdivide an hole, partitioning a
 *      non-manifold hole in more manifold ones)
 *    - BridgeFace + HolePatchFace: this combo is used to individuate faces added to subdivide
 *      a non manifold hole and also close a non manifold hole, ie an hole made up from only 3 edge
 *
 *        --------+-------+----+-----|     --------+-------+----+------|
 *         hole   /\ hole /\  hole   |       hole  /\  f0  /\ f1| hole |  f0: BridgeFace + PatchFace
 *           A   /  \  A /  \   A    |         A  /  \    /  \  |   B  |  f1: BridgeFace
 *              /    \  /    \       |           /    \  /    \ |      |
 *       ______/      \/      \______|    ______/      \/      \|______|
 */
template <class MESH>
class FgtHole : public vcg::tri::Hole<MESH>::Info
{
public:

private:
  enum State
  {
    NONE      = 0x0000,
    SELECTED  = 0x0001,
    FILLED    = 0x0002,
    ACCEPTED  = 0x0004,
    COMPENET  = 0x0008,
    NONMANIF  = 0x0010,
    BRIDGED   = 0x0020
  };

public:
	enum FillerMode
	{
		Trivial, MinimumWeight, SelfIntersection
	};

	typedef typename MESH::FaceType												FaceType;
	typedef typename MESH::FacePointer										FacePointer;
	typedef typename std::vector<FacePointer>							FacePointerVector;
	typedef typename MESH::VertexType											VertexType;
	typedef typename MESH::ScalarType											ScalarType;
	typedef typename vcg::face::Pos<FaceType>							PosType;
	typedef typename std::vector< PosType >								PosVector;
	typedef typename vcg::tri::Hole<MESH>									vcgHole;
	typedef typename vcgHole::Info												HoleInfo;
	typedef typename std::vector< FgtHole<MESH> >					HoleVector;
	typedef typename HoleVector::iterator									HoleIterator;
	typedef typename vcg::tri::TrivialEar<MESH>						TrivialEar;
	typedef typename vcg::tri::MinimumWeightEar<MESH>			MinimumWeightEar;
	typedef typename vcg::tri::SelfIntersectionEar<MESH>	SelfIntersectionEar;

	FgtHole(HoleInfo &hi, QString holeName, HoleSetManager<MESH> *parent) :
		HoleInfo(hi.p, hi.size, hi.bb)
	{
		parentManager = parent;
		name = holeName;
		_state = ACCEPTED;
		perimeter = HoleInfo::Perimeter();
		findNonManifoldness();
	};

	FgtHole(PosType startPos, QString holeName, HoleSetManager<MESH> *parent)
	{
		assert(startPos.IsBorder());
		parentManager = parent;
		name = holeName;
		_state = ACCEPTED;
		this->p = startPos;
		updateInfo();
	};

	~FgtHole() {};

	inline int Size() const { return this->size; };
	inline ScalarType Perimeter() const	{ return this->perimeter; };
	inline bool IsFilled() const { return (_state & FILLED) != 0; };
	inline bool IsSelected() const { return (_state & SELECTED) != 0; };
	inline bool IsCompenetrating() const { return IsFilled() && (_state & COMPENET) != 0; };
	inline bool IsAccepted() const { return !IsFilled() || ( (_state & ACCEPTED) != 0); };
	inline bool IsNonManifold() const { return (_state & NONMANIF) != 0; };
	inline bool IsBridged() const { return (_state & BRIDGED) != 0;; };
	inline void SetBridged(bool val)
	{
	  if(val) _state |= BRIDGED;
	  else _state &= (~BRIDGED);
  };

	void SetSelect(bool val)
	{
		bool oldVal = IsSelected();
	  if(val) _state |= SELECTED;
	  else _state &= (~SELECTED);
		if(oldVal != val)
		{
			if(val) parentManager->nSelected++;
			else parentManager->nSelected--;
		}
  };

	void SetAccepted(bool val)
	{
		bool oldVal = IsAccepted();
	  if(val) _state |= ACCEPTED;
	  else _state &= (~ACCEPTED);
		if(oldVal != val)
		{
			if(val) parentManager->nAccepted++;
			else parentManager->nAccepted--;
		}
  };
	
	inline void SetStartPos(PosType initP)
	{
		assert(!IsFilled());
		this->p = initP;
		assert(this->p.IsBorder());
		updateInfo();
	};

	void Draw() const
	{
		typename PosVector::const_iterator it = borderPos.begin();
		glBegin(GL_LINE_LOOP);
		for( ; it != borderPos.end(); it++)
			glVertex( it->v->P() );
		glEnd();
	};

	void DrawCompenetratingFace(GLenum glmode) const
	{
		assert(IsCompenetrating());

		typename std::vector<FacePointer>::const_iterator it;

		glBegin(glmode);
		for( it=patches.begin(); it != patches.end(); it++)
			if( parentManager->IsCompFace(*it) )
			{
				glVertex( (*it)->V(0)->P() );
				glVertex( (*it)->V(1)->P() );
				glVertex( (*it)->V(2)->P() );
			}

		glEnd();
	}

	/*  Reset flags used by this plugin (holeBorder and patch) to unmark this hole and its patch.
	 */
	void ResetFlag()
	{
		std::vector<FacePointer> bridgesFaces;
		if( IsFilled() )
		{
			typename std::vector<FacePointer>::iterator it;

			while(patches.size()>0)
			{
			  FacePointer f = patches.back();
        patches.pop_back();
				parentManager->ClearPatchAttr(f);
				parentManager->ClearCompAttr(f);

				for(int i=0; i<3; i++)
				{
					FacePointer adjF = f->FFp(i);
					parentManager->ClearHoleBorderAttr(adjF);
				}
			}
		}
		else
		{
			// we can walk the border to find hole's faces added by bridges
			PosType curPos = this->p;
			do{
				parentManager->ClearHoleBorderAttr(curPos.f);
				curPos.NextB();
			}while( curPos != this->p );
		}
	};


	/* Restore hole, remove patch faces applied to mesh to fill this hole. */
	void RestoreHole()
	{
		assert( IsFilled() );
		_state &= (~FILLED);
		
		typename std::vector<FacePointer>::iterator it;
		for(it = patches.begin(); it!=patches.end(); it++)
		{
			// PathcHoleFlag+BridgeFaceFlag is special case
			if(parentManager->IsBridgeFace(*it)) continue;

			assert(parentManager->IsPatchFace(*it));
			for(int e=0; e<3; e++)
			{
				if(!IsBorder(**it, e))
				{
					FacePointer adjF = (*it)->FFp(e);
					if(!parentManager->IsPatchFace(adjF))
					{
						int adjEI = (*it)->FFi(e);
						adjF->FFp( adjEI ) = adjF;
						adjF->FFi( adjEI ) = adjEI;
						assert(IsBorder(*adjF, adjEI));
					}
				}
			}
			if(!(**it).IsD())
				vcg::tri::Allocator<MESH>::DeleteFace(*parentManager->mesh, **it);
		}
		patches.clear();
	};

	void Fill(FillerMode mode, MESH &mesh, std::vector<FacePointer * > &local_facePointer)
	{
		assert(!IsFilled());
		assert(this->p.IsBorder());
		switch(mode)
		{
		case FgtHole<MESH>::Trivial:
				vcgHole::template FillHoleEar< TrivialEar >(mesh, *this, local_facePointer);
			break;
		case FgtHole<MESH>::MinimumWeight:
				vcgHole::template FillHoleEar< MinimumWeightEar >(mesh, *this, local_facePointer);
			break;
		case FgtHole<MESH>::SelfIntersection:
				std::vector<FacePointer * > vfp = local_facePointer;
				SelfIntersectionEar::AdjacencyRing().clear();
				PosType ip = this->p;
				do
				{
					PosType inp = ip;
					do
					{
						inp.FlipE();
						inp.FlipF();
						SelfIntersectionEar::AdjacencyRing().push_back(inp.f);
					} while(!inp.IsBorder());
					ip.NextB();
				}while(ip != this->p);

				typename std::vector<FacePointer>::iterator fpi = SelfIntersectionEar::AdjacencyRing().begin();
        for( ; fpi != SelfIntersectionEar::AdjacencyRing().end(); ++fpi)
					vfp.push_back( &*fpi );

				vcgHole::template FillHoleEar< SelfIntersectionEar >(mesh, *this, vfp);
				SelfIntersectionEar::AdjacencyRing().clear();
			break;
		}

		// hole filling leaves V flag to border vertex... resetting!
		typename PosVector::const_iterator it = borderPos.begin();
		for( ;it!=borderPos.end(); it++)
				it->v->ClearV();

		parentManager->faceAttr->UpdateSize();

		_state |= FILLED;
		_state |= ACCEPTED;
		_state &= (~COMPENET);
	}

	/* Check if face is a border face of this hole */
	bool HaveBorderFace(FacePointer bFace) const
	{
		assert(parentManager->IsHoleBorderFace(bFace));
		typename PosVector::const_iterator it;
		for(it=borderPos.begin(); it!= borderPos.end(); it++)
			if( bFace == it->f )
				return true;
		return false;
	}

	/* Check if pFace is a patch face of this hole */
	bool HavePatchFace(FacePointer pFace) const
	{
		assert( parentManager->IsPatchFace(pFace) );
		if( !IsFilled() )
			return false;

		typename std::vector<FacePointer>::const_iterator it;
		for(it = patches.begin(); it!=patches.end(); it++)
			if(pFace == *it)
				return true;
		return false;
	};

	/*  walk over hole border, watching each adiacent faces to its vertex
	 *  looking for bridge faces.
	 */ 
	void UpdateBridgingStatus()
	{
		assert(!IsFilled());
		PosType curPos = this->p;
		do{
			do{
				if( parentManager->IsBridgeFace(curPos.f) )
				{
					SetBridged(true);
					return;
				}
				curPos.FlipE();
				curPos.FlipF();
			}while(!curPos.IsBorder());
			curPos.FlipV();
		}while(curPos != this->p);
		SetBridged(false);
	};

	// concats its face reference to a vector
	void AddFaceReference(std::vector<FacePointer*> &facesReferences)
	{	
	  facesReferences.push_back(&this->p.f);

		typename PosVector::iterator pit;
		for(pit=borderPos.begin(); pit != borderPos.end(); pit++)
			facesReferences.push_back( &pit->f );

	  typename std::vector<FacePointer>::iterator fit;
	  for(fit=patches.begin(); fit!=patches.end(); fit++)
      facesReferences.push_back(&*fit);	
	};

private:

	/*  Walking the hole computing vcgHole::Info data and other info */
	void updateInfo()
	{
		assert(!IsFilled());
		borderPos.clear();
		_state &= (~NONMANIF);
		this->bb.SetNull();
		this->size = 0;

		PosType curPos = this->p;
		do{
			assert(!curPos.f->IsD());
			borderPos.push_back(curPos);
			parentManager->SetHoleBorderAttr(curPos.f);
			this->bb.Add(curPos.v->cP());
			++this->size;
			if(curPos.v->IsV())
				_state |= NONMANIF;
			else
				curPos.v->SetV();
			curPos.NextB();
			assert(curPos.IsBorder());
		}while( curPos != this->p );

		curPos = this->p;
		do{
			curPos.v->ClearV();
			curPos.NextB();
		}while( curPos != this->p );

		perimeter = HoleInfo::Perimeter();
	};

	/*  Walking the hole storing vertexes and finding non manifold one */
	void findNonManifoldness()
	{
		assert(!IsFilled());
		_state &= (~NONMANIF);
		PosType curPos = this->p;
		do{
			borderPos.push_back(curPos);
			if(curPos.v->IsV())
				_state |= NONMANIF;
			else
				curPos.v->SetV();
			curPos.NextB();
		}while( curPos != this->p );

		curPos = this->p;
		do{
			curPos.v->ClearV();
			curPos.NextB();
		}while( curPos != this->p );
	};

	/* set patch flag and auto-compenetration	flag when needed */
	void updatePatchState(int patchFlag)
	{
		assert( IsFilled() );
		_state &= (~COMPENET);
		vcg::GridStaticPtr<FaceType, ScalarType > gM;
		gM.Set(parentManager->mesh->face.begin(),parentManager->mesh->face.end());

		std::vector<FaceType*> inBox;
		vcg::Box3< ScalarType> bbox;
	  getPatchFaces(patchFlag);
		typename FacePointerVector::iterator pi = patches.begin();
		for( ; pi!=patches.end(); ++pi)
		{
			FacePointer f = *pi;
			if(TestFaceMeshCompenetration(*parentManager->mesh, gM, f))
			{
				_state |= COMPENET;
				parentManager->SetCompAttr(f);
			}
			(*pi)->ClearUserBit(patchFlag);
			parentManager->SetPatchAttr(*pi);			
		}
	};

	
	/* First patch face is the adjacent one to initial Pos ("p" field of Hole::Info)
	 * Other patch face are found looking adjacent face on each vertex of known patch faces.
	 * NB: looking adjacent faces to vertexes it can find patches also for non manifold hole.
	 */
	void getPatchFaces(int patchFlag)
	{
		assert( IsFilled() );
		patches.clear();
		std::vector<FacePointer> stack;
		PosType pos = this->p;
		pos.FlipF();
		assert(pos.f->IsUserBit(patchFlag));
		pos.f->SetV();
		stack.push_back(pos.f);
		while(stack.size()>0)
		{
			FacePointer f = stack.back();
			stack.pop_back();
			patches.push_back(f);

			// visit faces adjacent to f's vertexes
			for(int v=0; v<3; v++)
			{
				pos = PosType(f, v);
				do{
					pos.FlipF();
					pos.FlipE();
					if(pos.f->IsUserBit(patchFlag) && !pos.f->IsV())
					{
						pos.f->SetV();
						stack.push_back(pos.f);
					}
				}while(pos.f != f);
			}
		}

		typename std::vector<FacePointer>::iterator it;
		for(it=patches.begin(); it!=patches.end(); ++it)
			(*it)->ClearV();
	};

	/********* Static functions **********/
public:
		
	static bool TestFaceMeshCompenetration(MESH &mesh, vcg::GridStaticPtr<FaceType, ScalarType > &gM,
			const FacePointer f)
	{
		std::vector<FaceType*> inBox;
		vcg::Box3< ScalarType> bbox;

		f->GetBBox(bbox);
		vcg::tri::GetInBoxFace(mesh, gM, bbox,inBox);

		typename std::vector<FaceType*>::iterator fib;
		for(fib=inBox.begin();fib!=inBox.end();++fib)
		{
			if (f != *fib)
			{
				if(vcg::tri::Clean<MESH>::TestFaceFaceIntersection( *fib, f ))
					return true;
			}
		}
		return false;
	};


public:
	static int &HoleId(){static int _holeId=0; return _holeId;};
	static void ResetHoleId() { HoleId()=0; };
	static int GetHoleId() { return ++HoleId(); };
	QString name;

public:
	HoleSetManager<MESH>* parentManager;
	std::vector<FacePointer> patches;

private:
	int _state;
	ScalarType perimeter;

	std::vector<PosType> borderPos;
	
};

#endif
