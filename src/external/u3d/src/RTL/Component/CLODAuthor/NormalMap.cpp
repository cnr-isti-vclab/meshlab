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
#include <float.h>
#include "NormalMap.h"

NormalMap::NormalMap(int n) 
{

	// Figure out "reasonable" dimensions for the hash table:
	// Empirically I found the following technique to yield only about ~8% empty cells
	// for n between 1000 and 100,000. Lower bound will by a 3x5 hashtable
	hashWidth = (int) (sqrt( (float)n) / 2.0f);
	hashHeight = (int) (hashWidth / 2.0f);

	if ((hashWidth < 5) || (hashHeight < 3))
	{
		hashWidth = 5;
		hashHeight = 3;
	}

	numCells = hashWidth * hashHeight;
	numNormals = n;

	hashTable = new IntList[numCells];
	polarNormals = new PolarCoord[numNormals];
}

NormalMap::~NormalMap()
{
	// Loop over all of the cells in the hash table and free the lists:
	delete []hashTable;
	delete []polarNormals;
}

void NormalMap::vectorToPolar(IV3D *normal, float *phi, float *theta)
{
	// Convert normals into polar co-ordinates:
	// In general: phi = acos (z/(sqrt(x^2 + y^2 + z^2))) and 
	// theta = atan(y/x).  Since this is an normalized vector, we don't need to
	// do the square root...it's always 1.  Thus:

	if (normal->z > 1.0f) 
	{
		*phi = 0.0f;
	}
	else if (normal->z < -1.0f) 
	{
		*phi = 0.0f;
	}
	else
	{
		*phi = acosf(normal->z);
	}

	*theta = atan2f(normal->x, normal->y);
}

void NormalMap::insertNormals (IV3D *normals)
{
	BOOL duplicateNormal;
	U32 index;
	U32 actualNumNormals = numNormals;
	for (unsigned long n = 0; n < numNormals; n++)
	{
		int row, col;
		vectorToPolar (&normals[n], &polarNormals[n].phi, &polarNormals[n].theta);
		polarToRowColumn (polarNormals[n].phi, polarNormals[n].theta, &row, &col);
		int hashIndex = hash (row, col);
		IntList *list = &hashTable[hashIndex];

		// avoid putting duplicate normals in the hash table, it really slows things down.
		duplicateNormal = FALSE;
		if( list->GetFirst() )
			do 
			{
				index = list->GetCurrentData();
				if(normals[n].x == normals[index].x &&
					normals[n].y == normals[index].y &&
					normals[n].z == normals[index].z)

					duplicateNormal = TRUE;

			} while (!duplicateNormal && list->GetNext());

		if(!duplicateNormal)
			list->Push(n);
		else
			actualNumNormals--;
	}
}

void NormalMap::nearest (IV3D *normal, unsigned long *index, float *distance)
{
	float phi, theta;
	int row, col; // , initialRow, initialCol;
	vectorToPolar (normal, &phi, &theta);
	polarToRowColumn (phi, theta, &row, &col);

//	initialRow = row;
//	initialCol = col;
	// Find neighboring cells:
	int left = col - 1;
	int right = col + 1;
	int up = row - 1;
	int down = row + 1;

	float			closestDistance = FLT_MAX;  // Infinity
	float			testDistance;
	unsigned long	closest = 0;
	int				emptyNeighbors = TRUE;
	int r,c;

	for (r = up; r <= down; r++)
	{
		// Determine row:
		row = r;
		if (row < 0) 
			row += hashHeight;
		row = row % hashHeight;

		for (c = left; c <= right; c++)
		{
			// Determine col:
			col = c;
			if (col < 0) 
				col += hashWidth;
			col = col % hashWidth;

			// Grab a cell indexed by row,col:
			unsigned long hashIndex = hash (row, col);
			IntList *list = &hashTable[hashIndex];

			// Compare against each normal in this cell:
			if (list->GetFirst())
			{	
				do
				{
					unsigned long current = list->GetCurrentData();
					PolarCoord *pc = &polarNormals[current];
					testDistance = polarDistanceSquared (phi, theta, pc->phi, pc->theta);

					if (testDistance < closestDistance)
					{
						closestDistance = testDistance;
						closest = current;
						emptyNeighbors = FALSE;
					}
				} while(list->GetNext());
			}//if list->GetFirst

		}//for c
	}//for r

	if (!emptyNeighbors)
	{
		*index = closest;
		*distance = closestDistance;
		return;
	}

	// Otherwise...we'll search through all of the normals.  This could be less 
	// drastic and we could try more local searches first...I'll try to implement this later.
	// It won't be too much of an issue if you choose good hash dimensions which nicely
	// distribute the normals.
	for (unsigned long n = 0; n < numNormals; n++)
	{
		PolarCoord *pc = &polarNormals[n];
		testDistance = polarDistanceSquared (phi, theta, pc->phi, pc->theta);
		
		if (testDistance < closestDistance)
		{
			closestDistance = testDistance;
			closest = n;
		}
	}//for

	*index = closest;
	*distance = closestDistance;
}

void NormalMap::searchCell( float phi, float theta, int row, int col, 
						    float* closestDistance, unsigned long* closestIndex )
{
	float testDistance;
	// fix up row and col numbers that are beyond range
	if (row < 0) row += hashHeight;
	row = row % hashHeight;
	if (col < 0) col += hashWidth;
	col = col % hashWidth;

	// Grab a cell indexed by row,col:
	unsigned long hashIndex = hash (row, col);
	IntList *list = &hashTable[hashIndex];

	// Compare against each normal in this cell:
	if (list->GetFirst())
	{	
		do
		{
			unsigned long current = list->GetCurrentData();
			PolarCoord *pc = &polarNormals[current];
			testDistance = polarDistanceSquared (phi, theta, pc->phi, pc->theta);

			if (testDistance < *closestDistance)
			{
				*closestDistance = testDistance;
				*closestIndex = current;
			}
		} while(list->GetNext());
	}//if list->GetFirst
}

unsigned long NormalMap::hash (IV3D *normal)
{
	float phi, theta;
	int	row, col;

	vectorToPolar (normal, &phi, &theta);
	polarToRowColumn (phi, theta, &row, &col);
	unsigned long index = hash(row, col);

	return index;
}
