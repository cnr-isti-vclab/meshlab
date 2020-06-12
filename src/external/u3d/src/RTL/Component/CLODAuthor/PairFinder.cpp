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
#include <math.h>
#include <float.h>
#include "PairFinder.h"
#define VTXIDX(x) ((x) - vertices)

PairFinder::PairFinder(VertexPairContractor *v, CLODGenerator::Params *pParams)
{
	progressCallback = pParams->progressCallback;
	progressFrequency = pParams->progressFrequency;
	vpc = v;
	thresh = pParams->mergeThresh;
	within = pParams->mergeWithin;
	stack = NULL;
	vtags = NULL;

	if(thresh < (FLT_EPSILON * 1000))
		thresh = FLT_EPSILON * 1000;   // this is a quick fix, should go back and do it right  
									   // (make sure the math can never overflow).
	binsize = thresh;
	threshSquared = thresh * thresh;
	newPairs = 0;
	vertices = vpc->getVertices();
	numVerts = vpc->getNumVerts();
	if(numVerts < 1) return;
	computeBoundingBox();
	xtrans = 0.0f - minx;
	ytrans = 0.0f - miny;
	ztrans = 0.0f - minz;
	if(!within) 
	{	
		initStack();		
		initVtags();	
	}

	initHashTable();
	insertVertices();
}

void PairFinder::initVtags()
{
  	vtags = new int[numVerts];
	
	int i;
	for(i=0;i<numVerts;i++)
			vtags[i] = 0; 
	int id = 1;
	for(i = 0; i<numVerts; i++)
	{
		if(vtags[i] == 0)
		{
			tagConnectedVertices(i, id);  // tag all the vertices in the object
			id++;	// create new id tag.
		}
	}
}

void PairFinder::tagConnectedVertices(int index, int idtag)
{
	int count, newIndex;
	resetStack();
	do
	{
		vtags[index] = idtag; // tag this vertex as visited
		newIndex = index;
		// Find an unvisited neighbor. 
		// Alters index, returns number of neighbors of index that have not been visited.
		count = getNeighbor(&newIndex);

		// if there were no more unvisited neighbors then we need to check the stack
		while( (count < 1)  &&  (sizeStack() != 0) )
		{
			index = popStack();
			newIndex = index;
			count = getNeighbor(&newIndex);
		}
		if (count > 1)
			pushStack(index);		

		index = newIndex;

	} while(count > 0);
}

/**
	Returns the number of unvisited neighbors .
	index param is In and Out, it will be set to neighbor's index.
*/
int PairFinder::getNeighbor(int *index)
{
	Vertex *v = vertices + *index;
	SmallPtrSet *ps = v->GetPairSet();

	U32 SetCtx = 0;
	Pair* p = (Pair*)ps->Begin(SetCtx);
	int notTaggedCount = 0;
	while(notTaggedCount < 2  &&  p)
	{
		int neighborIndex = VTXIDX(p->otherVertex(v));
		if (vtags[neighborIndex] == 0)
		{
			*index = neighborIndex;
			notTaggedCount++;
		}
		p = (Pair*)ps->Next(SetCtx);
	}
	return notTaggedCount;
}

/** 
@todo: Clean this up later, this may have overflow problems when 
the thresh is small and dx is large.	REWrite this using algabreic 
substitution method., would still need to be carefull of tiny dz 
for 2d objects
*/
void PairFinder::initHashTable()
{
	float dx = maxx - minx;
	float dy = maxy - miny;
	float dz = maxz - minz;

	// bins required without tiling
	double xbinsd = ceil((double) dx / (double) binsize);
	double ybinsd = ceil((double) dy / (double) binsize);
	double zbinsd = ceil((double) dz / (double) binsize);
	
	if(xbinsd < 1.0) xbinsd = 1.0;
	if(ybinsd < 1.0) ybinsd = 1.0;
	if(zbinsd < 1.0) zbinsd = 1.0;

	
	double numbins = xbinsd * ybinsd * zbinsd;
	if(numbins > numVerts)	   // want roughly equal number of bins and vertices.
	{  // we should tile the space
		 double bins2verts = numbins / numVerts;
		 double tileFactor;
		 if((zbinsd > 1.0) & (ybinsd > 1.0) & (xbinsd > 1.0))	// verify that it's really 3 dimension
			tileFactor = floor( pow(bins2verts, (1.0/3.0)) );	// take cube root.
		 else  // it's only two or less.
			tileFactor = floor( pow(bins2verts, (1.0/2.0)) );	// take square root.
		 
		 xbins = (int)ceil( xbinsd / tileFactor);
		 ybins = (int)ceil( ybinsd / tileFactor); 
		 zbins = (int)ceil( zbinsd / tileFactor);	 
	} 
	else 
	{
		xbins = (int)xbinsd;
		ybins = (int)ybinsd;
		zbins = (int)zbinsd;
	}
	if(xbins<1) xbins=1;
	if(ybins<1) ybins=1;
	if(zbins<1) zbins=1;

	yzbins = zbins*ybins;
	zsize = (float)zbins;
	yzsize = (float)(ybins * zbins);
	
	tableEntries = xbins*ybins*zbins;
	hashTable = new VertexList*[tableEntries];
	int i;
	for( i=0; i<tableEntries; i++)
		hashTable[i] = NULL;
}

PairFinder::~PairFinder()
{
	delete hashTable;
	delete listElements;
	if(stack) delete stack;
	if(vtags) delete vtags;
}

/**
	go through entire hash table
*/
BOOL PairFinder::findPairs()
{
	// could make faster by segregating edge wrap around cases thus eliminate mod
	int x;
	int y;
	int z;
	for( x=0; x<xbins; x++)
		for( y=0; y<ybins; y++)
			for( z=0; z<zbins; z++) {
				if(vpc->getKill()) 
					return FALSE;
				if(within)
					findPairsAt(x,y,z);	 // find any pair within thresh
				else
					findConnectingPairsAt(x,y,z);   // only create a pair if it connects 
													// formerly unconnected objects.
			}
	return TRUE;
}

/**
	@note
	x,y,z must be in the -1,-1,-1 -> xbins+1, ybins+1, zbins+1 coordinate space (bounding box).
*/
int PairFinder::hash(int x, int y, int z)
{	
	x += xbins;	 // this removes the -1 we get from the wrap around
	y += ybins;
	z += zbins;
	float fx = float(x % xbins);  // wrap around 
	float fy = float(y % ybins);
	float fz = float(z % zbins);
	float rval = (fx * yzsize + fy * zsize + fz);
	return (int) rval;
}

/**
	@note
	x,y,z must be in the models coordinate space
*/
int PairFinder::hash(float x, float y , float z)
{	
	x += xtrans;
	y += ytrans;
	z += ztrans;
	
	// watch out for overflow here
	int ix = (int)floorf( x/binsize );
	int iy = (int)floorf( y/binsize );
	int iz = (int)floorf( z/binsize );
	ix = ix % xbins;
	iy = iy % ybins;
	iz = iz % zbins;
	float slot = float(ix * yzbins + iy * zbins + iz);

	return (int) slot;
}

/**
	look in the 3x3x3 neighborhood centered at xs,ys,zs.
	create new pairs between vertices even if they are in same object.
*/
void PairFinder::findPairsAt(int xs, int ys, int zs)
{
	int ci = hash(xs,ys,zs);  // center index
	VertexList *v1start = hashTable[ci]; // the list of vertices at the center bin
	if(v1start == NULL)
		return;
	// traverse the 3x3x3 array of bins centered at xs,ys,zs
	int x;
	int y;
	int z;
	for( x = xs-1; x <= xs+1; x++)
	{
		for( y = ys-1; y <= ys+1; y++)
		{
			for( z = zs-1; z <= zs+1; z++)
			{
				int i = hash(x,y,z);
				VertexList *v2 = hashTable[i];
				while(v2 != NULL)
				{
					VertexList *v1 = v1start; // v1 not null
					do 
					{
						if(v1->pVert != v2->pVert)
						{
							if(distanceSquared((IV3D*)&(v1->pVert->v), (IV3D*)&(v2->pVert->v)) <= threshSquared)
							{
								vpc->AddPair(v1->pVert, v2->pVert);
								newPairs++;
								if (progressCallback && !(newPairs % progressFrequency)) 
								{
									(*progressCallback)(newPairs, m_pUserData);
									if(vpc->getKill()) return;
								}
							}
						}
						v1 = v1->pNext;
					} while(v1 != NULL);					
					v2 = v2->pNext;
				}
			}
		}
	}
}
					
/**
	look in the 3x3x3 neighborhood centered at xs,ys,zs.
	Create new pairs only if they connect formerly unconnected objects.
*/
void PairFinder::findConnectingPairsAt(int xs, int ys, int zs)
{
	int ci = hash(xs,ys,zs);  // center index
	VertexList *v1start = hashTable[ci]; // the list of vertices at the center bin
	if(v1start == NULL)
		return;
	// traverse the 3x3x3 array of bins centered at xs,ys,zs
	int x;
	int y;
	int z;
	for( x = xs-1; x <= xs+1; x++)
	{
		for( y = ys-1; y <= ys+1; y++)
		{
			for( z = zs-1; z <= zs+1; z++)
			{
				int i = hash(x,y,z);
				VertexList *v2 = hashTable[i];
				while(v2 != NULL)
				{
					VertexList *v1 = v1start; // v1 not null
					do 
					{
						if( (vtags[VTXIDX(v1->pVert)] != vtags[VTXIDX(v2->pVert)]) && (v1->pVert != v2->pVert))
						{
							if(distanceSquared((IV3D*)&(v1->pVert->v), (IV3D*)&(v2->pVert->v)) <= threshSquared)
							{
								vpc->AddPair(v1->pVert, v2->pVert);
								newPairs++;
								if (progressCallback && !(newPairs % progressFrequency)) 
								{
									(*progressCallback)(newPairs, m_pUserData);
									if(vpc->getKill()) return;
								}
							}
						}
						v1 = v1->pNext;
					} while(v1 != NULL);					
					v2 = v2->pNext;
				}
			}
		}
	}
}

void PairFinder::insertVertices()
{
	VertexList *verts = new VertexList[numVerts];
	listElements = verts;

	int i;
	for( i=0; i<numVerts; i++)
	{
	  int index = hash(vertices[i].v.X(), vertices[i].v.Y(), vertices[i].v.Z());

	  // insert at front of list.
	  verts[i].pVert = vertices +i;
	  verts[i].pNext = hashTable[index];
	  hashTable[index] = &(verts[i]);
	}
}

void PairFinder::computeBoundingBox()
{
	Vertex *pv = vertices;
	
	minx = maxx = pv[0].v.X();
	miny = maxy = pv[0].v.Y();
	minz = maxz = pv[0].v.Z();

	int i;
	for( i = 0; i < numVerts; i++) 
	{
		if(pv[i].v.X() < minx) minx = pv[i].v.X();
		if(pv[i].v.X() > maxx) maxx = pv[i].v.X();
		if(pv[i].v.Y() < miny) miny = pv[i].v.Y();
		if(pv[i].v.Y() > maxy) maxy = pv[i].v.Y();
		if(pv[i].v.Z() < minz) minz = pv[i].v.Z();
		if(pv[i].v.Z() > maxz) maxz = pv[i].v.Z();
	}
}
