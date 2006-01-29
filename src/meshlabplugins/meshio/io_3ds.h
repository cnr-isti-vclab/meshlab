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

#ifndef __VCGLIB_IOTRIMESH_IO_3DS
#define __VCGLIB_IOTRIMESH_IO_3DS


#include<wrap/callback.h>

namespace vcg {
namespace tri {
namespace io {

  
class _3dsInfo
{
public:

  _3dsInfo()
  {
    mask=-1;
    cb=0;
  }

	/// It returns a bit mask describing the field preesnt in the ply file
  int mask;  

  /// a Simple callback that can be used for long obj parsing. 
  // it returns the current position, and formats a string with a description of what th efunction is doing (loading vertexes, faces...)
  CallBackPos *cb;

  /// number of vertices
  int numVertices;
 
	/// number of triangles
	int numTriangles;
	
	/// number of submeshes
	int numMeshes;

}; // end class
} // end namespace tri
} // end namespace io
} // end namespace vcg
#endif
