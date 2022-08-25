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
#ifndef PAIRFINDER_DOT_H
#define PAIRFINDER_DOT_H

#include "VertexPairContractor.h"

struct VertexList
{
	Vertex *pVert;
	VertexList *pNext;
};


class PairFinder 
{
private:
	VertexPairContractor *vpc;
	Vertex	   *vertices;
	VertexList **hashTable; ///< 3D hash table, of VertexList pointers
	VertexList  *listElements;	///< big block of lists elements for hash table
	
	// related to preventing intra-object pairs
	int		    *vtags;	///< per vertex object tag.
	int			*stack;
	int         sptr;  ///< stack pointer
	inline int popStack()        { sptr--; return stack[sptr]; };
	inline void pushStack(int i) { stack[sptr] = i; sptr++; };
	inline BOOL sizeStack()     { return sptr; };
	inline void initStack()      { stack = new int[(numVerts)+1]; sptr = 0;};
	inline void resetStack()     { sptr = 0; };


	int	numVerts;
	int	newPairs;
	float thresh;	 ///< any two points within thresh will be paired
	BOOL within;   ///< if true allow vertex merge within same object.
	float threshSquared;
	float minx,miny,minz,maxx,maxy,maxz; ///< bounding box of vertices
	float binsize;
	int xbins, ybins, zbins;  ///< dimensions of hashtable array
	int yzbins;
	float xspan, yspan, zspan;  ///< value range for each dim of table
	float yzsize;  ///< ybins*zbins
	float zsize;   ///< float version of zbins
	float xtrans, ytrans, ztrans;
	int tableEntries;
	CLODGenerator::PROGRESS_CALLBACK progressCallback;
	int   progressFrequency;
	U32	  *m_pUserData;

	void computeBoundingBox();
	void initHashTable();
	void insertVertices();
	void initVtags();
	void tagConnectedVertices(int index, int idtag);
	int getNeighbor(int *index);

	void findConnectingPairsAt(int x, int y, int z);
	void findPairsAt(int x, int y, int z);
	int hash(int x, int y, int z);
	int hash(float x, float y, float z);

public:
	PairFinder(VertexPairContractor *vpc, CLODGenerator::Params *pParams);
	BOOL findPairs();
	~PairFinder();
};

#endif // PAIRFINDER_DOT_H
