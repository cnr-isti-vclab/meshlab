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
Revision 1.6  2006/01/09 15:39:44  mariolatronico
bugfix : now only old vertices are counted for even pass. Check if the vertices belong to selection and if it is deleted. Commented not working algorithm

Revision 1.5  2005/12/26 16:14:38  mariolatronico
- added commented code for new even pass of Loop's algorithm
- added new parameter to RefineOddEvenE to get another callback
At the moment the SAME callback is used (range from 0-100 % two times)

Revision 1.4  2005/12/22 10:45:59  mariolatronico
Added interpolation between colors

Revision 1.3  2005/12/13 11:01:57  cignoni
Added callback management in standard refine

Revision 1.2  2005/12/08 13:52:01  mariolatronico
added preliminary version of callback. Now it counts only even point on RefineOddEven

Revision 1.1  2005/12/05 15:25:41  mariolatronico
comment out old (and working) version, commented new (and not working) versione

Revision 1.11  2005/11/30 08:48:07  mariolatronico
loop seem now to work !!!

Revision 1.10  2005/11/29 10:57:20  mariolatronico
correct line ending

Revision 1.9  2005/11/28 11:38:07  mariolatronico
first working version

Revision 1.8  2005/11/25 17:34:52  mariolatronico
changed vector declaration from oldVertVec(m.vert.size()) to oldVertVec.

Revision 1.7  2005/11/24 20:23:01  giec
fixed setting even vertex loop

Revision 1.6  2005/11/23 15:56:03  mariolatronico
new version of RefineOddEvenE.
Does not aborts or segfaults but gives incorrect faces.

Revision 1.5  2005/11/21 16:02:28  mariolatronico
work in progress on even iterations

Revision 1.4  2005/11/18 10:59:09  mariolatronico
cleaned RefineEvenOddE. It doesn't work because missed deleted flags

Revision 1.3  2005/11/15 00:01:34  giec
Implemented a loop for reserch the odd vertex to call on his the evenPointLoop function for smoothing the mesh.
Only one problem: the marker function are not implemented on vertexplus,that are a condition for use that algorithm.

Revision 1.2  2005/11/14 15:59:19  mariolatronico
work in progress on RefineEvenOddE for Loop alghoritm

Revision 1.1  2005/11/01 12:59:42  giec
Rename directory "tool" in "test"

Revision 1.4  2005/10/31 17:17:47  cignoni
Sketched the interface  of the odd/even refine function

Revision 1.3  2005/10/26 14:43:32  mariolatronico
on OddPointLoop added control on border case

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

#ifndef __VCGLIB_REFINE_LOOP
#define __VCGLIB_REFINE_LOOP

#include <vcg/complex/trimesh/refine.h>
#include <vcg/space/color4.h>
#include <iostream>
#include <math.h>
#include <QtGlobal>
#include <vcg/container/simple_temporary_data.h>
#define PI 3.14159265
namespace vcg{

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

		if (he.IsBorder()) {
			nv.P() = ((*l)*0.5 + (*r)*0.5);

		}
		else {
			he.FlipE();	he.FlipV();
			u = &he.v->P();
			he.FlipV();	he.FlipE();
			assert(&he.v->P()== r); // back to r
			he.FlipF();	he.FlipE();	he.FlipV();
			d = &he.v->P();
			
			// abbiamo i punti l,r,u e d per ottenere M in maniera pesata
			
			nv.P()=((*l)*(3.0/8.0)+(*r)*(3.0/8.0)+(*d)*(1.0/8.0)+(*u)*(1.0/8.0));
		}
		
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
		//	 	assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
};

// vecchi punti, ossia even vertices
template<class MESH_TYPE>
struct EvenPointLoop : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{

	void operator()(typename MESH_TYPE::CoordType &nP, face::Pos<typename MESH_TYPE::FaceType>  ep)	{	

		face::Pos<typename MESH_TYPE::FaceType> he(ep.f,ep.z,ep.f->V(ep.z));
		typename MESH_TYPE::CoordType *r, *l,  *curr;
		curr = &he.v->P();
		
		if (he.IsBorder()) {//half edge di bordo
			he.FlipV();
			r = &he.v->P();
			he.FlipV();
			assert(&he.v->P()== curr); // back to curr
			he.NextB();
			if (&he.v->P() == curr)
				he.FlipV();
			l = &he.v->P();
			nP = ( *(curr) * (3.0)/(4.0)  + (*l)*(1.0/8.0) + (*r)*(1.0/8.0));
		}
		else {
			// compute valence of this vertex
 			int k = 0; 
			face::Pos<typename MESH_TYPE::FaceType> heStart = he;
 			std::vector<typename MESH_TYPE::CoordType> otherVertVec; 
			if(he.v->IsB())return ;
			do {
				he.FlipV();
				otherVertVec.push_back(he.v->P());
				he.FlipV();
				he.FlipE();	he.FlipF();
				k++;
			}	while(he.f!=heStart.f || he.z!=heStart.z || he.v!=heStart.v);
			//			while(he != heStart);
	
			float beta = 3.0 / 16.0;
			if(k > 3 )
				beta = (1.0/(float)k) * (5.0/8.0 - pow((3.0/8.0 + 0.25 * cos(2*PI/k)),2));

			*curr = *curr * (1 - k * beta) ;
			std::vector<typename MESH_TYPE::CoordType>::iterator iter;
			for (iter = otherVertVec.begin();
					 iter != otherVertVec.end();
					 ++iter) {
				*curr = *curr + (*iter) * beta;

			}
			nP = *curr;
		}
	} // end of operator()

	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<typename MESH_TYPE::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}
	Color4b WedgeInterp(Color4b &c0, Color4b &c1)
	{
		Color4b cc;
		cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TCoord2<FL_TYPE,1> WedgeInterp(TCoord2<FL_TYPE,1> &t0, TCoord2<FL_TYPE,1> &t1)
	{
		TCoord2<FL_TYPE,1> tmp;
		// assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}

};

template<class CoordType> struct EvenParam {
	CoordType sum;
	bool border;
	int k;
} ;



template<class MESH_TYPE,class ODD_VERT, class EVEN_VERT>
bool RefineOddEvenE(MESH_TYPE &m, ODD_VERT odd, EVEN_VERT even,float length, 
										bool RefineSelected=false, CallBackPos *cbOdd = 0, CallBackPos *cbEven = 0)
{	 
	
	// n = numero di vertici iniziali
	int n = m.vn;
	// TD = temporary object for EvenParam structure data on vertex 
	SimpleTempData<MESH_TYPE::VertContainer, 
								 EvenParam<typename MESH_TYPE::CoordType> > TD(m.vert);
	
	// refine dei vertici odd, crea dei nuovi vertici in coda
	Refine< MESH_TYPE,OddPointLoop<MESH_TYPE> > (m, odd, length, RefineSelected, cbOdd);
	// momentaneamente le callback sono identiche, almeno cbOdd deve essere passata
	cbEven = cbOdd;

	
	// --------- BEGIN NEW CODE -------------

	assert (m.vn != n); // odd aggiunge i vertici

	
	//vcg::tri::UpdateTopology<MESH_TYPE>::FaceFace(m);
	vcg::tri::UpdateFlags<MESH_TYPE>::FaceBorderFromFF(m);
	// aggiorno i flag perche' IsB funzioni
	vcg::tri::UpdateFlags<MESH_TYPE>::VertexBorderFromFace (m);
	vcg::tri::UpdateColor<MESH_TYPE>::VertexBorderFlag(m);
	
	// marco i vertici even [ i primi n ] come visitati
	int evenFlag = MESH_TYPE::VertexType::NewBitFlag();
	for (int i = 0; i < n ; i++ ) {
		m.vert[i].SetUserBit(evenFlag);
	}
	
	//// inizializza l'oggetto temporaneo
	//EvenParam<typename MESH_TYPE::CoordType> evenParam;
	//evenParam.sum = MESH_TYPE::CoordType(0,0,0) ; 
	//evenParam.k = 0 ;
	//evenParam.border = false;

	//TD.Start(evenParam); 
	//// calcolo i valori per TD per tutti i vertici vecchi (marcati)
	//typename MESH_TYPE::FaceIterator fi;
	//for (fi = m.face.begin(); fi != m.face.end(); fi++) { //itero facce
	//	for (int i = 0; i < 3; i++) { //itero vert
	//		if ( (*fi).V(i)->IsUserBit(evenFlag) ) { // se e' even ...
	//		qDebug("P: %f\t%f\t%f", (*fi).V(i)->P()[0],(*fi).V(i)->P()[1],(*fi).V(i)->P()[2]);

	//			if ((*fi).V(i)->IsB()) { // ... ed e' di bordo

	//				if ( (*fi).V1(i)->IsB() ) // controlla l'altro vertice di bordo
	//					TD[(*fi).V(i)].sum += (*fi).V1(i)->P() / 8.0;
	//				if ( (*fi).V2(i)->IsB() )
	//          TD[(*fi).V(i)].sum += (*fi).V2(i)->P() / 8.0;

	//				TD[(*fi).V(i)].border = true;
	//				// k non viene usato nel caso del bordo
	//			} else { // non di bordo
	//				
	//				//TD[(*fi).V(i)].sum += ( (*fi).V1(i)->P() + (*fi).V2(i)->P() ) / 2.0;
	//				TD[(*fi).V(i)].sum += (*fi).V1(i)->P()/2;
	//				TD[(*fi).V(i)].sum += (*fi).V2(i)->P()/2;
	//				Point3f sumTemp = TD[(*fi).V(i)].sum;
	//				
	//				TD[(*fi).V(i)].k += 1;
	//				float kTemp = TD[(*fi).V(i)].k;
	//				TD[(*fi).V(i)].border = false;

	//			} // bordo / non bordo
	//		} // e' even ?
	//	} // for vert
	//} // for face
	//	
	//float beta = 0;
	//typename MESH_TYPE::VertexIterator vi;
	//for (vi = m.vert.begin(); vi != m.vert.end(); ++vi) {
	//	if ((*vi).IsUserBit(evenFlag)) { // selezionato
	//		qDebug("P: %f\t%f\t%f", (*vi).P()[0],(*vi).P()[1],(*vi).P()[2]);
	//		if ( ! TD[*vi].border ) { // non di bordo

	//			//beta = ((1.0/(float)TD[*vi].k) * (5.0/8.0 - pow((3.0/8.0 + 0.25 * cos(2*PI/(float)TD[*vi].k)),2)));			
	//			if ( TD[*vi].k > 3 && TD[*vi].k != 6) {
	//				//beta = ((1.0/(float)TD[*vi].k) * (5.0/8.0 - pow((3.0/8.0 + 0.25 * cos(2*PI/(float)TD[*vi].k)),2)));
	//				beta = 3.0 / (8.0 * TD[*vi].k);
	//			}
	//			if (TD[*vi].k == 6) {
	//				(*vi).P() = (*vi).P() * 10.0 / 16.0 + TD[*vi].sum * (1.0 / 16.0);
	//			}	
	//			if ( TD[*vi].k == 3 ) {
	//				beta = 3.0f / 16.0f;
	//				qDebug("b == 3, %f", beta);
	//			}
	//			float kTemp = TD[*vi].k;
	//			Point3f sumTemp = TD[*vi].sum;
	//			if (TD[*vi].k != 6)
	//				(*vi).P() = ( (*vi).P() * (1 - TD[*vi].k * beta) ) + (TD[*vi].sum * beta);
	//			
	//		}
	//		else { // bordo 
	//			assert((*vi).IsB()); // il vertice deve essere di bordo
	//			(*vi).P() = ( (*vi).P() * (3.0 / 4.0f)) + TD[*vi].sum;
	//		}
	//	}
	//}

	//TD.Stop();	

	

	// --------- END	 NEW CODE -------------


	int j = 0;
	typename MESH_TYPE::FaceType::ColorType color[6];  // per ogni faccia sono al piu' tre i nuovi valori 
	// di texture per wedge (uno per ogni edge) 

	typename MESH_TYPE::VertexIterator vi;
	typename MESH_TYPE::FaceIterator fi;
	for (fi = m.face.begin(); fi != m.face.end(); fi++) { //itero facce
		for (int i = 0; i < 3; i++) { //itero vert
			if ( (*fi).V(i)->IsUserBit(evenFlag) && ! (*fi).V(i)->IsD() ) {
				if (RefineSelected && !(*fi).V(i)->IsS() )
					break;
				face::Pos<typename MESH_TYPE::FaceType>aux (&(*fi),i);
				if( MESH_TYPE::HasPerVertexColor() ) {
					(*fi).V(i)->C().lerp((*fi).V0(i)->C() , (*fi).V1(i)->C(),0.5f);
				}

				if (cbEven) {
					(*cbEven)(100 * (float)j / (float)m.fn,"Refining");
					j++;
				}
				even((*fi).V(i)->P(), aux);
			}
		}
	}

	/*if (cbEven) {
		(*cbEven)(100, "Updating topology");
	}
	vcg::tri::UpdateTopology<MESH_TYPE>::FaceFace(m);*/
	return true;
}



} // namespace vcg




#endif



