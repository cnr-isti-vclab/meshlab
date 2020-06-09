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

#include <memory.h>
#include "IFXMesh.h"
#include "IFXFaceLists.h"


IFXFaceLists::IFXFaceLists()
{
	m_numVertices = 0;
	m_pEdgeTable = NULL;

	m_currV1 = 0;
	m_pCurrEdgeList = NULL;
	m_pEndEdgeList = NULL;
	m_pCurrEdge = NULL;
	m_pCurrFace = NULL;
}


IFXFaceLists::~IFXFaceLists()
{
	IFXDELETE(m_pEdgeTable);
}


// Set the internal iterator to the first edge.
// Returns FALSE if no edges.
BOOL IFXFaceLists::FirstEdge()
{
	m_currV1 = 0;
	m_pCurrEdgeList = m_pEdgeTable;
	m_pEndEdgeList = m_pCurrEdgeList + m_numVertices;

	while (m_pCurrEdgeList < m_pEndEdgeList && *m_pCurrEdgeList == NULL)
	{
		++m_pCurrEdgeList;
		++m_currV1;
	}

	if (m_pCurrEdgeList < m_pEndEdgeList)
	{
		m_pCurrEdge = *m_pCurrEdgeList;
		m_pCurrFace = m_pCurrEdge->first;
		IFXASSERT(m_pCurrFace);
		return TRUE;
	}
	return FALSE;
}


// Move the iternal iterator to the next edge.
// Returns FALSE if no more edges.
BOOL IFXFaceLists::NextEdge()
{
	m_pCurrEdge = m_pCurrEdge->next;
	if (m_pCurrEdge == NULL)
	{
		do  // edge lists can be sparse
		{
			++m_pCurrEdgeList;
			++m_currV1;
		} while (m_pCurrEdgeList < m_pEndEdgeList && *m_pCurrEdgeList == NULL);

		if (m_pCurrEdgeList < m_pEndEdgeList)
		{
			m_pCurrEdge = *m_pCurrEdgeList;
			m_pCurrFace = m_pCurrEdge->first;
			IFXASSERT(m_pCurrFace);
		}
	}
	else
	{
		m_pCurrFace = m_pCurrEdge->first;
		IFXASSERT(m_pCurrFace);
	}	

	return BOOL(NULL != m_pCurrEdge);
}


// Allocate memory for object.  Will return IFX_E_OUT_OF_MEMORY
// if failed.
IFXRESULT IFXFaceLists::Initialize(U32 numVertices)
{
	// Initialize fast memory allocator
	U32 approxNumEdges = numVertices * 2;
	U32 approxFaceAdds = numVertices * 2 * 3;
	U32 size = (sizeof(IFXFaceListsEdgeNode) * approxNumEdges) +
		(sizeof(IFXFaceListsFaceNode) * approxFaceAdds);
	size += size / 16;  // pad a little
	m_allocator.Initialize(size, size / 8);
	
	m_numVertices = numVertices;
	m_pEdgeTable = new IFXFaceListsEdgeNode*[numVertices];
	if (!m_pEdgeTable)
		return IFX_E_OUT_OF_MEMORY;
	memset(m_pEdgeTable, 0, sizeof(IFXFaceListsEdgeNode*)*numVertices);
	return IFX_OK;
}


// Add a face.  Will return IFX_E_OUT_OF_MEMORY
// if memory allocation fails.
IFXRESULT IFXFaceLists::AddFace(U32 meshIndex, U32 faceIndex, U32 cornerIndex, U32 v1, U32 v2)
{
	IFXASSERT(cornerIndex < 3);

	// Order v1 < v2
	U32 edgeOrderFlippedMask = 0;
	if (v2 < v1)
	{
		U32 tmp = v1;
		v1 = v2;
		v2 = tmp;
		edgeOrderFlippedMask = IFX_EDGE_ORDER_FLIPPED_MASK;
	}

	// Find the right face list
	IFXASSERT(v1 < m_numVertices);
	IFXFaceListsEdgeNode** ppPreLink = m_pEdgeTable + v1;
	IFXFaceListsEdgeNode* pNode = *ppPreLink;

	while (pNode != NULL)
	{
		if (pNode->vertexID == v2)
			break;
		ppPreLink = &(pNode->next);
		pNode = *ppPreLink;
	}

	if (pNode == NULL)
	{
		// Need to add a new node
		pNode = (IFXFaceListsEdgeNode*)m_allocator.Allocate(sizeof(IFXFaceListsEdgeNode));
		if (!pNode)
			return IFX_E_OUT_OF_MEMORY;

		*ppPreLink = pNode;		// add link to list
		pNode->vertexID = v2;
		pNode->first = NULL;
		pNode->next = NULL;
	}

	// Add face to face list
	IFXFaceListsFaceNode** ppPreLink2 = &(pNode->first);
	IFXFaceListsFaceNode* pNode2 = *ppPreLink2;

	while (pNode2 != NULL)
	{
		IFXASSERT(pNode2->meshIndex != meshIndex || pNode2->faceIndex != faceIndex);
		ppPreLink2 = &(pNode2->next);
		pNode2 = *ppPreLink2;
	}

	pNode2 = (IFXFaceListsFaceNode*)m_allocator.Allocate(sizeof(IFXFaceListsFaceNode));
	if (!pNode2)
		return IFX_E_OUT_OF_MEMORY;

	*ppPreLink2 = pNode2;		// add link to list
	pNode2->meshIndex = meshIndex;
	pNode2->faceIndex = faceIndex;
	pNode2->cornerInfo = cornerIndex | edgeOrderFlippedMask;
	pNode2->next = NULL;
	return IFX_OK;
}

