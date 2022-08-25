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

// vertex to fragment shader io
varying vec3 N;
varying vec3 I;
varying vec4 Cs;
varying vec3 LightDir;

// globals

uniform int useXslice;
uniform int useYslice;
uniform int useZslice;

uniform float Xsliceoff;
uniform float Ysliceoff;
uniform float Zsliceoff;

uniform int onlyXslice;
uniform int onlyYslice;
uniform int onlyZslice;

// entry point
void main()
{
  if(useXslice)
  {
    if (I.x < Xsliceoff)
      discard;
    if (I.x < Xsliceoff+1)
      Cs = vec4(1.0, 0.0, 0.0, 1.0);
	if((onlyXslice)&&(I.x > Xsliceoff+1))
      discard;	
  }	
  
  if(useYslice)
  {
    if (I.y < Ysliceoff)
      discard;
    if (I.y < Ysliceoff+1)
      Cs = vec4(0.0, 1.0, 0.0, 1.0);
	if((onlyYslice)&&(I.y > Ysliceoff+1))
      discard;		  
  }	  
  
  if(useZslice)
  {
    if (I.z < Zsliceoff)
      discard;
    if (I.z < Zsliceoff+1)
      Cs = vec4(0.0, 0.0, 1.0, 1.0);
	if((onlyZslice)&&(I.z > Zsliceoff+1))
      discard;		  
  }	  
  
  float opac = dot(normalize(N), normalize(LightDir));
  if(opac<0)
    opac=abs(opac) / 4.0;
    
	
  gl_FragColor =  opac * Cs;
}
