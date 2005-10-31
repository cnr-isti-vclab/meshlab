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

#ifndef __VCGLIB_NEW_REFINE
#define __VCGLIB_NEW_REFINE

#include<vcg/complex/trimesh/refine.h>
#include <iostream>

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
		assert(t0.n()== t1.n());
		tmp.n()=t0.n(); 
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
};

// vecchi punti, ossia even vertices
template<class MESH_TYPE>
struct EvenPointLoop : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType>
{
	
};


template<class MESH_TYPE, class EVEN_VERT, class ODD_VERT, class EDGEPRED>
bool RefineOddEvenE(MESH_TYPE &m, EVEN_VERT even, ODD_VERT odd, EDGEPRED ep,bool RefineSelected=false)
{
  1) calcola nuova pos vertici old e memorizza in un vett ausiliairio
  2) invoca RefineE e crea i nuovi vertici nella giusta posizione
  3) assegna ai vecchi vertici (i primi n) le posizioni calcolate al punto 1
}



} // namespace vcg




#endif
