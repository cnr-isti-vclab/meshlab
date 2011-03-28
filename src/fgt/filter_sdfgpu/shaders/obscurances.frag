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

uniform sampler2D 	vTexture;
uniform sampler2D 	nTexture;
uniform sampler2D	depthTextureFront;
uniform sampler2D	depthTextureBack;
uniform vec3 		viewDirection;
uniform mat4 		mvprMatrix;
uniform float 		viewpSize;
uniform float 		texSize;
uniform	float		depthTolerance;
uniform float		minCos;
uniform float		maxCos;
uniform float		tau;
uniform int 		firstRendering;

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}


void main(void)
{
		
    float obscurance = 0.0;
   // float PI 	     = 3.14159265358979323846264;
    vec2  coords     = vec2(gl_FragCoord.xy/viewpSize);
    vec4 V 	     = texture2D(vTexture, coords);
    vec4 N 	     = texture2D(nTexture, coords);
 
    N = normalize(N);

    vec4 P = project(V); //* (viewpSize/texSize);
     
    
    float zFront   = texture2D(depthTextureFront, P.xy).r;
    float zBack    = texture2D(depthTextureBack,  P.xy).r;    
    float cosAngle = max(0.0,dot(N.xyz, viewDirection));
      

    if ( (zFront-depthTolerance) <=  P.z && P.z <= (zFront+depthTolerance) && cosAngle > 0.0)
    {

	if(firstRendering != 0)
		obscurance =  (1.0 - exp(-tau*max(0.0,(zFront-zBack))))*cosAngle;
    
        else obscurance = cosAngle;
    }
  
	
    gl_FragColor = vec4( obscurance , obscurance , obscurance , 1.0);
}