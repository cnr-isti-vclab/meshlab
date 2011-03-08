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
uniform sampler2DShadow depthTextureFront;
uniform sampler2DShadow depthTextureBack;
uniform vec3 		viewDirection;
uniform mat4 		mvprMatrix;
uniform float 		viewpSize;
uniform float 		texSize;

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}


void main(void)
{
		
    float sdf = 0.0;

    vec2 coords = vec2(gl_FragCoord.xy/viewpSize);

    vec4 V = texture2D(vTexture, coords);
    vec4 N = texture2D(nTexture, coords);
     
    vec4 P = project(V * (viewpSize/texSize));
     
    P = P / P.w;

    float zFront = shadow2D(depthTextureFront, P.xyz).r;          

   /* if ( zFront  <=  P.z  )
    {

	float zBack  = shadow2D(depthTextureBack,  P.xyz).r;
	sdf = (zBack-zFront)*max(0.0,dot(viewDirection,N.xyz));
    }*/

    sdf = dot(viewDirection,N.xyz));

    gl_FragColor =  vec4(sdf,sdf,sdf,1.0);
 
}