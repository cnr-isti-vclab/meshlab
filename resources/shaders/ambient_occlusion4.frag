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

#version 110

uniform sampler3D vTexture;
uniform sampler3D nTexture;
uniform sampler2DShadow dTexture;

uniform vec3 viewDirection;
uniform mat4 mvprMatrix;

uniform float numTexPages;
uniform float viewpSize;
uniform float texSize;

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}

vec4 occlusionQuery(float zLevel, float zLevelMax)
{  
      vec4 R = vec4(0.0, 0.0, 0.0, 1.0);

      float zcoord = zLevel/zLevelMax;
      zcoord += 1.0/(zLevelMax*2.0);

      vec3 c3D = vec3(gl_FragCoord.xy/viewpSize, zcoord);

      vec4 N = texture3D(nTexture, c3D);

	  /*
	  if (N.r < -1.0 || N.r > 1.0)
		  return R;
      */
      vec4 P = project(texture3D(vTexture, c3D)) * (viewpSize/texSize);
      
      if ( shadow2DProj(dTexture, P).r > 0.5 )
         R.r = max(dot(N.xyz, viewDirection), 0.0);
         
      return R;
}

float getPotSize (float npotSize)
{
   float potSize = 0.0;

   if (npotSize <= 2.0)
      potSize = npotSize;

   if (npotSize > 2.0 && npotSize <= 4.0)
      potSize = 4.0;

   if (npotSize > 4.0 && npotSize <= 8.0)
      potSize = 8.0;

   if (npotSize > 8.0 && npotSize <= 16.0)
      potSize = 16.0;
   
   return potSize;
}

void main(void)
{
   float potTexPages = getPotSize(numTexPages);
   float z3D = 0.0;

   gl_FragData[0] = occlusionQuery(z3D, potTexPages);     //1RT
   z3D+=1.0;

   if (z3D < numTexPages)
   {
      gl_FragData[1] = occlusionQuery(z3D, potTexPages);  //2RT
      z3D+=1.0;
   }

   if (z3D < numTexPages)
   {
      gl_FragData[2] = occlusionQuery(z3D, potTexPages);  //3RT
      z3D+=1.0;
   }

   if (z3D < numTexPages)
   {
      gl_FragData[3] = occlusionQuery(z3D, potTexPages);  //4RT
      z3D+=1.0;
   }
}