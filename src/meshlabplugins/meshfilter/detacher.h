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
Revision 1.4  2006/01/25 21:09:02  giec
Bugfix

Revision 1.3  2006/01/23 21:47:58  giec
Update detucherdialog with the diagonal percentage spinbox.

Revision 1.2  2006/01/17 13:46:35  mariolatronico
spell check threshold, added typename for proper gcc compiling

Revision 1.1  2006/01/17 00:53:49  giec
Detacher function



****************************************************************************/
#ifndef __VCGLIB_DETACHER
#define __VCGLIB_DETACHER

#include<vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include<vcg/space/point3.h>
#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/simplex/face/topology.h>
#include <iostream>
#include <QtGlobal>

namespace vcg{

	template<class MESH_TYPE>
		void Detacher(MESH_TYPE &m, float threshold)
	{	
		typename MESH_TYPE::FaceIterator fi;

		for(fi = m.face.begin(); fi != m.face.end(); ++fi)
		{
			if(!(*fi).IsD())
			{
			
					float d1,d2,d3;
					d1 = Distance<float>((*fi).V(0)->P(),(*fi).V(1)->P());
					d2 = Distance<float>((*fi).V(1)->P(),(*fi).V(2)->P());
					d3 = Distance<float>((*fi).V(2)->P(),(*fi).V(0)->P());
					if(
						( d1 >= threshold )
						||( d2 >= threshold )
						||( d3 >= threshold )	)
					{
						m.fn--;
						(*fi).SetD();
					}
				
			}
		}
	}
}
#endif
