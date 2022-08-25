//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#include "Primitives.h"

void computePlaneEquation (IV3D *v1, IV3D *v2, IV3D *v3, float *A, float *B, float *C, float *D)
{
/** Un-normalized:
	*A = v1->y * (v2->z - v3->z) + v2->y * (v3->z - v1->z) + v3->y * (v1->z - v2->z);
	*B = v1->z * (v2->x - v3->x) + v2->z * (v3->x - v1->x) + v3->z * (v1->x - v2->x);
	*C = v1->x * (v2->y - v3->y) + v2->x * (v3->y - v1->y) + v3->x * (v1->y - v2->y);
	*D = - (v1->x * (v2->y * v3->z - v3->y * v2->z) 
		  + v2->x * (v3->y * v1->z - v1->y * v3->z) 
		  + v3->x * (v1->y * v2->z - v2->y * v1->z));
**/

	// Translate to the origin:
	IV3D tV1, tV2;
	subtract3D (v3, v1, &tV1);	// 3 ops
	subtract3D (v3, v2, &tV2);	// 3 ops

	IV3D vCross;
	crossprod (&tV1, &tV2, &vCross);   // 9 ops
	normalize3D (&vCross);

	
	float d = - (vCross.x * v1->x) - (vCross.y * v1->y) - (vCross.z * v1->z);

	*A = vCross.x;
	*B = vCross.y;
	*C = vCross.z;
	*D = d;
}

inline float vectorlen(IV3D* v)
{
	return (float ) sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
} 

#ifdef cl
#pragma optimize( "", off )
#endif
// Also according to LaGrange:
// area of a triangle = 1/2 ||a x b||

// Had to turn off optimization to get correct results in cases where
// trinagle has zero area.	The slow version is only used to cull the 
// zero area triangles from the initial model.
float triangleAreaSlow (IV3D *v1, IV3D *v2, IV3D *v3)
{	
	IV3D tV1, tV2, tV3;
	IV3D vCross;

	// Translate to the origin:
	subtract3D (v3, v1, &tV1);	// 3 ops
	subtract3D (v3, v2, &tV2);	// 3 ops	
	subtract3D (v1, v2, &tV3);

	// bail out early if vertices are in same position, this helps prevent float limitations from
	// messing up the cross product calculation.
	if(tV3.x == 0.0f && tV3.y == 0.0f && tV3.z == 0.0f)
		return 0.0f;
	if(tV2.x == 0.0f && tV2.y == 0.0f && tV2.z == 0.0f)
		return 0.0f;
	if(tV1.x == 0.0f && tV1.y == 0.0f && tV1.z == 0.0f)
		return 0.0f;

	crossprod (&tV1, &tV2, &vCross);   // 9 ops
	return 0.5f * vectorlen (&vCross);  // ~10 ops
}
#ifdef cl
#pragma optimize( "", on )
#endif

float triangleArea (IV3D *v1, IV3D *v2, IV3D *v3)
{
	IV3D tV1, tV2;
	IV3D vCross;

	// Translate to the origin:
	subtract3D (v3, v1, &tV1);	// 3 ops
	subtract3D (v3, v2, &tV2);	// 3 ops
	
	crossprod (&tV1, &tV2, &vCross);   // 9 ops

	return 0.5f * vectorlen (&vCross);  // ~10 ops
}

// get rid of divide for speed.

// According to LaGrange...Volume of a tetrahedron is:
// 1/6 * |(b x c) . a| when the 4th vertex is the origin.
float tetrahedronVolume (IV3D *v1, IV3D *v2, IV3D *v3, IV3D *v4)
{
	IV3D tV1, tV2, tV3;
	IV3D vCross;

	// Translate to the origin:
	subtract3D (v4, v1, &tV1);
	subtract3D (v4, v2, &tV2);
	subtract3D (v4, v3, &tV3);

	// Solve for volume...yeah, this actually works:
	crossprod (&tV1, &tV2, &vCross);
	float dot = dotProduct3D (&vCross, &tV3);
	float volume = 1.0f / 6.0f * dot; 

	return volume;
}
