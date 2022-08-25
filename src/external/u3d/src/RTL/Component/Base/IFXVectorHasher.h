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

/**
	@file	IFXVectorHasher.h

			Hash table of IFXVector3 objects.  Hash key is created from the 
			floating point values in the vector.

	@note	Has O(n) performance at best (widely distributed points) and 
			O(n^2) worst case (all vertices are the same position).

*/

#ifndef IFXVECTORHASHER_H
#define IFXVECTORHASHER_H

#include "IFXVector3.h"

//
//  Internally used structs
//

struct IFXVectorHasherVectorNode
{
	U32 meshIndex;
	U32 index;
	IFXVectorHasherVectorNode* pNext;
};

struct IFXVectorHasherBinNode
{
	IFXVector3* pVector;
	IFXVectorHasherVectorNode* pFront;
	IFXVectorHasherBinNode* pNext;		// next node in this bin
};

/** Iterator used to access nodes in a bin.
Instantiate this class and call IFXVectorHasherBinIterator::GetNodeIterator()
to initialize the node iterator. */
class IFXVectorHasherNodeIterator
{
public:
	IFXVectorHasherNodeIterator();
	~IFXVectorHasherNodeIterator();

	/** This method called by IFXVectorHasherBinIterator::
	GetNodeIterator to initialize iterator. */
	void Initialize(IFXVectorHasherVectorNode* pNode);

	/** Get mesh index and vertex index at current node. */
	void Get(U32* pMeshIndex, U32* pIndex);

	/** Go to the next node.  Returns FALSE if no more nodes. */
	BOOL Next();

private:
	IFXVectorHasherVectorNode* m_pCurrNode;
};



/**  Iterator used to access bins.  A bin contains a set of nodes.
Instantiate this class and call IFXVectorHasher::GetBinIterator()
to initialize the bin iterator. */
class IFXVectorHasherBinIterator
{
public:
	IFXVectorHasherBinIterator();
	~IFXVectorHasherBinIterator();

	/** This method called by IFXVectorHasher::GetBinIterator
	to initialize iterator. */
	void Initialize(IFXVectorHasherBinNode** ppCurrBinList,
					IFXVectorHasherBinNode** ppEndBinList);

	/** Initialize node iterator */
	void GetNodeIterator(IFXVectorHasherNodeIterator* pIter);
		
	/** Go to the next bin node.  Returns FALSE is no more nodes. */
	BOOL Next();

private:
	IFXVectorHasherBinNode** m_ppCurrBinList;
	IFXVectorHasherBinNode** m_ppEndBinList;
	IFXVectorHasherBinNode*	 m_pCurrBin;
};


/**  Hash table for IFXVector3 objects */
class IFXVectorHasher
{
public:
	IFXVectorHasher();
	~IFXVectorHasher();

	/** 
		Allocate hash table based on the number of vectors
		and the bounding box of the vectors.  Will return
		IFX_E_OUT_OF_MEMORY if a memory alloc fails. 
	
		@param numVectors number of vectors.
		@param pMin A pointer to the input value that is a min diagonal pointer.
		@param pMax A pointer to the input value that is a max diagonal pointer.
	*/
	IFXRESULT Initialize(U32 numVectors,
						 IFXVector3* pMin,
						 IFXVector3* pMax);

	/** 
		Add vector to hash table.  Will return
		IFX_E_OUT_OF_MEMORY if a memory alloc fails.
	
		Note: Stores pointer to given vector.  Do not free 
		the vector until this class's destructor is called. 
		
		@param pVector A pointer to the added 3D point.
		@param meshIndex Mesh index.
		@param vertexIndex Vertex index.
	*/
	IFXRESULT AddVector(IFXVector3* pVector, U32 meshIndex, U32 vertexIndex);

	/** 
		Initialize an iterator for accessing the bins.  Each bin
		contains a set of nodes.  Use the node iterator
		to access the individual nodes.  Returns FALSE if no bin nodes. 

		@param pIter A pointer to the output initialized iterator.
	*/
	BOOL GetBinIterator(IFXVectorHasherBinIterator* pIter);

private:
	// Hash the IFXVector3 into a 3D index.  Lookup the bin and
	// return the linked list of collisions.
	IFXVectorHasherBinNode** GetBinList(IFXVector3* pVector);

	// Look for a match in the linked list of collisions.  If not
	// found, add a collision node (IFXVectorHasherBinNode) to the 
	// collision list.  Return the collision node pointer.
	IFXVectorHasherBinNode* GetBinNode(IFXVectorHasherBinNode** ppFront, 
									   IFXVector3* pVector);
	
	// Add a reference of the hashed vector to the collision node's
	// list of instances.
	IFXRESULT AddVectorNode(IFXVectorHasherVectorNode** ppFront, 
		                    U32 meshIndex, U32 index);

	U32 m_dim;			///< dimension N of N x N x N cube 
	U32 m_dimSquared;	///< N * N
	U32 m_dimCubed;		///< N * N * N
	IFXVector3 m_origin;	///< used by hash key algorithm
	IFXVector3 m_scale;		///< used by hash key algorithm
	IFXVectorHasherBinNode** m_ppBins;	///< pointer to 3D array of bins
};

//
//  inlines
//

//
// IFXVectorHasherNodeIterator
//
IFXINLINE IFXVectorHasherNodeIterator::IFXVectorHasherNodeIterator()
{
	m_pCurrNode = NULL;
}

IFXINLINE IFXVectorHasherNodeIterator::~IFXVectorHasherNodeIterator()
{
	// iterator is a visitor, doesn't own the data
}

IFXINLINE void IFXVectorHasherNodeIterator::Initialize(
											IFXVectorHasherVectorNode* pNode)
{
	m_pCurrNode = pNode;
}

IFXINLINE void IFXVectorHasherNodeIterator::Get(U32* pMeshIndex, U32* pIndex)
{
	IFXASSERT(m_pCurrNode);
	IFXASSERT(pMeshIndex);
	IFXASSERT(pIndex);

	if ( (NULL != pMeshIndex) && (NULL != pIndex) && (NULL != m_pCurrNode) )
	{
		*pMeshIndex = m_pCurrNode->meshIndex;
		*pIndex = m_pCurrNode->index;
	}
	else
	{
		if (NULL != pMeshIndex) *pMeshIndex = 0;
		if (NULL != pIndex) *pIndex = 0;
	}
}

IFXINLINE BOOL IFXVectorHasherNodeIterator::Next()
{
	m_pCurrNode = m_pCurrNode->pNext;
	return BOOL(NULL != m_pCurrNode);
}


//
// IFXVectorHasherBinIterator
//
IFXINLINE IFXVectorHasherBinIterator::IFXVectorHasherBinIterator()
{
	m_ppCurrBinList = NULL;
	m_ppEndBinList = NULL;
	m_pCurrBin = NULL;
}

IFXINLINE IFXVectorHasherBinIterator::~IFXVectorHasherBinIterator()
{
	// iterator is a visitor, doesn't own the data
}

IFXINLINE void IFXVectorHasherBinIterator::Initialize(
										IFXVectorHasherBinNode** ppCurrBinList,
										IFXVectorHasherBinNode** ppEndBinList)
{
	IFXASSERT(ppCurrBinList);
	IFXASSERT(ppEndBinList);

	if ( (NULL != ppCurrBinList) && (NULL != ppEndBinList) )
	{
		IFXASSERT(*ppCurrBinList);

		if (NULL != *ppCurrBinList)
		{
			m_ppCurrBinList = ppCurrBinList;
			m_ppEndBinList = ppEndBinList;
			m_pCurrBin = *ppCurrBinList;
		}
	}
}

IFXINLINE void IFXVectorHasherBinIterator::GetNodeIterator(
											IFXVectorHasherNodeIterator* pIter)
{
	IFXASSERT(pIter);

	if (NULL != pIter)
	{
		pIter->Initialize(m_pCurrBin->pFront);
	}
}
	
IFXINLINE BOOL IFXVectorHasherBinIterator::Next()
{
	m_pCurrBin = m_pCurrBin->pNext;

	if (NULL == m_pCurrBin)
	{
		while(++m_ppCurrBinList < m_ppEndBinList  &&  *m_ppCurrBinList == NULL)
		{
		}

		if (m_ppCurrBinList < m_ppEndBinList)
			m_pCurrBin = *m_ppCurrBinList;
	}

	return BOOL(NULL != m_pCurrBin);
}

#endif
