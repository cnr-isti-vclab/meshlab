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
	@file	IFXNeighborResControllerInterface.h

	@note	Vertex merging often joins edges from separate meshes, for example the 
			edges in the teapot lid are "zippered" to the teapot.  The current 
			implementation of the neighbor resolution controller maintains the original
			boundaries even though the faces are adjacent in low resolutions.
*/

#ifndef IFXNeighborResControllerInterface_h
#define IFXNeighborResControllerInterface_h


class IFXUpdatesGroup;
class IFXResolutionChange;

class IFXCLODManager;
class IFXNeighborMesh;
class IFXVertexMapGroup;

/** 
	Distal Edge Merge Data
	Record that contains the information for merging the
	list around edge A with the list around edge B.
*/
class IFXDistalEdgeMerge
{
public:
	/** The "other" (non-child or parent) vertex of the merging edge.
	Used to prevent multiple merges for the same edge.
	[This is used only during preprocessor phase.]*/
	U32 m_otherVertex;		

	/** Pointer to a face containing "edge A". */
	U32 m_meshA;
	U32 m_faceA;
	/**Corner across from A. */
	U32 m_cornerA;	

	/** Pointer to a face containing "edge B". */
	U32 m_meshB;
	U32 m_faceB;
	/**Corner across from B. */
	U32 m_cornerB;	 

	IFXDistalEdgeMerge* m_pNext;	///< next merge
};

/**
	The IFXNeighborResController interface maintains original boundaries of vertices, 
	even though the faces are adjacent in low resolutions. Vertex merging often joins 
	edges from separate meshes.
*/
class IFXNeighborResControllerInterface
{
public:
	/**
		This method generates a clodolution neighbor mesh.
	
		@param	pMeshGroup	Pointer to the IFXMeshGroup
		@param	pCLODManager	Pointer to the IFXCLODManager
		@param	pNeighborMesh	Pointer to the IFXNeighborMesh
		@param	pUpdatesGroup	Pointer to the IFXUpdatesGroup
		@param	pVertexMapGroup	Pointer to the IFXVertexMapGroup

		@return	IFXResult
	*/
	virtual IFXRESULT IFXAPI  InitMesh(	
								IFXMeshGroup* pMeshGroup,
								IFXCLODManager* pCLODManager,
								IFXNeighborMesh* pNeighborMesh,
								IFXUpdatesGroup* pUpdatesGroup,
								IFXVertexMapGroup* pVertexMapGroup) = 0;
	//
	// Methods for progressive download of clod neighbor mesh
	//
	/**
		This method This method initializes resolution controller with a clodolution 
		neighbor mesh that is ready for clod operations. This is used in conjunction with 
		progressive download of clod neighbor mesh. Requires setting distal edge merge 
		records as appropriate.
	
		@param	pNeighborMesh	Pointer to the IFXNeighborMesh
		@param	pUpdatesGroup	Pointer to the IFXUpdatesGroup
		

		@return	IFXResult

		@note	Can return IFX_E_OUT_OF_MEMORY
	*/
		  
	virtual IFXRESULT IFXAPI  Initialize(
									IFXNeighborMesh* pNeighborMesh,
									IFXUpdatesGroup* pUpdatesGroup) = 0;

	/** 
		Gets the distal edge merge list for a resolution.Returns NULL if the no edge 
		merges.

		@param	resolution	Resolution value
		
		@return	IFXDistalEdgeMerge*	
	*/
	virtual IFXDistalEdgeMerge* IFXAPI GetDistalEdgeMerges(U32 resolution) = 0;

	/**
		Sets the list of distal edge merges for a resolution. pList can be NULL.
		
		@param	resolution	Resolution value
		@param	pList	Pointer to the IFXDistalEdgeMerge object
		
		@note	The m_otherVertex value is not used (set it to 0).
		The distal edge records are allocated by calling AllocateDistalEdgeRecord() 
		and are freed by the destructor.
	*/
	virtual void IFXAPI  SetDistalEdgeMerges(U32 resolution, IFXDistalEdgeMerge* pList) = 0;

	/** 
		This method allocates an empty distal edge merge record for the purpose 
		of creating a distal edge merge list.

		@return	IFXDistalEdgeMerge*	
	*/
	virtual IFXDistalEdgeMerge* IFXAPI AllocateDistalEdgeRecord() = 0;

	/** 
		This method sets the resolution.

		@param	resolution	Resolution value		
	*/
	virtual void IFXAPI  SetResolution(U32 resolution) = 0;

	/** 
		This method increases the resolution.			
	*/
	virtual void IFXAPI  IncreaseResolution() = 0;
	
	/** 
		This method decreases the resolution.			
	*/
	virtual void IFXAPI  DecreaseResolution() = 0;

protected:
	virtual ~IFXNeighborResControllerInterface() {};
};

extern "C"
{
/** 
	Class factory methods for static linking of the
	technology through a library.

  @todo Should be removed when IFXNeighborResControllerInterface
		is defined as IFXCOM interface.
*/
IFXNeighborResControllerInterface* IFXCreateNeighborResController();
void IFXDeleteNeighborResController(IFXNeighborResControllerInterface* pController);
}


#endif
