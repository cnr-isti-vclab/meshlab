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
Revision 1.3  2006/03/29 07:30:54  zifnab1974
use fabs instead of abs for floats, needed for gcc 3.4.5

Revision 1.2  2006/01/31 14:44:41  mariolatronico
removed unused and commented old code

Revision 1.1  2006/01/30 20:43:57  giec
Added filter dialog files

Revision 1.11  2006/01/26 16:41:06  giec
Changed the sgnatureof decimatorfunction, added 3 parameter to perform three different subdivision on the three axises.

Revision 1.10  2006/01/03 23:42:11  cignoni
changed clean::IsComplexManifold(m) to the new name clean::IsTwoManifoldFace(m) naming

Revision 1.9  2005/12/22 14:17:44  mariolatronico
Decimator returned the number of eliminated vertices

Revision 1.8  2005/12/19 16:10:35  mariolatronico
slightly changed the algorithm , inserted some debug information (IsComplexManifold).

WARNING: the Decimator procedure gives non 2-manfiold meshes actually

Revision 1.7  2005/12/12 20:26:52  giec
new version with real average point

Revision 1.6  2005/12/09 20:53:49  giec
Change vetor<vetor<vetor<Point3f>>> to Point3f***.
Semplify the mesh but with a litter error.

****************************************************************************/

#ifndef __VCGLIB_DECIMATOR
#define __VCGLIB_DECIMATOR

#include<vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include<vcg/space/triangle3.h>
#include<vcg/complex/trimesh/update/topology.h>

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
  int Decimator(MESH_TYPE &m, int Xn, int Yn, int Zn)
  {	 
		typedef struct media_struct
		{
			bool init; // true se trovo il puntatore vp
			Point3f accum;
			int num;//numero di vertici sommati
			typename MESH_TYPE::VertexType *vp; // puntatore al vertice 
		}structMedia;
    
		structMedia ***Vett;
		Vett = new structMedia **[Xn];
		
		for(int i = 0; i < Xn; ++i)
		{
			Vett[i] = new structMedia *[Yn];
				for(int j = 0; j < Yn; ++j)
			{
				Vett[i][j] = new structMedia[Zn];
				for(int z = 0 ; z < Zn ; ++z)
				{
					Vett[i][j][z].accum.Zero();
					Vett[i][j][z].num = 0;
					Vett[i][j][z].init = false;
					Vett[i][j][z].vp = 0;

				}
			}
		}

	 typename MESH_TYPE::CoordType Cmin,Cmax;
	 Cmax = m.bbox.max;
	 Cmin = m.bbox.min;
	 
	 Point3f tras = Cmin;
	 tras.Zero();
	 if ( Cmin.X() < 0.00000001 ) {
		 tras.X() = fabs( Cmin.X());
	 }
	 if ( Cmin.Y() < 0.00000001 ) {
		 tras.Y() = fabs(Cmin.Y());
	 }
	 if ( Cmin.Z() < 0.00000001 ) {
		 tras.Z() = fabs(Cmin.Z());
	 }
	 
	 //Passo di divisione per ogni asse della bounding box
	 float Px = (Cmax[0] + tras[0])/Xn;
	 float Py = (Cmax[1] + tras[1])/Yn;
	 float Pz = (Cmax[2] + tras[2])/Zn;
	 
	 
	 int idx,idy,idz;
	 float x=0.0f,y=0.0f,z=0.0f;
	 int referredBit = MESH_TYPE::VertexType::NewBitFlag();
	 typename MESH_TYPE::VertexIterator vi;
	 //calcolo i nuovi vertici dalla media di quelli di ogni cella
	 for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
		 {
			 
			 //calcolo l'indice della cella in cui cade il vertice
			 Point3f app = (*vi).P();
			 app = (app + tras);
			 x=app[0];
			 y=app[1];
			 z=app[2];
			 
			 idx = (floor(x / Px));
			 idy = (floor(y / Py));
			 idz = (floor(z / Pz));
			 
			 if(idx > Xn-1)idx = Xn-1;
			 if(idy > Yn-1)idy = Yn-1;
			 if(idz > Zn-1)idz = Zn-1;
			 
			 Vett[idx][idy][idz].accum += (app - tras);
			 Vett[idx][idy][idz].num++;
			 
			 (*vi).ClearUserBit(referredBit);

			}

		float area = -0.00000001f;
    typename MESH_TYPE::FaceIterator fi;
    //reimposto i vertici con la media di quelli che cadevano nel suo quadrante
    for(fi = m.face.begin(); fi != m.face.end(); ++fi)
      {
				if(!(*fi).IsD())
					{//se non devo cancellare la faccia
						for(int i = 0; i < 3; ++i)
							{//per ogni vertice del triangolo
								if(!(*fi).V(i)->IsUserBit(referredBit))
									{//se non l'ho gia cambiato
										
										Point3f app = (*fi).V(i)->P();
										app = (app + tras);
										x=app[0];
										y=app[1];
										z=app[2];
										
										idx = (floor(x / Px));
										idy = (floor(y / Py));
										idz = (floor(z / Pz));
										//
										if(idx > (Xn - 1)) idx = Xn - 1;
										if(idy > (Yn - 1)) idy = Yn - 1;
										if(idz > (Zn - 1)) idz = Zn - 1;
																

										if(Vett[idx][idy][idz].init)
										{
											(*fi).V(i)=(Vett[idx][idy][idz].vp);
										}
										else
										{
											(*fi).V(i)->P() = (Vett[idx][idy][idz].accum / Vett[idx][idy][idz].num);
											Vett[idx][idy][idz].vp = (*fi).V(i);
											Vett[idx][idy][idz].init = true;
										}
										(*fi).V(i)->SetUserBit(referredBit);

									}
							}
						for(int idf =0 ;idf <3;++idf)
						{
							if(((*fi).V(idf) == (*fi).V1(idf)) || ((*fi).V(idf) == (*fi).V2(idf)))
								(*fi).SetD();
						}
					}
			}
		int vertexDeleted = tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
		tri::Clean<CMeshO>::RemoveZeroAreaFace(m);
		tri::UpdateTopology<MESH_TYPE>::VertexFace(m);
		tri::UpdateTopology<MESH_TYPE>::FaceFace(m);
		if (tri::Clean<CMeshO>::IsTwoManifoldFace(m))
		for(int i = 0; i < Xn; ++i)
		{
			for(int j = 0; j < Yn; ++j)
			{
				delete[] Vett[i][j];
			}
			delete[] Vett[i];
		}

		delete[] Vett;
	

    return vertexDeleted;

  }

} // namespace vcg

#endif
