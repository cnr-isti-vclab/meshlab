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


/** Class entirely static, it offers functions to manage bridges between
 *  different FgtHoles or into the same one.
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
 *             |                   |			|      |\ f0 |      |
 *             |                   |			|      | \   |      |
 *             |       hole        |      | hole |  \  | hole |
 *             |                   |      |      |   \ |      |
 *             |                   |      |      | f1 \|      |
 *             +------+-----+------+      +------+-----+------+
 *                   / \ A / \                  / \ A / \      
 *                                             GOOD BRIDGE
 *                             f0 and f1 are adjacent only with one mesh face
 *
 *
 *   \ / B \ /       \ / B \ /           |
 *    +-----+---      +-----+---         |       ---+------+------      ---+------+--------
 *   /|              /| f1 /|            |           \ A  /                 \ A  /|\
 *  / |             / |   / |            |            \  /                   \  / | \
 *  C |  hole       C |  /  | hole       |        hole \/    hole     hole    \/  |  \  hole
 *  \ |             \ | /   |            |             /\                     /\f0|   \
 *   \|              \|/ f0 |            |            /  \                   /  \ | f1 \
 *    +-----+---      +-----+---         |           / B  \                 / B  \|     \
 *   / \ A / \       / \ A / \           |       ---+------+------      ---+------+------+---
 *                  NO GOOD BRIDGE	     |                                 NO GOOD BRIDGE
 *               f1 adjacent to B and C                                f0 adjacent to A and B
 */
template <class MESH>
class FgtBridge
{
public:
	typedef typename MESH::FaceType							FaceType;
	typedef typename MESH::FacePointer					FacePointer;
	typedef typename MESH::FaceIterator					FaceIterator;
	typedef typename vcg::face::Pos<FaceType>		PosType;
	typedef typename std::vector<PosType>				PosVector;
	typedef FgtHole<MESH>												HoleType;
	typedef typename std::vector<HoleType>			HoleVector;
	typedef typename MESH::VertexType						VertexType;
	typedef typename MESH::ScalarType						ScalarType;
	

/****	Static functions	******/
public:
	
	/*  Build a bridge between 2 border edge.
	 *  If the bridge is inside the same hole it cannot be adjacent the hole border, 
	 *  this means fill another sub hole.
	 */
	static bool CreateBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB, 
		HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> *app=0)
	{
		assert( holesManager->IsHoleBorderFace(sideA.f) && 
						holesManager->IsHoleBorderFace(sideB.f));
		assert(!sideA.h->IsFilled() && !sideB.h->IsFilled());

		std::vector<FacePointer *> tmpFaceRef;
		if(app!=0)
			tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
		FgtHole<MESH>::AddFaceReference(holesManager->holes, tmpFaceRef);

		if(sideA.h == sideB.h)
		{
			if( testAbutmentDistance(sideA, sideB))
				return subdivideHoleWithBridge(sideA, sideB, holesManager, tmpFaceRef);
			else
				return false;
		}
		else
			return unifyHolesWithBridge(sideA, sideB, holesManager, tmpFaceRef);
	};

	// clear flags of faces marked as bridge, bridge faces become mesh faces
	static void AcceptBridges(HoleSetManager<MESH>* holeManager)
	{
		// contains bridge faces reached navigating the holes.
		std::vector<FacePointer> bridgeFaces;

		// contains all half-edge located over non-bridge face and over edge shared with bridge face.
		// these half-edges will become border edge when bridge faces are removed.
		PosVector adjBorderPos;

		PosType curPos;
		getBridgeInfo(holeManager, bridgeFaces, adjBorderPos);
		typename std::vector<FacePointer>::iterator fit;
		for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
		{
			holeManager->ClearPatchAttr(*fit);
			holeManager->ClearCompAttr(*fit);
			holeManager->ClearBridgeAttr(*fit);
		}
		
		typename HoleVector::iterator it = holeManager->holes.begin();
		for( ; it!=holeManager->holes.end(); it++ )
			it->SetBridged(false);
	};

	/* Remove all face marked as bridge. */
	static void RemoveBridges(HoleSetManager<MESH> *holeManager)
	{
		// contains bridge faces reached navigating the holes.
		std::vector<FacePointer> bridgeFaces;

		// contains all half-edge located over non-bridge face and over edge shared with bridge face.
		// these half-edges will become border edge when bridge faces are removed.
		PosVector adjBorderPos;

		PosType curPos;
		getBridgeInfo(holeManager, bridgeFaces, adjBorderPos);

		// remove all holes which have a bridge face on its border and
		// remove all bridge face
		typename std::vector<FacePointer>::iterator fit;
		for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
		{
			if(holeManager->IsHoleBorderFace(*fit))
			{
				typename HoleVector::iterator hit;
				if(holeManager->FindHoleFromFace(*fit, hit) != -1)
				{
					assert(!hit->IsFilled());
					holeManager->holes.erase(hit);
				}
			}
			if( !(*fit)->IsD() )
				vcg::tri::Allocator<MESH>::DeleteFace(*holeManager->mesh, **fit);
		}

		// remove also hole which have on its border faces finded from half-edge adjacent to bridge face
		// these holes will be reinsert at the end
		typename std::vector<PosType>::iterator pit;
		for(pit=adjBorderPos.begin(); pit!=adjBorderPos.end(); pit++)
			if(holeManager->IsHoleBorderFace(pit->f))
			{
				typename HoleVector::iterator hit;
				if(holeManager->FindHoleFromFace(pit->f, hit) != -1)
				{
					assert(!hit->IsFilled());
					holeManager->holes.erase(hit);
				}
			}

		// update mesh topology after bridge faces removal, restore border
		for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
		{
			for(int e=0; e<3; e++)
			{	
				if(!IsBorder(**fit, e))
				{
					FacePointer adjF = (*fit)->FFp(e);
					if(!holeManager->IsBridgeFace(adjF))
					{								
						int adjEI = (*fit)->FFi(e);
						adjF->FFp( adjEI ) = adjF;
						adjF->FFi( adjEI ) = adjEI;
						assert(IsBorder(*adjF, adjEI));
					}
				}
			}
		}
				
		// update hole list inserting holes touched by bridge
		// use adjBorderPos element as start pos to walk over the border, if walking doesn't
		// visit some adjBorderPos element means this belongo to other hole.
		PosType initPos;
		typename PosVector::iterator it;
		for( it=adjBorderPos.begin(); it!=adjBorderPos.end(); it++)
		{
			assert( it->IsBorder() );
			bool sel=it->f->IsS();
			it->f->ClearS();

			if(it->f->IsV())
				continue;
			
			curPos = initPos = *it;
			do{
				curPos.f->SetV();
				sel = sel || curPos.f->IsS();
				curPos.f->ClearS();
				curPos.NextB();
				assert(curPos.IsBorder());
			}while(curPos != initPos);
			
			FgtHole<MESH> newHole(initPos, QString("Hole_%1").arg(HoleType::GetHoleId(),3,10,QChar('0')), holeManager);
			newHole.SetSelect(sel);
			holeManager->holes.push_back( newHole );
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

	

	/*  Build a bridge inner to the same hole. It chooses the best bridge computing quality 
	 *  of 2 faces and similarity (as number of edge) of two next hole. Bridge is build follow
	 *  bridge's rule, bridge must have 2 border edge.
	 *  infoLabel paramter is used to show work progress.
	 *  Return number of bridge builded.
	 */
	 static int AutoSelfBridging(HoleSetManager<MESH>* holesManager, double dist_coeff=0.0, std::vector<FacePointer *> *app=0)
	{
		time_t timer;
		if(holesManager->autoBridgeCB != 0)
		{
			holesManager->autoBridgeCB->Invoke(0);
			timer = clock();
		}
		int nb = 0;
		vcg::GridStaticPtr<FaceType, ScalarType > gM;
		gM.Set(holesManager->mesh->face.begin(),holesManager->mesh->face.end());
		
		std::vector<FacePointer *> tmpFaceRef;
		HoleType* oldRef = 0;
		BridgeAbutment<MESH> sideA, sideB;

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

					ScalarType oldq = maxQuality;

					VertexType* vA0 = initP.f->V0( initP.z ); // first vertex of pos' 1-edge 
					VertexType* vA1 = initP.f->V1( initP.z ); // second vertex of pos' 1-edge
					VertexType* vB0 = endP.f->V0( endP.z ); // first vertex of pos' 2-edge 
					VertexType* vB1 = endP.f->V1( endP.z ); // second vertex of pos' 2-edge 

					// solution A 
					FaceType bfA0;
					bfA0.V(0) = vA1; bfA0.V(1) = vA0;	bfA0.V(2) = vB0;

					FaceType bfA1;
					bfA1.V(0) = vB1; bfA1.V(1) = vB0; bfA1.V(2) = vA0;
					
					if( !HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfA0) &&
							!HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfA1) )
					{
						ScalarType Aq = QualityFace(bfA0)+ QualityFace(bfA1) + dist_coeff * j;
						if(  Aq > maxQuality)
							maxQuality = Aq;
					}

					// solution B
					FaceType bfB0;
					bfB0.V(0) = vA1; bfB0.V(1) = vA0;	bfB0.V(2) = vB1;

					FaceType bfB1;
					bfB1.V(0) = vB1; bfB1.V(1) = vB0; bfB1.V(2) = vA1;
					
					if( !HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfB0) &&
							!HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfB1) )
					{						
						ScalarType Bq = QualityFace(bfB0)+ QualityFace(bfB1) + dist_coeff * j;
						if(  Bq > maxQuality)
							maxQuality = Bq;
					}
					
					if(oldq < maxQuality)
					{
						sideA.f=initP.f; sideA.z=initP.z; sideA.h=&thehole;
						sideB.f=endP.f; sideB.z=endP.z; sideB.h=&thehole;
					}

					if(holesManager->autoBridgeCB != 0)
					{
						if(clock()- timer > holesManager->autoBridgeCB->GetOffset())
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

			if( oldRef != &*holesManager->holes.begin() )
			{
				// si può 
				tmpFaceRef.clear();
				if(app!=0)
					tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
				FgtHole<MESH>::AddFaceReference(holesManager->holes, tmpFaceRef);
				tmpFaceRef.push_back(&sideA.f);
				tmpFaceRef.push_back(&sideB.f);
				oldRef = &*holesManager->holes.begin();				
			}

			if(subdivideHoleWithBridge(sideA, sideB, holesManager, tmpFaceRef) )
			{
				nb++;
				gM.Set(holesManager->mesh->face.begin(), holesManager->mesh->face.end());
				// la subdivideHole.. aggiunge un hole pertanto bisogna aggiornare anche la lista di 
				// reference a facce
				tmpFaceRef.push_back(&holesManager->holes.back().p.f);
			}
			
		} //scansione degli holes
		return nb;
	};


	/*  It connects iteratively selected holes with the best bridge. 
	 *  Result is unique hole instead of init holes.
	 *  Return number of bridges builded.
	 */
	static int AutoMultiBridging(HoleSetManager<MESH>* holesManager, double dist_coeff=0.0, std::vector<FacePointer *> *app=0 )
	{
		time_t timer;
		if(holesManager->autoBridgeCB != 0)
		{
			holesManager->autoBridgeCB->Invoke(0);
			timer = clock();
		}

		int nb = 0;
		vcg::GridStaticPtr<FaceType, ScalarType > gM;
		
		std::vector<FacePointer *> tmpFaceRef;
		BridgeAbutment<MESH> sideA, sideB;
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
				return nb;
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
					PosType ph1=(*shit1)->p;
					PosType ph2=(*shit2)->p;
					do{	//scansione edge di bordo del primo buco
						do{
							VertexType* vA0 = ph1.f->V0( ph1.z ); // first vertex of pos' 1-edge 
							VertexType* vA1 = ph1.f->V1( ph1.z ); // second vertex of pos' 1-edge
							VertexType* vB0 = ph2.f->V0( ph2.z ); // first vertex of pos' 2-edge 
							VertexType* vB1 = ph2.f->V1( ph2.z ); // second vertex of pos' 2-edge 

							ScalarType oldq = maxQuality;

							// solution A 
							FaceType bfA0;
							bfA0.V(0) = vA1; bfA0.V(1) = vA0;	bfA0.V(2) = vB0;

							FaceType bfA1;
							bfA1.V(0) = vB1; bfA1.V(1) = vB0; bfA1.V(2) = vA0;
							
							if( !HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfA0) &&
									!HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfA1) )
							{
								ScalarType Aq = QualityFace(bfA0)+ QualityFace(bfA1);
								if(  Aq > maxQuality)
									maxQuality = Aq;
							}

							// solution B
							FaceType bfB0;
							bfB0.V(0) = vA1; bfB0.V(1) = vA0;	bfB0.V(2) = vB1;

							FaceType bfB1;
							bfB1.V(0) = vB1; bfB1.V(1) = vB0; bfB1.V(2) = vA1;
							
							if( !HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfB0) &&
									!HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfB1) )
							{						
								ScalarType Bq = QualityFace(bfB0)+ QualityFace(bfB1);
								if(  Bq > maxQuality)
									maxQuality = Bq;
							}

							if(oldq < maxQuality)
							{
								sideA = BridgeAbutment<MESH>(ph1.f, ph1.z, *shit1);
								sideB = BridgeAbutment<MESH>(ph2.f, ph2.z, *shit2);
							}

							if(holesManager->autoBridgeCB != 0)
							{
								if(clock() - timer > holesManager->autoBridgeCB->GetOffset())
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
			FgtHole<MESH>::AddFaceReference(holesManager->holes, tmpFaceRef);
			tmpFaceRef.push_back(&sideA.f);
			tmpFaceRef.push_back(&sideB.f);
			
			if(unifyHolesWithBridge(sideA, sideB, holesManager, tmpFaceRef))
				nb++;
			
			iteration ++;
		}while(true);
		return nb;
	};



	/* Walk over selected non-manifold holes, find vertex visited more times,
	 * add face adjacent to non-manifold vertex.
	 * Return number of faces added.
	 */
	static int CloseNonManifoldVertex(HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> *app=0)
	{
		int startNholes = holesManager->holes.size();
		int nf = 0;

		std::vector<FacePointer *> tmpFaceRef;
		HoleType* oldRef = 0;	

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
					if( oldRef != &*holesManager->holes.begin() )
					{
						// holes vector is been reallocated... tmpFaceRes must be recomputed
						tmpFaceRef.clear();
						if(app!=0)
							tmpFaceRef.insert(tmpFaceRef.end(), app->begin(), app->end());
						FgtHole<MESH>::AddFaceReference(holesManager->holes, tmpFaceRef);
						oldRef = &*holesManager->holes.begin();
					}

					tmpFaceRef.push_back(&p0.f);
					tmpFaceRef.push_back(&curPos.f);
					FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(*holesManager->mesh, 1, tmpFaceRef);
					holesManager->faceAttr->UpdateSize();
					nf++;
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
						tmpFaceRef.push_back(&holesManager->holes.back().p.f);
						
						// adding hole can reallocate hole vector so h must be updated
						if( oldRef != &*holesManager->holes.begin() )
							h = &holesManager->holes.at(i);
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
		return nf;		
	};


private:

	/*  Compute distance between bridge side to allow no bridge adjacent to hole border. 
	 *  A bridge must have 2 border faces.
	 */
	static bool testAbutmentDistance(const BridgeAbutment<MESH> &sideA, const BridgeAbutment<MESH> &sideB)
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

	static bool subdivideHoleWithBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB, 
		HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> &app)
	{
		PosType newP0, newP1;
		if( !build(holesManager, sideA, sideB, newP0, newP1, app) )
			return false;
		
		holesManager->SetBridgeAttr(newP0.f);
		holesManager->SetBridgeAttr(newP1.f);
		
		sideA.h->SetStartPos(newP0);
		sideA.h->SetBridged(true);
		FgtHole<MESH> newHole(newP1, QString("Hole_%1").arg(HoleType::GetHoleId(),3,10,QChar('0')), holesManager);
		if(sideA.h->IsSelected())
			newHole.SetSelect(true);
		newHole.SetBridged(true);
		holesManager->holes.push_back( newHole );
		return true;
	};

	static bool unifyHolesWithBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB, 
		HoleSetManager<MESH>* holesManager, std::vector<FacePointer *> &app)
	{
		assert(holesManager->IsHoleBorderFace(sideA.f));
		assert(holesManager->IsHoleBorderFace(sideB.f));

		PosType newP0, newP1;
		if( !build(holesManager, sideA, sideB, newP0, newP1, app) )
			return false;

		holesManager->SetBridgeAttr(newP0.f);
		holesManager->SetBridgeAttr(newP1.f);

		sideA.h->SetStartPos(newP0);
		if(sideB.h->IsSelected())
			sideA.h->SetSelect(true);
		sideA.h->SetBridged(true);

		typename HoleVector::iterator hit;
		for(hit=holesManager->holes.begin(); hit!=holesManager->holes.end(); ++hit)
			if(&*hit == sideB.h)
			{
				holesManager->holes.erase(hit);
				break;
			}
		return true;
	};


	/**
	 *  Insert faces rapresenting the bridge into mesh
	 *  app is the vector of pointer at faces, it is needed because ading faces can resize 
	 *  and so reallocate the faces' vector
	 *
	 *	Connect 2 different face adding 2 face between its edge.
	 *					 
	 *	 /|    |\            /|¯¯¯/|\        /|\¯¯¯|\
	 *	/ |    | \    --\   / |	 / | \  or  / |	\  | \
	 *	\ |    | /    --/   \ |	/  | /      \ |  \ | /
	 *	 \|    |/	           \|/___|/        \|___\|/
	 *
	 * Return  the pos located into new 2 faces added over its border edge 
	*/
	static bool build(HoleSetManager<MESH> *holesManager, BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB,
		PosType &pos0, PosType &pos1, std::vector<FacePointer *> &app)
	{	
		vcg::GridStaticPtr<FaceType, ScalarType > gM;
		gM.Set(holesManager->mesh->face.begin(),holesManager->mesh->face.end());
		
		VertexType* vA0 = sideA.f->V0( sideA.z ); // first vertex of pos' 1-edge 
		VertexType* vA1 = sideA.f->V1( sideA.z ); // second vertex of pos' 1-edge
		VertexType* vB0 = sideB.f->V0( sideB.z ); // first vertex of pos' 2-edge 
		VertexType* vB1 = sideB.f->V1( sideB.z ); // second vertex of pos' 2-edge 

		// case A 
		FaceType bfA0;
		bfA0.V(0) = vA1; bfA0.V(1) = vA0;	bfA0.V(2) = vB0;

		FaceType bfA1;
		bfA1.V(0) = vB1; bfA1.V(1) = vB0; bfA1.V(2) = vA0;
		
		ScalarType Aq;
		if( HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfA0) ||
			  HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfA1) )
			Aq = -1;
		else
			Aq = QualityFace(bfA0)+ QualityFace(bfA1);
		
		// case B
		FaceType bfB0;
		bfB0.V(0) = vA1; bfB0.V(1) = vA0;	bfB0.V(2) = vB1;

		FaceType bfB1;
		bfB1.V(0) = vB1; bfB1.V(1) = vB0; bfB1.V(2) = vA1;

		ScalarType Bq;
		if( HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfB0) ||
			  HoleType::TestFaceMeshCompenetration(*holesManager->mesh, gM, &bfB1) )
			Bq = -1;
		else
			Bq = QualityFace(bfB0)+ QualityFace(bfB1);

		// both solution are compentrating with mesh
		if(Aq == -1 && Bq == -1)
			return false;

		FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(*holesManager->mesh, 2, app);
		holesManager->faceAttr->UpdateSize();
		FacePointer f0 = &*fit;
		FacePointer f1 = &*(fit+1);

		holesManager->ClearHoleBorderAttr(sideA.f);
		holesManager->ClearHoleBorderAttr(sideB.f);
		holesManager->SetHoleBorderAttr(f0);
		holesManager->SetHoleBorderAttr(f1);

		// the index of edge adjacent between new 2 face, is the same for both new faces
		int adjEdgeIndex = -1;		
		
		// the index of edge adjacent between new 2 face, is the same for both new faces
		int sideEdgeIndex = -1;

		if( Aq > Bq )
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

		// Set the returned value
		pos0.f=f0; pos0.z=sideEdgeIndex; pos0.v=pos0.f->V(sideEdgeIndex);
		pos1.f=f1; pos1.z=sideEdgeIndex; pos1.v=pos1.f->V(sideEdgeIndex);
		return true;
	};

	/*  Starting from an hole look for faces added to mesh as bridge jumping also into
	 *  holes finded crossing bridges. If bridges'll be removed these holes could belong to the same hole.
	 *  Put into bridgeFaces all faces added to mesh as bridge
	 *  Put into adjBridgePos half-edges located over the edge which will become border edge 
	 *  after bridge face removing also mark with flag S the faces related to adjBridgePos
	 */
	static void getBridgeInfo(HoleSetManager<MESH> *holesManager, std::vector<FacePointer> &bridgeFaces, std::vector<PosType> &adjBridgePos)
	{
		//scorro gli hole
		PosType curPos;
		typename HoleVector::iterator hit;
		bridgeFaces.clear();
		adjBridgePos.clear();


		for(hit=holesManager->holes.begin(); hit!=holesManager->holes.end(); ++hit)
		{
			assert(!hit->IsFilled());
			if(!hit->IsBridged())
				continue;

			// an hole could be "bridged" but haven't a boorder face which is also bridged. 
			// This happens after non-manifold closure, it adds only a face to a side of non.manifold vertex,
			// so a resultant "sub-hole" haven't bridged+border face.
			// To know this hole is connected to other hole from bridge, we add its half-edge on adjBorderPos.
			if(!holesManager->IsBridgeFace(hit->p.f) )
			{
				if(hit->IsSelected())
					hit->p.f->SetS();
				if(!hit->p.f->IsV())
				{
					hit->p.f->SetV();
					adjBridgePos.push_back(PosType(hit->p.f, hit->p.z));
				}
			}

			// walk over the hole border, for each vertex scan its vertex-adjacent faces looking for Bridge faces.
			curPos = hit->p;
			do{
				assert(curPos.IsBorder());
				FacePointer lastF;
				PosType cp2 = curPos;
				do{
					lastF = cp2.f;
					cp2.FlipE();
					cp2.FlipF();
					if(holesManager->IsBridgeFace(cp2.f) )
					{
						if(hit->IsSelected()) 
							cp2.f->SetS();
						if(!cp2.f->IsV())
						{
							cp2.f->SetV();
							bridgeFaces.push_back(cp2.f);
						}
					}
				}while(lastF != cp2.f);				

				curPos.NextB();				
			}while(curPos != hit->p);
		}//for(hit=holes.begin(); hit!=holes.end(); ++hit)

		// in bridgesFaces there are all bridge faces adjacent to each vertex of holes already scanned
		// now inspect these faces, their adjacent (to vertex) faces could be bridge face but not already seen.
		// Eg. consecutive bridge, non-manifold closure...
		for(int k=0; k<bridgeFaces.size(); k++)
		{
			FacePointer vf = bridgeFaces.at(k);
			assert(vf->IsV());

			// for each faces found look for other bridge faces walking on its adjacent faces.
			// Also look for half-edges between patch and border face, they'll be on border face.
			for(int e=0; e<3; e++)
			{
				// look for other bridge faces
				curPos = PosType(vf, e);
				PosType lastPos;
				do{
					lastPos = curPos;
					curPos.FlipE();
					curPos.FlipF();
					if(holesManager->IsBridgeFace(curPos.f))
					{
						if(vf->IsS()) 
							curPos.f->SetS();
						if(!curPos.f->IsV())
						{
							curPos.f->SetV();
							bridgeFaces.push_back(curPos.f);
						}
					}
				}while(curPos.f != lastPos.f && curPos.f != vf);

				// look for half-edge
				FacePointer adjF = vf->FFp(e);
				if(!holesManager->IsBridgeFace(adjF) && !adjF->IsV())
				{
					adjF->SetV();
					if(vf->IsS()) adjF->SetS();
					else adjF->ClearS();
					adjBridgePos.push_back(PosType(adjF, vf->FFi(e)));
				}
			}
		} // holes scan

		// restting of flag V
		typename std::vector<FacePointer>::iterator bfit;
		for(bfit=bridgeFaces.begin(); bfit!=bridgeFaces.end(); bfit++)
			(*bfit)->ClearV();

		typename std::vector<PosType>::iterator pit;
		for(pit=adjBridgePos.begin(); pit!=adjBridgePos.end(); pit++)
			pit->f->ClearV();
	};	
};
#endif
