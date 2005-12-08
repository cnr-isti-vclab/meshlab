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
Revision 1.4  2005/12/08 14:53:36  mariolatronico
changed implementation of Vett type (from array [] [] [] to 3 std::vector nested)

Revision 1.3  2005/12/05 18:47:08  mariolatronico
first try with correct Set and User bit

Revision 1.2  2005/12/02 21:34:21  mariolatronico
correct bounding box coordinate and indexes (idx, idy and idz).
Doesn't work yet, work in progress. Need to check UpdateBoundingBox
and correct set of flags (SetS and IsS)

Revision 1.1  2005/12/01 20:46:43  giec
Test program for clustering decimator.



****************************************************************************/

#ifndef __VCGLIB_DECIMATOR
#define __VCGLIB_DECIMATOR

#include<vcg/complex/trimesh/base.h>

#include <iostream>
#include <math.h>
#include <QtGlobal>
#include <limits>
#define PI 3.14159265

using std::cout;
using std::endl;

namespace vcg{


  /*
    Metodo di clustering
  */
  template<class MESH_TYPE>
  bool Decimator(MESH_TYPE &m, int n)
  {	 
    //int n3 = n*n*n;
    //std::vector <typename MESH_TYPE::CoordType > Vett(n3);
		const int p = n;
		std::vector<MESH_TYPE::CoordType> primaDim(n);
		
		std::vector< std::vector< std::vector< MESH_TYPE::CoordType> > > Vett(n); 	
		 	for(int i = 0 ; i < n ; ++i)
				for(int j = 0 ; j < n ; ++j) {
						Vett[i].resize(n);	
						Vett[i][j].resize(n);
				}

		//Point3f Vett[n][n][n];// = new Point3f[n][n][n];
    typename MESH_TYPE::CoordType Cmin,Cmax;
	//	Cmin.Zero(); Cmax.Zero();
		for (int i = 0; i < 3; i++) {
			Cmin[i] = numeric_limits<float>::max();
			Cmax[i] = numeric_limits<float>::min();
	

		}

		//inizializzo a zero gli elementi
     	for(int i = 0 ; i < n ; ++i)
				for(int j = 0 ; j < n ; ++j)
					for(int k = 0 ; k < n ; ++k)
     				Vett[i][j][k].Zero();
		
		typename MESH_TYPE::VertexIterator vi;
    for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
      {
				Cmin.X() = min(Cmin.X(), (*vi).P().X());
				Cmin.Y() = min(Cmin.Y(), (*vi).P().Y());
				Cmin.Z() = min(Cmin.Z(), (*vi).P().Z());
				Cmax.X() = max(Cmax.X(), (*vi).P().X());
				Cmax.Y() = max(Cmax.Y(), (*vi).P().Y());
				Cmax.Z() = max(Cmax.Z(), (*vi).P().Z());

			}		
		

		//    Cmin = /*Point3f(-0.1,-0.1,-0.1);*/m.bbox.min;
		//    Cmax = /*Point3f(0.1,0.1,0.1);*/m.bbox.max;
	

    if(Cmin > Cmax)
      {
				typename MESH_TYPE::CoordType t = Cmax;
				Cmax=Cmin;
				Cmin = t;
      }
		Point3f tras;
		tras.Zero();
		if ( Cmin.X() < 0.00000001 ) {
			tras.X() =abs( Cmin.X());
		}
		if ( Cmin.Y() < 0.00000001 ) {
			tras.Y() = abs(Cmin.Y());
		}
		if ( Cmin.Z() < 0.00000001 ) {
			tras.Z() = abs(Cmin.Z());
		}

    //Passo di divisione per ogni asse della bounding box
    float Px = (Cmax[0] + tras[0])/n;
    float Py = (Cmax[1] + tras[1])/n;
    float Pz = (Cmax[2] + tras[2])/n;

  	
    int idx,idy,idz;
		int referredBit = MESH_TYPE::VertexType::NewBitFlag();
    //int id;
    //calcolo i nuovi vertici dalla media di quelli di ogni cella
    for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
      {
				
				//calcolo l'indice della cella in cui cade il vertice
				Point3f app = (*vi).P();
				app = (app + tras);
				
				idx = ((app[0]) / Px);
				idy = ((app[1]) / Py);
				idz = ((app[2]) / Pz);
				
				// 			 	std::cout << (*vi).P().X() << " " 
				//  									<< (*vi).P().Y() << " " 
				//  									<< (*vi).P().Z() << " " << endl;
				//			std::cout <<  idx << " " << idy <<" "  << idz <<std::endl;

		
					Vett[idx][idy][idz] = (Vett[idx][idy][idz] + app)/2;
					(*vi).ClearUserBit(referredBit);
					//(*vi).SetUserBit(referredBit);
					//cout << (*vi).Flags() << endl;
      }
		
    typename MESH_TYPE::FaceIterator fi;
    //reimposto i vertici con la media di quelli che cadevano nel suo quadrante
    for(fi = m.face.begin(); fi != m.face.end(); ++fi)
      {
				static int sdfsdf = 0;
				if(!(*fi).IsD())
					{//se non devo cancellare la faccia
						for(int i = 0; i < 3; ++i)
							{//per ogni vertice del triangolo
								if(!(*fi).V(i)->IsUserBit(referredBit))
							
									{//se non l'ho gia cambiato
										
										Point3f app = (*fi).V(i)->P();
										app = (app + tras);
										
										idx = ((app[0]) / Px);
										idy = ((app[1]) / Py);
										idz = ((app[2]) / Pz);
										//lo aggiorno
										//		std::cout << idx << " " << idy <<" "  << idz <<std::endl;
										std::cout << sdfsdf++ << endl;
										
										(*fi).V(i)->P() = Vett[idx][idy][idz];
										//									 	(*fi).V(i)->SetR();
										(*fi).V(i)->SetUserBit(referredBit);
										//cout << (*fi).V(i)->Flags() << endl;

									}
							}
						//calcolo l'area e controllo se uguale a 0
						float a = (*fi).V(0) - (*fi).V(1);
						float b = (*fi).V(1) - (*fi).V(2);
						float c = (*fi).V(2) - (*fi).V(0);
						float p = (a + b + c)/2;
						float area = sqrt( p * (p - a) * (p - b) * (p - c));
						if(abs(area)  < numeric_limits<float>::epsilon() ) {
								(*fi).SetD();
							}
					}
			}

		// std::vector<typename MESH_TYPE::FaceType> fv;
// 		for ( fi = m.face.begin(); fi != m.face.end(); ++fi) {
// 			static int aa = 0;
// 			if (! (*fi).IsD()) {
// 				cout << aa++ << " " << fv.size() << endl;
// 				fv.push_back(*fi);
// 			}
			
// 		}

// 		m.face.clear();
// 		for ( fi = fv.begin(); fi != fv.end(); ++fi) {

// 			m.face.push_back(*fi);

// 		}
		
//		delete[] Vett;		
    return true;

  }

} // namespace vcg

#endif
