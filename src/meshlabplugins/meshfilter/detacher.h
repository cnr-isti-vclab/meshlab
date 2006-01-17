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
Revision 1.1  2006/01/17 00:53:49  giec
Detacher function



****************************************************************************/
#ifndef __VCGLIB_DETACHER
#define __VCGLIB_DETACHER

#include<vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include<vcg/space/point3.h>
#include<vcg/complex/trimesh/update/topology.h>

#include <iostream>
#include <QtGlobal>

namespace vcg{

float Distanza(Point3f p1, Point3f p2)
{
	float distanza;
	Point3f aux;
	aux = p1 - p2;
	distanza=(float)sqrt((aux[0]*aux[0])+(aux[1]*aux[1])+(aux[2]*aux[2]));
	return distanza;
}


	template<class MESH_TYPE>
		void Detacher(MESH_TYPE &m, float trashold)
	{	
		//assert(m.HasFFTopology());

		typename MESH_TYPE::FaceIterator fi;

		for(fi = m.face.begin(); fi != m.face.end(); ++fi)
		{
			if(!(*fi).IsD())
			{
			
					float d1,d2,d3;
					d1 = Distanza((*fi).V(0)->P(),(*fi).V(1)->P());
					d2 = Distanza((*fi).V(1)->P(),(*fi).V(2)->P());
					d3 = Distanza((*fi).V(2)->P(),(*fi).V(0)->P());
					if(
						( d1 >= trashold )
						||( d2 >= trashold )
						||( d3 >= trashold )	)
					{

						if(!face::IsBorder<MESH_TYPE::FaceType>((*fi),0 ) )
							face::FFDetach<MESH_TYPE::FaceType>((*fi),0);
						if(!face::IsBorder<MESH_TYPE::FaceType>((*fi),1 ) )
							face::FFDetach<MESH_TYPE::FaceType>((*fi),1);
						if(!face::IsBorder<MESH_TYPE::FaceType>((*fi),2 ) )
							face::FFDetach<MESH_TYPE::FaceType>((*fi),2);
						(*fi).SetD();

					}
				
			}
		}
	}
}
#endif