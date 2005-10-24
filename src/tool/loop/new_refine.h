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
/****************************************************************************
  History

$Log$
Revision 1.2  2005/10/24 13:58:22  mariolatronico
Added a preliminary version of EvenPointLoop function of Loop's alghoritm

Revision 1.1  2005/10/24 11:41:54  giec
Creata la sottocartella test/loop con dentro un progetto da cui iniziare per implementare l'algoritmo di Loop

Revision 1.6  2005/07/11 13:13:33  cignoni
small gcc-related compiling issues (typenames,ending cr, initialization order)

Revision 1.5  2005/06/29 15:25:41  callieri
deleted a wrong declaration "typename typename"

Revision 1.4  2005/06/17 00:48:27  cignoni
Corrected the type name of wedge tex coords WedgeInterp in RefineE

Revision 1.3  2005/02/25 10:28:04  pietroni
added #include<vcg/complex/trimesh/update/topology.h> use of update topology in refineE

Revision 1.2  2005/02/02 16:01:13  pietroni
1 warning corrected

Revision 1.1  2004/10/12 15:42:29  ganovelli
first working version


****************************************************************************/

#ifndef __VCGLIB_REFINE
#define __VCGLIB_REFINE

#include <functional>
#include <map>
#include <vector>
#include <vcg/space/sphere3.h>
#include <vcg/space/plane3.h>
#include <vcg/space/tcoord2.h>
#include <vcg/space/color4.h>
#include <vcg/simplex/face/pos.h>
#include<vcg/complex/trimesh/allocate.h>
#include<vcg/complex/trimesh/update/topology.h>

namespace vcg{
/* Tabella che codifica le modalita' di split a seconda di quali dei tre edge sono da splittare 

Il primo numero codifica il numero di triangoli, 
le successive 5 triple codificano i triangoli 
secondo la seguente convenzione:
 0..2 vertici originali del triangolo 
 3..5 mp01, mp12, mp20 midpoints of the three edges

Nel caso "due lati splittati" e' necessario fare la triangolazione del trapezio bene.
Per cui in questo caso sono specificati 5 triangoli, il primo e' quello ovvio, 
se dei due lati splittati e' minore il primo va scelta la prima coppia altrimenti la seconda coppia

il campo swap codificano le due diagonali da testare per scegliere quale triangolazione usare 
per il caso con tre triangoli: se diag1<diag2 si swappa la il primo lato dell'ultimo triangolo
(e.g la seconda diag ha come indici i primi due numeri dell'ultimo triangolo)
*/

class Split {
public:
	int TriNum;			// numero di triangoli
	int TV[4][3];   // The triangles coded as the following convention 
									//     0..2 vertici originali del triangolo 
									//     3..5 mp01, mp12, mp20 midpoints of the three edges
	int swap[2][2]; // the two diagonals to test for swapping
	int TE[4][3];   // the edge-edge correspondence between refined triangles and the old one
									//      (3) means the edge of the new triangle is internal;
};

const Split SplitTab[8]={
/* m20 m12 m01 */
/*  0   0   0 */	{1, {{0,1,2},{0,0,0},{0,0,0},{0,0,0}}, {{0,0},{0,0}},  {{0,1,2},{0,0,0},{0,0,0},{0,0,0}} },
/*  0   0   1 */	{2, {{0,3,2},{3,1,2},{0,0,0},{0,0,0}}, {{0,0},{0,0}},  {{0,3,2},{0,1,3},{0,0,0},{0,0,0}} },
/*  0   1   0 */	{2, {{0,1,4},{0,4,2},{0,0,0},{0,0,0}}, {{0,0},{0,0}},  {{0,1,3},{3,1,2},{0,0,0},{0,0,0}} },
/*  0   1   1 */	{3, {{3,1,4},{0,3,2},{4,2,3},{0,0,0}}, {{0,4},{3,2}},  {{0,1,3},{0,3,2},{1,3,3},{0,0,0}} },
/*  1   0   0 */	{2, {{0,1,5},{5,1,2},{0,0,0},{0,0,0}}, {{0,0},{0,0}},  {{0,3,2},{3,1,2},{0,0,0},{0,0,0}} },
/*  1   0   1 */	{3, {{0,3,5},{3,1,5},{2,5,1},{0,0,0}}, {{3,2},{5,1}},  {{0,3,2},{0,3,3},{2,3,1},{0,0,0}} },
/*  1   1   0 */	{3, {{2,5,4},{0,1,5},{4,5,1},{0,0,0}}, {{0,4},{5,1}},  {{2,3,1},{0,3,2},{3,3,1},{0,0,0}} },
/*  1   1   1 */	//{4, {{0,3,5},{3,1,4},{5,4,2},{3,4,5}}, {{0,0},{0,0}},  {{0,3,2},{0,1,3},{3,1,2},{3,3,3}} },
/*  1   1   1 */	{4, {{3,4,5},{0,3,5},{3,1,4},{5,4,2}}, {{0,0},{0,0}},  {{3,3,3},{0,3,2},{0,1,3},{3,1,2}} },
};

// Classe di suddivisione base. Taglia il lato esattamente a meta'.
template<class MESH_TYPE>
struct MidPoint : public   std::unary_function<face::Pos<typename MESH_TYPE::FaceType> ,  typename MESH_TYPE::CoordType >
{
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType>  ep){
		nv.P()=   (ep.f->V(ep.z)->P() + ep.f->V1(ep.z)->P())/2.0;

		if( MESH_TYPE::HasPerVertexNormal())
			nv.N()= (ep.f->V(ep.z)->N()+ep.f->V1(ep.z)->N()).Normalize();

		if( MESH_TYPE::HasPerVertexColor())
			nv.C().lerp(ep.f->V(ep.z)->C(),ep.f->V1(ep.z)->C(),.5f);
	}

	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TCoord2<FL_TYPE,1> WedgeInterp(TCoord2<FL_TYPE,1> &t0, TCoord2<FL_TYPE,1> &t1)
	{
		TCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
};



template<class MESH_TYPE>
struct MidPointArc : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> ,  typename MESH_TYPE::CoordType>
{
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType> ep)
	{
		const typename MESH_TYPE::ScalarType EPS =1e-10;
		typename MESH_TYPE::CoordType vp = (ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0;
		typename MESH_TYPE::CoordType  n = (ep.f->V(ep.z)->N()+ep.f->V1(ep.z)->N())/2.0;
		typename MESH_TYPE::ScalarType w =n.Norm();
		if(w<EPS) { nv.P()=(ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0; return;}
		n/=w;
		typename MESH_TYPE::CoordType d0 = ep.f->V(ep.z)->P() - vp;
		typename MESH_TYPE::CoordType d1 = ep.f->V1(ep.z)->P()- vp;
		typename MESH_TYPE::ScalarType d=Distance(ep.f->V(ep.z)->P(),ep.f->V1(ep.z)->P())/2.0;

		typename MESH_TYPE::CoordType  nn = ep.f->V1(ep.z)->N() ^ ep.f->V(ep.z)->N();
		typename MESH_TYPE::CoordType  np = n ^ d0; // vettore perp al piano edge, normale interpolata
		np.Normalize();
		double sign=1;
		if(np*nn<0) sign=-1; // se le normali non divergono sposta il punto nella direzione opposta
    
		typename MESH_TYPE::CoordType n0=ep.f->V(ep.z)->N() -np*(ep.f->V(ep.z)->N()*np);
		n0.Normalize();
		typename MESH_TYPE::CoordType n1=ep.f->V1(ep.z)->N()-np*(ep.f->V1(ep.z)->N()*np);
		assert(n1.Norm()>EPS);
		n1.Normalize();
		typename MESH_TYPE::ScalarType cosa0=n0*n;
		typename MESH_TYPE::ScalarType cosa1=n1*n;
		if(2-cosa0-cosa1<EPS) {nv.P()=(ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0;return;}
		typename MESH_TYPE::ScalarType cosb0=(d0*n)/d;
		typename MESH_TYPE::ScalarType cosb1=(d1*n)/d;
		assert(1+cosa0>EPS);
		assert(1+cosa1>EPS);
		typename MESH_TYPE::ScalarType delta0=d*(cosb0 +sqrt( ((1-cosb0*cosb0)*(1-cosa0))/(1+cosa0)) );
		typename MESH_TYPE::ScalarType delta1=d*(cosb1 +sqrt( ((1-cosb1*cosb1)*(1-cosa1))/(1+cosa1)) );
		assert(delta0+delta1<2*d);
		nv.P()=vp+n*sign*(delta0+delta1)/2.0;
		return ;
	}

	// Aggiunte in modo grezzo le due wedgeinterp
	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TCoord2<FL_TYPE,1> WedgeInterp(TCoord2<FL_TYPE,1> &t0, TCoord2<FL_TYPE,1> &t1)
	{
		TCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}

};

/*
Versione Della Midpoint basata sul paper:
S. Karbacher, S. Seeger, G. Hausler
A non linear subdivision scheme for triangle meshes

	Non funziona!
	Almeno due problemi:
	1) il verso delle normali influenza il risultato (e.g. si funziona solo se le normali divergono)
		 Risolvibile controllando se le normali divergono
  2) gli archi vanno calcolati sul piano definito dalla normale interpolata e l'edge.
		 funziona molto meglio nelle zone di sella e non semplici.

*/
template<class MESH_TYPE>
struct MidPointArcNaive : public std::unary_function< face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	typename MESH_TYPE::CoordType operator()(face::Pos<typename MESH_TYPE::FaceType>  ep)
	{

		typename MESH_TYPE::CoordType vp = (ep.f->V(ep.z)->P()+ep.f->V1(ep.z)->P())/2.0;
		typename MESH_TYPE::CoordType  n = (ep.f->V(ep.z)->N()+ep.f->V1(ep.z)->N())/2.0;
		n.Normalize();
		typename MESH_TYPE::CoordType d0 = ep.f->V(ep.z)->P() - vp;
		typename MESH_TYPE::CoordType d1 = ep.f->V1(ep.z)->P()- vp;
		typename MESH_TYPE::ScalarType d=Distance(ep.f->V(ep.z)->P(),ep.f->V1(ep.z)->P())/2.0;

		typename MESH_TYPE::ScalarType cosa0=ep.f->V(ep.z)->N()*n;
		typename MESH_TYPE::ScalarType cosa1=ep.f->V1(ep.z)->N()*n;
		typename MESH_TYPE::ScalarType cosb0=(d0*n)/d;
		typename MESH_TYPE::ScalarType cosb1=(d1*n)/d;

		typename MESH_TYPE::ScalarType delta0=d*(cosb0 +sqrt( ((1-cosb0*cosb0)*(1-cosa0))/(1+cosa0)) );
		typename MESH_TYPE::ScalarType delta1=d*(cosb1 +sqrt( ((1-cosb1*cosb1)*(1-cosa1))/(1+cosa1)) );
		
		return vp+n*(delta0+delta1)/2.0;
	}
};



/*

A partire da una mesh raffina una volta tutti i lati dei triangoli minore di thr 
Se RefineSelected == true allora raffina SOLO le facce selezionate 
i cui edge sono piu'corti di thr.

I nuovi vertici e facce della mesh sono aggiunte in fondo 
Requirement: Topologia (in effetti se non si usa la il raffinamento per selezione non servirebbe)

Restituisce false se non raffina nemmeno una faccia.

Si assume che se la mesh ha dati per wedge la funzione di midpoint interpoli i nuovi wedge
tutti uguali per tutti i wedge sulla stessa 'vecchia' faccia.

*/

// binary predicate che dice quando splittare un edge 
// la refine usa qt funzione di default: raffina se l'edge^2 e' piu lungo di thr2


template <class FLT>
class EdgeLen
{
	public:
  FLT thr2;
	bool operator()(const Point3<FLT> &p0, const  Point3<FLT> &p1) const
	{
		return SquaredDistance(p0,p1)>thr2;
	}
};
/*
template<class MESH_TYPE, class MIDPOINT>
bool Refine(MESH_TYPE &m, MIDPOINT mid, typename MESH_TYPE::ScalarType thr=0,bool RefineSelectedP=false)
{
	volatile RefineW<MESH_TYPE,MIDPOINT,EdgeLen<typename MESH_TYPE::ScalarType>,true> RT;
	volatile RefineW<MESH_TYPE,MIDPOINT,EdgeLen<typename MESH_TYPE::ScalarType>,false> RF;

	bool retval;
	if(RefineSelectedP)   retval=RT.RefineT(m,mid,thr);
	if(!RefineSelectedP)	retval=RF.RefineT(m,mid,thr);
	return retval;
}

template<class MESH_TYPE, class MIDPOINT, class EDGEPRED>
bool RefineE(MESH_TYPE &m, MIDPOINT mid, EDGEPRED ep, bool RefineSelectedP=false)
{
	RefineW<MESH_TYPE,MIDPOINT,EDGEPRED,true> RT;
	RefineW<MESH_TYPE,MIDPOINT,EDGEPRED,false> RF;

	bool retval;
	if(RefineSelectedP)   retval=RT.RefineT(m,mid,ep);
	if(!RefineSelectedP)	retval=RF.RefineT(m,mid,ep);
	return retval;
}
*/
/*********************************************************/
/*********************************************************/
/*********************************************************/

template<class MESH_TYPE,class MIDPOINT>
bool Refine(MESH_TYPE &m, MIDPOINT mid, typename MESH_TYPE::ScalarType thr=0,bool RefineSelected=false)
{
	EdgeLen <typename MESH_TYPE::ScalarType> ep;
	ep.thr2=thr*thr;
  return RefineE(m,mid,ep,RefineSelected);
}

template<class MESH_TYPE,class MIDPOINT, class EDGEPRED>
bool RefineE(MESH_TYPE &m, MIDPOINT mid, EDGEPRED ep,bool RefineSelected=false)
{
	int j,NewVertNum=0,NewFaceNum=0;
	typedef std::pair<typename MESH_TYPE::VertexPointer,typename MESH_TYPE::VertexPointer> vvpair;
	std::map<vvpair,typename MESH_TYPE::VertexPointer> Edge2Vert;
	// Primo ciclo si conta quanti sono i vertici  e facce da aggiungere 
	typename MESH_TYPE::FaceIterator fi;
	for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
		for(j=0;j<3;j++){
				if(ep((*fi).V(j)->P(),(*fi).V1(j)->P()) && 
					(!RefineSelected || ((*fi).IsS() && (*fi).FFp(j)->IsS())) ){
					++NewFaceNum;
					if(((*fi).V(j)<(*fi).V1(j)) || 
						(*fi).IsB(j))  
						++NewVertNum;
				}
		}
	if(NewVertNum==0) return false;
	typename MESH_TYPE::VertexIterator lastv = tri::Allocator<MESH_TYPE>::AddVertices(m,NewVertNum);
//	typename MESH_TYPE::VertexIterator lastv=m.AddVertices(NewVertNum);
	
	// Secondo Ciclo si inizializza la mappa da edge a vertici
	// e la posizione dei nuovi vertici
	for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
		for(j=0;j<3;j++)
			if(ep((*fi).V(j)->P(),(*fi).V1(j)->P())  && 
					(!RefineSelected || ((*fi).IsS() && (*fi).FFp(j)->IsS())) )
				if((*fi).V(j)<(*fi).V1(j) || (*fi).IsB(j)){
						(*lastv).UberFlags()=0;
						mid( (*lastv), face::Pos<typename MESH_TYPE::FaceType> (&*fi,j));
						//(*lastv).P()=((*fi).V(j)->P()+(*fi).V1(j)->P())/2;
						Edge2Vert[ vvpair((*fi).V(j),(*fi).V1(j)) ] = &*lastv;
						++lastv;
				}
	assert(lastv==m.vert.end());

	typename MESH_TYPE::FaceIterator lastf = tri::Allocator<MESH_TYPE>::AddFaces(m,NewFaceNum);
//	MESH_TYPE::FaceIterator lastf = m.AddFaces(NewFaceNum);
//    int ddd=0; distance(m.face.begin(),lastf,ddd);
	typename MESH_TYPE::FaceIterator oldendf=lastf; 
	
/*
          v0

   
          f0

    mp01      mp02

          f3
      f1       f2

 v1      mp12       v2

*/

	typename MESH_TYPE::VertexPointer vv[6]; // i sei vertici in gioco 
																		//     0..2 vertici originali del triangolo 
																		//     3..5 mp01, mp12, mp20 midpoints of the three edges
	typename MESH_TYPE::FacePointer nf[4];   // le quattro facce in gioco.

  typename MESH_TYPE::FaceType::TexCoordType wtt[6];  // per ogni faccia sono al piu' tre i nuovi valori 
																							 // di texture per wedge (uno per ogni edge) 

	int fca=0,fcn =0;
	for(fi=m.face.begin();fi!=oldendf;++fi) if(!(*fi).IsD())
		{fcn++;
				vv[0]=(*fi).V(0);
				vv[1]=(*fi).V(1);
				vv[2]=(*fi).V(2);
				bool e0=ep((*fi).V(0)->P(),(*fi).V(1)->P());
				bool e1=ep((*fi).V(1)->P(),(*fi).V(2)->P());
				bool e2=ep((*fi).V(2)->P(),(*fi).V(0)->P());

				if(e0) 
						if((*fi).V(0)<(*fi).V(1)|| (*fi).IsB(0)) vv[3]=Edge2Vert[ vvpair((*fi).V(0),(*fi).V(1)) ];
																										 else vv[3]=Edge2Vert[ vvpair((*fi).V(1),(*fi).V(0)) ];
				else vv[3]=0;
				if(e1)
						if((*fi).V(1)<(*fi).V(2)|| (*fi).IsB(1)) vv[4]=Edge2Vert[ vvpair((*fi).V(1),(*fi).V(2)) ];
																										 else vv[4]=Edge2Vert[ vvpair((*fi).V(2),(*fi).V(1)) ];
				else vv[4]=0;
				if(e2)
						if((*fi).V(2)<(*fi).V(0)|| (*fi).IsB(2)) vv[5]=Edge2Vert[ vvpair((*fi).V(2),(*fi).V(0)) ];
																										 else vv[5]=Edge2Vert[ vvpair((*fi).V(0),(*fi).V(2)) ];
				else vv[5]=0;
				int ind=((&*vv[3])?1:0)+((&*vv[4])?2:0)+((&*vv[5])?4:0);
				
				nf[0]=&*fi;
				int i;
				for(i=1;i<SplitTab[ind].TriNum;++i){
						nf[i]=&*lastf; ++lastf; fca++;
						if(RefineSelected) (*nf[i]).SetS();
				}
        
				if(MESH_TYPE::HasPerWedgeTexture())
					for(i=0;i<3;++i)	{
						wtt[i]=(*fi).WT(i);
						wtt[3+i]=mid.WedgeInterp((*fi).WT(i),(*fi).WT((i+1)%3));
					}

				int orgflag=	(*fi).UberFlags();
				for(i=0;i<SplitTab[ind].TriNum;++i)
					for(j=0;j<3;++j){
						(*nf[i]).V(j)=&*vv[SplitTab[ind].TV[i][j]];
						
						if(MESH_TYPE::HasPerWedgeTexture()) //analogo ai vertici...
									(*nf[i]).WT(j)=wtt[SplitTab[ind].TV[i][j]];

						assert((*nf[i]).V(j)!=0);
						if(SplitTab[ind].TE[i][j]!=3){
							if(orgflag & (MESH_TYPE::FaceType::BORDER0<<(SplitTab[ind].TE[i][j]))) 
								(*nf[i]).SetB(j); 
							else
								(*nf[i]).ClearB(j);
						} 
						else (*nf[i]).ClearB(j);						
					}

		if(SplitTab[ind].TriNum==3 && 
							SquaredDistance(vv[SplitTab[ind].swap[0][0]]->P(),vv[SplitTab[ind].swap[0][1]]->P()) < 
							SquaredDistance(vv[SplitTab[ind].swap[1][0]]->P(),vv[SplitTab[ind].swap[1][1]]->P()) )
							{ // swap the last two triangles
								(*nf[2]).V(1)=(*nf[1]).V(0);
								(*nf[1]).V(1)=(*nf[2]).V(0);
								if(MESH_TYPE::HasPerWedgeTexture()){ //analogo ai vertici...
									(*nf[2]).WT(1)=(*nf[1]).WT(0);
									(*nf[1]).WT(1)=(*nf[2]).WT(0);
								}
								
								if((*nf[1]).IsB(0)) (*nf[2]).SetB(1); else (*nf[2]).ClearB(1);
								if((*nf[2]).IsB(0)) (*nf[1]).SetB(1); else (*nf[1]).ClearB(1);
								(*nf[1]).ClearB(0);
								(*nf[2]).ClearB(0);
							}
		}
	m.fn= m.face.size();
	assert(lastf==m.face.end());	for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD()){
					assert((*fi).V(0)>=&*m.vert.begin() && (*fi).V(0)<&*m.vert.end() );
					assert((*fi).V(1)>=&*m.vert.begin() && (*fi).V(1)<&*m.vert.end() );
					assert((*fi).V(2)>=&*m.vert.begin() && (*fi).V(2)<&*m.vert.end() );
	}
	vcg::tri::UpdateTopology<MESH_TYPE>::FaceFace(m);	
	return true;
}
/*************************************************************************/

/*
Modified Butterfly interpolation scheme, 
as presented in 
Zorin, Schroeder
Subdivision for modeling and animation
Siggraph 2000 Course Notes
*/

/*

    vul-------vu--------vur
		  \      /  \      /
			 \    /    \    /
        \  /  fu  \  /
         vl--------vr
        /  \  fd  /  \
       /    \    /    \
      /      \  /      \
    vdl-------vd--------vdr

*/

template<class MESH_TYPE>
struct MidPointButterfly : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType>  ep)
	{	
		face::Pos<typename MESH_TYPE::FaceType> he(ep.f,ep.z,ep.f->V(ep.z));
		typename MESH_TYPE::CoordType *vl,*vr;
		typename MESH_TYPE::CoordType *vl0,*vr0; 
		typename MESH_TYPE::CoordType *vu,*vd,*vul,*vur,*vdl,*vdr;
		vl=&he.v->P();
		he.FlipV();
		vr=&he.v->P();
		if(he.IsBorder())
		{
			he.NextB();
			vr0=&he.v->P();
			he.FlipV();
			he.NextB();
			assert(&he.v->P()==vl);
			he.NextB();
			vl0=&he.v->P();
			nv.P()=((*vl)+(*vr))*(9.0/16.0)-((*vl0)+(*vr0))/16.0 ;
		}
		else 
		{
			he.FlipE();he.FlipV();
			vu=&he.v->P();
			he.FlipF();he.FlipE();he.FlipV(); 
			vur=&he.v->P();
			he.FlipV();he.FlipE();he.FlipF(); assert(&he.v->P()==vu); // back to vu (on the right)
			he.FlipE();
			he.FlipF();he.FlipE();he.FlipV();
			vul=&he.v->P();
			he.FlipV();he.FlipE();he.FlipF(); assert(&he.v->P()==vu); // back to vu (on the left)
			he.FlipV();he.FlipE();he.FlipF(); assert(&he.v->P()==vl);// again on vl (but under the edge)
			he.FlipE();he.FlipV();
			vd=&he.v->P();
			he.FlipF();he.FlipE();he.FlipV();
			vdl=&he.v->P();
			he.FlipV();he.FlipE();he.FlipF(); assert(&he.v->P()==vd);// back to vd (on the right)
			he.FlipE();
			he.FlipF();he.FlipE();he.FlipV();
			vdr=&he.v->P();

			nv.P()=((*vl)+(*vr))/2.0+((*vu)+(*vd))/8.0 - ((*vul)+(*vur)+(*vdl)+(*vdr))/16.0;
			}
	}

	/// Aggiunte in modo grezzo le due wedge interp
	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TCoord2<FL_TYPE,1> WedgeInterp(TCoord2<FL_TYPE,1> &t0, TCoord2<FL_TYPE,1> &t1)
	{
		TCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
};


#if 0
			int rule=0;
			if(vr==vul) rule+=1;
			if(vl==vur) rule+=2;
			if(vl==vdr) rule+=4;
			if(vr==vdl) rule+=8;
			switch(rule){
/*      */
/*      */			case  0 :	return ((*vl)+(*vr))/2.0+((*vu)+(*vd))/8.0 - ((*vul)+(*vur)+(*vdl)+(*vdr))/16.0;
/* ul   */  		case  1 : return (*vl*6 + *vr*10 + *vu + *vd*3 - *vur - *vdl -*vdr*2 )/16.0; 
/* ur   */  		case  2 : return (*vr*6 + *vl*10 + *vu + *vd*3 - *vul - *vdr -*vdl*2 )/16.0; 
/* dr   */  		case  4 : return (*vr*6 + *vl*10 + *vd + *vu*3 - *vdl - *vur -*vul*2 )/16.0; 
/* dl   */  		case  8 : return (*vl*6 + *vr*10 + *vd + *vu*3 - *vdr - *vul -*vur*2 )/16.0; 
/* ul,ur */  		case  3 : return (*vl*4 + *vr*4 + *vd*2 + - *vdr - *vdl )/8.0; 
/* dl,dr */  		case 12 : return (*vl*4 + *vr*4 + *vu*2 + - *vur - *vul )/8.0; 

/* ul,dr */  		case  5 :
/* ur,dl */  		case 10 :						
								default:				
									return (*vl+ *vr)/2.0; 
			}



#endif
/*
    vul-------vu--------vur
		  \      /  \      /
			 \    /    \    /
        \  /  fu  \  /
         vl--------vr
        /  \  fd  /  \
       /    \    /    \
      /      \  /      \
    vdl-------vd--------vdr

*/

// Versione modificata per tenere di conto in meniara corretta dei vertici con valenza alta

template<class MESH_TYPE>
struct MidPointButterfly2 : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	typename MESH_TYPE::CoordType operator()(face::Pos<typename MESH_TYPE::FaceType>  ep)
	{	
double Rules[11][10] = 
{
	{.0}, // valenza 0 
	{.0}, // valenza 1 
	{.0}, // valenza 2 
	{  .4166666667, -.08333333333 , -.08333333333  }, // valenza 3 
	{  .375       ,  .0           ,  -0.125        ,  .0          }, // valenza 4											
	{  .35        ,  .03090169945 ,  -.08090169945 , -.08090169945,  .03090169945	}, // valenza 5	
	{  .5         ,  .125         ,  -0.0625       ,  .0          ,  -0.0625      , 0.125       }, // valenza 6	
	{  .25        ,  .1088899050  , -.06042933822  , -.04846056675, -.04846056675, -.06042933822,  .1088899050  }, // valenza 7	  
	{  .21875     ,  .1196383476  , -.03125        , -.05713834763, -.03125      , -.05713834763, -.03125      ,.1196383476  }, // valenza 8	  
	{  .1944444444,  .1225409480  , -.00513312590  , -.05555555556, -.03407448880, -.03407448880, -.05555555556, -.00513312590, .1225409480  }, // valenza 9	  
	{  .175       ,  .1213525492  ,  .01545084973  , -.04635254918, -.04045084973, -.025        , -.04045084973, -.04635254918,  .01545084973,  .1213525492  } // valenza 10	  
};

face::Pos<typename MESH_TYPE::FaceType> he(ep.f,ep.z,ep.f->V(ep.z));
	typename MESH_TYPE::CoordType *vl,*vr;
	vl=&he.v->P();
	vr=&he.VFlip()->P();
	if(he.IsBorder())
		{	//Bordo
			he.FlipV();
			typename MESH_TYPE::CoordType *vl0,*vr0; 
			he.NextB();
			vr0=&he.v->P();
			he.FlipV();
			he.NextB();
			assert(&he.v->P()==vl);
			he.NextB();
			vl0=&he.v->P();
			return ((*vl)+(*vr))*(9.0/16.0)-((*vl0)+(*vr0))/16.0 ;
		}

	int kl=0,kr=0; // valence of left and right vertices
	bool bl=false,br=false; // if left and right vertices are of border
  face::Pos<typename MESH_TYPE::FaceType> heStart=he;assert(he.v->P()==*vl);
	do { // compute valence of left vertex
		he.FlipE();he.FlipF();
		if(he.IsBorder()) bl=true; 
		++kl;
	}	while(he!=heStart);
	
	he.FlipV();heStart=he;assert(he.v->P()==*vr);
	do { // compute valence of right vertex
		he.FlipE();he.FlipF();
		if(he.IsBorder()) br=true; 
		++kr;
	}	while(he!=heStart);
  if(br||bl) return MidPointButterfly<MESH_TYPE>()( ep );
	if(kr==6 && kl==6) return MidPointButterfly<MESH_TYPE>()( ep );
	TRACE("odd vertex among valences of %i %i\n",kl,kr);
	typename MESH_TYPE::CoordType newposl=*vl*.75, newposr=*vr*.75;
	he.FlipV();heStart=he; assert(he.v->P()==*vl);
	int i=0;
	if(kl!=6) 
	do { // compute position  of left vertex
		newposl+= he.VFlip()->P() * Rules[kl][i];
		he.FlipE();he.FlipF();
		++i;
	}	while(he!=heStart);
	i=0;he.FlipV();heStart=he;assert(he.v->P()==*vr);
	if(kr!=6)
	do { // compute position of right vertex
		newposr+=he.VFlip()->P()* Rules[kr][i];
		he.FlipE();he.FlipF();
		++i;
	}	while(he!=heStart);
	if(kr==6) return newposl;
	if(kl==6) return newposr;
	return newposl+newposr;
	}
};

/*
Metodo di Loop dalla documentazione "Siggraph 2000 course on subdivision"

		d4------d3							d4------d3					
	 /	\		 / 	\						 /	\		 / 	\							u			
	/		 \  /  	 \					/		e4--e3 	 \					 / \		
 /	   	\/	 		\				 /	 / 	\/	\		\					/		\		
d5------d1------d2	->	d5--e5--d1--e2--d2			 l--M--r	
 \	   	/\	 		/				 \	 \ 	/\	/		/				  \	  /		
	\		 /  \ 	 /					\		e6--e7	 /					 \ /		
	 \	/		 \ 	/						 \	/		 \ 	/							d			
		d6------d7							d6------d7					

*******************************************************

*/

// Nuovi punti (e.g. midpoint), ossia odd vertices
// 
template<class MESH_TYPE>
struct OddPointLoop : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType>  ep)	{	
		face::Pos<typename MESH_TYPE::FaceType> he(ep.f,ep.z,ep.f->V(ep.z));
		typename MESH_TYPE::CoordType *l,*r,*u,*d;
		l = &he.v->P();
		he.FlipV();
		r = &he.v->P();
		he.FlipE();	he.FlipV();
		u = &he.v->P();
		he.FlipV();	he.FlipE();
		assert(&he.v->P()== r); // back to r
		he.FlipF();	he.FlipE();	he.FlipV();
		d = &he.v->P();

		// abbiamo i punti l,r,u e d per ottenere M in maniera pesata
	
		nv.P()=((*l)*(3.0/8.0)+(*r)*(3.0/8.0)+(*d)*(1.0/8.0)+(*u)*(1.0/8.0));
		// dovrebbe pigliare il vertice medio
	}

}

// vecchi punti, ossia even vertices
template<class MESH_TYPE>
struct EvenPointLoop : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
}


template<class MESH_TYPE>
struct MidPointPlane : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	Plane3<typename MESH_TYPE::ScalarType> pl;
	typedef Point3<typename MESH_TYPE::ScalarType> Point3x;
	
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType> ep){
		Point3x &p0=ep.f->V0(ep.z)->P();
		Point3x &p1=ep.f->V1(ep.z)->P();
		double pp= Distance(p0,pl)/(Distance(p0,pl) - Distance(p1,pl));
		
    nv.P()=p1*pp + p0*(1.0-pp);
	}

	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TCoord2<FL_TYPE,1> WedgeInterp(TCoord2<FL_TYPE,1> &t0, TCoord2<FL_TYPE,1> &t1)
	{
		TCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
};


template <class FLT>
class EdgeSplPlane
{
	public:
  Plane3<FLT> pl;
	bool operator()(const Point3<FLT> &p0, const  Point3<FLT> &p1) const
	{
		if(Distance(pl,p0)>0) {
			if(Distance(pl,p1)>0) return false;
			else return true;
		}
		else if(Distance(pl,p1)<=0) return false;
		return true;
	}
};


template<class MESH_TYPE>
struct MidPointSphere : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	Sphere3<typename MESH_TYPE::ScalarType> sph;
	typedef Point3<typename MESH_TYPE::ScalarType> Point3x;
	
	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType>  ep){
		Point3x &p0=ep.f->V0(ep.z)->P();
		Point3x &p1=ep.f->V1(ep.z)->P();
    nv.P()= sph.c+((p0+p1)/2.0 - sph.c ).Normalize();
	}

	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TCoord2<FL_TYPE,1> WedgeInterp(TCoord2<FL_TYPE,1> &t0, TCoord2<FL_TYPE,1> &t1)
	{
		TCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
};


template <class FLT>
class EdgeSplSphere
{
	public:
  Sphere3<FLT> sph;
	bool operator()(const Point3<FLT> &p0, const  Point3<FLT> &p1) const
	{
		if(Distance(sph,p0)>0) {
			if(Distance(sph,p1)>0) return false;
			else return true;
		}
		else if(Distance(sph,p1)<=0) return false;
		return true;
	}
};

} // namespace vcg




#endif
