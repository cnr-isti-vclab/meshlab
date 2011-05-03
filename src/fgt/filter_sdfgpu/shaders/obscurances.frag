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
uniform sampler2D	depthTextureNextBack;
uniform vec3 		viewDirection;
uniform mat4 		mvprMatrix;
uniform float 		viewpSize;
uniform float 		texSize;
uniform	float		depthTolerance;
uniform float		minCos;
uniform float		maxCos;
uniform float		tau;
uniform int 		firstRendering;
uniform float		maxDist;

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}


void main(void)
{
		
    float obscurance = 0.0;
    vec2  coords     = vec2(gl_FragCoord.xy/viewpSize);
    vec4 V 	     = texture2D(vTexture, coords);
    vec4 N 	     = texture2D(nTexture, coords);
 
    N = normalize(N);

    float cosAngle  = max(0.0,dot(N.xyz, viewDirection));

    if( cosAngle > 0.0 )
    {

    	vec4 P = project(V); //* (viewpSize/texSize);
     
    	float zBack = texture2D(depthTextureBack,  P.xy).r;
    	
    	if( firstRendering != 1 )
    	{		 
		float zFront    = texture2D(depthTextureFront,     P.xy).r;
		float zNextBack = texture2D(depthTextureNextBack,  P.xy).r;   

		if ( (zBack+depthTolerance) <=  P.z && P.z <= (zNextBack-depthTolerance) )
		{
 			float dist = max(0.0,(zFront-zBack))*maxDist;
			
			obscurance = max(0.0, 1.0 - exp(-tau*dist))*cosAngle;
			
			
			
    
		}
    	}
    	else if( P.z <= (zBack-depthTolerance) )  
		obscurance = cosAngle;
    
    	
    }
	
    gl_FragColor = vec4(obscurance , obscurance , obscurance , 1.0);
}