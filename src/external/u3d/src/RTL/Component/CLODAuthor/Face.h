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
#ifndef FACE_H
#define FACE_H

#include "Primitives.h"
#include "Matrix4x4.h"
#include "GeometryObject.h"

class Pair;
class Vertex;

class Face : public GeometryObject
{
public:
	Face ();

	void Set(Pair *pa, Pair *pb, Pair *pc, int idx);
	BOOL isCounterClockwise (Vertex *x, Vertex *y);

	void remove();
	inline void ReplacePair (Pair *oldPair, Pair *newPair)
	{
		if (a == oldPair) a = newPair;
		else if (b == oldPair) b = newPair;
		else if (c == oldPair) c = newPair;
	};
	
	Pair *a,*b,*c; 
	
	U32 index;

private:
	Matrix4x4 quadric;
	float A, B, C, D;
	float area;
	void computeArea();

public:
	Matrix4x4 &Quadric() {	return quadric; };
	void computeQuadric();
	inline void planeEquation (float &rA, float &rB, float &rC, float &rD)
	{ 
		rA = A;  rB = B;  rC = C;  rD = D; 
	};
	inline float getArea() 
	{ 
		if(area != -1.0) 
		{
			return area; 
		}	
		computeArea();
		return area; 
	};
	inline void  setArea(float a) { area = a; };
	float computeNewArea(Vertex *kv, Vertex *rv);
};

#endif
