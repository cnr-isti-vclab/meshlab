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

/*
Author: Andrea Baldacci baldacci85@hotmail.it
*/


#define MEDIAN_SEARCH_ITERATIONS 64
#define EXTRA_VALS 81

uniform sampler2D 	vTexture;
uniform sampler2D 	nTexture;
uniform sampler2D	depthTextureFront;
uniform sampler2D	depthTextureBack;
uniform sampler2D	normalTextureBack;
uniform sampler2D       depthTexturePrevBack;
uniform vec3 		viewDirection;
uniform mat4 		mvprMatrix;
uniform float 		viewpSize;
uniform float 		texSize;
uniform float		minCos;
uniform int		firstRendering;
uniform mat4		mvprMatrixINV;
uniform int 		removeFalse;
uniform int		removeOutliers;
float			extraVals[EXTRA_VALS];



vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}


/*
Recostruct normal at the point of intersection and check for false intersection.
For each ray we check the normal at the point of intersection,
and ignore intersections where the normal at the intersection
points is in the same direction as the point-of-origin
(the same direction is defined as an angle difference less than 90 degrees)
*/
bool isFalseIntersection(vec3 P, vec3 objSpaceNormal)
{

    //normal at intersection point
    vec3 intersectionNormal = texture2D(normalTextureBack, P.xy).rgb;

    //if angles dont differ at least 90 degrees reject intersection
    if(dot(intersectionNormal, objSpaceNormal) > 0.0) return true;

    return false;
}



float calculateSdf(vec3 P, vec3 objSpacePos, vec3 objSpaceNormal)
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
	{
    		sdf = max(0.0,(zBack-zFront) ) ; 
    			
	}
    }//We have are interested in vertices belonging to the "front" depth layer
     //, so we check vertex's depth against the previous layer and the next one
    else if ( zPrevBack <= P.z && P.z <= zBack )
    		sdf =  max(0.0,(zBack-zFront) ) ; 


     if(removeFalse==1)
	if( isFalseIntersection(P,objSpaceNormal) ) return 0.0;	
    

   if( sdf != 0.0 && removeOutliers == 1)
   {

	float mean = 0.0;
	float min_val = 100000.0;
	float max_val = 0.0;

	int index = 0;

	for( float j = -4.0; j <= 4.0; j += 1.0 )
	{
		for( float i = -4.0; i <= 4.0 ; i += 1.0 )
		{
			if(index >= EXTRA_VALS) return 0.0;


			float s =  texture2D(depthTextureBack, P.xy + vec2(j/texSize,i/texSize)).r;
			
			if(s == 1.0) break;
			
			extraVals[index] = s; 	
	
			mean   += extraVals[index];
			min_val = min(extraVals[index],min_val);
			max_val = max(extraVals[index], max_val);

			index++;
					
		}

	}

	index += 1;	

	float trialMedian = mean/float(index);
	
	int counter = 0;
	
	
	for(int n = 0; n < MEDIAN_SEARCH_ITERATIONS; n++)
	{
		for(int j = 0; j < index; j++ )
			if( extraVals[j] > trialMedian ) counter++; 	
	

		if( counter > (index/2) )
		{		
			trialMedian += (max_val - trialMedian)/2.0;
			min_val = trialMedian;
		}
		else if( counter < (index/2) )
		{
			trialMedian -= (trialMedian - min_val)/2.0;
			max_val = trialMedian;
		}
		else
		    break;
		

		counter = 0;
	}

	
	sdf = max(0.0, trialMedian - zFront);
  
    }   


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
     
	sdf =  calculateSdf( P.xyz , V.xyz, N.xyz ) * cosAngle;
    }
    

    if(sdf == 0.0) cosAngle = 0.0;
      

	
    gl_FragData[0] = vec4(sdf, cosAngle , 0.0, 1.0);
    gl_FragData[1] = vec4(viewDirection,1.0) * sdf;
}
