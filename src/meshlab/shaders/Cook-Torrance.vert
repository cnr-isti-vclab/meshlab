/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

//
// Cook-Torrance reflectance model (per-pixel)
//
// by
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

void main()
{
	// vertex normal 
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	// vertex position (in eye space)
	vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = ftransform();

} 
