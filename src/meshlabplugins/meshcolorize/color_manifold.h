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
Revision 1.1  2006/01/06 11:14:44  giec
Change location meshfilter to meshcolorize.
----------------------------------------------------------------------
----------------------------------------------------------------------

Revision 1.1  2005/12/30 10:15:41  giec
Filter that it colors the edge non manifold



****************************************************************************/
#ifndef __VCGLIB_COLOR_MANIFOLD
#define __VCGLIB_COLOR_MANIFOLD

#include<vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/clean.h>
#include<vcg/space/triangle3.h>
#include<vcg/complex/trimesh/update/topology.h>

#include <iostream>
#include <QtGlobal>

//using std::cout;
//using std::endl;

namespace vcg{

	template<class MESH_TYPE>
		void ColorManifold(MESH_TYPE &m)
	{	
		assert(m.HasFFTopology());

		typename MESH_TYPE::VertexIterator vi;
		typename MESH_TYPE::FaceIterator fi;

		// Test per gli edge manifold
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
		{
			if (!fi->IsD())
			{
				if(!IsManifold(*fi,0)){
					(*fi).V(0)->C().SetRGB(255,0,0);
					(*fi).V(1)->C().SetRGB(255,0,0);
				}
				if(!IsManifold(*fi,1)){
					(*fi).V(1)->C().SetRGB(255,0,0);
					(*fi).V(2)->C().SetRGB(255,0,0);
				}
				if(!IsManifold(*fi,2)){
					(*fi).V(2)->C().SetRGB(255,0,0);
					(*fi).V(0)->C().SetRGB(255,0,0);
				}
			}
		}
	}
}
#endif