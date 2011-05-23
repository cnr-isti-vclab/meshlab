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
(the same direction is defined as an angle difference lessthan 90)
*/
bool isFalseIntersection(vec3 P, vec3 objSpaceNormal)
{

    //Recostructing the normal at intersection point

    vec2 coordX1 = vec2( P.x - 1.0/texSize, P.y );
    vec2 coordX2 = vec2( P.x + 1.0/texSize, P.y );
    vec2 coordY1 = vec2( P.x , P.y - 1.0/texSize);
    vec2 coordY2 = vec2( P.x , P.y + 1.0/texSize);

    if(coordX1.x < 0.0 ) coordX1.x = 0.0;
    if(coordX2.x > 1.0 ) coordX2.x = 1.0;
    if(coordY1.y < 0.0 ) coordY1.y = 0.0;
    if(coordY2.y > 1.0 ) coordY2.y = 1.0;

    float sampleX1 = texture2DLod(depthTextureBack, coordX1, 0).r;
    float sampleX2 = texture2DLod(depthTextureBack, coordX2, 0).r;
    float sampleY1 = texture2DLod(depthTextureBack, coordY1, 0).r;
    float sampleY2 = texture2DLod(depthTextureBack, coordY2, 0).r;
     
    vec3  x1 = vec3( coordX1.x, coordX1.y, sampleX1 );
    vec3  x2 = vec3( coordX2.x, coordX2.y, sampleX2 );
    vec3  y1 = vec3( coordY1.x, coordY1.y, sampleY1 );
    vec3  y2 = vec3( coordY2.x, coordY2.y, sampleY2 );
    
    //
    vec3 intersectionPointNormal = normalize(cross((x2-x1),(y2-y1)));//  * 2.0 -0.5;
  
    vec4 tmpNormal = vec4( intersectionPointNormal, 0.0 );

    //from texture space to object space
    tmpNormal = mvprMatrixINV * tmpNormal ;

    intersectionPointNormal = tmpNormal.xyz; 
    
   // intersectionPointNormal = normalize(intersectionPointNormal);
    
    //if angles dont differ at least 90 degrees reject intersection
    if(dot(-intersectionPointNormal, objSpaceNormal) > 0.0) return true;

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
   	vec2 V = dir.xy*1.41422f;	
        
	//
	float cotan = sqrt(length(dir)*length(dir) - dir.z*dir.z) / (dir.z);
	
	float coordStepSize = StepSize * cotan;
	
	vec2  coordOffset = vec2(0.0,0.0);
	
	
	while(StepIndex < LinearSearchSteps)
	{
		Depth 		+= StepSize;
		coordOffset 	+= V.xy * coordStepSize;
		
		vec2 tc = P + coordOffset;
		
		CurrD = texture2DLod( depthTextureBack, tc, 0 ).r;

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

    float zFront    = texture2DLod(depthTextureFront,    P.xy, 0).r;
    float zBack     = texture2DLod(depthTextureBack,     P.xy, 0).r;
    float zPrevBack = texture2DLod(depthTexturePrevBack, P.xy, 0).r; 

    
    	

    //first hit of the ray on the mesh. We compare vertex's depth to the next depth layer (back faces),
    // this technique is called second-depth shadow mapping
    if(firstRendering==1)
    {
    	if (  P.z <= zBack )
	{
    		sdf =  max(0.0,(zBack-zFront) ) ; 
    			
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

	float valids   = EXTRA_RAYS - i;
	int median     = int(valids / 2.0);
	int percentile = int(valids / 10.0);

		
	if( sdf < _vals[i+median-4*percentile ] || sdf > _vals[i+median+4*percentile] ) return 0.0;	

      //  sdf = _vals[i+median];

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
