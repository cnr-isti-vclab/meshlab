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
#ifndef PRIMITIVES_DOT_H
#define PRIMITIVES_DOT_H

#include <math.h>
#include "IFXResult.h"
#include "IFXDataTypes.h"

typedef struct IV3D_TAG 
{
	F32 x;
	F32 y;
	F32 z;
} IV3D;

#define PIE 3.141592653589793f

inline void crossprod(IV3D* u, IV3D* v, IV3D *c)
{
	c->x = u->y*v->z - u->z*v->y;
	c->y = u->z*v->x - v->z*u->x;
	c->z = u->x*v->y - u->y*v->x;
} /* crossprod */

inline float distance3D (IV3D *v1, IV3D *v2)
{
	float xDiff, yDiff, zDiff, distance;
	double distanceSquared;

	xDiff = v1->x - v2->x;
	yDiff = v1->y - v2->y;
	zDiff = v1->z - v2->z;

	//converting all float type data to double to improve calculation accuracy.
	distanceSquared = (double)xDiff*(double)xDiff + (double)yDiff*(double)yDiff + (double)zDiff*(double)zDiff;
	distance = (float)sqrt(distanceSquared);

	return distance;
} 


inline float distanceSquared(IV3D *v1, IV3D *v2)
{
	float d1,d2,d3;

	d1 = v1->x - v2->x;
	d1 *= d1;
	d2 = v1->y - v2->y;
	d2 *= d2;
	d3 = v1->z - v2->z;
	d3 *= d3;
	return(d1 + d2 + d3);
}


inline float normalize3D (IV3D *v1)
{
	IV3D origin = {0.0f, 0.0f, 0.0f};
 	
	float distance = distance3D(v1, &origin);

	if (0.0 != distance)
	{
		v1->x /= distance;
		v1->y /= distance;
		v1->z /= distance;
	}

	return (distance);
}

inline void subtract3D (IV3D *v1, IV3D *v2, IV3D *v3)
{
	v3->x = v1->x - v2->x;
	v3->y = v1->y - v2->y;
	v3->z = v1->z - v2->z;
}

inline void add3D (IV3D *v1, IV3D *v2, IV3D *v3)
{
	v3->x = v1->x + v2->x;
	v3->y = v1->y + v2->y;
	v3->z = v1->z + v2->z;
}

inline void scalarMultiply3D (IV3D *v1, float scalar, IV3D *v2)
{
	v2->x = v1->x * scalar;
	v2->y = v1->y * scalar;
	v2->z = v1->z * scalar;
}

inline float dotProduct3D (IV3D *v1, IV3D *v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}


// Function prototypes:
float vectorlen(IV3D* v);
void computePlaneEquation (IV3D *v1, IV3D *v2, IV3D *v3, float *A, float *B, float *C, float *D);
float triangleArea (IV3D *v1, IV3D *v2, IV3D *v3);
float triangleAreaSlow (IV3D *v1, IV3D *v2, IV3D *v3);
float tetrahedronVolume (IV3D *v1, IV3D *v2, IV3D *v3, IV3D *v4);

#endif
