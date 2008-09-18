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
	int z;					// edge index
	FgtHole<MESH>* h;
};

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
	typedef typename std::vector<PosType>			PosVector;
	typedef FgtHole<MESH>							HoleType;
	typedef typename std::vector<HoleType>			HoleVector;
	
	typedef typename MESH::VertexType				VertexType;
	typedef typename MESH::CoordType				CoordType;
	typedef typename MESH::ScalarType				ScalarType;
	typedef typename vcg::Triangle3<ScalarType>		TriangleType;


/****	Static functions	******/
public:
	
	/*  Build a bridge between 2 border edge.
	 *  If the bridge is inside the same hole it cannot be adjacent the hole border, 
	 *  this means fill another sub hole.
	 */
	
	static bool CreateBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB,  MESH &mesh, 
		HoleVector &holes, std::vector<FacePointer *> *app=0)
	{
		assert(FgtHole<MESH>::IsHoleBorderFace(*sideA.f) && FgtHole<MESH>::IsHoleBorderFace(*sideB.f));
		assert(!sideA.h->IsFilled() && !sideB.h->IsFilled());

		std::vector<FacePointer *> tmpFaceRef;
		if(app!=0)
			tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
		FgtHole<MESH>::AddFaceReference(holes, tmpFaceRef);

		if(sideA.h == sideB.h)
		{
			if( testAbutmentDistance(sideA, sideB))
				subdivideHoleWithBridge(sideA, sideB, mesh, holes, tmpFaceRef);
			else
				return false;
		}
		else
			unifyHolesWithBridge(sideA, sideB, mesh, holes, tmpFaceRef);

		return true;
	};

	
	static void RemoveBridges(MESH  &mesh, HoleVector &holes)
	{
		HoleType* h;

		// vettore usato per contenere tutte le facce di bridge trovate a partire da un hole
		std::vector<FacePointer> bridgeFaces;

		// vettore usato per contenere tutti gli half-edge adiacenti alle facce dei bridge che sono anche
		PosVector adjBorderPos;

		PosType curPos;
		getBridgeInfo(holes, bridgeFaces, adjBorderPos);

		// elimino gli hole relativi alle facce bridge trovate,
		typename std::vector<FacePointer>::iterator fit;
		for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
		{
			// se la faccia bridge in esame si affaccia su un'altro hole rispetto 
			// a quello in esame rimuovo anche quell'hole
			if(FgtHole<MESH>::IsHoleBorderFace(**fit))
			{
				typename HoleVector::iterator hit;
				if(FgtHole<MESH>::FindHoleFromBorderFace(*fit, holes, hit) != -1)
				{
					assert(!hit->IsFilled());
					holes.erase(hit);
				}
			}
			if( !(*fit)->IsD() )
				vcg::tri::Allocator<MESH>::DeleteFace(mesh, **fit);
		}

		// elimino anche gli hole trovati dalle facce adiacenti, solo quelle che però sono di bordo, 
		// ovvero facce di hole derivanti da chiusura di vertici non manifold, che però non hanno
		// edge bridged sul loro bordo
		typename std::vector<PosType>::iterator pit;
		for(pit=adjBorderPos.begin(); pit!=adjBorderPos.end(); pit++)
			if(FgtHole<MESH>::IsHoleBorderFace(*pit->f))
			{
				typename HoleVector::iterator hit;
				if(FgtHole<MESH>::FindHoleFromBorderFace(pit->f, holes, hit) != -1)
				{
					assert(!hit->IsFilled());
					holes.erase(hit);
				}
			}

		
		// rimuovo le facce bridge ed aggiorno la topologia della faccia della 
		// mesh adiacente al bridge la faccia "dove poggia la spalla del ponte"					
		for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
		{
			for(int e=0; e<3; e++)
			{	
				if(!IsBorder(**fit, e))
				{
					FacePointer adjF = (*fit)->FFp(e);
					if(!FgtHole<MESH>::IsBridgeFace(*adjF))
					{								
						int adjEI = (*fit)->FFi(e);
						adjF->FFp( adjEI ) = adjF;
						adjF->FFi( adjEI ) = adjEI;
						assert(IsBorder(*adjF, adjEI));
					}
				}
			}
		}
		
		// ho staccato le facce bridge dalla mesh e rimosso gli hole adiacenti al bridge
		// adesso aggiorno l'hole ed re-inserisco eventuali hole che si sono formati 
		// dalla rimozione del bridge (il bridge legava più hole distinti)
		
		// per ogni faccia adiacente al bridge scorro il buco marcando le facce visitate
		// in modo da scoprire quali facce vengono si affacciano su buchi nuovi				
		PosType initPos;
		typename PosVector::iterator it;
		for( it=adjBorderPos.begin(); it!=adjBorderPos.end(); it++)
		{
			assert( it->IsBorder() );
			if(it->f->IsV())
				continue;
			
			curPos = initPos = *it;
			do{
				curPos.f->SetV();
				curPos.NextB();
				assert(curPos.IsBorder());
			}while(curPos != initPos);
			
			
			// ho trovato una faccia di un'altro buco
			FgtHole<MESH> newHole(initPos, QString("Hole_%1").arg(holes.size(),3,10,QChar('0')) );
			if(h->IsSelected())
				newHole.SetSelect(true);
			holes.push_back( newHole );
		}

		// resetto falg visited sulle facce degli hole interessati
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

	/** Return boolean indicatind if face is a face of an hole of the list.
	 *  Also return a pointer to appartenent hole and pos finded
	 */
	static bool FindBridgeAbutmentFromPick(FacePointer bFace, int pickedX, int pickedY, 
		HoleVector &holes, BridgeAbutment<MESH> &pickedResult) 
	{ 
		if( vcg::face::BorderCount(*bFace) == 0 )
			return false;

		typename HoleVector::iterator hit;
		if( FgtHole<MESH>::FindHoleFromBorderFace(bFace, holes, hit) < 0 )
		{
			pickedResult.SetNull();
			return false;
		}

		pickedResult.h = &*hit;
		pickedResult.f = bFace;
		if( vcg::face::BorderCount(*bFace) == 1 )
		{
			for(int i=0; i<3; i++)
				if(vcg::face::IsBorder<FaceType>(*bFace, i))
					pickedResult.z = i;				
		}
		else
		{
			PosType retPos = getClosestPos(bFace, pickedX, pickedY);
			pickedResult.f = retPos.f;
			pickedResult.z = retPos.z;
		}	
		
		return true; // means no find hole
	};

	
	/*  Per scegliere la miglior combinazione di edge all'interno di un hole, teto la qualità dei 
	 *  triangoli ottenuti da tutte le possibili combinazioni che rispettino il concetto di bridge
	 *  ovvero che non siano adiacenti o con 1 edge che li divide
	 */
	static void AutoSelfBridging(MESH &mesh, HoleVector &holes, double dist_coeff=0.0, std::vector<FacePointer *> *app=0)
	{
		std::vector<FacePointer *> tmpFaceRef;
		HoleType* oldRef = 0;
		BridgeAbutment<MESH> sideA, sideB;

		int nh = holes.size();
		for(int h=0; h<nh; ++h)
		{
			HoleType &thehole = holes.at(h);
			if(!thehole.IsSelected() || thehole.Size()<6 )
				continue;
			assert(!thehole.IsFilled());

			ScalarType maxQuality = -1;
			
			// si scorre l'edge di partenza
			PosType initP = thehole.p;
			for(int i=0; i<thehole.Size();i++)
			{
				// posiziono il secondo edge, la seconda spalla del ponte
				PosType endP = initP;
				endP.NextB();endP.NextB();					
				for(int j=3; j<=thehole.Size()/2; j++)
				{	
					endP.NextB();

					// i due edge di bordo sono già collegati da 2 triangoli adiacenti,
					// il bridge si sovrapporrebbe a questi 2 triangoli
					if( endP.f->FFp(0) == initP.f ||
						endP.f->FFp(1) == initP.f ||
						endP.f->FFp(2) == initP.f )
						continue;

					BridgeAbutment<MESH> a(initP.f, initP.z, &thehole);
					BridgeAbutment<MESH> b(endP.f, endP.z, &thehole);
					if(!testAbutmentDistance(a,b))
						continue;

					VertexType* vA0 = initP.f->V0( initP.z ); // first vertex of pos' 1-edge 
					VertexType* vA1 = initP.f->V1( initP.z ); // second vertex of pos' 1-edge
					VertexType* vB0 = endP.f->V0( endP.z ); // first vertex of pos' 2-edge 
					VertexType* vB1 = endP.f->V1( endP.z ); // second vertex of pos' 2-edge 

					// solution A 
					TriangleType bfA0(vA1->P(), vA0->P(), vB0->P());
					TriangleType bfA1(vB1->P(), vB0->P(), vA0->P());

					// solution B
					TriangleType bfB0(vA1->P(), vA0->P(), vB1->P());
					TriangleType bfB1(vB1->P(), vB0->P(), vA1->P());

					ScalarType oldq = maxQuality;
					ScalarType q = bfA0.QualityFace()+ bfA1.QualityFace() + dist_coeff * j;
					if(  q > maxQuality)
						maxQuality = q;
					q = bfB0.QualityFace()+ bfB1.QualityFace() + dist_coeff * j;
					if(  q > maxQuality)
						maxQuality = q;

					if(oldq < maxQuality)
					{
						//sideA = BridgeAbutment<MESH>(initP.f, initP.z, &thehole);
						//sideB = BridgeAbutment<MESH>(endP.f, endP.z, &thehole);
						sideA.f=initP.f; sideA.z=initP.z; sideA.h=&thehole;
						sideB.f=endP.f; sideB.z=endP.z; sideB.h=&thehole;
					}					
				}// scansione del edge di arrivo
				
				initP.NextB();
			}// scansione dell'edge di partenza

			assert(vcg::face::IsBorder<FaceType>(*sideA.f, sideA.z));
			assert(vcg::face::IsBorder<FaceType>(*sideB.f, sideB.z));

			if( oldRef != &*holes.begin() )
			{
				// si può 
				tmpFaceRef.clear();
				if(app!=0)
					tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
				FgtHole<MESH>::AddFaceReference(holes, tmpFaceRef);
				tmpFaceRef.push_back(&sideA.f);
				tmpFaceRef.push_back(&sideB.f);
				oldRef = &*holes.begin();				
			}

			subdivideHoleWithBridge(sideA, sideB, mesh, holes, tmpFaceRef);
			// la subdivideHole.. aggiunge un hole pertanto bisogna aggiornare anche la lista di 
			// reference a facce
			tmpFaceRef.push_back(&holes.back().p.f);
		} //scansione degli holes
	};


	static void AutoMultiBridging(MESH &mesh, HoleVector &holes, double dist_coeff=0.0, std::vector<FacePointer *> *app=0)
	{
		std::vector<FacePointer *> tmpFaceRef;
		BridgeAbutment<MESH> sideA, sideB;
		std::vector<HoleType*> selectedHoles;
		typename std::vector<HoleType*>::iterator shit1, shit2; 
		typename HoleVector::iterator hit;
		do{
			sideA.SetNull();
			sideB.SetNull();

			// prendo gli hole selezionati
			selectedHoles.clear();
			for(hit=holes.begin(); hit!=holes.end(); hit++)
				if(hit->IsSelected())
					selectedHoles.push_back(&*hit);
			
			if(selectedHoles.size() < 2)
				return;

			// cerco la miglior combinazione tra le facce di un hole con quelle di un'altro

			ScalarType maxQuality = -1;
			for(shit1=selectedHoles.begin(); shit1!=selectedHoles.end(); shit1++)
				for(shit2=shit1+1; shit2!=selectedHoles.end(); shit2++)
				{
					PosType ph1=(*shit1)->p;
					PosType ph2=(*shit2)->p;

					do{	//scansione edge di bordo del primo buco
						do{
							VertexType* vA0 = ph1.f->V0( ph1.z ); // first vertex of pos' 1-edge 
							VertexType* vA1 = ph1.f->V1( ph1.z ); // second vertex of pos' 1-edge
							VertexType* vB0 = ph2.f->V0( ph2.z ); // first vertex of pos' 2-edge 
							VertexType* vB1 = ph2.f->V1( ph2.z ); // second vertex of pos' 2-edge 

							// solution A 
							TriangleType bfA0(vA1->P(), vA0->P(), vB0->P());
							TriangleType bfA1(vB1->P(), vB0->P(), vA0->P());

							// solution B
							TriangleType bfB0(vA1->P(), vA0->P(), vB1->P());
							TriangleType bfB1(vB1->P(), vB0->P(), vA1->P());

							ScalarType oldq = maxQuality;
							ScalarType q = bfA0.QualityFace()+ bfA1.QualityFace();
							if(  q > maxQuality)
								maxQuality = q;
							q = bfB0.QualityFace()+ bfB1.QualityFace();
							if(  q > maxQuality)
								maxQuality = q;

							if(oldq < maxQuality)
							{
								sideA = BridgeAbutment<MESH>(ph1.f, ph1.z, *shit1);
								sideB = BridgeAbutment<MESH>(ph2.f, ph2.z, *shit2);
							}

							ph2.NextB();
						}while(ph2 != (*shit2)->p);

						ph1.NextB();
					}while(ph1 != (*shit1)->p);
				} // for(shit2=shit1+1; shit2!=selectedHoles.end(); shit2++)

			// adesso ho la miglior coppia di edge si deve creare il bridge
			assert(!sideA.IsNull() && !sideB.IsNull());
			
			// la rimozione di hole dovuta alla unifyHoles.. provoca l'aggiornamento della lista di holes
			// pertanto si deve avere sempre aggiornata la lista dei riferimenti alle facce
			// la quale dipende anche dagli holes
			tmpFaceRef.clear();
			if(app!=0)
				tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
			FgtHole<MESH>::AddFaceReference(holes, tmpFaceRef);
			tmpFaceRef.push_back(&sideA.f);
			tmpFaceRef.push_back(&sideB.f);
			unifyHolesWithBridge(sideA, sideB, mesh, holes, tmpFaceRef);

		}while(true);
	};



	/* 
	*/
	static void CloseNonManifoldVertex(MESH &mesh, HoleVector &holes, std::vector<FacePointer *> *app=0)
	{
		int startNholes = holes.size();

		// i riferimenti alle facce presenti nella lista di hole vengono gestiti qui perchè
		// questo metodo può fare inserimenti sulla lista di hole e quindi provocarne il riallocamento
		// e perdere così i riferimenti.
		std::vector<FacePointer *> tmpFaceRef;
		if(app!=0)
			tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
		FgtHole<MESH>::AddFaceReference(holes, tmpFaceRef);
		HoleType* oldRef = &*holes.begin();

		for(int i=0; i<startNholes; i++)
		{
			HoleType *h = &holes.at(i);
			if(!(h->IsNonManifold() && h->IsSelected()))
				continue;
			
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
					tmpFaceRef.push_back(&p0.f);
					tmpFaceRef.push_back(&curPos.f);
					FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(mesh, 1, tmpFaceRef);
					tmpFaceRef.pop_back();
					tmpFaceRef.pop_back();

					// torno indietro lungo il bordo resettando il flag V dei vertici (tranne quello condiviso)
					// e prendo l'edge che condivide un vertice (p1)
					int dist = 0;
					p1 = p0;
					p1.FlipV();
					do{
						dist++;
						p1.v->ClearV();
						p1.NextB();
					}while(p0.v != p1.v);

					PosType p2 = p0;
					p2.FlipV();
					p2.NextB();

					// si costruisce la faccia in modo da avere vertice 0 il vertice non manifold
					// e quindi l'edge 1 l'edge di bordo
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
					fit->SetUserBit(FgtHole<MESH>::BridgeFlag());
						
					if(dist==2)
					{
						// il buco da chiudere è formato solo da 3 edge e quidni viene completamente chiuso
						// senza possibilità di ripristinarlo, tanto la soluzione di riempimento è solo una
						fit->FFp(1) = p2.f;
						fit->FFi(1) = p2.z;
						p2.f->FFp(p2.z) = &*fit;
						p2.f->FFi(p2.z) = 1;						
					}
					else
					{
						fit->FFp(1) = &*fit;
						fit->FFi(1) = 1;

						HoleType newhole(PosType(&*fit, 1), QString("Hole_%1").arg(holes.size(),3,10,QChar('0')));
						if(h->IsSelected())
							newhole.SetSelect(true);
						newhole.SetBridged(true);
						holes.push_back(newhole);

						if( oldRef != &*holes.begin() )
						{
							tmpFaceRef.clear();
							if(app!=0)
								tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
							FgtHole<MESH>::AddFaceReference(holes, tmpFaceRef);
							oldRef = &*holes.begin();
							h = &holes.at(i);
						}
						else
							tmpFaceRef.push_back(&holes.back().p.f);
					}
					p0.SetNull();
				}
			}while( curPos != h->p );

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



private:

	/* Compute distance between bridge side to allow no bridge adjacent hole border	
	 *
	 *                              \ / B \ /                           \ / B \ /		|
	 *                               +-----+---                           +-----+---		|	  ---+------+------		   ---+------+--------
	 *			/|                                      /| f1  /|		|		  \ A  /		        \ A  / |\
	 *                          /   |                                     / |    /  |		|		   \  /			         \  /   |  \
	 *                            C |  hole                           C |  /    | hole             |	    hole   \/    hole             hole    \/    |    \  hole
	 *                            \  |                                   \  | /     |		|		    /\	                               /\f0|     \
	 *                              \|                                     \|/ f0 |		|		   /  \	                              /  \   | f1  \
	 *                               +-----+---		 	 +-----+---		|	 	  / B  \                            / B  \  |        \
	 *                              / \ A / \			/ \ A / \		|	  ---+------+------		---+------+------+---
	 *                                                                NO GOOD BRIDGE	|					   NO GOOD BRIDGE
	 */
	static bool testAbutmentDistance(const BridgeAbutment<MESH> &sideA, const BridgeAbutment<MESH> &sideB)
	{
		if(sideA.h != sideB.h) return true;

		if(!sideA.h->IsNonManifold())
		{
			// mi assicuro che il prossimo edge di bordo A partire in entrambi i sensi non
			// condivida vertici con l'edge di bordo B, questo mi garantisce che nei casi non manifold
			// c'è almeno 2 edge di bordo che dividono A e B
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
			// tra i 2 edge ci deve essere almeno 2 edge che li separino
			// pertanto cerco un edge che condivide un vertice con enrambi
			int dist=0;
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

	static void subdivideHoleWithBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB, MESH &mesh,
		HoleVector &holes, std::vector<FacePointer *> &app)
	{
		PosType newP0, newP1;
		build(mesh, sideA, sideB, newP0, newP1, app);
		
		newP0.f->SetUserBit(FgtHole<MESH>::BridgeFlag());
		newP1.f->SetUserBit(FgtHole<MESH>::BridgeFlag());
		
		sideA.h->SetStartPos(newP0);
		sideA.h->SetBridged(true);
		FgtHole<MESH> newHole(newP1, QString("Hole_%1").arg(holes.size(),3,10,QChar('0')) );
		if(sideA.h->IsSelected())
			newHole.SetSelect(true);
		sideA.h->SetBridged(true);
		holes.push_back( newHole );
	};

	static void unifyHolesWithBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB,  MESH &mesh,
		HoleVector &holes, std::vector<FacePointer *> &app)
	{
		assert(FgtHole<MESH>::IsHoleBorderFace(*sideA.f));
		assert(FgtHole<MESH>::IsHoleBorderFace(*sideB.f));

		PosType newP0, newP1;
		build(mesh, sideA, sideB, newP0, newP1, app);
		newP0.f->SetUserBit(FgtHole<MESH>::BridgeFlag());
		newP1.f->SetUserBit(FgtHole<MESH>::BridgeFlag());

		sideA.h->SetStartPos(newP0);
		if(sideB.h->IsSelected())
			sideA.h->SetSelect(true);
		sideA.h->SetBridged(true);

		typename HoleVector::iterator hit;
		for(hit=holes.begin(); hit!=holes.end(); ++hit)
			if(&*hit == sideB.h)
			{
				holes.erase(hit);
				break;
			}
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
	 *
	 * Return  the pos located into new 2 faces added over its border edge 
	*/
	static void build(MESH &mesh, BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB,
		PosType &pos0, PosType &pos1, std::vector<FacePointer *> &app)
	{	
		// prima faccia del bridge
		VertexType* vA0 = sideA.f->V0( sideA.z ); // first vertex of pos' 1-edge 
		VertexType* vA1 = sideA.f->V1( sideA.z ); // second vertex of pos' 1-edge
		VertexType* vB0 = sideB.f->V0( sideB.z ); // first vertex of pos' 2-edge 
		VertexType* vB1 = sideB.f->V1( sideB.z ); // second vertex of pos' 2-edge 

		// solution A 
		TriangleType bfA0(vA1->P(), vA0->P(), vB0->P());
		TriangleType bfA1(vB1->P(), vB0->P(), vA0->P());

		// solution B
		TriangleType bfB0(vA1->P(), vA0->P(), vB1->P());
		TriangleType bfB1(vB1->P(), vB0->P(), vA1->P());

		FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(mesh, 2, app);
		FacePointer f0 = &*fit;
		FacePointer f1 = &*(fit+1);

		sideA.f->ClearUserBit(FgtHole<MESH>::HoleFlag());
		sideB.f->ClearUserBit(FgtHole<MESH>::HoleFlag());
		f0->SetUserBit(FgtHole<MESH>::HoleFlag());
		f1->SetUserBit(FgtHole<MESH>::HoleFlag());

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

		//edges delle facce adiacenti alla mesh
		f0->FFp(0) = sideA.f;
		f0->FFi(0) = sideA.z;
		f1->FFp(0) = sideB.f;
		f1->FFi(0) = sideB.z;
		
		sideA.f->FFp(sideA.z) = f0;
		sideA.f->FFi(sideA.z) = 0;
		sideB.f->FFp(sideB.z) = f1;
		sideB.f->FFi(sideB.z) = 0;

		// edges adiacenti tra le 2 facce appena inserite
		f0->FFp(adjEdgeIndex) = f1;	
		f0->FFi(adjEdgeIndex) = adjEdgeIndex;
		f1->FFp(adjEdgeIndex) = f0;
		f1->FFi(adjEdgeIndex) = adjEdgeIndex;

		//edges laterali del ponte, quelli che restano di bordo
		f0->FFp(sideEdgeIndex) = f0; 
		f0->FFi(sideEdgeIndex) = sideEdgeIndex;
		f1->FFp(sideEdgeIndex) = f1;
		f1->FFi(sideEdgeIndex) = sideEdgeIndex;

		// Set the returned value
		pos0.f=f0; pos0.z=sideEdgeIndex; pos0.v=pos0.f->V(sideEdgeIndex);
		pos1.f=f1; pos1.z=sideEdgeIndex; pos1.v=pos1.f->V(sideEdgeIndex);
	};

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


	/*  Starting from an hole look for faces added to mesh as bridge jumping also into
	 *  adjacent hole from bridge finded
	 *  Put into bridgeFaces all faces added to mesh as bridge
	 */
	static void getBridgeInfo(HoleVector &holes, std::vector<FacePointer> &bridgeFaces, std::vector<PosType> &adjBridgePos)
	{
		//scorro gli hole
		PosType curPos;
		typename HoleVector::iterator hit;
		for(hit=holes.begin(); hit!=holes.end(); ++hit)
		{
			assert(!hit->IsFilled());
			if(!hit->IsBridged())
				continue;


			// il buco può essere bridged senza essere posizioneto con p su una faccia bridge
			// come nel caso della chiusura di vertici non manifold, un hole sarà marchiato
			// bridged ma non avrà sul suo bordo edge di facce bridge
			if(!FgtHole<MESH>::IsBridgeFace(*hit->p.f) && !hit->p.f->IsV())
			{
				hit->p.f->SetV();
				adjBridgePos.push_back(PosType(hit->p.f, hit->p.z));
			}


			// scorro il bordo, per ogni vertice guardo le facce a lui adicentie e se sono BRIDGE le
			// inserisco in lista
			curPos = hit->p;
			do{
				assert(curPos.IsBorder());
				FacePointer lastF;
				PosType cp2 = curPos;
				do{
					lastF = cp2.f;
					cp2.FlipE();
					cp2.FlipF();
					if(FgtHole<MESH>::IsBridgeFace(*cp2.f) && !cp2.f->IsV())
					{
						cp2.f->SetV();
						bridgeFaces.push_back(cp2.f);
					}
				}while(lastF != cp2.f);				

				curPos.NextB();				
			}while(curPos != hit->p);
		}//for(hit=holes.begin(); hit!=holes.end(); ++hit)

		// Ho preso le facce bridge adiacenti ai vertici degli hole
		// adesso gardo intorno a queste facce se ce ne sono altre, magari dovute
		// a bridge consecutivi o chiusure nonmanifold di buchi a 3 facce

		for(int k=0; k<bridgeFaces.size(); k++)
		{
			FacePointer vf = bridgeFaces.at(k);
			assert(vf->IsV());

			// per ogni faccia trovata cerco altre facce bridge tra quelle adiacenti
			// ai loro vertici. In più trovo gli half-edge che diventeranno di bordo una volta rimosse
			// le facce
			for(int e=0; e<3; e++)
			{
				curPos = PosType(vf, e);
				PosType lastPos;
				do{
					lastPos = curPos;
					curPos.FlipE();
					curPos.FlipF();
					if(FgtHole<MESH>::IsBridgeFace(*curPos.f) && !curPos.f->IsV())
					{
						curPos.f->SetV();
						bridgeFaces.push_back(curPos.f);
					}
				}while(curPos.f != lastPos.f && curPos.f != vf);

				FacePointer adjF = vf->FFp(e);
				if(!FgtHole<MESH>::IsBridgeFace(*adjF) && !adjF->IsV())
				{
					adjF->SetV();
					adjBridgePos.push_back(PosType(adjF, vf->FFi(e)));
				}
			}
		}

		// risetto i flag V sia per le facce bridge che quelle adiacenti
		typename std::vector<FacePointer>::iterator bfit;
		for(bfit=bridgeFaces.begin(); bfit!=bridgeFaces.end(); bfit++)
			(*bfit)->ClearV();

		typename std::vector<PosType>::iterator pit;
		for(pit=adjBridgePos.begin(); pit!=adjBridgePos.end(); pit++)
			pit->f->ClearV();
	};	
};
#endif
