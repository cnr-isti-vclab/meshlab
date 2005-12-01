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
Revision 1.1  2005/12/01 20:46:43  giec
Test program for clustering decimator.



****************************************************************************/

#ifndef __VCGLIB_NEW_REFINE
#define __VCGLIB_NEW_REFINE

#include<vcg/complex/trimesh/refine.h>
#include <iostream>
#include <math.h>
#include <QtGlobal>
#define PI 3.14159265
namespace vcg{


/*
Metodo di clustering
*/

template<class MESH_TYPE>
bool Decimator(MESH_TYPE &m,int n)
{	 
	vector <typename MESH_TYPE::CoordType > Vett(n*n*n);

	typename MESH_TYPE::CoordType Cmin,Cmax;

	int i;
	//inizializzo a zero gli elementi
	for(i=0;i<3*n;++i)
				Vett[i].Zero();
	
	Cmin = m.bbox.min;
	Cmax = m.bbox.max;

	if(Cmin > Cmax)
	{
		typename MESH_TYPE::CoordType t = Cmax;
		Cmax=Cmin;
		Cmin = t;
	}


	//Passo di divisione per ogni asse della bounding box
	float Px = abs((Cmax[0] - Cmin[0]))/n;
	float Py = abs((Cmax[1] - Cmin[1]))/n;
	float Pz = abs((Cmax[2] - Cmin[2]))/n;

	typename MESH_TYPE::VertexIterator vi;

	int idx,idy,idz;
	int id;
	//calcolo i nuovi vertici dalla media di quelli di ogni cella
	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
	{
		//calcolo l'indice della cella in cui cade il vertice
		Point3f app = (*vi).P();

		idx = (int) app[0] / Px;
		idy = (int) app[1] / Py;
		idz = (int) app[2] / Pz;

		//idx = (int)  (*vi).P()/ Px;
		//idy = (int) (*vi).P() / Px;
		//idz = (int) (*vi).P() / Px;
		id = idx + (idy * n)+ (idz * 2 * n);
		Vett[id] = (Vett[id] + (*vi).P())/2;
	}

	typename MESH_TYPE::FaceIterator fi;
	//reimposto i vertici con la media di quelli che cadevano nel suo quadrante
	for(fi = m.face.begin(); fi != m.face.end(); ++fi)
	{
		if(!(*fi).IsD())
		{//se non devo cancellare la faccia
			for(i =0; i<3;++i)
			{//per ogni vertice del triangolo
				if(! (*fi).V(i)->IsS())
				{//se non l'ho gia cambiato
					idx = (int) (*fi).V(i)->P().V(0) / Px;
					idy = (int) (*fi).V(i)->P().V(1) / Px;
					idz = (int) (*fi).V(i)->P().V(2) / Px;
					id = idx + (idy * n)+ (idz * 2 * n);
					//lo aggiorno
					(*fi).V(i)->P() = Vett[id];
					(*fi).V(i)->SetS();
				}
			}
		//calcolo l'area e controllo se uguale a 0
		float a = (*fi).V(0) - (*fi).V(1);
		float b = (*fi).V(1) - (*fi).V(2);
		float c = (*fi).V(2) - (*fi).V(0);
		float p = (a + b + c)/2;
		float area = sqrt( p * (p - a) * (p - b) * (p - c));
		std::cout << area << "\n";
		if(area == 0)(*fi).SetD();
		}
	}

	return true;

}

} // namespace vcg

#endif
