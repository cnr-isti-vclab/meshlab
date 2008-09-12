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

	typedef typename FgtHole<MESH>					HoleType;
	typedef typename std::vector< FgtHole<MESH> >	HoleVector;
	
	typedef typename MESH::VertexType				VertexType;
	typedef typename MESH::CoordType				CoordType;
	typedef typename MESH::ScalarType				ScalarType;


/****	Static functions	******/
public:
	
	/*  Build a bridge between 2 border edge.
	 *  If the bridge is inside the same hole it cannot be adjacent the hole border, 
	 *  this means fill another sub hole.
	 */
	
	static bool CreateBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB,  MESH &mesh, 
		HoleVector &holes, std::vector<FacePointer *> &app)
	{
		assert(FgtHole<MESH>::IsHoleBorderFace(*sideA.f) && FgtHole<MESH>::IsHoleBorderFace(*sideB.f));
		assert(!sideA.h->IsFilled() && !sideB.h->IsFilled());

		if(sideA.h == sideB.h)
		{
			if( testAbutmentDistance(sideA, sideB))
				subdivideHoleWithBridge(sideA, sideB, mesh, holes, app);
			else
				return false;
		}
		else
			unifyHolesWithBridge(sideA, sideB, mesh, holes, app);

		return true;
	};

	
	static void RemoveBridges(MESH  &mesh, HoleVector &holes)
	{
		HoleType* h;

		// vettore usato per contenere tutte le facce di bridge trovate a partire da un hole
		std::vector<FacePointer> bridgeFaces;

		// vettore usato per contenere tutti gli half-edge adiacenti alle facce dei bridge che sono anche
		PosVector adjBorderPos;

		for(int i=0; i<holes.size(); i++ )
		{
			h = &holes.at(i);
			PosType curPos;
			curPos = h->p;
			if( FgtHole<MESH>::IsBridgeFace(*h->p.f) )
			{
				getBridgeInfo(*h, bridgeFaces, adjBorderPos );
				std::vector<FacePointer>::iterator fit;

				for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
				{
					// se la faccia bridge in esame si affaccia su un'altro hole rispetto 
					// a quello in esame rimuovo anche quell'hole
					if(FgtHole<MESH>::IsHoleBorderFace(**fit))
					{
						HoleVector::iterator hit;
						if(FgtHole<MESH>::FindHoleFromBorderFace(*fit, holes, hit) != -1)
						{
							if(&*hit != h)
							{
								assert(!hit->IsFilled());
								holes.erase(hit);
							}
						}
					}
				}

				for(fit=bridgeFaces.begin(); fit!=bridgeFaces.end(); fit++ )
				{
					// rimuovo le facce bridge ed aggiorno la topologia della faccia della 
					// mesh adiacente al bridge la faccia "dove poggia la spalla del ponte"
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
					
					if( !(*fit)->IsD() )
						vcg::tri::Allocator<MESH>::DeleteFace(mesh, **fit);
				}
				
				// ho staccato le facce bridge dalla mesh e rimosso gli hole adiacenti al bridge
				// adesso aggiorno l'hole ed re-inserisco eventuali hole che si sono formati 
				// dalla rimozione del bridge (il bridge legava più hole distinti)
				
				PosType initPos;
				PosVector::iterator it = adjBorderPos.begin();
				
				initPos = *it;
				curPos = initPos;
				do{
					curPos.f->SetV();
					curPos.NextB();
					assert(curPos.IsBorder());
				}while(curPos != initPos);
				h->SetStartPos(initPos);
		
				//guardo quali facce adiacenti ai bridge, salvate da adjBorderPos,
				// non fanno parte dell'hole in esame
				it++;
				for( ; it!=adjBorderPos.end(); it++)
				{
					assert( it->IsBorder() );
					if(!it->f->IsV())
					{
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
			}
		}		
	}

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
	static void AutoSelfBridging(MESH &mesh, HoleVector &holes, std::vector<FacePointer *> &app, double dist_coeff=0.0)
	{
		int nh = holes.size();
		for(int h=0; h<nh; ++h)
		{
			HoleType &thehole = holes.at(h);
			if(!thehole.IsSelected() || thehole.Size()<6 )
				continue;
			assert(!thehole.IsFilled());

			ScalarType maxQuality = -1;
			BridgeAbutment<MESH> sideA, sideB;

			// si scorre l'edge di partenza
			for(int i=0; i<thehole.Size();i++)
			{
				// posiziono il primo edge, la prima spalla del ponte 
				PosType initP = thehole.p;
				for(int j=0; j<i; j++)
					initP.NextB();
				
				// posiziono il secondo edge, la seconda spalla del ponte
				PosType endP = initP;
				endP.NextB();endP.NextB();					
				for(int j=3; j<=thehole.Size()/2; j++)
				{	
					endP.NextB();

					// mi assicuro di non scegliere di fare un ponte dove ne è già esistente uno
					if( endP.f->FFp(0) == initP.f ||
						endP.f->FFp(1) == initP.f ||
						endP.f->FFp(2) == initP.f )
						continue;

					VertexType* vA0 = initP.f->V0( initP.z ); // first vertex of pos' 1-edge 
					VertexType* vA1 = initP.f->V1( initP.z ); // second vertex of pos' 1-edge
					VertexType* vB0 = endP.f->V0( endP.z ); // first vertex of pos' 2-edge 
					VertexType* vB1 = endP.f->V1( endP.z ); // second vertex of pos' 2-edge 

					// solution A 
					Triangle3<ScalarType> bfA0(vA1->P(), vA0->P(), vB0->P());
					Triangle3<ScalarType> bfA1(vB1->P(), vB0->P(), vA0->P());

					// solution B
					Triangle3<ScalarType> bfB0(vA1->P(), vA0->P(), vB1->P());
					Triangle3<ScalarType> bfB1(vB1->P(), vB0->P(), vA1->P());

					ScalarType oldq = maxQuality;
					ScalarType q = bfA0.QualityFace()+ bfA1.QualityFace() + dist_coeff * j;
					if(  q > maxQuality)
						maxQuality = q;
					q = bfB0.QualityFace()+ bfB1.QualityFace() + dist_coeff * j;
					if(  q > maxQuality)
						maxQuality = q;

					if(oldq < maxQuality)
					{
						sideA = BridgeAbutment<MESH>(initP.f, initP.z, &thehole);
						sideB = BridgeAbutment<MESH>(endP.f, endP.z, &thehole);
					}					
				}// scansione del edge di arrivo
			}// scansione dell'edge di partenza

			assert(vcg::face::IsBorder<FaceType>(*sideA.f, sideA.z));
			assert(vcg::face::IsBorder<FaceType>(*sideB.f, sideB.z));

			app.push_back(&sideA.f);
			app.push_back(&sideB.f);
			subdivideHoleWithBridge(sideA, sideB, mesh, holes, app);
			app.pop_back(); app.pop_back();
			app.push_back(&holes.back().p.f);
		} //scansione degli holes
	};


	static void AutoMultiBridging(MESH &mesh, HoleVector &holes, std::vector<FacePointer *> &app, double dist_coeff=0.0)
	{
		BridgeAbutment<MESH> sideA, sideB;
		std::vector<HoleType*> selectedHoles;
		std::vector<HoleType*>::iterator shit1, shit2; 
		HoleVector::iterator hit;
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
							Triangle3<ScalarType> bfA0(vA1->P(), vA0->P(), vB0->P());
							Triangle3<ScalarType> bfA1(vB1->P(), vB0->P(), vA0->P());

							// solution B
							Triangle3<ScalarType> bfB0(vA1->P(), vA0->P(), vB1->P());
							Triangle3<ScalarType> bfB1(vB1->P(), vB0->P(), vA1->P());

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
			app.push_back(&sideA.f);
			app.push_back(&sideB.f);
			unifyHolesWithBridge(sideA, sideB, mesh, holes, app);

		}while(true);
	};


private:

	/* Compute distance between bridge side to allow no bridge adjacent hole border	
	 *
	 *			\ / B \	/			\ / B \ /
	 *			 +-----+---			 +-----+---
	 *			/|					/| f1 /|			f1 is adjacent the hole border
	 *		   / |				   / |   / |			it's uniteresting bridge
	 *		   C |  hole		   C |  /  | hole
	 *		   \ |				   \ | /   |
	 *			\|					\|/ f0 |
	 *			 +-----+---		 	 +-----+---
	 *			/ \ A /	\			/ \ A / \
	 */
	static bool testAbutmentDistance(const BridgeAbutment<MESH> &sideA, const BridgeAbutment<MESH> &sideB)
	{
		if(sideA.h != sideB.h) return true;

		// test if face condivide one vertex
		for(int i=0; i<3; i++)
			for(int j=0; j<3; j++)
				if(sideA.f->V(i) == sideB.f->V(j))
					return false;
		
		// testo se tra gli edge su cui si vorrebbe creare il ponte c'e' un solo bridge che li divide

		FacePointer adjf1, adjf2;

		// test if adjacent borderface to sideA face condivide one vertex with sideB face
		PosType pos(sideB.f, sideB.z);

		pos.NextB();
		adjf1 = pos.f;

		// riposizionato sulla faccia di partenza
		pos.FlipV();
		pos.NextB();

		pos.NextB();
		adjf2 = pos.f;	// faccia di bordo adiacente dall'altra parte rispetto a adjf1
		
		for(int i=0; i<3; i++)
			for(int j=0; j<3; j++)
				if(sideA.f->V(i) == adjf1->V(j) || sideA.f->V(i) == adjf2->V(j))
					return false;

		// testo anche sulle facce di bordo adiacenti a sideA per riconoscere i casi in cui
		// un vertice di bordo di sideB è non manifold
		pos = PosType(sideA.f, sideA.z);

		pos.NextB();
		adjf1 = pos.f;

		// riposizionato sulla faccia di partenza
		pos.FlipV();
		pos.NextB();

		pos.NextB();
		adjf2 = pos.f;	// faccia di bordo adiacente dall'altra parte rispetto a adjf1
		
		for(int i=0; i<3; i++)
			for(int j=0; j<3; j++)
				if(sideB.f->V(i) == adjf1->V(j) || sideB.f->V(i) == adjf2->V(j))
					return false;

		// BUG: se i vertici dalla stessa parte di entrambe le spalle del ponte
		// sono non manifold viene ritenuta buono anche se c'è un solo edge che li divide

		return true;
	};

	static void subdivideHoleWithBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB, MESH &mesh,
		HoleVector &holes, std::vector<FacePointer *> &app)
	{
		PosType newP0, newP1;
		build(mesh, sideA, sideB, newP0, newP1, app);
		
		newP0.f->SetUserBit(FgtHole<MESH>::BridgeFlag);
		newP1.f->SetUserBit(FgtHole<MESH>::BridgeFlag);
		
		sideA.h->SetStartPos(newP0);
		FgtHole<MESH> newHole(newP1, QString("Hole_%1").arg(holes.size(),3,10,QChar('0')) );
		if(sideA.h->IsSelected())
			newHole.SetSelect(true);
		holes.push_back( newHole );
	};

	static void unifyHolesWithBridge(BridgeAbutment<MESH> &sideA, BridgeAbutment<MESH> &sideB,  MESH &mesh,
		HoleVector &holes, std::vector<FacePointer *> &app)
	{
		assert(FgtHole<MESH>::IsHoleBorderFace(*sideA.f));
		assert(FgtHole<MESH>::IsHoleBorderFace(*sideB.f));

		PosType newP0, newP1;
		build(mesh, sideA, sideB, newP0, newP1, app);
		newP0.f->SetUserBit(FgtHole<MESH>::BridgeFlag);
		newP1.f->SetUserBit(FgtHole<MESH>::BridgeFlag);

		sideA.h->SetStartPos(newP0);
		if(sideB.h->IsSelected())
			sideA.h->SetSelect(true);

		HoleVector::iterator hit;
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
		Triangle3<ScalarType> bfA0(vA1->P(), vA0->P(), vB0->P());
		Triangle3<ScalarType> bfA1(vB1->P(), vB0->P(), vA0->P());

		// solution B
		Triangle3<ScalarType> bfB0(vA1->P(), vA0->P(), vB1->P());
		Triangle3<ScalarType> bfB1(vB1->P(), vB0->P(), vA1->P());

		FaceIterator fit = vcg::tri::Allocator<MESH>::AddFaces(mesh, 2, app);
		FacePointer f0 = &*fit;
		FacePointer f1 = &*(fit+1);

		sideA.f->ClearUserBit(FgtHole<MESH>::HoleFlag);
		sideB.f->ClearUserBit(FgtHole<MESH>::HoleFlag);
		f0->SetUserBit(FgtHole<MESH>::HoleFlag);
		f1->SetUserBit(FgtHole<MESH>::HoleFlag);

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


	/*  Starting from an hole look for faces added to mesh as bridge jumping also into
	 *  adjacent hole from bridge finded
	 *  Put into bridgeFaces all faces added to mesh as bridge
	 *  Put into adjBorderPos the border half-edge next to the border side of a bridge
	 */
	static void getBridgeInfo(HoleType &h, std::vector<FacePointer> &bridgeFaces, 
		std::vector<PosType> &adjBorderPos)
	{
		assert(!h.IsFilled());
		assert(FgtHole<MESH>::IsHoleBorderFace(*h.p.f));
		PosType curPos;
		bridgeFaces.clear();
		adjBorderPos.clear();

		bridgeFaces.push_back(h.p.f);
		h.p.f->SetV();

		// in tmp ci sono le facce aggiunte da dei bridge
		for(int k=0; k<bridgeFaces.size(); k++)
		{
			/*guardo le facce adiacenti le stacco dalla mesh e
			   - se sono Bridge e Border: cerco il loro hole e lo elimino dalla lista
				 questo buco sarà inglobato da quello inziale 
			   - se sono solo bridge (conseguenza di bridge da facce bridge) le rimuovo semplicemente
			   - altrimenti non faccio niente
			*/
			FacePointer vf = bridgeFaces.at(k);
			assert(vf->IsV());

			// indice dell'edge della faccia che è adicente alla mesh
			for(int e=0; e<3; e++)
			{
				FacePointer adjF = vf->FFp(e);						
				if(adjF == vf) // e è l'indice di un edge di bordo
				{
					curPos = PosType(vf, e);
					curPos.NextB();
					
					bool adjPosAdded = false;

					// scorro il bordo in cerca di altre possibili facce bridge
					// e mi tengo un riferimento alla nuova posizione iniziale che avrà l'hole
					do{
						if(!adjPosAdded && !FgtHole<MESH>::IsBridgeFace(*curPos.f) )
						{
							// aggiungo l'haf-edge adiacente al bridge assicurandomi che non sia
							// una faccia bridge e quindi valida anche dopo l'eliminazione delle
							// facce bridge
							adjBorderPos.push_back(curPos);
							adjPosAdded = true;
						}
					
						if( FgtHole<MESH>::IsBridgeFace(*curPos.f) && !curPos.f->IsV())
						{
							curPos.f->SetV();
							bridgeFaces.push_back(curPos.f);
						}
						curPos.NextB();
					}while(curPos.f != vf);

				}
				else if(FgtHole<MESH>::IsBridgeFace(*adjF) )
				{
					if(!adjF->IsV())
					{
						adjF->SetV();
						bridgeFaces.push_back(adjF);
					}
				}										
			}
		} //for(int k=0; k<bridgeFaces.size(); k++)
	};


};

#endif