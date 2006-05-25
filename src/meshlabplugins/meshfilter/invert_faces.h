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
// History

/*
 * $Log$
 * Revision 1.4  2006/05/25 04:57:45  cignoni
 * Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
 * Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.
 *
 * Revision 1.3  2006/05/06 17:05:18  mariolatronico
 * - Added license and history
 * - On invert face also swap the texture coordinates if the
 *   mesh has them
 *
 */

#ifndef __VCGLIB_INVERT_FACES
#define __VCGLIB_INVERT_FACES

#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/update/normal.h>
#include <algorithm>

#include <iostream>
#include <QtGlobal>

// $Log, invert_faces.h

namespace vcg{

	template<class MESH_TYPE>
		void InvertFaces(MESH_TYPE &m)
	{	
		typename MESH_TYPE::FaceIterator fi;
		
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
		{				
				swap((*fi).V1(0), (*fi).V2(0));
				// swap also texture coordinates
				if (HasPerWedgeTexture(m)) {
					swap((*fi).WT(0),(*fi).WT(1));
				//	swap((*fi).WT(1),(*fi).WT(2));
				//	swap((*fi).WT(2),(*fi).WT(0));
				}
		}
	}	

} // end of namespace

#endif
