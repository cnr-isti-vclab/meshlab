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
#include "holePatch.h"

using namespace std;

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
	typedef typename vcg::face::Pos<FaceType> PosType;
	typedef typename pair< PosType, PosType > PosPatch;

	FgtHole( PosType &firstPos)	
	{		
		startPos = firstPos;
		isSelected = false;
	};

	void Draw() const
	{
		PosType curPos;
		glBegin(GL_LINE_LOOP);

		// scorro gli edge del buco
		list<PosPatch>::const_iterator it = posPatches.begin();
		curPos = startPos;
		do
		{
			glVertex( curPos.v->P() );
			if( it!= posPatches.end() )
				if( curPos == (*it).first)
				{
					curPos = (*it).second;
					it++;
					continue;
				}
			curPos.NextB();
		}while(curPos != startPos);
	   glEnd();
	};

	/** Walking the hole inserting each pos found into a vector */
	void GetPosVector(vector<PosType> &posVector) const
	{
		// scorro gli edge del buco
		PosType curPos;	
		list<PosPatch>::const_iterator it = posPatches.begin();
		curPos = startPos;
		do
		{
			posVector.push_back( curPos );
			if( it!= posPatches.end() )
				if( curPos == (*it).first)
				{
					curPos = (*it).second;
					it++;
					continue;
				}
			curPos.NextB();
		}while(curPos != startPos);
	}

public:
	PosType startPos;
	list<PosPatch> posPatches; 
	bool isSelected;
};


template <class MESH>
class HoleFinder
{
public:
	typedef typename MESH::FaceType FaceType;
    typedef typename MESH::FacePointer FacePointer;
	typedef typename MESH::FaceIterator FaceIterator;
	typedef typename vcg::face::Pos<FaceType> PosType;
    typedef typename pair<PosType, PosType> PosPatch;
	typedef typename vector<FgtHole<MESH> > HoleVector;
	
 
	/** This mesthod return bounded holes finded into mesh.
	 *  Mesh must have updated MM_FACETOPO and MM_BORDERFLAG flags.
	 *  Finded hole are append to ret, so programmer who call this functions have to worry 
	 *  about cleaning of holes already existing.
	 */
	static void GetBoundHoles(MESH &mesh, vector< FgtHole<MESH> >&ret)
	{	
		PosType fstPos;
		FaceIterator fi;
		for(fi = mesh.face.begin(); fi != mesh.face.end(); ++fi)
		{
			// per ciascuna faccia controllo se ha almeno un vertice di bordo (scartando quelli già visti)
			getPosBorderNV( (*fi), fstPos );
					
			if( !fstPos.IsNull() )
			{
				// è stata trovata una faccia con un vertice di bordo quindi si segue il bordo per delimitare il buco

				// Si aggiunge fstV alla lista di "hole"			
				FgtHole<MESH> hole(fstPos);
				ret.push_back( hole );
							
				// si scorrono i vertici vicini in cerca di quelli di bordo
				// fino a ritornare al vertice iniziale; tali vertici vengono segnati come 
				// visitati in modo da evitare di riselezionarli scorrendo le facce
				vcg::face::Pos<MESH::FaceType> curPos = fstPos;
				do
				{				
					curPos.v->SetV();
					curPos.NextB();
				}while( curPos != fstPos );		
			}
		}
	}


	/** This mesthod return holes finded into mesh. Holes which share a vertex (non manifold vertex) are
	 *  considered as different holes.
	 *  Mesh must have updated MM_FACETOPO and MM_BORDERFLAG flags.
	 *  Finded hole are append to ret, so programmer who call this functions have to worry 
	 *  about cleaning of holes already existing.
	 */
	static void GetHoles(MESH &mesh, vector< FgtHole<MESH> >&ret) {
		int nmBit = CMeshO::VertexType::NewBitFlag(); // non manifoldness bit
	
		PosType fstPos;
		FaceIterator fi;	
		for(fi=mesh.face.begin(); fi!=mesh.face.end(); ++fi) {
			// per ciascuna faccia controllo se ha almeno un vertice di bordo (scartando quelli già visti)
			getPosBorderNV( (*fi), fstPos );
		
			if( !fstPos.IsNull() ) {
				assert( fstPos.IsBorder() );
				// è stata trovata una faccia con un vertice di bordo quindi si segue il bordo per delimitare il buco
							

				// Si cerca di far scorrere il buco nel verso corretto, antiorario
				// si posiziona l'half-edge (pos) sul vertice del buco più a destra
				//<Test>
				PosType specPos = fstPos;
				specPos.FlipV();

				int fv = -1;
				if(fstPos.V() == fstPos.F()->V(0)) fv = 0;
				else if(fstPos.V() == fstPos.F()->V(1)) fv = 1;
				else if(fstPos.V() == fstPos.F()->V(2)) fv = 2;

				int sv = -1;
				if(specPos.V() == fstPos.F()->V(0)) sv = 0;
				else if(specPos.V() == fstPos.F()->V(1)) sv = 1;
				else if(specPos.V() == fstPos.F()->V(2)) sv = 2;

				int i=0;
				if(sv != ((fv+1)%3) )
				{
					// siamo nel caso in cui sull'edge di bordo più a destra c'è i vertice opposto
					// a quello trovato
					fstPos.FlipV();
				}	
				//</Test>


				// si scorrono i vertici vicini in cerca di quelli di bordo
				// fino a ritornare al vertice iniziale; tali vertici vengono segnati come 
				// visitati in modo da evitare di riselezionarli
				
				PosType curPos = fstPos;			
				// si resetta tutto il bordo del buco
				do {
					curPos.V()->ClearUserBit(nmBit);
					curPos.V()->ClearV();
					curPos.NextB();
				}while(curPos != fstPos);
			
				// riscorro il bordo del buco segnando i vertici che appartengono a più buchi e equanti hole appartengono
				vector< pair<MESH::VertexPointer, int> > EARS;
				do{
					if(!curPos.V()->IsV())
						curPos.V()->SetV(); // All the vertexes that are visited more than once are non manifold
					else 
					{
						// è un vertice già visto aggiorno
						curPos.V()->SetUserBit(nmBit);
						vector< pair<MESH::VertexPointer, int> >::iterator it = EARS.begin();
						bool find=false;
						for( ; it!=EARS.end(); ++it)
						{
							if( (*it).first == curPos.V() )
							{
								find = true;
								(*it).second++;
								break;
							}
						}
						if(!find) // è la prima volta che si ripassa da un vertice già visto
							EARS.push_back( pair<MESH::VertexPointer, int>(curPos.V(), 2) );
					}
					curPos.NextB();
				} while(curPos != fstPos); 
				
				// controllo se ci sono hole adiacenti a questo (che condividono un vertice)
				if(EARS.size() == 0) {					
					FgtHole<MESH> hole(fstPos);
					ret.push_back( hole );
					continue;
				}
				
				// scorrendo il bordo quando trovo un EAR apro un nuovo hole, quando ripasso da un EAR già visto chiudo
				// l'hole settandogli che quale pos devo sostuire nella scansione del bordo
				curPos = fstPos;
				vector<PosType> posStack;
				// Si aggiunge fstV alla lista di "hole"
				FgtHole<MESH> hole(fstPos);
				vector<FgtHole<MESH>> holeStack;
				holeStack.push_back( hole );			
				do {				
					if( curPos.V()->IsUserBit(nmBit) && EARS.size()>0 )	{
						// sono su un EAR, se avevo già visto il vertice EAR chiudo l'hole
						vector< pair<MESH::VertexPointer, int> >::iterator it = EARS.begin();
						for( ; it!=EARS.end() ; ++it) {
							if( (*it).first == curPos.V() ) {
								assert( (*it).second >0 );							
								if( (--(*it).second)%2 == 1) {
									// è un numero dispari di volte che passo dall'EAR, devo salvare la posizione per poter applicare 
									// la patch sull hole che sto abbandonando, in più devo aprire un nuovo hole
									posStack.push_back( curPos );

									// il nuovo buco da aprire partire con la posizione di bordo successiva
									curPos.NextB();
									FgtHole<MESH> anHole(curPos);
									holeStack.push_back( anHole );
								}
								else {
									// è un numero pari di volte che passo dall'EAR, devo chiudere l'hole (dallo stack lo inserisco nella lista finale)
									// e applicare la patch sull'ultimo buco lasciato
									FgtHole<MESH> anHole = holeStack.back();
									holeStack.pop_back();

									FgtHole<MESH>::PosPatch p( curPos, anHole.startPos );
									
									anHole.posPatches.push_back( p ); // aggiungo la patch per l'hole chiuso
									ret.push_back( anHole );

									if( (*it).second == 0 )	{
										// scorrendo sul bordo non ripasserò più da questo vertice lo elimino dalle EAR da visitare
										// applico la patch all'ultimo hole ancora aperto
										EARS.erase(it);
										
										// applico la patch all'ultimo hole ancor aperto
										FgtHole<MESH> &last = holeStack.back();
										p.first = posStack.back();
										curPos.NextB();
										p.second = curPos;
										last.posPatches.push_back( p );
										posStack.pop_back();
									}
									else {
										// in questo vertice ci incide un'altro hole ancora ne apro uno nuovo
										curPos.NextB();
										FgtHole<MESH> anHole(curPos);
										holeStack.push_back( anHole );
									}
								}
								// ho trovato l'ear relativo... stoppo la ricerca
								break;
							}		
						} // for che scorre gli EAR incontrati					
					}
					else								
						curPos.NextB();
				}while( curPos != fstPos );
				
				FgtHole<MESH> anHole = holeStack.back();
				holeStack.pop_back();				
				ret.push_back( anHole );				

			} // abbiamo trovato un nuovo bordo
		} // for su tutte le facce della mesh
		FaceType::VertexType::DeleteBitFlag(nmBit); // non manifoldness bit
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
			// per ogni buco della mesh scorro il bordo cercando la faccia richiesta
			PosType curPos, fstPos;
			curPos = fstPos = (*hit).startPos;
			list<PosPatch>::const_iterator pit = (*hit).posPatches.begin();
			// scorro il bordo applicando le eventuali patches
			do
			{			
				if(curPos.F() == bFace) 
					return index;
				if( pit!= (*hit).posPatches.end() )
					if( curPos == (*pit).first)
					{
						curPos = (*pit).second;
						pit++;
						continue;
					}			
				curPos.NextB();
			}while( curPos != fstPos );	
			index++;
		}
		return -1; // means no find hole
	}


private:
	
	/** If face have a border edge and its vertex aren't already seen pos is set on this edge.
	 *  Pos is null otherwise.
	 */
	static void getPosBorderNV(FaceType& face, PosType &pos) {
		pos.SetNull();

		// TO DO: vedere se si possono migliorare questi controlli
		if(  ( face.V(0)->IsB() && !face.V(0)->IsV() ) && 
			 ( face.V(1)->IsB() && !face.V(1)->IsV() ) )
		{
			face.V(0)->SetV();
			face.V(1)->SetV();
			pos.Set( &face, 0, face.V(0) );
		}
		else if( ( face.V(1)->IsB() && !face.V(1)->IsV() ) && 
				 ( face.V(2)->IsB() && !face.V(2)->IsV() ) )
		{
			face.V(1)->SetV();
			face.V(2)->SetV();
			pos.Set( &face, 1, face.V(1) );
		}
		else if( ( face.V(2)->IsB() && !face.V(2)->IsV() ) && 
				 ( face.V(0)->IsB() && !face.V(0)->IsV() ) )
		{
			face.V(2)->SetV();
			face.V(0)->SetV();
			pos.Set( &face, 2, face.V(2) );
		}
	}

};



template <class MESH>
class HoleFiller
{
public:
	typedef typename MESH::FaceType FaceType;
    typedef typename MESH::FacePointer FacePointer;
	typedef typename MESH::FaceIterator FaceIterator;
	typedef typename vcg::face::Pos<FaceType> PosType;
	typedef typename MESH::VertexType VertexType;
    typedef typename MESH::CoordType CoordType;


	/**
	 *  This function creates a patch for an hole. The hole goes filled adding triangles created closing 2 
	 *  border edge consecutive. Iterating this procedure hole come always more "little" until it's become closed.
	 *  Algorithm chooses as better compact triangles and triangles which have more similar normal with
	 *  adjiacent tringles.
	 */
	static void GreedyFilling( const FgtHole<MESH> &hole, HolePatch<MESH> &resHolePatch)
	{
		const double bC = 1.0;	// baricenter coefficent
		const double nC = 1.0;	// normal coefficent

		vector<PosType> posVect;
		hole.GetPosVector(posVect);

		vector<FacePointer *> pu; // vettore dei vertici che viene aggiornato
		FaceIterator f = tri::Allocator<MESH>::AddFaces(resHolePatch.patchMesh, posVect.size()-2, pu);

		double *bW = 0;
		double *nW = 0;
		if( posVect.size() >=3 )
		{
			// si alloca la quantità massima, iterando l'algoritmo utilizzerà sempre meno locazioni
			bW = new double[posVect.size()-2];	// array where store baricenter weigth values
			nW = new double[posVect.size()-2];	// array where store normal weigth values
		}

		// Si aggiunge un triangolo fin tanto non ne resta uno solo possibile da inserire
		while( posVect.size() >=3 )
		{
			// scelgo il tringolo con il baricentro più vicino ai lati, ovvero più compatto.
			// Inolte cerco di selezionare il triangolo che non è compenetrante con la mesh.
			// Per scegliere il miglior tiangolo si valutano tutti i triangoli creabili "chiudendo"
			// due edge consecutivi. La valuazione consiste nella somma dei due coefficienti normalizzati
			// (compattezza e differenza normali) moltiplicati da 2 parametri.

			CoordType holeBaricenter = getHoleBaricenter(posVect);
			const int nTriangles = posVect.size()-2;
			double maxBW = DBL_MIN;	// max value of baricenter weigth (vertex distance from baricenter, compactness)
			double maxNW = DBL_MIN;	// max value of normal weigth (difference from normal)
			
			for(int i=0; i<nTriangles; i++)
			{
				PosType p0 = posVect.at(i);
				PosType p1 = posVect.at(i+1);
				PosType p2 = posVect.at( (i+2)%posVect.size() );

				bW[i] = getBaricenterWeigth( p0.V()->P(), p1.V()->P(), p2.V()->P() );
				nW[i] = getNormalWeigth(p0, p1, p2);

				if(bW[i] > maxBW) maxBW = bW[i];
				if(nW[i] > maxNW) maxNW = nW[i];
			}
			
			int firstPos = 0;
			double minWeigth = DBL_MAX;
			for(int i=0; i<nTriangles; i++)
			{
				// Evito di prendere 2 border edge di una "sporgenza" convessa all'interno del buco
				// questo potrebbe portare ad inserire un triangolo sovrapposto a quello "sporgente" dentro il buco
				if(posVect.at(i).F() == posVect.at(i+1).F())
					continue;
				
				// si normalizza ciascun valore baricenter e normal weigth e si computa la
				// stima di ciascun triangolo
				double tmpWeigth = bW[i]/maxBW * bC + nW[i]/maxNW * nC;
				if(tmpWeigth < minWeigth)
				{
					firstPos = i;
					minWeigth = tmpWeigth;
				}
			}	
		
			
			(*f).V(0) = posVect.at(firstPos).V();
			(*f).V(1) = posVect.at(firstPos+1).V();
			(*f).V(2) = posVect.at( (firstPos+2)%posVect.size() ).V();
			PosType newPos( &(*f), 2, (*f).V(2) );
			
			// rimuovo il secondo pos (il primo edge) e sostituisco il secondo pos ()con il nuovo edge inserito
			posVect.erase( posVect.begin()+firstPos+1 );
			PosType &nPos = posVect.at(firstPos+1);
			nPos.f = &(*f);
			nPos.z = 2;
			// il vertice resta lo stesso

			ComputeNormal(*f);

/*
			// Aggiorno l'adiacenza
			(*f).FFp(0) = e0.f;
			(*f).FFi(0) = e0.z;
			(*f).FFp(1) = e1.f;
			(*f).FFi(1) = e1.z;
			(*f).FFp(2) = f;
			(*f).FFi(2) = 2;

			e0.f->FFp(e0.z)=f;
			e0.f->FFi(e0.z)=0;	

			e1.f->FFp(e1.z)=f;
			e1.f->FFi(e1.z)=1;
*/
			f++;
		} 

		if(bW != 0) 
		{
			delete bW;
			delete nW;
		}
	}

	/**	Return the point rapresenting hole baricenter.
	 */
	static CoordType getHoleBaricenter(std::vector<PosType> &hole)
	{
		if(hole.size()==0)
			return 0;

		VertexType* v = 0;
		CoordType res = hole.at(0).V()->P();
		for(int i=1; i<hole.size(); i++)
			res += hole.at(i).V()->P();
		
		res /= hole.size();
		
		return res;
	}


	/** Return the sum of quadratic distance from vertex to triangle baricenter point.
	 *  It used to stimate quality of triangle, small equilater triangle are better than another.
	 */
	static double getBaricenterWeigth(const CoordType &v0, const CoordType v1, const CoordType &v2) 
	{
		// calcolo il baricentro del triangolo
		CoordType baricenter;
		baricenter[0] = (v0[0] + v1[0] + v2[0])/3.0;
		baricenter[1] = (v0[1] + v1[1] + v2[1])/3.0;
		baricenter[2] = (v0[2] + v1[2] + v2[2])/3.0;

		// sommo la distanza di ciascun punto dal baricentro
		double qdist = pow(v0[0]- baricenter[0] ,2) + pow(v0[1]- baricenter[1], 2) + pow(v0[2]- baricenter[2], 2);
		qdist += pow(v1[0]- baricenter[0], 2) + pow(v1[1]- baricenter[1], 2) + pow(v1[2]- baricenter[2], 2);
		qdist += pow(v2[0]- baricenter[0], 2) + pow(v2[1]- baricenter[1], 2) + pow(v2[2]- baricenter[2], 2);
		
		return qdist;
	}
	
	/**
	 *  Return mesure of difference from normal of triangle we want insert into hole patch and
	 *  existing face normal closest to this trinagle.
	 */
	static double getNormalWeigth(PosType &p0, PosType &p1, PosType &p2)
	{
		Point3f n = NormalizedNormal( p0.V()->P(), p1.V()->P(), p2.V()->P() );
		
		Point3f n1 = NormalizedNormal(p1.F()->V(0)->P(), p1.F()->V(1)->P(), p1.F()->V(2)->P());
		Point3f n2 = NormalizedNormal(p2.F()->V(0)->P(), p2.F()->V(1)->P(), p2.F()->V(2)->P());
		//Point3f n2 = p2.F()->NormalizedNormal();
		
		float dist = SquaredDistance(n, n1);
		dist += SquaredDistance(n, n2);

		return dist;
	}
};


#endif