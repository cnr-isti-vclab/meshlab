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
	@file	IFXNeighborResController.h

	@note	Vertex merging often joins edges from separate meshes, for example 
			the edges in the teapot lid are "zippered" to the teapot. The 
			current implementation of the neighbor resolution controller 
			maintains the original boundries even though the faces are adjacent
			in low resolutions.
*/

#ifndef IFXNeighborResController_h
#define IFXNeighborResController_h

#include "IFXNeighborResControllerIntfc.h"

class IFXUpdatesGroup;
class IFXResolutionChange;
class IFXCoincidentVertexMap;
class IFXCLODManager;

class IFXNeighborResController : public IFXNeighborResControllerInterface
{
public:
	IFXNeighborResController();
	virtual ~IFXNeighborResController();

	/// Method used to generate clod neighbor mesh
	IFXRESULT IFXAPI 	InitMesh(
			IFXMeshGroup* pMeshGroup,
			IFXCLODManager* pCLODManager,
			IFXNeighborMesh* pNeighborMesh,
			IFXUpdatesGroup* pUpdatesGroup,
			IFXVertexMapGroup* pVertexMapGroup);


	/**
		Methods for progressive download of clod neighbor mesh

		Initilize resolution controller with a clodolution
		neighbor mesh that is ready for clod operations.  This
		is used in conjuction with progressive download of clod
		neighbor mesh.  Requires setting distal edge merge records
		as appropriate.

		@note  Can return IFX_E_OUT_OF_MEMORY
	*/
	IFXRESULT IFXAPI 	Initialize(
							IFXNeighborMesh* pNeighborMesh,
							IFXUpdatesGroup* pUpdatesGroup);

	/**
		Get the distal edge merge list for a resolution.
		Returns NULL if the no edge merges.
	*/
	IFXDistalEdgeMerge* IFXAPI GetDistalEdgeMerges(U32 resolution);

	/**
		 Set the list of distal edge merges for a resolution.
		 pList can be NULL.

		 NOTE:  - The m_otherVertex value is not used.
				- The distal edge records are allocated by the calling
				- procedure, but this destructor will free them.
	*/
	void IFXAPI 	SetDistalEdgeMerges(U32 resolution, IFXDistalEdgeMerge* pList);

	/** 
		Allocate an empty distal edge merge record for the
		purpose of creating a distal edge merge list.
	*/
	IFXDistalEdgeMerge* IFXAPI AllocateDistalEdgeRecord();

	//
	// Methods to change the resolution
	//
	void IFXAPI 	SetResolution(U32 resolution);
	void IFXAPI 	IncreaseResolution();
	void IFXAPI 	DecreaseResolution();

	enum EdgeType { KeeperEdge, CollapseEdge, EdgeEdge };

private:

	//
	// Edge to face map
	//
	class EdgeEntry
	{
	public:
		U32 m_vertex2;  ///< larger coincident vertex index of edge
		U32 m_mesh;     ///< mesh index
		U32 m_face;     ///< face index
		U32 m_corner;   ///< corner index
		EdgeEntry* m_next;
	};

	class EdgeMap
	{
	public:
		IFXRESULT Initialize(U32 maxVertexIndex, IFXNeighborMesh* pNeighborMesh);
		void FreeAll();
		IFXRESULT AddEdge(U32 vertex1, U32 vertex2, U32 mesh, U32 face, U32 corner);
		IFXRESULT AddOrReplaceEdge(
						U32 vertex1, U32 vertex2, U32 mesh, U32 face, U32 corner);
		BOOL FindEdge(U32 vertex1, U32 vertex2, U32* pMesh, U32* pFace, U32* pCorner);
		void RemoveEdge(U32 vertex1, U32 vertex2);

	private:
		void Lookup(
				U32 vertex1, U32 vertex2, EdgeEntry*** pppPreLink, EdgeEntry** ppNode);

		IFXNeighborMesh* m_pNeighborMesh;
		U32 m_numVertices;
		EdgeEntry** m_pTable;
	};

	EdgeMap* m_pEdgeMap;

	IFXDistalEdgeMerge** m_distalMerges;

	//
	// Pre-processing methods
	//
	IFXRESULT BuildEdgeMap();
	void DeleteEdgeMap();
	void ResetCLODFlags();
	void DetermineCollapsedEdges();
	void AnalyzeMergingEdges(U32 meshIndex, U32 resolution);
	BOOL CheckForDistalMerge(U32 other, U32 child, U32 parent);
	void AddDistalMergeRecord(U32 resolution, U32 other, U32 child, U32 parent);
	void UpdateEdgesInMap(U32 meshIndex);
	void FindNonDeletedFaceForEdge(U32 v1, U32 v2);
	void ApplyEdgeMerge(IFXDistalEdgeMerge* pMerge);

	void DecrementFaceCount(U32 meshIndex);
	BOOL IsDeletedFace(U32 meshIndex, U32 faceIndex);
	void MarkDuplicatesFromCorner(U32 meshIndex, U32 faceIndex, U32 cornerIndex);
	void MarkDuplicates(U32 meshIndex, U32 faceIndex);
	BOOL IsNotDuplicate(U32 meshIndex, U32 faceIndex);
	EdgeType ClassifyEdge(U32 meshIndex, U32 faceIndex, U32 cornerIndex);
	BOOL FindCollapseEdge(
				U32 meshIndex,
				U32* collapseFaceIndex,
				U32* collapseCornerIndex);
	BOOL FindEdgeEdge(
				U32 meshIndex,
				U32* collapseFaceIndex,
				U32* collapseCornerIndex);
	void MarkCollapseEdges(U32 meshIndex, U32 faceIndex, U32 cornerIndex);
	void MarkCollapseEdgesInMesh(U32 meshIndex);
	void AltRemoveFaces(U32 meshIndex);

	//
	// Runtime methods
	//
	void RemoveFaces(U32 meshIndex);
	void RemoveFace(U32 meshIndex, U32 faceIndex, U32 xCornerIndex);

	void AddFaces(U32 meshIndex);
	void AddFace(U32 meshIndex, U32 faceIndex, U32 xCornerIndex);

	//
	// Data
	//
	IFXMeshGroup* m_pMeshGroup;
	IFXCLODManager* m_pCLODManager;
	IFXNeighborMesh* m_pNeighborMesh;
	IFXUpdatesGroup* m_pUpdatesGroup;
	IFXCoincidentVertexMap* m_pVertexMap;
	U32 m_resolution;

	/**
		Maximum resolution after all updates have been downloaded.
	*/
	U32 m_finalMaxResolution;

	struct ResolutionState
	{
		U32 resolutionChangeIndex;
		U32 faceUpdateIndex;
		U32 prevNumFaces;
		U32 numFaces;
	};

	U32 m_numMeshes;
	ResolutionState* m_pMeshStates;
};

IFXINLINE IFXDistalEdgeMerge* IFXNeighborResController::AllocateDistalEdgeRecord()
{
	return new IFXDistalEdgeMerge;
}

#endif
