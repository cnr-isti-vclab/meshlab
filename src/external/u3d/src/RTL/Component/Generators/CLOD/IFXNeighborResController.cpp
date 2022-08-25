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
	@file	IFXNeighborResController.cpp

	@note	Vertex merging often joins edges from separate meshes, for example the
			edges in the teapot lid are "zippered" to the teapot.  The current
			implementation of the neighbor resolution controller maintains the original
			boundries even though the faces are adjacent in low resolutions.
*/

#include "IFXMesh.h"
#include "IFXNeighborResController.h"
#include "IFXUpdatesGroup.h"
#include "IFXUpdates.h"
#include "IFXCoincidentVertexMap.h"
#include "IFXCLODManagerInterface.h"
#include "IFXNeighborMesh.h"

#define visitedFlag       faceFlag3


IFXNeighborResControllerInterface* IFXCreateNeighborResController()
{
  return new IFXNeighborResController;
}

void IFXDeleteNeighborResController(IFXNeighborResControllerInterface* pController)
{
  IFXASSERT(pController);
  delete (IFXNeighborResController*)pController;
}

//
// IFXNeighborResolutionController
//

IFXNeighborResController::IFXNeighborResController()
{
  m_pVertexMap = NULL;
  m_pEdgeMap = NULL;
  m_pMeshGroup = NULL;
  m_pNeighborMesh = NULL;
  m_pUpdatesGroup = NULL;
  m_resolution = 0;
  m_finalMaxResolution = 0;
  m_numMeshes = 0;
  m_pMeshStates = NULL;
  m_distalMerges = NULL;
}

IFXNeighborResController::~IFXNeighborResController()
{
  IFXRELEASE(m_pUpdatesGroup);
  IFXDELETE(m_pVertexMap);
  IFXDELETE_ARRAY(m_pMeshStates);
  IFXRELEASE(m_pCLODManager);

  if (m_distalMerges)
  {
    U32 finalMaxResolution = m_finalMaxResolution + 1;
    U32 i;
    for (i = 0; i < finalMaxResolution; ++i)
    {
      IFXDistalEdgeMerge* pMerge = m_distalMerges[i];
      while (pMerge)
      {
        IFXDistalEdgeMerge* pNextMerge = pMerge->m_pNext;
        delete pMerge;
        pMerge = pNextMerge;
      }
    }
    delete m_distalMerges;
    m_distalMerges = NULL;
  }
}


//
// Initialization
//

IFXRESULT IFXNeighborResController::InitMesh(
										IFXMeshGroup* pMeshGroup,
										IFXCLODManager* pCLODManager,
										IFXNeighborMesh* pNeighborMesh,
										IFXUpdatesGroup* pUpdatesGroup,
										IFXVertexMapGroup* pVertexMapGroup)
{
  IFXASSERT(pMeshGroup && pNeighborMesh && pUpdatesGroup && pVertexMapGroup);

  m_pMeshGroup = pMeshGroup;

  // A multres manager is used to clod the
  // mesh for the preprocessing phase.  This is
  // needed for correct lookups into the edge
  // table.
  IFXRELEASE(m_pCLODManager);
  m_pCLODManager = pCLODManager;
  m_pCLODManager->AddRef();
  m_numMeshes = pMeshGroup->GetNumMeshes();
  m_pNeighborMesh = pNeighborMesh;
  m_pUpdatesGroup = pUpdatesGroup;
  pUpdatesGroup->AddRef();
  IFXRESULT res = IFX_OK;

  // Build coincident vertex map
  m_pVertexMap = new IFXCoincidentVertexMap;
  if (!m_pVertexMap)
    res = IFX_E_OUT_OF_MEMORY;
  if (res == IFX_OK)
  {
    res = m_pVertexMap->Initialize(pMeshGroup, pVertexMapGroup);
  }

    // Build edge map
  if (res == IFX_OK)
  {
    res = BuildEdgeMap();
  }

  if (res != IFX_OK)
  {
    return res;
  }

  // Gather information from update records and setup
  // neighbor mesh for clodolution changes.
  m_resolution = pUpdatesGroup->GetMaxResolution();
  m_finalMaxResolution = m_resolution;  

  // Allocate distal edge merge table
  m_resolution = pUpdatesGroup->GetMaxResolution();
  m_distalMerges = new IFXDistalEdgeMerge*[m_finalMaxResolution + 1];
  if (!m_distalMerges)
    return IFX_E_OUT_OF_MEMORY;
  memset(m_distalMerges, 0, sizeof(IFXDistalEdgeMerge*)*(m_resolution + 1));

  m_pMeshStates = new ResolutionState[m_numMeshes];
  U32 i;
  IFXMesh* pMesh;
  for (i = 0; i < m_numMeshes; ++i)
  {
    pMeshGroup->GetMesh(i, pMesh);
    m_pMeshStates[i].numFaces = pMesh->GetMaxNumFaces();
    m_pMeshStates[i].prevNumFaces = 0;
    IFXUpdates* pUpdates = pUpdatesGroup->GetUpdates(i);
    m_pMeshStates[i].resolutionChangeIndex = pUpdates->numResChanges;
    m_pMeshStates[i].faceUpdateIndex = pUpdates->numFaceUpdates;
    IFXRELEASE(pMesh);
  }

  ResetCLODFlags();
  DetermineCollapsedEdges();
  DeleteEdgeMap();

  // Reset to max resolution
  for (i = 0; i < m_numMeshes; ++i)
  {
    pMeshGroup->GetMesh(i, pMesh);
    m_pMeshStates[i].numFaces = pMesh->GetMaxNumFaces();
    m_pMeshStates[i].prevNumFaces = 0;
    IFXUpdates* pUpdates = pUpdatesGroup->GetUpdates(i);
    m_pMeshStates[i].resolutionChangeIndex = pUpdates->numResChanges;
    m_pMeshStates[i].faceUpdateIndex = pUpdates->numFaceUpdates;
    IFXRELEASE(pMesh);
  }

  return IFX_OK;
}


//
// Initialization
//

IFXRESULT IFXNeighborResController::Initialize(
										IFXNeighborMesh* pNeighborMesh,
										IFXUpdatesGroup* pUpdatesGroup)
{
  IFXASSERT(pNeighborMesh && pUpdatesGroup);

  m_numMeshes = pNeighborMesh->GetNumMeshes();
  m_pNeighborMesh = pNeighborMesh;
  m_pUpdatesGroup = pUpdatesGroup;
  pUpdatesGroup->AddRef();

  // Gather information from update records and setup
  // neighbor mesh for clodolution changes.
  m_resolution = 0;
  m_finalMaxResolution = pUpdatesGroup->GetFinalMaxResolution(); 

  // Allocate distal edge merge table
  //m_resolution = pUpdatesGroup->maxResolution;
  m_distalMerges = new IFXDistalEdgeMerge*[m_finalMaxResolution + 1];
  if (!m_distalMerges)
    return IFX_E_OUT_OF_MEMORY;
  memset(m_distalMerges, 0, sizeof(IFXDistalEdgeMerge*)*(m_finalMaxResolution + 1));

  m_pMeshStates = new ResolutionState[m_numMeshes];
  U32 i;
  for (i = 0; i < m_numMeshes; ++i)
  {
    m_pMeshStates[i].numFaces = 0;
    m_pMeshStates[i].prevNumFaces = 0;
    m_pMeshStates[i].resolutionChangeIndex = 0;
    m_pMeshStates[i].faceUpdateIndex = 0;
  }

  return IFX_OK;
}


/**
	Get the distal edge merge list for a resolution.
	Returns NULL if the no edge merges.
*/
IFXDistalEdgeMerge* IFXNeighborResController::GetDistalEdgeMerges(U32 resolution)
{
    IFXASSERT(resolution <= m_finalMaxResolution);
	return m_distalMerges[resolution];
}

/** 
	Set the list of distal edge merges for a resolution.
	pList can be NULL.

	@note  the m_otherVertex value is not used
*/
void IFXNeighborResController::SetDistalEdgeMerges(
									U32 resolution, 
									IFXDistalEdgeMerge* pList)
{
    IFXASSERT(resolution <= m_finalMaxResolution);
	m_distalMerges[resolution] = pList;
}


//
// Edge to face table
//

IFXRESULT IFXNeighborResController::EdgeMap::
  Initialize(U32 numVertices, IFXNeighborMesh* pNeighborMesh)
{
  m_pNeighborMesh = pNeighborMesh;
  m_numVertices = numVertices;
  m_pTable = new EdgeEntry*[numVertices];
  if (!m_pTable)
    return IFX_E_OUT_OF_MEMORY;
  memset(m_pTable, 0, sizeof(EdgeEntry*) * numVertices);
  return IFX_OK;
}


void IFXNeighborResController::EdgeMap::
  FreeAll()
{
  U32 i;
  for ( i = 0; i < m_numVertices; ++i)
    IFXASSERT(m_pTable[i] == NULL);

  delete m_pTable;
}


void IFXNeighborResController::EdgeMap::
  Lookup(U32 vertex1, U32 vertex2, EdgeEntry*** pppPreLink, EdgeEntry** ppNode)
{
  if (vertex1 > vertex2)
  {
    U32 tmp = vertex1;
    vertex1 = vertex2;
    vertex2 = tmp;
  }

  // Lookup/add edge to the table
  IFXASSERT(vertex1 < m_numVertices);
  EdgeEntry** ppPreLink = &m_pTable[vertex1];
  EdgeEntry* pNode = *ppPreLink;

  while (pNode != NULL)
  {
    if (pNode->m_vertex2 == vertex2)
      break;
    ppPreLink = &(pNode->m_next);
    pNode = *ppPreLink;
  }

  *pppPreLink = ppPreLink;
  *ppNode = pNode;
}


IFXRESULT IFXNeighborResController::EdgeMap::
  AddEdge(U32 vertex1, U32 vertex2, U32 mesh, U32 face, U32 corner)
{
  if (vertex1 > vertex2)
  {
    U32 tmp = vertex1;
    vertex1 = vertex2;
    vertex2 = tmp;
  }

  // Lookup/add edge to the table
  IFXASSERT(vertex1 < m_numVertices);
  EdgeEntry** ppPreLink = &m_pTable[vertex1];
  EdgeEntry* pNode = *ppPreLink;

  while (pNode != NULL)
  {
    if (pNode->m_vertex2 == vertex2)
      break;
    ppPreLink = &(pNode->m_next);
    pNode = *ppPreLink;
  }

  if (pNode == NULL)
  {
    // Need to add a new node
    pNode = new EdgeEntry;
    if (!pNode)
      return IFX_E_OUT_OF_MEMORY;

    *ppPreLink = pNode;   // add link to list
    pNode->m_vertex2 = vertex2;
    pNode->m_mesh = mesh;
    pNode->m_face = face;
    pNode->m_corner = corner;
    pNode->m_next = NULL;
  }

  return IFX_OK;
}


IFXRESULT IFXNeighborResController::EdgeMap::
  AddOrReplaceEdge(U32 vertex1, U32 vertex2, U32 mesh, U32 face, U32 corner)
{
  if (vertex1 > vertex2)
  {
    U32 tmp = vertex1;
    vertex1 = vertex2;
    vertex2 = tmp;
  }

  // Lookup/add edge to the table
  IFXASSERT(vertex1 < m_numVertices);
  EdgeEntry** ppPreLink = &m_pTable[vertex1];
  EdgeEntry* pNode = *ppPreLink;

  while (pNode != NULL)
  {
    if (pNode->m_vertex2 == vertex2)
      break;
    ppPreLink = &(pNode->m_next);
    pNode = *ppPreLink;
  }

  if (pNode == NULL)
  {
    // Need to add a new node
    pNode = new EdgeEntry;
    if (!pNode)
      return IFX_E_OUT_OF_MEMORY;

    *ppPreLink = pNode;   // add link to list
    pNode->m_vertex2 = vertex2;
    pNode->m_next = NULL;
  }

  // Replace data if existing
  pNode->m_mesh = mesh;
  pNode->m_face = face;
  pNode->m_corner = corner;
  return IFX_OK;
}

BOOL IFXNeighborResController::EdgeMap::
  FindEdge(U32 vertex1, U32 vertex2, U32* pMesh, U32* pFace, U32* pCorner)
{
  if (vertex1 > vertex2)
  {
    U32 tmp = vertex1;
    vertex1 = vertex2;
    vertex2 = tmp;
  }

  // Lookup edge in the table
  IFXASSERT(vertex1 < m_numVertices);
  EdgeEntry** ppPreLink = &m_pTable[vertex1];
  EdgeEntry* pNode = *ppPreLink;

  while (pNode != NULL)
  {
    if (pNode->m_vertex2 == vertex2)
      break;
    ppPreLink = &(pNode->m_next);
    pNode = *ppPreLink;
  }

  if (pNode)
  {
    *pMesh = pNode->m_mesh;
    *pFace = pNode->m_face;
    *pCorner = pNode->m_corner;
    return TRUE;
  }
  return FALSE;
}


void IFXNeighborResController::EdgeMap::
  RemoveEdge(U32 vertex1, U32 vertex2)
{
  if (vertex1 > vertex2)
  {
    U32 tmp = vertex1;
    vertex1 = vertex2;
    vertex2 = tmp;
  }

  // Lookup edge in the table
  IFXASSERT(vertex1 < m_numVertices);
  EdgeEntry** ppPreLink = &m_pTable[vertex1];
  EdgeEntry* pNode = *ppPreLink;

  while (pNode != NULL)
  {
    if (pNode->m_vertex2 == vertex2)
      break;
    ppPreLink = &(pNode->m_next);
    pNode = *ppPreLink;
  }

  if (pNode)
  {
    *ppPreLink = pNode->m_next;
    delete pNode;
  }
}


IFXRESULT IFXNeighborResController::BuildEdgeMap()
{
  m_pEdgeMap = new EdgeMap;
  if (!m_pEdgeMap)
    return IFX_E_OUT_OF_MEMORY;

  IFXRESULT res;
  res = m_pEdgeMap->Initialize(m_pVertexMap->GetNumVertexID(), m_pNeighborMesh);
  if (res != IFX_OK)
    return res;

  IFXFaceIter faceIter;
  U32 meshIndex;
  for ( meshIndex = 0; res == IFX_OK  &&  meshIndex < m_numMeshes; ++meshIndex)
  {
    IFXMesh* pMesh = 0;
    m_pMeshGroup->GetMesh(meshIndex, pMesh);
    pMesh->GetFaceIter(faceIter);
    U32 numFaces = pMesh->GetNumFaces();
    U32 faceIndex;
    for ( faceIndex = 0; faceIndex < numFaces; ++faceIndex)
    {
      IFXFace* pFace = faceIter.Next();
      U32 v0 = m_pVertexMap->Convert(meshIndex, pFace->VertexA());
      U32 v1 = m_pVertexMap->Convert(meshIndex, pFace->VertexB());
      U32 v2 = m_pVertexMap->Convert(meshIndex, pFace->VertexC());

      res = m_pEdgeMap->AddEdge(v0, v1, meshIndex, faceIndex, 2);
      if (res == IFX_OK)
        res = m_pEdgeMap->AddEdge(v1, v2, meshIndex, faceIndex, 0);
      if (res == IFX_OK)
        res = m_pEdgeMap->AddEdge(v2, v0, meshIndex, faceIndex, 1);
    }
    IFXRELEASE(pMesh);
  }
  return res;
}


void IFXNeighborResController::DeleteEdgeMap()
{
  m_pEdgeMap->FreeAll();
  delete m_pEdgeMap;
  m_pEdgeMap = NULL;
}


void IFXNeighborResController::AnalyzeMergingEdges(U32 meshIndex, U32 resolution)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  IFXUpdates* pUpdates = m_pUpdatesGroup->GetUpdates(meshIndex);
  IFXResolutionChange* rc = &(pUpdates->pResChanges[state->resolutionChangeIndex - 1]);

  IFXMesh* pMesh = 0;
  m_pMeshGroup->GetMesh(meshIndex, pMesh);
  IFXFaceIter faceIter;
  pMesh->GetFaceIter(faceIter);

  U32 endUpdate = state->faceUpdateIndex;
  U32 update;
  for ( update = endUpdate - rc->numFaceUpdates; update < endUpdate; ++update)
  {
    IFXFaceUpdate* pFaceUpdate = &pUpdates->pFaceUpdates[update];
    U32 p = m_pVertexMap->Convert(meshIndex, pFaceUpdate->newDown);
    U32 q = m_pVertexMap->Convert(meshIndex, pFaceUpdate->newUp);
    if (p == q)
    {
      // This is a face update for a non-position attribute change.
      // Example:  better normals at corners of cube.
      // We are only concerned with updates that affect geometry.
      continue;
    }

    IFXFace* pFace = faceIter.Index(pFaceUpdate->face);
    U32 childCorner = pFaceUpdate->corner;
    U32 cwCorner = Clockwise(childCorner);
    U32 ccwCorner = CounterClockwise(childCorner);
    IFXASSERT(pFaceUpdate->newUp == pFace->Vertex(childCorner));
    U32 c = m_pVertexMap->Convert(meshIndex, pFace->Vertex(childCorner));
    U32 x = m_pVertexMap->Convert(meshIndex, pFace->Vertex(cwCorner));
    U32 y = m_pVertexMap->Convert(meshIndex, pFace->Vertex(ccwCorner));

    // Let c = child vertex
    // Let p = parent vertex
    // Let x = cw of child
    // Let y = ccw of child
    //
    // If the xp edge exists:  Look for a face that is common
    //   to the lists around edge xp and xc.  If a face exists,
    //   it is assumed that this face is a deleted face and
    //   the disparate lists will be joined when the face removed.
    //   If a common face is not found, then this is a distal
    //   edge merge.
    //
    // If the xp edge does not exist:  A merge does not exist
    //   because there is only one list.
    //
    // The check can be accomplished by walking the faces
    // around the first edge and setting a visited flag.  Then
    // we walk around the second edge looking for a visited flag.
    BOOL distalMerge = CheckForDistalMerge(x, c, p); // xc and xp edge
    if (distalMerge)
    {
      AddDistalMergeRecord(resolution, x, c, p); // xc and xp edge
    }

    distalMerge = CheckForDistalMerge(y, c, p); // yc and yp edge
    if (distalMerge)
    {
      AddDistalMergeRecord(resolution, y, c, p); // xc and xp edge
    }
  }

  IFXRELEASE(pMesh);
}


// Check for the distal merge between two edges
// (no face between the merged edges).
BOOL IFXNeighborResController::CheckForDistalMerge(U32 other, U32 child, U32 parent)
{
  U32 mesh2 = 0;
  U32 face2 = 0;
  U32 corner2 = 0;
  BOOL found = m_pEdgeMap->FindEdge(other, parent, &mesh2, &face2, &corner2);
  if (!found)
    return FALSE; // not an edge merge

  U32 mesh1 = 0;
  U32 face1 = 0;
  U32 corner1 = 0;
  found = m_pEdgeMap->FindEdge(other, child, &mesh1, &face1, &corner1);
  IFXASSERT(found);

  IFXCornerIter faceIter;
  m_pNeighborMesh->GetCornerIter(mesh1, face1, corner1, faceIter);
  do
  {
    faceIter.GetFaceFlags()->visitedFlag = TRUE;
    faceIter.JumpAcross();
  } while ((faceIter.GetFaceIndex() != face1) ||
    (faceIter.GetMeshIndex() != mesh1));

  found = FALSE;
  m_pNeighborMesh->GetCornerIter(mesh2, face2, corner2, faceIter);
  do
  {
    if (faceIter.GetFaceFlags()->visitedFlag)
    {
      found = TRUE;
      break;
    }
    faceIter.JumpAcross();
  } while ((faceIter.GetFaceIndex() != face2) ||
    (faceIter.GetMeshIndex() != mesh2));

  m_pNeighborMesh->GetCornerIter(mesh1, face1, corner1, faceIter);
  do
  {
    faceIter.GetFaceFlags()->visitedFlag = FALSE;
    faceIter.JumpAcross();
  } while ((faceIter.GetFaceIndex() != face1) ||
    (faceIter.GetMeshIndex() != mesh1));

  return (!found);
}


void IFXNeighborResController::AddDistalMergeRecord(
									U32 resolution, U32 other, U32 child, U32 parent)
{
  // Add only one merge record for each edge
  IFXDistalEdgeMerge* pMerge = m_distalMerges[resolution];
  while (pMerge)
  {
    if (pMerge->m_otherVertex == other)
      return;
    pMerge = pMerge->m_pNext;
  }

  pMerge = new IFXDistalEdgeMerge;
  pMerge->m_otherVertex = other;
#ifdef _DEBUG
  BOOL found =
#endif
  m_pEdgeMap->FindEdge(other, child,
    &(pMerge->m_meshA), &(pMerge->m_faceA), &(pMerge->m_cornerA));
  IFXASSERT(found);

#ifdef _DEBUG
  found =
#endif
  m_pEdgeMap->FindEdge(other, parent,
    &(pMerge->m_meshB), &(pMerge->m_faceB), &(pMerge->m_cornerB));
  IFXASSERT(found);

  IFXDistalEdgeMerge* pNextMerge = m_distalMerges[resolution];
  m_distalMerges[resolution] = pMerge;
  pMerge->m_pNext = pNextMerge;
}


void IFXNeighborResController::UpdateEdgesInMap(U32 meshIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  IFXUpdates* pUpdates = m_pUpdatesGroup->GetUpdates(meshIndex);
  IFXResolutionChange* rc = &(pUpdates->pResChanges[state->resolutionChangeIndex - 1]);

  IFXMesh* pMesh = 0;
  m_pMeshGroup->GetMesh(meshIndex, pMesh);
  IFXFaceIter faceIter;
  pMesh->GetFaceIter(faceIter);

  // Update modified edges
  U32 endUpdate = state->faceUpdateIndex;
  U32 update;
  for ( update = endUpdate - rc->numFaceUpdates; update < endUpdate; ++update)
  {
    IFXFaceUpdate* pFaceUpdate = &(pUpdates->pFaceUpdates[update]);
    U32 p = m_pVertexMap->Convert(meshIndex, pFaceUpdate->newDown);
    U32 q = m_pVertexMap->Convert(meshIndex, pFaceUpdate->newUp);
    if (p == q)
    {
      // This is a face update for a non-position attribute change.
      // Example:  better normals at corners of cube.
      // We are only concerned with updates that affect geometry.
      continue;
    }

    U32 faceIndex = pFaceUpdate->face;
    IFXFace* pFace = faceIter.Index(faceIndex);
    U32 childCorner = pFaceUpdate->corner;
    U32 cwCorner = Clockwise(childCorner);
    U32 ccwCorner = CounterClockwise(childCorner);
    U32 c = m_pVertexMap->Convert(meshIndex, pFace->Vertex(childCorner));
    U32 x = m_pVertexMap->Convert(meshIndex, pFace->Vertex(cwCorner));
    U32 y = m_pVertexMap->Convert(meshIndex, pFace->Vertex(ccwCorner));

    // AddOrReplaceEdge does one of two things:
    //   In the case of vertex merge, it will add any new edges.
    //   In the case of a manifold merge, it will replace the
    //     reference of a potentially deleted neighbor face with
    //     a valid reference.
    m_pEdgeMap->AddOrReplaceEdge(x, p, meshIndex, faceIndex, ccwCorner);
    m_pEdgeMap->AddOrReplaceEdge(y, p, meshIndex, faceIndex, cwCorner);

    m_pEdgeMap->RemoveEdge(x, c);
    m_pEdgeMap->RemoveEdge(y, c);
  }

  // Remove deleted edges
  U32 prevNumFaces = state->prevNumFaces;
  U32 faceIndex;
  for ( faceIndex = state->numFaces; faceIndex < prevNumFaces; ++faceIndex)
  {
    IFXFace* pFace = faceIter.Index(faceIndex);
    U32 a = m_pVertexMap->Convert(meshIndex, pFace->Vertex(0));
    U32 b = m_pVertexMap->Convert(meshIndex, pFace->Vertex(1));
    U32 c = m_pVertexMap->Convert(meshIndex, pFace->Vertex(2));

    FindNonDeletedFaceForEdge(a, b);
    FindNonDeletedFaceForEdge(b, c);
    FindNonDeletedFaceForEdge(c, a);
  }

  IFXRELEASE(pMesh);
}

// This function must be called BEFORE the neighbor mesh links
// are adjusted for deleted faces.
void IFXNeighborResController::FindNonDeletedFaceForEdge(U32 v1, U32 v2)
{
  U32 mesh = 0;
  U32 face = 0;
  U32 corner = 0;
  BOOL found = m_pEdgeMap->FindEdge(v1, v2, &mesh, &face, &corner);

  if ( (!found) || (face < m_pMeshStates[mesh].numFaces) )
  {
    return;
  }

  IFXCornerIter faceIter;
  m_pNeighborMesh->GetCornerIter(mesh, face, corner, faceIter);
  do
  {
    U32 currMesh = faceIter.GetMeshIndex();
    U32 currFace = faceIter.GetFaceIndex();

    if (currFace < m_pMeshStates[currMesh].numFaces)
    {
      m_pEdgeMap->AddOrReplaceEdge(
						v1, v2, currMesh, currFace, faceIter.GetCornerIndex());
      return;
    }
    faceIter.JumpAcross();
  } while ((faceIter.GetFaceIndex() != face) ||
    (faceIter.GetMeshIndex() != mesh));

  // All faces around edge deleted...remove edge from list
  m_pEdgeMap->RemoveEdge(v1, v2);
}


void IFXNeighborResController::ApplyEdgeMerge(IFXDistalEdgeMerge* pMerge)
{
  IFXCornerIter walkerA;
  m_pNeighborMesh->GetCornerIter(
						pMerge->m_meshA, pMerge->m_faceA, pMerge->m_cornerA, walkerA);
  IFXCornerIter walkerQ;
  walkerQ.Clone(&walkerA);
  walkerQ.JumpAcross();

  IFXCornerIter walkerB;
  m_pNeighborMesh->GetCornerIter(
						pMerge->m_meshB, pMerge->m_faceB, pMerge->m_cornerB, walkerB);
  IFXCornerIter walkerR;
  walkerR.Clone(&walkerB);
  walkerR.JumpAcross();

  walkerA.MoveLink(&walkerR);
  walkerB.MoveLink(&walkerQ);
}


//
//
//

#define NO_COLLAPSE_INDEX   3


void IFXNeighborResController::ResetCLODFlags()
{
  IFXMesh* pMesh = 0;
  U32 meshIndex;
  for ( meshIndex = 0; meshIndex < m_numMeshes; ++meshIndex)
  {
    IFXNeighborFace* pFaces = m_pNeighborMesh->GetNeighborFaceArray(meshIndex);
    m_pMeshGroup->GetMesh(meshIndex, pMesh);
    U32 numFaces = pMesh->GetNumFaces();
    U32 faceIndex;
    for ( faceIndex = 0; faceIndex < numFaces; ++faceIndex)
    {
      IFXFaceFlags* pFlags = pFaces[faceIndex].GetFaceFlags();
      pFlags->collapseIndex = NO_COLLAPSE_INDEX;
      pFlags->duplicateFlag = 0;
    }

    IFXRELEASE(pMesh);
  }
}


void IFXNeighborResController::DetermineCollapsedEdges()
{
  IFXTRACE_GENERIC(L"IFX Neighbor res:\n");
  // Reduce the mesh resolution
  U32 resolution;
  for (resolution = m_pUpdatesGroup->GetMaxResolution() - 1; 
	   (I32)resolution >= 0; 
	   --resolution)
  {
    IFXTRACE_GENERIC(L"%d%c", resolution, (resolution % 10) ? ' ' : '\n' );

    // First pass:
    //   Determine which faces are deleted by this resolution change.
    //   Express as a range:
    //     m_pMeshStates[i].numFaces => deleted < m_pMeshStates[i].prevNumFaces
    //
    U32 i;
    for(i = 0; i < m_numMeshes; i++)
    {
      U32* syncTable = m_pUpdatesGroup->GetSyncTable(i);
      U32  localRes = m_pMeshStates[i].resolutionChangeIndex;

      if (localRes > 0  &&  resolution <= syncTable[localRes-1])
      {
        DecrementFaceCount(i);
      }
      else
      {
        // no faces deleted in this mesh; invalidate range
        m_pMeshStates[i].prevNumFaces = 0;
      }
    }

    // Analyze modified faces for merged edges
    for(i = 0; i < m_numMeshes; i++)
    {
      U32* syncTable = m_pUpdatesGroup->GetSyncTable(i);
      U32  localRes = m_pMeshStates[i].resolutionChangeIndex;

      if (localRes > 0  &&  resolution <= syncTable[localRes-1])
      {
        AnalyzeMergingEdges(i, resolution);
      }
    }

    // Mark duplicate faces and determine the collapse edges
    // by analyzing the unique faces.  Duplicate faces must be
    // marked in the reverse order that faces are removed.  Faces
    // are removed from highest face index to lowest, from mesh 0
    // to mesh n.  Thus duplicate faces are marked from lowest face
    // face index to highest, from mesh n to 0.
    for(I32 j = m_numMeshes - 1; j >= 0; j--)
    {
      U32* syncTable = m_pUpdatesGroup->GetSyncTable(j);
      U32  localRes = m_pMeshStates[j].resolutionChangeIndex;

      if (localRes > 0  &&  resolution <= syncTable[localRes-1])
      {
        MarkCollapseEdgesInMesh(j);
      }
    }

    // Remove deleted edges from edge map
    for(i = 0; i < m_numMeshes; i++)
    {
      U32* syncTable = m_pUpdatesGroup->GetSyncTable(i);
      U32  localRes = m_pMeshStates[i].resolutionChangeIndex;

      if (localRes > 0  &&  resolution <= syncTable[localRes-1])
      {
        UpdateEdgesInMap(i);
      }
    }

    // Third pass:
    //   Actually remove faces from connectivity
    for(i = 0; i < m_numMeshes; i++)
    {
      U32* syncTable = m_pUpdatesGroup->GetSyncTable(i);
      U32  localRes = m_pMeshStates[i].resolutionChangeIndex;

      if (localRes > 0  &&  resolution <= syncTable[localRes-1])
      {
        AltRemoveFaces(i);
      }
    }

    // Apply distal merges
    IFXDistalEdgeMerge* pMerge = m_distalMerges[resolution];
    while (pMerge)
    {
      ApplyEdgeMerge(pMerge);
      pMerge = pMerge->m_pNext;
    }


    m_pCLODManager->SetResolution(resolution);
  }

  // Restore the mesh resolution
  for (resolution = 1; resolution <= m_pUpdatesGroup->GetMaxResolution(); ++resolution)
  {
    // Undo distal merges
    IFXDistalEdgeMerge* pMerge = m_distalMerges[resolution-1];
    while (pMerge)
    {
      ApplyEdgeMerge(pMerge);
      pMerge = pMerge->m_pNext;
    }
    for(I32 i = m_numMeshes - 1; i >= 0; i--)
    {
      U32* syncTable = m_pUpdatesGroup->GetSyncTable(i);
      U32 maxLocalRes = m_pUpdatesGroup->GetUpdates(i)->numResChanges;
      U32 localRes = m_pMeshStates[i].resolutionChangeIndex;

      if (localRes < maxLocalRes  &&  resolution > syncTable[localRes])
      {
        AddFaces(i);
      }
    }
    m_pCLODManager->SetResolution(resolution);
  }
}



// Determine range of deleted faces for this resolution change
void IFXNeighborResController::DecrementFaceCount(U32 meshIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  IFXUpdates* pUpdates = m_pUpdatesGroup->GetUpdates(meshIndex);
  IFXResolutionChange* rc = &(pUpdates->pResChanges[state->resolutionChangeIndex - 1]);

  state->prevNumFaces = state->numFaces;
  state->numFaces -= rc->deltaFaces;
}


BOOL IFXNeighborResController::IsDeletedFace(U32 meshIndex, U32 faceIndex)
{
  ResolutionState& state = m_pMeshStates[meshIndex];
  return (faceIndex < state.prevNumFaces  &&  faceIndex >= state.numFaces);
}



void IFXNeighborResController::MarkDuplicatesFromCorner(
									U32 meshIndex, U32 faceIndex, U32 cornerIndex)
{
  IFXCornerIter faceIter;
  m_pNeighborMesh->GetCornerIter(meshIndex, faceIndex, cornerIndex, faceIter);

  // Mark faces around the first edge as "has been visited".
  faceIter.JumpAcross();
  while ((faceIter.GetFaceIndex() != faceIndex) ||
    (faceIter.GetMeshIndex() != meshIndex))
  {
    faceIter.GetCurrentFace()->GetFaceFlags()->visitedFlag = TRUE;
    faceIter.JumpAcross();
  }

  // Loop around the second edge looking for "has been visited"
  // faces.  If found, mark faces as duplicates.
  faceIter.MoveClockwise();
  faceIter.JumpAcross();
  while ((faceIter.GetFaceIndex() != faceIndex) ||
    (faceIter.GetMeshIndex() != meshIndex))
  {
    IFXFaceFlags* pFaceFlags = faceIter.GetCurrentFace()->GetFaceFlags();
    if (pFaceFlags->visitedFlag)
    {
      IFXASSERT(pFaceFlags->duplicateFlag || cornerIndex == 0);
      pFaceFlags->duplicateFlag = TRUE;
    }
    faceIter.JumpAcross();
  }

  // Clean up our "has been visited"
  faceIter.MoveCounterClockwise();
  faceIter.JumpAcross();
  while ((faceIter.GetFaceIndex() != faceIndex) ||
    (faceIter.GetMeshIndex() != meshIndex))
  {
    faceIter.GetCurrentFace()->GetFaceFlags()->visitedFlag = FALSE;
    faceIter.JumpAcross();
  }
}

void IFXNeighborResController::MarkDuplicates(U32 meshIndex, U32 faceIndex)
{
  U32 corner;
  for ( corner = 0; corner < 3; ++corner)
    MarkDuplicatesFromCorner(meshIndex, faceIndex, corner);
}


BOOL IFXNeighborResController::IsNotDuplicate(U32 meshIndex, U32 faceIndex)
{
  return 
	  !(m_pNeighborMesh->GetNeighborFaceArray(meshIndex)
			[faceIndex].GetFaceFlags()->duplicateFlag);
}


//
// When duplicate faces are ignored, then for a given collapse operation
// there are 3 types of edges:  KeeperEdge, CollapseEdge, and EdgeEdge.
//
// A deleted face will have at most 2 KeeperEdges, although it may have zero.
// A deleted face will have at most 1 CollapseEdge, although it may have zero.
// We must look for the CollapseEdge first.  If not found, then the first
// EdgeEdge is marked as the collapsed edge.
//
IFXNeighborResController::EdgeType 
IFXNeighborResController::ClassifyEdge(U32 meshIndex, U32 faceIndex, U32 cornerIndex)
{
  // Loop through non-duplicate faces around the edge looking for
  // a deleted or keeper faces.
  IFXCornerIter faceIter;
  m_pNeighborMesh->GetCornerIter(meshIndex, faceIndex, cornerIndex, faceIter);
  faceIter.JumpAcross();
  while ((faceIter.GetMeshIndex() != meshIndex) ||
    (faceIter.GetFaceIndex() != faceIndex))
  {
    if (! faceIter.GetCurrentFace()->GetFaceFlags()->duplicateFlag)
    {
      // is it to be deleted?
      if (IsDeletedFace(faceIter.GetMeshIndex(), faceIter.GetFaceIndex()))
        return CollapseEdge;
      else
        return KeeperEdge;
    }
    faceIter.JumpAcross();
  }
  return EdgeEdge;
}


BOOL IFXNeighborResController::FindCollapseEdge(U32 meshIndex,
                        U32* collapseFaceIndex,
                        U32* collapseCornerIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  U32 prevNumFaces = state->prevNumFaces;
  U32 faceIndex;
  for ( faceIndex = state->numFaces; faceIndex < prevNumFaces; ++faceIndex)
  {
    if (IsNotDuplicate(meshIndex, faceIndex))
    {
      MarkDuplicates(meshIndex, faceIndex);

      U32 cornerIndex;
      for ( cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
      {
        // Classify each edge as KeeperEdge, CollapseEdge, or EdgeEdge
        EdgeType type = ClassifyEdge(meshIndex, faceIndex, cornerIndex);
        if (type == CollapseEdge)
        {
          *collapseFaceIndex = faceIndex;
          *collapseCornerIndex = cornerIndex;
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}


BOOL IFXNeighborResController::FindEdgeEdge(U32 meshIndex,
                      U32* collapseFaceIndex,
                      U32* collapseCornerIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  U32 faceIndex = state->numFaces;
  if (faceIndex >= state->prevNumFaces)
    return FALSE; // no faces to delete

  U32 cornerIndex;
  for ( cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
  {
    // Classify each edge as KeeperEdge, CollapseEdge, or EdgeEdge
    EdgeType type = ClassifyEdge(meshIndex, faceIndex, cornerIndex);
    if (type == EdgeEdge)
    {
      *collapseFaceIndex = faceIndex;
      *collapseCornerIndex = cornerIndex;
      return TRUE;
    }
  }
  // Shouldn't get here because this method is only called if
  // CollapseEdge type edges were not found.  If no edges are
  // CollpaseEdge, then at least one edge has to be an EdgeEdge.
  IFXASSERT(FALSE);
  return FALSE;
}


static void SetCollapseIndex(IFXCornerIter* pFaceIter)
{
  U32 collapseIndex = pFaceIter->GetCornerIndex();
  IFXFaceFlags* pFlags = pFaceIter->GetFaceFlags();
  IFXASSERT(pFlags->collapseIndex == NO_COLLAPSE_INDEX ||
    pFlags->collapseIndex == collapseIndex);
  pFlags->collapseIndex = collapseIndex;
}


void IFXNeighborResController::MarkCollapseEdgesInMesh(U32 meshIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  U32 prevNumFaces = state->prevNumFaces;
  U32 faceIndex;
  for ( faceIndex = state->numFaces; faceIndex < prevNumFaces; ++faceIndex)
  {
    if (IsNotDuplicate(meshIndex, faceIndex))
    {
      MarkDuplicates(meshIndex, faceIndex);

      U32 collapseFace   = 0;
      U32 collapseCorner = 0;
      BOOL collapseFound = FALSE;
      U32 cornerIndex    = 0;
      for ( cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
      {
        // Classify each edge as KeeperEdge, CollapseEdge, or EdgeEdge
        EdgeType type = ClassifyEdge(meshIndex, faceIndex, cornerIndex);
        if (type == CollapseEdge)
        {
          collapseFace = faceIndex;
          collapseCorner = cornerIndex;
          collapseFound = TRUE;
          break;
        }
      }

      if (!collapseFound)
      {
        U32 cornerIndex;
        for ( cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
        {
          // Classify each edge as KeeperEdge, CollapseEdge, or EdgeEdge
          EdgeType type = ClassifyEdge(meshIndex, faceIndex, cornerIndex);
          if (type == EdgeEdge)
          {
            collapseFace = faceIndex;
            collapseCorner = cornerIndex;
            collapseFound = TRUE;
            break;
          }
        }
      }

      IFXASSERT(collapseFound);

      if (collapseFound)
      {
        // Mark opposite corners on all faces around collapse edge
        // with the "collapse edge" flag.
        IFXCornerIter faceIter;
        m_pNeighborMesh->GetCornerIter(
							meshIndex, collapseFace, collapseCorner, faceIter);
        IFXCornerFlags* pCornerFlags;
        do
        {
          SetCollapseIndex(&faceIter);
          faceIter.JumpAcross(&pCornerFlags);
        } while ((faceIter.GetFaceIndex() != collapseFace) ||
          (faceIter.GetMeshIndex() != meshIndex));
      }
    }
  }
}


// Alternate (almost identical) version of RemoveFaces that does not
// decrement state->numFaces.
void IFXNeighborResController::AltRemoveFaces(U32 meshIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  --state->resolutionChangeIndex;

  IFXUpdates* pUpdates = m_pUpdatesGroup->GetUpdates(meshIndex);
  IFXResolutionChange* rc = &(pUpdates->pResChanges[state->resolutionChangeIndex]);

  // Decrement the face updates index to reflect modified faces
  state->faceUpdateIndex -= rc->numFaceUpdates;

  if (rc->deltaFaces == 0)
    return;

  IFXNeighborFace* pNeighborFaces = m_pNeighborMesh->GetNeighborFaceArray(meshIndex);

  IFXASSERT(state->prevNumFaces);
  I32 faceIndex = state->prevNumFaces - 1;
  while (faceIndex >= I32(state->numFaces))
  {
    // Determine corner across from collapse edge.
    U32 xCornerIndex = pNeighborFaces[faceIndex].GetFaceFlags()->collapseIndex;
    if (xCornerIndex < NO_COLLAPSE_INDEX)
    {
      RemoveFace(meshIndex, faceIndex, xCornerIndex);
    }
    --faceIndex;
  }
}



//
// Fast resolution change
//

void IFXNeighborResController::SetResolution(U32 resolution)
{
  if (resolution > m_pUpdatesGroup->GetMaxResolution()) 
  {
    resolution = m_pUpdatesGroup->GetMaxResolution(); 
  }

  while (resolution > m_resolution)
    IncreaseResolution();

  while (resolution < m_resolution)
    DecreaseResolution();
}


void IFXNeighborResController::DecreaseResolution()
{
  --m_resolution;
  U32 meshIndex;
  for( meshIndex = 0; meshIndex < m_numMeshes; meshIndex++)
  {
    U32* syncTable = m_pUpdatesGroup->GetSyncTable(meshIndex);
    U32  localRes = m_pMeshStates[meshIndex].resolutionChangeIndex;

    if (localRes > 0  &&  m_resolution <= syncTable[localRes-1])
    {
      RemoveFaces(meshIndex);
    }
  }

  // Apply distal merges
  IFXDistalEdgeMerge* pMerge = m_distalMerges[m_resolution];
  while (pMerge)
  {
    ApplyEdgeMerge(pMerge);
    pMerge = pMerge->m_pNext;
  }
}


void IFXNeighborResController::RemoveFaces(U32 meshIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  --state->resolutionChangeIndex;
  IFXUpdates* pUpdates = m_pUpdatesGroup->GetUpdates(meshIndex);
  IFXResolutionChange* rc = &(pUpdates->pResChanges[state->resolutionChangeIndex]);
  if (rc->deltaFaces == 0)
    return;

  IFXNeighborFace* pNeighborFaces = m_pNeighborMesh->GetNeighborFaceArray(meshIndex);

  I32 faceIndex = state->numFaces - 1;
  state->numFaces -= rc->deltaFaces;
  while (faceIndex >= I32(state->numFaces))
  {
    // Determine corner across from collapse edge.
    U32 xCornerIndex = pNeighborFaces[faceIndex].GetFaceFlags()->collapseIndex;
    if (xCornerIndex < NO_COLLAPSE_INDEX)
      RemoveFace(meshIndex, faceIndex, xCornerIndex);
    --faceIndex;
  }
}


void IFXNeighborResController::RemoveFace(U32 meshIndex, U32 faceIndex, U32 xCornerIndex)
{
  // let p be the parent vertex
  // let c be the child vertex
  // let x be the corner across from collapsed edge
  // let F be the face to remove
  // let Ec be the edge across from c
  // let Ep be the edge across from p
  // let Ex be the edge being collapsed
  //
  //        p
  //   Ec  /|
  //     /  |
  // x /  F |
  //   \    | Ex
  //     \  |
  //   Ep  \|
  //        c
  //
  //  let Lc be the linked list of faces around edge Ec
  //  let Lp be the linked list of faces around edge Ep
  //
  //  Steps:
  //    Find the face Q in Lc that points to face F.
  //    Find the face R that F points to in Lp.
  //    Make Q point to R.
  //
  //    Find the face S in Lp that points to face F.
  //    Find the face T that F points to in Lc.
  //    Make S point to T.
  //
  //  This joins lists Lc and Lp into a circular list not
  //  containing face F.
  //
  //  Exceptions:
  //  If R is F (Ep is an edge), then make Q point to T
  //  If T is F (Ec is an edge), then make S point to R
  //  If R and T is F, then do nothing
  //
  IFXCornerIter walkerA;
  m_pNeighborMesh->GetCornerIter(meshIndex, faceIndex, xCornerIndex, walkerA);

  IFXFaceFlags* pFaceFlags = walkerA.GetCurrentFace()->GetFaceFlags();

  IFXCornerIter walkerB;
  walkerB.Clone(&walkerA);

  // We'll call this the child corner (it doesn't matter)
  walkerA.MoveClockwise();
  IFXCornerIter prevWalkerA;
  do
  {
    prevWalkerA.Clone(&walkerA);  // face Q
    walkerA.JumpAcross();
  } while ((walkerA.GetFaceIndex() != faceIndex) ||
    (walkerA.GetMeshIndex() != meshIndex));
  walkerA.JumpAcross();  // face T

  // We'll call this the parent corner
  walkerB.MoveCounterClockwise();
  IFXCornerIter prevWalkerB;
  do
  {
    prevWalkerB.Clone(&walkerB);  // face S
    walkerB.JumpAcross();
  } while ((walkerB.GetFaceIndex() != faceIndex) ||
    (walkerB.GetMeshIndex() != meshIndex));
  walkerB.JumpAcross();  // face R

  if (pFaceFlags->duplicateFlag)
  {
    // This is a duplicate face.  We need to remove this face
    // from lists Lc and Lp (but we don't merge them).
    prevWalkerA.MoveLink(&walkerA);
    prevWalkerB.MoveLink(&walkerB);
    return;
  }

  // Check if Ep is an edge (if R is F)
  BOOL isEpEdge = 
	  (walkerB.GetMeshIndex() == meshIndex  &&  walkerB.GetFaceIndex() == faceIndex);

  // Check if Ec is an edge (if T is F)
  BOOL isEcEdge = 
	  (walkerA.GetMeshIndex() == meshIndex  &&  walkerA.GetFaceIndex() == faceIndex);

  if (isEpEdge && isEcEdge)
  {
    // both sides are edges -- do nothing
    //U32 tmp = 0;
  }
  else if (isEpEdge)
  {
    // we have an edge -- make Q point to T
    // Make face/corner Q point to face/corner T
    prevWalkerA.MoveLink(&walkerA);

  }
  else if (isEcEdge)
  {
    // we have an edge -- make S point to R
    // Make face/corner S point to face/corner R
    prevWalkerB.MoveLink(&walkerB);
  }
  else
  {
    // join circular linked lists
    // Make face/corner Q point to face/corner R
    prevWalkerA.MoveLink(&walkerB);
    prevWalkerB.MoveLink(&walkerA);
  }
}


void IFXNeighborResController::IncreaseResolution()
{
  // Undo distal merges
  IFXDistalEdgeMerge* pMerge = m_distalMerges[m_resolution];
  while (pMerge)
  {
    ApplyEdgeMerge(pMerge);
    pMerge = pMerge->m_pNext;
  }

  ++m_resolution;
  for(I32 meshIndex = m_numMeshes - 1; meshIndex >= 0; meshIndex--)
  {
    U32* syncTable = m_pUpdatesGroup->GetSyncTable(meshIndex);
    U32 maxLocalRes = m_pUpdatesGroup->GetUpdates(meshIndex)->numResChanges;
    U32  localRes = m_pMeshStates[meshIndex].resolutionChangeIndex;

    if (localRes < maxLocalRes  &&  m_resolution > syncTable[localRes])
    {
      AddFaces(meshIndex);
    }
  }
}


void IFXNeighborResController::AddFaces(U32 meshIndex)
{
  ResolutionState* state = &(m_pMeshStates[meshIndex]);
  IFXUpdates* pUpdates = m_pUpdatesGroup->GetUpdates(meshIndex);
  IFXResolutionChange* rc = &(pUpdates->pResChanges[state->resolutionChangeIndex]);
  ++state->resolutionChangeIndex;
  // Increment the face updates index to reflect modified faces
  state->faceUpdateIndex += rc->numFaceUpdates;
  if (rc->deltaFaces == 0)
    return;

  IFXNeighborFace* pNeighborFaces = m_pNeighborMesh->GetNeighborFaceArray(meshIndex);

  U32 faceIndex = state->numFaces;
  state->numFaces += rc->deltaFaces;
  while (faceIndex < state->numFaces)
  {
    // Determine corner across from collapse edge.
    U32 xCornerIndex = pNeighborFaces[faceIndex].GetFaceFlags()->collapseIndex;
    if (xCornerIndex < NO_COLLAPSE_INDEX)
      AddFace(meshIndex, faceIndex, xCornerIndex);
    ++faceIndex;
  }
}

void IFXNeighborResController::AddFace(U32 meshIndex, U32 faceIndex, U32 xCornerIndex)
{
  // let p be the parent vertex
  // let c be the child vertex
  // let x be the corner across from collapsed edge
  // let F be the face to remove
  // let Ec be the edge across from c
  // let Ep be the edge across from p
  // let Ex be the edge being collapsed
  //
  //        p
  //   Ec  /|
  //     /  |
  // x /  F |
  //   \    | Ex
  //     \  |
  //   Ep  \|
  //        c
  //
  //  Before expansion, Ec and Ep are the same edge.
  //  let L be the linked list of faces around edge Ec/Ep.
  //
  //  Steps:
  //    Find the face Q in L pointed to by corner c from F.
  //    Find the face R in L that points to Q.
  //    Make R point to F.
  //
  //    Find the face S in L pointed to by corner p from F.
  //    Find the face T in L that points to S.
  //    Make T point to F.
  //
  //  This breaks list L into two circular linked lists,
  //  Lc and Lp, containing face F.
  //
  //  Exceptions:
  //    If Q is F (Ec is edge), then make T point to corner p on F.
  //    If S is F (Ep is edge), then make R point to corner c on F.


  IFXCornerIter walkerA;
  m_pNeighborMesh->GetCornerIter(meshIndex, faceIndex, xCornerIndex, walkerA);

  IFXFaceFlags* pFaceFlags = walkerA.GetCurrentFace()->GetFaceFlags();

  IFXCornerIter walkerB;
  walkerB.Clone(&walkerA);

  IFXCornerIter newFaceF;
  newFaceF.Clone(&walkerA);

  walkerA.MoveClockwise(); // We'll call this the child corner (it doesn't matter)
  //U32 childCorner = walkerA.GetCornerIndex();
  walkerA.JumpAcross(); // face Q
  U32 meshIndex1 = walkerA.GetMeshIndex();
  U32 faceIndex1 = walkerA.GetFaceIndex();
  BOOL isEcEdge = (meshIndex1 == meshIndex) && (faceIndex1 == faceIndex);
  IFXCornerIter prevWalkerA;
  do
  {
    prevWalkerA.Clone(&walkerA);  // face R
    walkerA.JumpAcross();
  } while ((walkerA.GetFaceIndex() != faceIndex1) ||
    (walkerA.GetMeshIndex() != meshIndex1));


  walkerB.MoveCounterClockwise(); // We'll call this the parent corner
  //U32 parentCorner = walkerB.GetCornerIndex();
  walkerB.JumpAcross(); // face S
  U32 meshIndex2 = walkerB.GetMeshIndex();
  U32 faceIndex2 = walkerB.GetFaceIndex();
  BOOL isEpEdge = (meshIndex2 == meshIndex) && (faceIndex2 == faceIndex);
  IFXCornerIter prevWalkerB;
  do
  {
    prevWalkerB.Clone(&walkerB);  // face T
    walkerB.JumpAcross();
  } while ((walkerB.GetFaceIndex() != faceIndex2) ||
    (walkerB.GetMeshIndex() != meshIndex2));

  if (pFaceFlags->duplicateFlag)
  {
    // This is a duplicate face.  We need to add this face
    // to the list on each edge.
    newFaceF.MoveClockwise(); // child corner
    prevWalkerA.MoveLink(&newFaceF);
    newFaceF.MoveClockwise(); // parent corner
    prevWalkerB.MoveLink(&newFaceF);
    return;
  }

  if (isEcEdge && isEpEdge)
  {
    // do nothing
    //U32 tmp = 0;
  }
  else if (isEcEdge)
  {
    // Make face/corner T point to parent corner in face F
    newFaceF.MoveCounterClockwise();  // parent corner
    prevWalkerB.MoveLink(&newFaceF);
  }
  else if (isEpEdge)
  {
    // Make face/corner R point to child corner in face F
    newFaceF.MoveClockwise();     // child corner
    prevWalkerA.MoveLink(&newFaceF);
  }
  else
  {
    // Make face/corner R point to parent corner in face F
    newFaceF.MoveCounterClockwise();  // parent corner
    prevWalkerA.MoveLink(&newFaceF);
    newFaceF.MoveCounterClockwise();  // child corner
    prevWalkerB.MoveLink(&newFaceF);
  }
}
