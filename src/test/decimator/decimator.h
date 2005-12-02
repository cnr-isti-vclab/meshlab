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
#define PI 3.14159265

using std::cout;
using std::endl;

namespace vcg{


  /*
    Metodo di clustering
  */

  template<class MESH_TYPE>
  bool Decimator(MESH_TYPE &m,int n)
  {	 
    int n3 = n*n*n;
    //std::vector <typename MESH_TYPE::CoordType > Vett(n3);

    Point3f Vett[n][n][n];
    typename MESH_TYPE::CoordType Cmin,Cmax;
		Cmin.Zero(); Cmax.Zero();
    int i,j,k;
    //inizializzo a zero gli elementi
     	for(i = 0 ; i < n ; ++i)
				for(j = 0 ; j < n ; ++j)
					for(k = 0 ; k < n ; ++k)
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
    //int id;
    //calcolo i nuovi vertici dalla media di quelli di ogni cella
    for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
      {
				
				//calcolo l'indice della cella in cui cade il vertice
				Point3f app = (*vi).P();
				app = (app + tras);
				
				idx = ((app[0]) / Px)+1;
				idy = ((app[1]) / Py)+1;
				idz = ((app[2]) / Pz)+1;

				// 		idx = (int) (*vi).P()/ Px;
				// 		idy = (int) (*vi).P() / Px;
				// 		idz = (int) (*vi).P() / Px;
				//id = idx + (idy * n)+ (idz * n * n);
// 			 	std::cout << (*vi).P().X() << " " 
//  									<< (*vi).P().Y() << " " 
//  									<< (*vi).P().Z() << " " << endl;
				//			std::cout <<  idx << " " << idy <<" "  << idz <<std::endl;

		
				//		Vett[id] = (Vett[id] + (*vi).P())/2;
					Vett[idx][idy][idz] = (Vett[idx][idy][idz] + (*vi).P())/2;
      }
		//	std::cout << "Puppa -------------------------" << endl;
    typename MESH_TYPE::FaceIterator fi;
    //reimposto i vertici con la media di quelli che cadevano nel suo quadrante
    for(fi = m.face.begin(); fi != m.face.end(); ++fi)
      {
				static int sdfsdf = 0;
				if(!(*fi).IsD())
					{//se non devo cancellare la faccia
						for(i = 0; i < 3; ++i)
							{//per ogni vertice del triangolo
								//if(! (*fi).V(i)->IsS())
								if ((*fi).V(i)->Flags() != 32)
									{//se non l'ho gia cambiato
										//					id = idx + (idy * n)+ (idz * 2 * n);
										
										Point3f app = (*fi).V(i)->P();
										app = (app + tras);
										
										idx = ((app[0]) / Px)+1;
										idy = ((app[1]) / Py)+1;
										idz = ((app[2]) / Pz)+1;
										//lo aggiorno
										//		std::cout << idx << " " << idy <<" "  << idz <<std::endl;
										//										std::cout << sdfsdf++ << endl;
										
										(*fi).V(i)->P() = Vett[idx][idy][idz];
										(*fi).V(i)->SetS();
										cout << (*fi).V(i)->Flags() << endl;

									}
							}
						//calcolo l'area e controllo se uguale a 0
						float a = (*fi).V(0) - (*fi).V(1);
						float b = (*fi).V(1) - (*fi).V(2);
						float c = (*fi).V(2) - (*fi).V(0);
						float p = (a + b + c)/2;
						float area = sqrt( p * (p - a) * (p - b) * (p - c));
						if(abs(area)  < 0.00000001f ) {
							(*fi).SetD();
		
						}
					}
      }

		std::vector<typename MESH_TYPE::FaceType> fv;
		for ( fi = m.face.begin(); fi != m.face.end(); ++fi) {
			static int aa = 0;
			if (! (*fi).IsD()) {
				cout << aa++ << " " << fv.size() << endl;
				fv.push_back(*fi);
			}
			
		}

		m.face.clear();
		for ( fi = fv.begin(); fi != fv.end(); ++fi) {

			m.face.push_back(*fi);

		}
		
		
    return true;

  }

} // namespace vcg

#endif
