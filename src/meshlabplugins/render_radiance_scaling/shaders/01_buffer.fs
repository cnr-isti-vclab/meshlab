/****************************************************************************
* Render Radiance Scaling                                                   *
* Meshlab's plugin                                                          *
*                                                                           *
* Copyright(C) 2010                                                         *
* Vergne Romain, Dumas Olivier                                              *
* INRIA - Institut Nationnal de Recherche en Informatique et Automatique    *
*                                                                           *
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
#version 120
#extension GL_ARB_draw_buffers : enable

varying vec3  normal;
varying vec3  view;
varying float depth;

void main(void) {
  const float eps = 0.01;
  const float foreshortening = 0.4;

  vec3 n = normalize(normal);
  
  float gs  = n.z<eps ? 1.0/eps : 1.0/n.z;

  gs = pow(gs,foreshortening);

  float gx  = -n.x*gs; 
  float gy  = -n.y*gs;

  gl_FragData[0] = vec4(gx,gy,depth,1.0);          
  //gl_FragData[1] = vec4(n,depth);
  gl_FragData[1] = vec4(n,gl_FragCoord.z);
  gl_FragData[2] = gl_Color;
}
