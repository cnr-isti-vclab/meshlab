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
uniform sampler2D       depthTexturePrevBack;
uniform vec3 		viewDirection;
uniform mat4 		mvprMatrix;
uniform float 		viewpSize;
uniform float 		texSize;
uniform float		minCos;
uniform int		firstRendering;
float 			_vals[17];

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}

void InsertionSort(int n) 
{
   int i, j;

   float app;
 
   for (i = 1; i < n; i++)
   {
      app = _vals[i];
 
      for (j = i - 1; (j >= 0) && (_vals[j] > app); j--)
               _vals[j+1] = _vals[j];
 
      _vals[j + 1] = app;
   }
}


float calculateSdf(vec3 P)
{
	
    float sdf = 0.0;

    float zFront    = texture2D(depthTextureFront,    P.xy).r;
    float zBack     = texture2D(depthTextureBack,     P.xy).r;
    float zPrevBack = texture2D(depthTexturePrevBack, P.xy).r; 
    
    //first hit of the ray on the mesh. We compare vertex's depth to the next depth layer (back faces),
    // this technique is called second-depth shadow mapping
    if(firstRendering==1)
    {
    	if (  P.z <= zBack )
    		sdf =  max(0.0,(zBack-zFront) ) ; 
    	
    }//We have are interested in vertices belonging to the "front" depth layer
     //, so we check vertex's depth against the previous layer and the next one
    else if ( zPrevBack <= P.z && P.z <= zBack )
    		sdf =  max(0.0,(zBack-zFront) ) ; 
    

    return sdf;
}

void main(void)
{
		
    float sdf = 0.0;

    vec2 coords = vec2(gl_FragCoord.xy/viewpSize);

    vec4 V = texture2D(vTexture, coords);
    vec4 N = texture2D(nTexture, coords);
    
    N = normalize(N);

    vec4 P = project(V); //* (viewpSize/texSize);
     
   
    float cosAngle  = max(0.0,dot(N.xyz, viewDirection));

    
    if( cosAngle  >= minCos )
    {

	//supersampling, we take the median value only
	_vals[0] = calculateSdf( vec3( P.x		, P.y,       P.z  ) );
	_vals[1] = calculateSdf( vec3( P.x - 10.0/texSize, P.y,       P.z  ) );
	_vals[2] = calculateSdf( vec3( P.x + 10.0/texSize, P.y,       P.z  ) );
	_vals[3] = calculateSdf( vec3( P.x , P.y - 10.0/texSize,      P.z  ) );
	_vals[4] = calculateSdf( vec3( P.x , P.y + 10.0/texSize,      P.z  ) );
	_vals[5] = calculateSdf( vec3( P.x - 10.0/texSize, P.y  - 10.0/texSize,       P.z  ) );
	_vals[6] = calculateSdf( vec3( P.x + 10.0/texSize, P.y  + 10.0/texSize,       P.z  ) );
	_vals[7] = calculateSdf( vec3( P.x + 10.0/texSize, P.y - 10.0/texSize,      P.z  ) );
	_vals[8] = calculateSdf( vec3( P.x - 10.0/texSize, P.y + 10.0/texSize,      P.z  ) );

	_vals[9] = calculateSdf( vec3( P.x - 20.0/texSize, P.y,       P.z  ) );
	_vals[10] = calculateSdf( vec3( P.x + 20.0/texSize, P.y,       P.z  ) );
	_vals[11] = calculateSdf( vec3( P.x , P.y - 20.0/texSize,      P.z  ) );
	_vals[12] = calculateSdf( vec3( P.x , P.y + 2.0/texSize,      P.z  ) );
	_vals[13] = calculateSdf( vec3( P.x - 20.0/texSize, P.y  - 20.0/texSize,       P.z  ) );
	_vals[14] = calculateSdf( vec3( P.x + 20.0/texSize, P.y  + 20.0/texSize,       P.z  ) );
	_vals[15] = calculateSdf( vec3( P.x + 20.0/texSize, P.y - 20.0/texSize,      P.z  ) );
	_vals[16] = calculateSdf( vec3( P.x - 20.0/texSize, P.y + 20.0/texSize,      P.z  ) );

	InsertionSort(17);

	sdf = _vals[8] * cosAngle;	
    }
    else
	cosAngle = 0.0;
      

	
    gl_FragColor = vec4( sdf, cosAngle, 0.0, 1.0);
}
