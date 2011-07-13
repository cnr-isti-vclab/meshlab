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

#define EXTRA_RAYS 40

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
uniform vec3		coneRays[EXTRA_RAYS];
float			_vals[EXTRA_RAYS];



vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}

//Sort values
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

}

//Based on the paper "Dynamic Parallax Occlusion Mapping with Approximate Soft Shadows" Natalya Tatarchuk ATI Research, Inc.
float TraceRay(vec2 P, vec3 dir, float sourceHeight)
{
	
	int LinearSearchSteps = 256; 
	
	float  StepSize       = 1.0/256.0;
	float  Depth 	      = sourceHeight; //Initial depth is that of front vertex
        int    StepIndex      = 0;
	
	float  CurrD          = 0.0;
	float  PrevD          = sourceHeight;
	
	vec2   p1 	      = vec2(0.0);
  	vec2   p2 	      = vec2(0.0);

	// Compute max displacement:
    vec2 V = dir.xy*1.41422;
        
	//
	float cotan = sqrt(length(dir)*length(dir) - dir.z*dir.z) / (dir.z);
	
	float coordStepSize = StepSize * cotan;
	
	vec2  coordOffset = vec2(0.0,0.0);
	
	
	while(StepIndex < LinearSearchSteps)
	{
		Depth 		+= StepSize;
		coordOffset 	+= V.xy * coordStepSize;
		
		vec2 tc = P + coordOffset;
		
    CurrD = texture2D( depthTextureBack, tc).r;

		if(CurrD < Depth)
		{
			p1 = vec2(Depth, CurrD);
			p2 = vec2(Depth - StepSize, PrevD);
			StepIndex =  LinearSearchSteps;
		}
		else
		{

			//no intersections found, the height map values along the edges is below one. 
			//In this case the parallax vector near the edges crosses the texture boundary without hitting any features
			if(tc.x < 0.0 || tc.x > 1.0 || tc.y < 0.0 || tc.y > 1.0) return 0.0;
		
			StepIndex++;
			PrevD = CurrD;
		}
	}


	//Linear approximation using current and last step
	float d2 = p2.x - p2.y;
	float d1 = p1.x - p1.y;
      
      	float fDenominator = d2 - d1;
	
	if(fDenominator == 0.0)
	{

		return 0.0;
	}
	//y=m0*x+b0 and y=m1*x+b1 intersection
	float destHeight =  (p1.x * d2 - p2.x * d1) / fDenominator;
      
	float sdf = sqrt((destHeight - sourceHeight)* (destHeight - sourceHeight) + length(coordOffset)*length(coordOffset));

      return sdf;

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
	
    
	int i = 0;

	for( ; i < EXTRA_RAYS ; i++ )
	{
		_vals[i] = TraceRay( P.xy, coneRays[i] , zFront );	
	}


	
	InsertionSort(EXTRA_RAYS);


	for( i = 0;  i < EXTRA_RAYS; i++ )
	{
		if(_vals[i] != 0.0 ) break;

	}

 	 float valids     = float(EXTRA_RAYS - i);
	int median     = int(valids / 2.0);
	int percentile = int(valids / 10.0);

		
	//if( sdf < _vals[i+median-4*percentile ] || sdf > _vals[i+median+4*percentile] ) return 0.0;	

	 sdf = _vals[i+median];
	
  
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
      

	
    gl_FragColor = vec4(sdf, cosAngle , 0.0, 1.0);
}
