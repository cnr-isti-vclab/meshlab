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
//
//	IFXFaceLists.h
//
//	DESCRIPTION
//		For every edge, this object stores a list of faces that share
//		the edge.
//
//	NOTES
//
//***************************************************************************

#ifndef IFXFaceLists_h
#define IFXFaceLists_h

#include "IFXFastAllocator.h"

// Masks used for IFXFaceLists::GetFace(...pCornerInfo)
#define IFX_CORNER_INDEX_MASK			0x00000003
#define IFX_EDGE_ORDER_FLIPPED_MASK		0x00000004


//
//  Internally used structs
//
struct IFXFaceListsFaceNode
{
	U32 meshIndex;
	U32 faceIndex;
	U32 cornerInfo;		// corner index and edge order flag
	IFXFaceListsFaceNode* next;
};

struct IFXFaceListsEdgeNode
{
	U32 vertexID;
	IFXFaceListsFaceNode* first;	// the face list (around an edge) starts here
	IFXFaceListsEdgeNode* next;
};


//
// Stores lists of faces that are around each edge.
//
class IFXFaceLists
{
public:
	IFXFaceLists();
	~IFXFaceLists();

	// Allocate memory for object.  Will return IFX_E_OUT_OF_MEMORY
	// if failed.
	IFXRESULT Initialize(U32 numVertices);

	// Add a face.  Will return IFX_E_OUT_OF_MEMORY
	// if memory allocation fails.
	IFXRESULT AddFace(U32 meshIndex, U32 faceIndex, U32 cornerIndex, U32 v1, U32 v2);

	//
	//  Data access methods
	//

	// Set the internal iterator to the first edge.
	// Returns FALSE if no edges.
	BOOL FirstEdge();

	// Move the iternal iterator to the next edge.
	// Returns FALSE if no more edges.
	BOOL NextEdge();

	// Get the current face in the circular linked list of faces.
	// (pCornerInfo & IFX_CORNER_MASK) is the corner index.
	// (pCornerInfo & IFX_EDGE_ORDER_FLIPPED_MASK) indicates if the
	//  v1,v2 order was flipped when face was added to list.
	void GetFace(U32* pMeshIndex, U32* pFaceIndex, U32* pCornerInfo);

	// Go to the next face around the current edge
	BOOL NextFace();

private:
	IFXFastAllocator m_allocator;
	U32 m_numVertices;
	IFXFaceListsEdgeNode** m_pEdgeTable;	// smaller vertex ID is index into this list

	// State variables for iterating through information
	U32 m_currV1;
	IFXFaceListsEdgeNode** m_pCurrEdgeList;
	IFXFaceListsEdgeNode** m_pEndEdgeList;
	IFXFaceListsEdgeNode* m_pCurrEdge;
	IFXFaceListsFaceNode* m_pCurrFace;
};


//
//  inlines
//

inline void IFXFaceLists::GetFace(U32* pMeshIndex, U32* pFaceIndex, U32* pCornerInfo)
{
	*pMeshIndex = m_pCurrFace->meshIndex;
	*pFaceIndex = m_pCurrFace->faceIndex;
	*pCornerInfo = m_pCurrFace->cornerInfo;

	// If you need vertex indices, use the following:
	//*pV1 = m_currV1;
	//*pV2 = m_pCurrEdge->vertexID;
}

inline BOOL IFXFaceLists::NextFace()
{
	m_pCurrFace = m_pCurrFace->next;
	return BOOL(NULL != m_pCurrFace);
}

#endif
