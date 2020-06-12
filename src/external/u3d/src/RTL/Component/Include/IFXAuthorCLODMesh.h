//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
	@file	IFXAuthorCLODMesh.h

			CLOD related interfaces to the AuthorMesh. Many exporter writers won't
			need to know about this, so we kept it separate from AuthorMesh interface.
*/

#ifndef IFXAuthorCLODMesh_H
#define IFXAuthorCLODMesh_H

#include "IFXAuthorMesh.h"
#include "IFXAuthorUpdate.h"

// {AC25DA88-E651-4C6A-B690-B5D7C1E400EE}
IFXDEFINE_GUID(IID_IFXAuthorCLODMesh,
0xAC25DA88, 0xE651, 0x4C6A, 0xB6, 0x90, 0xB5, 0xD7, 0xC1, 0xE4, 0x00, 0xEE);

/**
 *	An IFXAuthorMesh derivative that describes a multi-resolution 
 *  representation of an IFXAuthorMesh.  This representation differs from
 *  the IFXAuthorMesh in that its data may be reordered, and it contains
 *  additional "update records" which describe how to add or remove
 *  mesh vertices, faces, etc. at each mesh resolution change.
 */
class IFXAuthorCLODMesh : public IFXAuthorMesh
{
public:

	/**
	*  Hands over the update records from CLODGen to the IFXAuthorMesh.
	*  The IFXAuthorCLODMesh takes ownership of the update records and
	*  deletes them when the mesh goes away.\n 
	*  The Array of updates is assumed to be the same size as the 
	*  NumPositions in the  MaxMeshDesc. 
	*
	*  @param  pUpdates  The array of updates describing the
	*          clodolution representation of the mesh.
	*
	*
	*  @return An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*
	*  @note  Intended for internal use only.
	*/
	virtual IFXRESULT IFXAPI  SetUpdates(IFXAuthorVertexUpdate* pUpdates) = 0;

	/**
	*  Gets the update records from the mesh for direct modification. 
	*  Mesh must be locked for this to work.
	*
	*  @param ppOutUpdates  The updates to modify
	*
	*  @return An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_INVALID_POINTER	The input argument was NULL.
	*  @retval  IFX_E_AUTHOR_MESH_NOT_LOCKED  The IFXAuthorMesh was not locked
	*             before this method was called.
	*/
	virtual IFXRESULT IFXAPI  GetUpdates(IFXAuthorVertexUpdate** ppOutUpdates) = 0;


	/**
	*  Retrieves a specific vertex update for this mesh.
	*
	*  @param  index             The update record to retrieve.
	*  @param  pOutVertexUpdate   The returned update record
	*
	*  @return An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_INVALID_POINTER  The return pointer is invalid. 
	*      Possible a NULL pointer parameter is passed in to this function,
	*      i.e. ir = GetVertexUpdate( uIndex, NULL);
	*  @retval  IFX_E_INVALID_RANGE	   The index value specified may be
	*	out of range, this parameter cannot be less than zero (0) or greater
	*   than NumPositions in the Mesh Description.
	*/
	virtual IFXRESULT IFXAPI  GetVertexUpdate(
									U32 index,
									const IFXAuthorVertexUpdate*& pOutVertexUpdate) = 0;

	/**
	*  Sets a specific vertex update for this mesh.
	*
	*  @param  index             The update record to set.
	*  @param  pInVertexUpdate   The new update record
	*
	*  @return An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_INVALID_POINTER  The return pointer is invalid. 
	*      Possible a NULL pointer parameter is passed in to this function,
	*      i.e. ir = SetVertexUpdate( uIndex, NULL);
	*  @retval  IFX_E_INVALID_RANGE	   The index value specified may be
	*	out of range, this parameter cannot be less than zero (0) or greater
	*   than NumPositions in the Mesh Description.
	*/
	virtual IFXRESULT IFXAPI  SetVertexUpdate(
									U32 index,
									const IFXAuthorVertexUpdate* pInVertexUpdate) = 0;

	/**
	*  As the mesh streams in, indicates current 
	*  max resolution (expressed in vertices).  This function is called
	*  every time more of the mesh is streamed in.
	*
	*  @param  maxResolution  The new maximum resolution (in vertices).
	*
	*  @return The new maximum resolution (in vertices).
	*/
	virtual U32 IFXAPI 	SetMaxResolution(U32 maxResolution) = 0;

	/**
	*  The number of vertices currently available in the mesh.  This value
	*  can change as more of the mesh streams in from the file.
	*
	*  @return  The current maximum mesh resolution
	*/
	virtual U32 IFXAPI  GetMaxResolution() = 0;

	/**
	*  Sets the minimum resolution of the mesh.  Mesh data above this 
	*  is stored in a clodolution representation, and streams in over
	*  time.  Mesh data at or below this resolution is loaded as a single
	*  block.  The mesh cannot be rendered at a resolution below the value
	*  specified here.
	*
	*  @param in_minResolution  The minimum resolution value (in vertices).
	*
	*  @param  maxResolution  The new minimum mesh resolution (in vertices).

	*  @note  This is a compression-time setting.  After the mesh has been
	*  compressed, the minimum resolution of the mesh cannot be adjusted.
	*/
	virtual U32 IFXAPI  SetMinResolution(U32 in_minResolution) = 0;

	/**
	*  Returns the minimum resolution of the mesh.  Mesh data above this 
	*  is stored in a clodolution representation, and streams in over
	*  time.  Mesh data at or below this resolution is loaded as a single
	*  block.  The mesh cannot be rendered at a resolution below the value
	*  specified here.
	*
	*  @return  The current minimum mesh resolution (in vertices).
	*/
	virtual U32 IFXAPI  GetMinResolution() = 0;
	
	/**
	*  Specifies the total number of vertices that will be available in
	*  the mesh after all streaming is complete (the multi-resolution portion
	*  of the mesh is the only part that streams).
	*
	*  @param in_finalMaxResolution  The total number of vertices present
	*                                in the mesh when at full resolution.
	*
	*  @return  The new final maximum mesh resolution (in vertices).
	*/
	virtual U32 IFXAPI  SetFinalMaxResolution(U32 in_finalMaxResolution) = 0;

	/**
	*  Returns the total number of vertices that will be available in
	*  the mesh after all streaming is complete (the multi-resolution portion
	*  of the mesh is the only part that streams).
	*
	*  @return The final maximum mesh resolution (in vertices).
	*/
	virtual U32 IFXAPI  GetFinalMaxResolution() = 0;
	
	/**
	*  Changes the mesh's resolution by applying update records to alter the
	*  current clodolution representation of the mesh.
	*
	*  @param  in_newResolution  The new resolution of the mesh (in vertices).
	*
	*  @return  The new resolution of the mesh, which may differ from
	*   in_newResolution if the mesh has no update records, whether or not the mesh is
	*   locked, or if in_newResolution is outside the bounds of the mesh's
	*   minimum or maximum resolution.
	*/
	virtual U32 IFXAPI  SetResolution(U32 in_newResolution) = 0;

	/**
	*  Returns the mesh's resolution.
	*
	*  @return  The resolution of the mesh (in vertices).
	*/
	virtual U32 IFXAPI  GetResolution() = 0;
};

#endif
