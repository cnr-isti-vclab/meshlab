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
#ifndef NORMALMAP_DOT_H
#define NORMALMAP_DOT_H

#include "IFXSList.h"
#include <math.h>
#include "Primitives.h"

#define NM_HALFPIE	1.5707963f
#define NM_PIE		3.1415927f
#define NM_TWOPIE	6.2831853f

typedef IFXSList<int> IntList;

class PolarCoord
{
public:
	float phi, theta;
};

class NormalMap
{
public:
	NormalMap (int n);
	~NormalMap ();
	void insertNormals (IV3D *normals);
	void nearest (IV3D *normal, unsigned long *index, float *d);

private:
	unsigned long	numNormals;
	int				hashHeight, hashWidth;
	int				numCells;
	IntList*		hashTable;
	PolarCoord*		polarNormals;
	
	void searchCell(float phi, float theta, int row, int col, float* closestDistance, unsigned long* closestIndex);
	// Implementation of the hash function:
	void vectorToPolar(IV3D *normal, float *phi, float *theta);
	inline void polarToRowColumn(float phi, float theta, int *row, int *col);
	inline unsigned long hash (int row, int col);
	unsigned long hash (IV3D *normal);

	inline float polarDistanceSquared(float phi1, float theta1, float phi2, float theta2);
};
	

inline void NormalMap::polarToRowColumn(float phi, float theta, int *row, int *col)
{
	// Scale phi and theta over the range of 0.0 to 1.0:
	phi		/= NM_PIE;
	theta	+= NM_PIE;
	theta	/= NM_TWOPIE;

	// Clamp values:
	if (phi < 0.0f) 	phi = 0.0f;
	if (phi >= 1.0f)	phi = 0.999f;
	if (theta < 0.0f)	theta = 0.0f;
	if (theta >= 1.0f)	theta = 0.999f;

	// Now produce hash index:
	*row = int(phi * hashHeight);
	*col = int(theta * hashWidth);
}


inline unsigned long NormalMap::hash (int row, int col)
{		
	unsigned long hashValue = row * hashWidth + col;
	return hashValue;
}

inline float NormalMap::polarDistanceSquared(float phi1, float theta1, float phi2, float theta2)
{
	float phiDiff = phi1 - phi2;
	float thetaDiff = theta1 - theta2;
	phiDiff *= phiDiff;
	thetaDiff *= thetaDiff;
	
	return (phiDiff + thetaDiff);
}

#endif
