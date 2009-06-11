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

#include <qstring.h>
#include <utility>
#include <vcg/simplex/face/topology.h>
#include "vcg/space/line2.h"
#include "vcg/space/triangle3.h"
#include "fgtHole.h"
#include "holeSetManager.h"
#include <ctime>

template <class MESH> class FgtHole;
template <class MESH> class HoleSetManager;

/* Struct rappresenting the mesh edge where the bridge starts/ends. */
template <class MESH>
struct BridgeAbutment
{
public:
	BridgeAbutment() { f=0; z=0; h=0; };

	BridgeAbutment(typename MESH::FacePointer pface, int edgeIndex, FgtHole<MESH>* phole)
	{
		f = pface;
		z = edgeIndex;
		h = phole;
	};

	inline void SetNull() { f=0; };
	inline bool IsNull() const { return f==0; };

	typename MESH::FacePointer f;
	int z;
	FgtHole<MESH>* h;
};

/*  Class rappresenting callback for feedback while auto bridging is running.
 *  It's necessary because auto bridging can spent a lot of time computing
 *  and user must know it is working.
 */
class AutoBridgingCallback
{
public:
	inline int GetOffset() const { return offset; };
	virtual void Invoke(int) = 0;

protected:
	int offset; // minor time before 2 calling (in ms)
};

template <class MESH>
class FgtBridgeBase
{
public:
	typedef typename MESH::FaceType							FaceType;
	typedef typename MESH::FacePointer					FacePointer;
	typedef typename vcg::face::Pos<FaceType>		PosType;

public:
	virtual PosType GetAbutmentA() const =0;
	virtual PosType GetAbutmentB() const =0;

	virtual void ResetFlag() = 0;
	virtual void DeleteFromMesh() = 0;
	virtual inline void AddFaceReference(std::vector<FacePointer*> & /* facesReferences */) {};

	virtual inline bool IsNull() const = 0;
	virtual inline bool IsDeleted() const = 0;

protected:
	HoleSetManager<MESH>* parentManager;
};


/** Bridge between different FgtHoles or into the same one.
 *  Bridges rapresent connection between two border edges of different faces.
 *  Connection consists in 2 face adjcent each other over an edge and adjcent
 *  with a mesh face over another edge, so both faces have a border edge.
 *
 *  Bridge can connect 2 edge belong the same hole, result are 2 new holes.
 *  In this case bridge could be adjacent to mesh if edge are next, so this
 *  bridge could not been build.
 *
 *  Bridge can also connect 2 or more different holes in only one hole.
 *
 *                   \ / B \ /                  \ / B \ /
 *             +------+-----+------+      +------+-----+------+
 *             |                   |			|      |\ f1 |      |
 *             |                   |			|      | \   |      |
 *             |       hole        |      | hole |  \  | hole |
 *             |                   |      |      |   \ |      |
 *             |                   |      |      | f0 \|      |
 *             +------+-----+------+      +------+-----+------+
 *                   / \ A / \                  / \ A / \
 *                                             GOOD BRIDGE
 *                             f0 and f1 are adjacent only with one mesh face
 *
 *   \ / B \ /       \ / B \ /           |
 *    +-----+---      +-----+---         |    ---+------+------      ---+------+--------
 *   /|              /| f1 /|            |	      \ A  /                 \ A  /|\
 *  / |             / |   / |            |         \  /                   \  / | \
 *  C |  hole       C |  /  | hole       |     hole \/    hole     hole    \/  |  \  hole
 *  \ |             \ | /   |            |          /\                     /\f0|   \
 *   \|              \|/ f0 |            |         /  \                   /  \ | f1 \
 *    +-----+---      +-----+---         |        / B  \                 / B  \|     \
 *   / \ A / \       / \ A / \           |    ---+------+------      ---+------+------+---
 *                  NO GOOD BRIDGE	     |     Hole Non-Manifold         NO GOOD BRIDGE
 *               f1 adjacent to B and C                               f0 adjacent to A and B
 */

#define SIDE_EDGE_OPTA	2
#define ADJ_EDGE_OPTA		1

#define SIDE_EDGE_OPTB	1
#define ADJ_EDGE_OPTB		2

template <class MESH>
class FgtBridge: public FgtBridgeBase<MESH>
{
  typedef BridgeAbutment<MESH>        				                AbutmentType;
  typedef typename MESH::FaceType							                FaceType;
	typedef typename MESH::ScalarType						                ScalarType;
	typedef typename vcg::face::Pos<FaceType>               		PosType;
	typedef FgtHole<MESH>											                	HoleType;
	typedef typename std::vector<HoleType>			                HoleVector;
	typedef typename MESH::FacePointer					                FacePointer;
  typedef typename MESH::FaceIterator					                FaceIterator;
  typedef typename vcg::GridStaticPtr<FaceType, ScalarType >	GridType;
	typedef typename std::vector< FgtBridge<MESH> >							BridgeVector;
	typedef typename BridgeVector::iterator											BridgeIterator;
  typedef typename MESH::VertexType						                VertexType;

public:
	enum BridgeOption {NoOne, OptA, OptB};

	FgtBridge(HoleSetManager<MESH>* parent)
	{
		f0=0;
		f0=0;
		this->parentManager = parent;
	};

	inline void AddFaceReference(std::vector<FacePointer*> &facesReferences)
	{
		assert(!IsNull());
		assert(!IsDeleted());
		facesReferences.push_back(&f0);
		facesReferences.push_back(&f1);
	};

	inline PosType GetAbutmentA() const {
		return PosType( f0->FFp(0), f0->FFi(0) );
	};
	inline PosType GetAbutmentB() const {
		return PosType( f1->FFp(0), f1->FFi(0) );
	};

	inline PosType GetSideA() const{
		if( opt==OptA )	return PosType(f0, SIDE_EDGE_OPTA);
		else return PosType(f0, SIDE_EDGE_OPTB);
	};

	inline PosType GetSideB() const{
		if( opt==OptA )	return PosType(f1, SIDE_EDGE_OPTA);
		else return PosType(f1, SIDE_EDGE_OPTB);
	};

	inline bool IsNull() const { return f0==0 && f1==0; };
	inline bool IsDeleted() const { return f0->IsD() && f1->IsD(); };

	void ResetFlag()
	{
		assert( !IsNull() );
		assert( this->parentManager->IsBridgeFace( f0 ) );
		assert( this->parentManager->IsBridgeFace( f1 ) );

		this->parentManager->ClearBridgeAttr( f0 );
		this->parentManager->ClearBridgeAttr( f1 );
	};

	void DeleteFromMesh()
	{
		assert(!IsNull() && !IsDeleted());
		if( !f0->IsD() )
		vcg::tri::Allocator<MESH>::DeleteFace(*this->parentManager->mesh, *f0);
		if( !f1->IsD() )
			vcg::tri::Allocator<MESH>::DeleteFace(*this->parentManager->mesh, *f1);

		// update mesh topology after bridge faces removal, restore border
		for(int e=0; e<3; e++)
		{
			if(!vcg::face::IsBorder<FaceType>(*f0, e))
			{
				FacePointer adjF = f0->FFp(e);
				if(!this->parentManager->IsBridgeFace(adjF))
				{
					int adjEI = f0->FFi(e);
					adjF->FFp( adjEI ) = adjF;
					adjF->FFi( adjEI ) = adjEI;
					assert(vcg::face::IsBorder<FaceType>(*adjF, adjEI));
				}
			}

			if(!vcg::face::IsBorder<FaceType>(*f1, e))
			{
				FacePointer adjF = f1->FFp(e);
				if(!this->parentManager->IsBridgeFace(adjF))
				{
					int adjEI = f1->FFi(e);
					adjF->FFp( adjEI ) = adjF;
					adjF->FFi( adjEI ) = adjEI;
					assert(vcg::face::IsBorder<FaceType>(*adjF, adjEI));
				}
			}
		}
	};

private:

	void build(AbutmentType sideA, AbutmentType sideB, BridgeOption o, std::vector<FacePointer *> &app, bool test=false)
	{
		opt = o;
		if(test)
			if(!testAbutmentDistance(sideA, sideB) &&  (opt=computeBestBridgeOpt(sideA, sideB)) == NoOne)
			{
				f0 = 0;	f1 = 0;
				opt = NoOne;
				return;
			}

		assert(testAbutmentDistance(sideA, sideB));
		assert(opt!=NoOne);

		app.push_back(&sideA.f);
		app.push_back(&sideB.f);
		FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(*this->parentManager->mesh, 2, app);
		this->parentManager->faceAttr->UpdateSize();
		app.pop_back();
		app.pop_back();

		f0 = &*fit;
		f1 = &*(fit+1);

		this->parentManager->SetBridgeAttr(f0);
		this->parentManager->SetBridgeAttr(f1);
		this->parentManager->SetHoleBorderAttr(f0);
		this->parentManager->SetHoleBorderAttr(f1);

		this->parentManager->ClearHoleBorderAttr(sideA.f);
		this->parentManager->ClearHoleBorderAttr(sideB.f);

		// the index of edge adjacent between new 2 face, is the same for both new faces
		int adjEdgeIndex = -1;

		// the index of edge adjacent between new 2 face, is the same for both new faces
		int sideEdgeIndex = -1;

		setVertexByOption(sideA, sideB, opt, *f0, *f1);
		if( opt == OptA )
		{
			adjEdgeIndex = ADJ_EDGE_OPTA;
			sideEdgeIndex = SIDE_EDGE_OPTA;
		}
		else
		{
			adjEdgeIndex = ADJ_EDGE_OPTB;
			sideEdgeIndex = SIDE_EDGE_OPTB;
		}

		ComputeNormal(*f0);
		ComputeNormal(*f1);

		//edges delle facce adiacenti alla mesh
		f0->FFp(0) = sideA.f;
		f0->FFi(0) = sideA.z;
		f1->FFp(0) = sideB.f;
		f1->FFi(0) = sideB.z;

		sideA.f->FFp(sideA.z) = f0;
		sideA.f->FFi(sideA.z) = 0;
		sideB.f->FFp(sideB.z) = f1;
		sideB.f->FFi(sideB.z) = 0;

		// edges adiacenti tra le 2 facce appena inserite, edge "condiviso"
		f0->FFp(adjEdgeIndex) = f1;
		f0->FFi(adjEdgeIndex) = adjEdgeIndex;
		f1->FFp(adjEdgeIndex) = f0;
		f1->FFi(adjEdgeIndex) = adjEdgeIndex;

		//edges laterali del ponte, quelli che restano di bordo
		f0->FFp(sideEdgeIndex) = f0;
		f0->FFi(sideEdgeIndex) = sideEdgeIndex;
		f1->FFp(sideEdgeIndex) = f1;
		f1->FFi(sideEdgeIndex) = sideEdgeIndex;

		assert( vcg::face::BorderCount(*f0)==1 );
		assert( vcg::face::BorderCount(*f1)==1 );
		assert( this->parentManager->IsBridgeFace( f0 ) );
		assert( this->parentManager->IsBridgeFace( f1 ) );
	};

/*** funzioni statiche ***/

public:

	/*  Build a bridge between 2 border edge.
	 *  If the bridge is inside the same hole it cannot be adjacent the hole border,
	 *  this means fill another sub hole.
	 */
	static bool CreateBridge(AbutmentType &sideA, AbutmentType &sideB, HoleSetManager<MESH>* holesManager, QString &err)
	{
		assert( vcg::face::IsBorder<FaceType>(*sideA.f, sideA.z) &&
						vcg::face::IsBorder<FaceType>(*sideB.f, sideB.z));
		assert(!sideA.h->IsFilled() && !sideB.h->IsFilled());

		std::vector<FacePointer *> tmpFaceRef;
		//if(app!=0)
		//	tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
		//holesManager->AddFaceReference(tmpFaceRef);

		ScalarType q;
		BridgeOption opt = computeBestBridgeOpt(sideA, sideB, &q);
		if(opt == NoOne)
		{
			err = QString("Bridge is compenetrating with mesh.");
			return false;
		}

		if(sideA.h == sideB.h)
		{
			if( testAbutmentDistance(sideA, sideB) )
				subdivideHoleWithBridge(sideA, sideB, opt, holesManager, tmpFaceRef);
			else
			{
				err = QString("Bridge has sides adjacent to mesh.");
				return false;
			}
		}
		else
			unifyHolesWithBridge(sideA, sideB, opt, holesManager, tmpFaceRef);
		return true;
	};


	/*  Build a bridge inner to the same hole. It chooses the best bridge computing quality
	 *  of 2 faces and similarity (as number of edge) of two next hole. Bridge is build follow
	 *  bridge's rule, bridge must have 2 border edge.
	 *  infoLabel paramter is used to show work progress.
	 *  Return number of bridge builded.
	 */
	 static bool AutoSelfBridging(HoleSetManager<MESH>* holesManager, double dist_coeff=0.0, std::vector<FacePointer *> *app=0)
	{
		bool err = false;
		time_t timer;
		if(holesManager->autoBridgeCB != 0)
		{
			holesManager->autoBridgeCB->Invoke(0);
			timer = clock();
		}
		GridType gM;
		gM.Set(holesManager->mesh->face.begin(),holesManager->mesh->face.end());

		std::vector<FacePointer *> tmpFaceRef;
		AbutmentType sideA, sideB;
		BridgeOption bestOpt;

		int nh = holesManager->holes.size();
		for(int h=0; h<nh; ++h)
		{
			HoleType &thehole = holesManager->holes.at(h);
			if(!thehole.IsSelected() || thehole.Size()<6 )
				continue;
			assert(!thehole.IsFilled());

			ScalarType maxQuality = -1;

			// initP: first bridge abutment
			PosType initP = thehole.p;
			for(int i=0; i<thehole.Size();i++)
			{
				// initP: second bridge abutment
				PosType endP = initP;
				endP.NextB();endP.NextB();
				for(int j=3; j<=thehole.Size()/2; j++)
				{
					endP.NextB();

					// two edge used as bridge abutment are adjacent to the same face... bridge can't be build
					// i due edge di bordo sono gia' collegati da 2 triangoli adiacenti,
					// il bridge si sovrapporrebbe a questi 2 triangoli
					if( endP.f->FFp(0) == initP.f ||
						endP.f->FFp(1) == initP.f ||
						endP.f->FFp(2) == initP.f )
						continue;

					AbutmentType a(initP.f, initP.z, &thehole);
					AbutmentType b(endP.f, endP.z, &thehole);
					if(!testAbutmentDistance(a,b))
						continue;

					ScalarType q;
					BridgeOption opt = computeBestBridgeOpt(a, b, &q, &gM);
					if(opt != NoOne)
					{
						q += dist_coeff * j; // add distance weight
						if(  q > maxQuality)
						{
							maxQuality = q;
							bestOpt = opt;
							sideA.f=initP.f; sideA.z=initP.z; sideA.h=&thehole;
							sideB.f=endP.f; sideB.z=endP.z; sideB.h=&thehole;
						}
					}

					if(holesManager->autoBridgeCB != 0)
					{
						if(int(clock()) - timer > holesManager->autoBridgeCB->GetOffset())
						{
							float progress = (float)(((float)( ((float)j/(thehole.Size()-3)) + i) / thehole.Size()) + h) / nh;
							holesManager->autoBridgeCB->Invoke(progress*100);
							timer = clock();
						}
					}
				}// scansione del edge di arrivo

				initP.NextB();
			}// scansione dell'edge di partenza

			assert(vcg::face::IsBorder<FaceType>(*sideA.f, sideA.z));
			assert(vcg::face::IsBorder<FaceType>(*sideB.f, sideB.z));

			if( maxQuality > -1)
			{
				tmpFaceRef.clear();
				if(app!=0)
					tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
				holesManager->AddFaceReference(tmpFaceRef);
				subdivideHoleWithBridge(sideA, sideB, bestOpt, holesManager, tmpFaceRef);
				gM.Set(holesManager->mesh->face.begin(), holesManager->mesh->face.end());
			}
			else
				err = true;
		} //scansione degli holes
		if(err) return false;
		else return true;
	};


	/*  It connects iteratively selected holes with the best bridge.
	 *  Result is unique hole instead of init holes.
	 *  Return number of bridges builded.
	 */
	static void AutoMultiBridging(HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> *app=0 )
	{
		int timer;
		if(holesManager->autoBridgeCB != 0)
		{
			holesManager->autoBridgeCB->Invoke(0);
			timer = clock();
		}

		GridType gM;

		std::vector<FacePointer *> tmpFaceRef;
		AbutmentType sideA, sideB;
		BridgeOption bestOpt;

		std::vector<HoleType*> selectedHoles;
		typename std::vector<HoleType*>::iterator shit1, shit2;
		typename HoleVector::iterator hit;

		int nIteration = -1;
		int iteration = 0;
		// iterate, unify holes, until selected hole is only one
		do{
			sideA.SetNull();
			sideB.SetNull();

			// prendo gli hole selezionati
			selectedHoles.clear();
			for(hit=holesManager->holes.begin(); hit!=holesManager->holes.end(); hit++)
				if(hit->IsSelected())
					selectedHoles.push_back(&*hit);

			if(selectedHoles.size() < 2)
				return;
			gM.Set(holesManager->mesh->face.begin(),holesManager->mesh->face.end());

			float casesViewed = 0, cases2View = 0;
			for(shit1=selectedHoles.begin(); shit1!=selectedHoles.end(); shit1++)
				for(shit2=shit1+1; shit2!=selectedHoles.end(); shit2++)
					cases2View += (*shit1)->Size() * (*shit2)->Size();

			if(nIteration == -1)
				nIteration = selectedHoles.size()-1;

			// cerco la miglior combinazione tra le facce di un hole con quelle di un'altro
			ScalarType maxQuality = -1;
			for(shit1=selectedHoles.begin(); shit1!=selectedHoles.end(); shit1++)
			{
				for(shit2=shit1+1; shit2!=selectedHoles.end(); shit2++)
				{
					PosType ph1((*shit1)->p.f, (*shit1)->p.z);
					PosType ph2((*shit2)->p.f, (*shit2)->p.z);
					do{	//scansione edge di bordo del primo buco
						do{
							ScalarType q;
							AbutmentType a( ph1.f, ph1.z, *shit1 );
							AbutmentType b( ph2.f, ph2.z, *shit2 );
							BridgeOption opt = computeBestBridgeOpt(a, b, &q, &gM);
							if(opt != NoOne)
								if(q > maxQuality)
								{
									maxQuality = q;
									sideA = a;
									sideB = b;
									bestOpt = opt;
								}

							if(holesManager->autoBridgeCB != 0)
							{
								if(int(clock()) - timer > holesManager->autoBridgeCB->GetOffset())
								{
									int progress = ( (100 * ( iteration +(casesViewed/cases2View)))/nIteration );
									holesManager->autoBridgeCB->Invoke(progress);
									timer = clock();
								}
							}

							casesViewed++;
							ph2.NextB();
						}while(ph2 != (*shit2)->p);

						ph1.NextB();
					}while(ph1 != (*shit1)->p);
				} // for(shit2=shit1+1; shit2!=selectedHoles.end(); shit2++)
			}

			// adesso ho la miglior coppia di edge si deve creare il bridge
			assert(!sideA.IsNull() && !sideB.IsNull());

			// la rimozione di hole dovuta alla unifyHoles.. provoca l'aggiornamento della lista di holes
			// pertanto si deve avere sempre aggiornata la lista dei riferimenti alle facce
			// la quale dipende anche dagli holes
			tmpFaceRef.clear();
			if(app!=0)
				tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
			holesManager->AddFaceReference(tmpFaceRef);
			
			if(maxQuality > -1)
				unifyHolesWithBridge(sideA, sideB, bestOpt, holesManager, tmpFaceRef);
			else
				return;
			iteration ++;
		}while(true);
		return;
	};


private:
	/*  Compute distance between bridge side to allow no bridge adjacent to hole border.
	 *  A bridge must have 2 border faces.
	 */
	static bool testAbutmentDistance(const AbutmentType &sideA, const AbutmentType &sideB)
	{
		if(sideA.h != sideB.h) return true;

		// at least 2 edges have to be between 2 bridge side.
		if(!sideA.h->IsNonManifold())
		{
			// so adjacent edges of a side haven't to share a vertex with other side.
			PosType pos(sideA.f, sideA.z);
			assert(pos.IsBorder());
			pos.NextB();
			if(pos.v == sideB.f->V0(sideB.z)) return false;
			if(pos.v == sideB.f->V1(sideB.z)) return false;

			pos = PosType(sideA.f, sideA.z);
			pos.FlipV();
			pos.NextB();
			if(pos.v == sideB.f->V0(sideB.z)) return false;
			if(pos.v == sideB.f->V1(sideB.z)) return false;
		}
		else
		{
			// if exist a face which share a vertex with each side then the bridge cannot be built
			PosType initPos(sideA.f, sideA.z);
			PosType curPos=initPos;

			VertexType* va0=sideA.f->V0(sideA.z);
			VertexType* va1=sideA.f->V1(sideA.z);
			VertexType* vb0=sideB.f->V0(sideB.z);
			VertexType* vb1=sideB.f->V1(sideB.z);

			do{
				VertexType* cv0=curPos.f->V0(curPos.z);
				VertexType* cv1=curPos.f->V1(curPos.z);
				if(	cv0 == va0 || cv1 == va0 ||
					  cv0 == va1 || cv1 == va1 )
					if( cv0 == vb0 || cv1 == vb0 ||
					    cv0 == vb1 || cv1 == vb1 )
						return false;

				curPos.NextB();
			}while( curPos != initPos );
		}
		return true;
	};

	static void subdivideHoleWithBridge(AbutmentType &sideA, AbutmentType &sideB,
		BridgeOption bo, HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> &app)
	{
		assert(sideA.h==sideB.h);
		assert(testAbutmentDistance(sideA, sideB));
		FgtBridge<MESH>* b = new FgtBridge<MESH>(holesManager);
		b->build(sideA, sideB, bo, app);
		holesManager->bridges.push_back(b);

		sideA.h->SetStartPos(b->GetSideA());
		sideA.h->SetBridged(true);
		FgtHole<MESH> newHole(b->GetSideB(), QString("Hole_%1").arg(HoleType::GetHoleId(),3,10,QChar('0')), holesManager);
		if(sideA.h->IsSelected())
			newHole.SetSelect(true);
		newHole.SetBridged(true);
		holesManager->holes.push_back( newHole );
	};

	static void unifyHolesWithBridge(AbutmentType &sideA, AbutmentType &sideB,
				BridgeOption bo, HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> &app)
	{
		assert(vcg::face::IsBorder<FaceType>(*sideA.f, sideA.z));
		assert(vcg::face::IsBorder<FaceType>(*sideB.f, sideB.z));
		assert( sideA.h!=sideB.h);

		FgtBridge<MESH>* b = new FgtBridge<MESH>(holesManager);
		b->build(sideA, sideB, bo, app);
		holesManager->bridges.push_back(b);

		sideA.h->SetStartPos(b->GetSideA());
		assert( sideA.h->p.IsBorder() );
		if(sideB.h->IsSelected())
			sideA.h->SetSelect(true);
		sideA.h->SetBridged(true);

		typename HoleVector::iterator hit;
		for(hit=holesManager->holes.begin(); hit!=holesManager->holes.end(); ++hit)
			if(&*hit == sideB.h)
			{
				holesManager->holes.erase(hit);
				return;
			}
	};

	/*  Set bridge vertexes according bridge build option
	 */
	static void setVertexByOption(AbutmentType &sideA, AbutmentType &sideB, BridgeOption o,
		FaceType &bf0, FaceType &bf1)
	{
		VertexType* vA0 = sideA.f->V0( sideA.z ); // first vertex of pos' 1-edge
		VertexType* vA1 = sideA.f->V1( sideA.z ); // second vertex of pos' 1-edge
		VertexType* vB0 = sideB.f->V0( sideB.z ); // first vertex of pos' 2-edge
		VertexType* vB1 = sideB.f->V1( sideB.z ); // second vertex of pos' 2-edge

		// Quality
		if(o==OptA)
		{
			bf0.V(0) = vA1; bf0.V(1) = vA0;	bf0.V(2) = vB0;
			bf1.V(0) = vB1; bf1.V(1) = vB0; bf1.V(2) = vA0;
		}
		else
		{
			bf0.V(0) = vA1; bf0.V(1) = vA0;	bf0.V(2) = vB1;
			bf1.V(0) = vB1; bf1.V(1) = vB0; bf1.V(2) = vA1;
		}
	}

	/*  Find how triangolate two bridge faces.
	 *  If return "NoOne" means bridge is compenetrating with mesh for both option
	 */
	static BridgeOption computeBestBridgeOpt(AbutmentType sideA,
		AbutmentType sideB, ScalarType* quality=0, GridType* gM=0)
	{
		HoleSetManager<MESH>* hm = sideA.h->parentManager;
		bool delgm = false;
		if(gM==0)
		{
			gM = new GridType();
			gM->Set(hm->mesh->face.begin(),hm->mesh->face.end());
			delgm = true;
		}

		FaceType bf0, bf1;
		ScalarType qA = -1;
		// Caso A
		setVertexByOption(sideA, sideB, OptA, bf0, bf1);
		if( !HoleType::TestFaceMeshCompenetration(*hm->mesh, *gM, &bf0) &&
				!HoleType::TestFaceMeshCompenetration(*hm->mesh, *gM, &bf1) )
					qA = QualityFace(bf0)+ QualityFace(bf1);

		// Caso B
		ScalarType qB = -1;
		setVertexByOption(sideA, sideB, OptB, bf0, bf1);
		if( !HoleType::TestFaceMeshCompenetration(*hm->mesh, *gM, &bf0) &&
				!HoleType::TestFaceMeshCompenetration(*hm->mesh, *gM, &bf1) )
					qB = QualityFace(bf0)+ QualityFace(bf1);

		if(delgm)
		{
			delete gM;
			gM=0;
		}

		if(quality!=0)
		{
			if(qA>qB) *quality = qA;
			else *quality = qB;
		}

		if(qA==-1 && qB==-1)
			return NoOne;	// both autocompenetrant with mesh
		else if(qA>qB)
			return OptA;
		else
			return OptB;
	};


private:
	BridgeOption opt;

public:
	FacePointer f0;
	FacePointer f1;
};



/*  "Bridge" (face) added to close non manifold holes
 *
 *  
 *	  -----+------+------      ----+------+--------
 *		      \ A  /                  \ A  /|
 *	         \  /                    \  / |
 *		 holeX  \/  holeX        holeX  \/  |  holeY
 *	          /\                      /\f0|  
 *	         /  \                    /  \ |
 *	        / B  \                  / B  \|
 *	  -----+------+------      ----+------+--------
 *   HoleX is Non-Manifold     HoleX and HoleY aren't 
 *	                                Non-Manifold
 */
template <class MESH>
class FgtNMBridge: public FgtBridgeBase<MESH>
{
  typedef typename MESH::FaceType							FaceType;
	typedef typename MESH::FacePointer					FacePointer;
  typedef typename vcg::face::Pos<FaceType>		PosType;
  typedef FgtHole<MESH>												HoleType;
  typedef typename MESH::FaceIterator					FaceIterator;

public:

	FgtNMBridge(FacePointer f, HoleSetManager<MESH>* parent)
	{
		f0 = f;
		this->parentManager = parent;
	};

	inline PosType GetAbutmentA() const {
		return PosType( f0->FFp(0), f0->FFi(0));
	};
	inline PosType GetAbutmentB() const {
		return PosType( f0->FFp(2), f0->FFi(2));
	};

	inline bool IsNull() const { return f0==0; };
	inline bool IsDeleted() const { return f0->IsD(); };

	inline void AddFaceReference(std::vector<FacePointer*> &facesReferences)
	{
		assert(!IsNull());
		assert(!IsDeleted());
		facesReferences.push_back(&f0);
	};


	void ResetFlag()
	{
		assert( !IsNull() );
		assert(this->parentManager->IsBridgeFace(f0) );
		this->parentManager->ClearBridgeAttr(f0);
	};

	void DeleteFromMesh()
	{
		assert( !IsNull() );
		assert( this->parentManager->IsBridgeFace(f0) );
		if( !f0->IsD() )
			vcg::tri::Allocator<MESH>::DeleteFace(*this->parentManager->mesh, *f0);

		// update mesh topology after bridge faces removal, restore border
		for(int e=0; e<3; e++)
		{
			if(!vcg::face::IsBorder<FaceType>(*f0, e))
			{
				FacePointer adjF = f0->FFp(e);
				if(!this->parentManager->IsBridgeFace(adjF))
				{
					int adjEI = f0->FFi(e);
					adjF->FFp( adjEI ) = adjF;
					adjF->FFi( adjEI ) = adjEI;
					assert(vcg::face::IsBorder<FaceType>(*adjF, adjEI));
				}
			}
		}
	};

	/* Walk over selected non-manifold holes, find vertex visited more times,
	 * add face adjacent to non-manifold vertex.
	 */
	static void CloseNonManifoldVertex(HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> *app=0)
	{
		int startNholes = holesManager->holes.size();

		std::vector<FacePointer *> tmpFaceRef;

		for(int i=0; i<startNholes; i++)
		{
			HoleType *h = &holesManager->holes.at(i);
			if(!(h->IsNonManifold() && h->IsSelected()))
				continue;

			// walk the border, mark as visit each vertex. If walk twice over the same vertex go back over the border
			// to find other edge sharing this vertex
			PosType curPos = h->p;
			assert(curPos.IsBorder());
			assert(!h->IsFilled());
			PosType p0, p1;
			p0.SetNull();
			p1.SetNull();
			do{
				assert(p0.IsNull());
				if(curPos.v->IsV())
					p0.Set(curPos.f, curPos.z, curPos.v);
				else
					curPos.v->SetV();
				curPos.NextB();
				assert(curPos.IsBorder());

				if(!p0.IsNull())
				{
					tmpFaceRef.clear();
					if(app!=0)
						tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
					holesManager->AddFaceReference(tmpFaceRef);
					
					// faces allocation and local face reference management
					tmpFaceRef.push_back(&p0.f);
					tmpFaceRef.push_back(&curPos.f);
					FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(*holesManager->mesh, 1, tmpFaceRef);
					holesManager->faceAttr->UpdateSize();
					tmpFaceRef.pop_back();
					tmpFaceRef.pop_back();


					// non-manifold vertex found, go back over the border to find other edge share the vertex with p0
					int dist = 0;
					p1 = p0;
					p1.FlipV();
					do{
						dist++;
						p1.v->ClearV();
						p1.NextB();
					}while(p0.v != p1.v);

					// p2 is used only in case face added close sub-hole, sub-hole distance is 2
					// p2 is half-edge adjcent to connect to patch edge which usually is border
					PosType p2 = p0;
					p2.FlipV();
					p2.NextB();

					// face is build to have as vertex 0 the non-manifold vertex, so it have adge 1 as border edge
					fit->V(0) = p0.v;
					if( p0.VInd() == p0.z)
					{
						fit->V(1) = p1.f->V(p1.z);
						fit->V(2) = p0.f->V1(p0.z);

						fit->FFp(0) = p1.f;
						fit->FFi(0) = p1.z;
						fit->FFp(2) = p0.f;
						fit->FFi(2) = p0.z;

						p0.f->FFp(p0.z) = &*fit;
						p0.f->FFi(p0.z) = 2;
						p1.f->FFp(p1.z) = &*fit;
						p1.f->FFi(p1.z) = 0;
					}
					else
					{
						fit->V(1) = p0.f->V(p0.z);
						fit->V(2) = p1.f->V1(p1.z);

						fit->FFp(0) = p0.f;
						fit->FFi(0) = p0.z;
						fit->FFp(2) = p1.f;
						fit->FFi(2) = p1.z;

						p0.f->FFp(p0.z) = &*fit;
						p0.f->FFi(p0.z) = 0;
						p1.f->FFp(p1.z) = &*fit;
						p1.f->FFi(p1.z) = 2;
					}

					ComputeNormal(*fit);
					holesManager->SetBridgeAttr(&*fit);
					holesManager->bridges.push_back( new FgtNMBridge(&*fit, holesManager) );

					if(dist==2)
					{
						// face used to close non-manifold holes, close entirely a "sub-hole" (sub-hole has
						// only 3 border edge). This face become a patch face wich fill an invisible subhole.
						holesManager->SetPatchAttr(&*fit);
						fit->FFp(1) = p2.f;
						fit->FFi(1) = p2.z;
						p2.f->FFp(p2.z) = &*fit;
						p2.f->FFi(p2.z) = 1;
					}
					else
					{
						fit->FFp(1) = &*fit;
						fit->FFi(1) = 1;

						HoleType newhole(PosType(&*fit, 1), QString("Hole_%1").arg(HoleType::GetHoleId(),3,10,QChar('0')), holesManager);
						if(h->IsSelected())
							newhole.SetSelect(true);
						newhole.SetBridged(true);
						holesManager->holes.push_back(newhole);
					}
					p0.SetNull();
				}
			}while( curPos != h->p );

			// hole now is divided intosub hole
			curPos = h->p;
			do{
				curPos.v->ClearV();
				curPos.NextB();
			}while( curPos!= h->p);

			//forzo l'aggiornamento delle info dell'hole
			h->SetStartPos(h->p);
			h->SetBridged(true);
		}// for(int i=0; i<startNholes...
	};

public:
	FacePointer f0;
};

#endif
