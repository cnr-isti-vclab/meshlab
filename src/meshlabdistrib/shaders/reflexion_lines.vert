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
// reflexion_lines.vert: Vertex shader for rendering with reflexion lines
//
// author: Gael Guennebaud
//

varying vec3 EyePos;
varying vec3 Normal;
void main(void)
{
  //gl_Position = ftransform();
  gl_Position  =  gl_ModelViewProjectionMatrix * gl_Vertex;
  Normal = normalize(vec3(gl_NormalMatrix * gl_Normal));
  vec4 position = gl_ModelViewMatrix * gl_Vertex;
  EyePos = position.xyz;
}
