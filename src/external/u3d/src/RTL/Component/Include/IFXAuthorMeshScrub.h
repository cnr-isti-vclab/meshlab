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
	@file	IFXAuthorMeshScrub.h

			Declaration of IFXAuthorMeshScrub interface
*/

#ifndef IFXAuthorMeshScrub_H
#define IFXAuthorMeshScrub_H

#include <float.h>
#include "IFXAuthorMesh.h"
#include "IFXAuthorMeshMap.h"
#include "IFXProgressCallback.h"

// {C6BD0E94-6EC6-4FDF-A714-8F896E953E60}
IFXDEFINE_GUID(IID_IFXAuthorMeshScrub,
0xC6BD0E94, 0x6EC6, 0x4FDF, 0xA7, 0x14, 0x8F, 0x89, 0x6E, 0x95, 0x3E, 0x60);

/**
*  A structure whose parameters control the mesh scrubbing operation.
*  By default, bInvalidIndices, bUnusedAttributes, and RemoveUnusedMaterials
*  are true if no other conditions are specified.
*  
*/
struct IFXAuthorMeshScrubParams
{
	IFXAuthorMeshScrubParams();

	BOOL bOperatateInPlace; /**< Don't copy the mesh before scrubbing it. 
							This will cause this input data to be overwritten.
							*/
	BOOL bInvalidIndices; /**< Checks for out of range indices on any of the 
								Face attribute arrays, Faces with invalid 
								Indices will be removed. Additionally this 
								step will set unused face attributes to 
								IFX_AUTHOR_INVALID_INDEX;
						  */
	BOOL bZeroAreaFaces; /**< Finds any faces that are of Zero Area. 
						 */
	F32 ZeroAreaFaceTolerance; /**< Epsilon value used to determine if a 
					    Face is Zero Area. Defaults to 100.0f * FLT_EPSILON.
						 */
	BOOL bNormalizeNormals;/**<  Makes sure all normals are unit vectors.
							*/
	
	BOOL bUnusedAttributes;/**<  A master control which determines if
						         any of the following "RemoveUnused"
								 operations take place.
							*/
	BOOL RemoveUnusedMaterials;/**<  Removes all materials which are not
							         actually in use by the mesh.  
									 bUnusedAttributes must be TRUE for a
									 TRUE value of this variable.
								*/
	BOOL RemoveUnusedPositions;/**<  Removes any vertices that are not
							         actually used by the mesh
									 bUnusedAttributes must be TRUE for a
									 TRUE value of this variable.
								*/
	BOOL RemoveUnusedNormals;/**<  Removes any normals that are not
							       actually used by the mesh
								   bUnusedAttributes must be TRUE for a
								   TRUE value of this variable.
								*/
	BOOL RemoveUnusedDiffuseColors;/**<  Removes any diffuse vertex colors
								         that are not actually used by the
										 mesh.
									     bUnusedAttributes must be TRUE for a
									     TRUE value of this variable.
								*/
	BOOL RemoveUnusedSpecularColors;/**< Removes any specular vertex colors
									     that are not actually used by the
										 mesh.
									     bUnusedAttributes must be TRUE for a
									     TRUE value of this variable.
									*/
	BOOL RemoveUnusedTextureCoordinates;/**<  Removes any texture coordinates
										that are not actually used by the
										mesh.
									    bUnusedAttributes must be TRUE for a
									    TRUE value of this variable.
									*/
};

/**
*  The interface for the mesh scrubber.  Memory usage is approximately:
*  sizeof(U32) * 2 * Max(AttribSizes in IFXAuthorMeshDesc)
*		+ Additional Mesh copy if not operating in place. 
*/
class IFXAuthorMeshScrub : virtual public IFXUnknown
{
public:
	/**
	*  Removes illegal or troubling elements from a mesh.  This helps
	*  ensure trouble-free operation of the mesh compiler, and removes
	*  information that would waste space when streaming mesh
	*  data over the internet.
	*
	*  @param pInAuthorMesh     The IFXAuthorMesh to be scrubbed
	*  @param ppOutAuthorMesh   The scrubbed IFXAuthorMesh.  If this
	*    operation succeeds, ppOutAuthorMesh will have a reference
	*    added to it even if the algorithm is operating in place (in which
	*    case the input mesh will be AddRefed and returned as the output).
	*    You will need to release this reference to avoid leaking the
	*    memory associated with the mesh.
    *  @param ppOutMeshMap      A map between pInAuthorMesh and the
	*                            output mesh ppOutAuthorMesh which informs
	*                            the developer how the mesh and its
	*                            attributes were changed by the scrubbing
	*                            operation.
	*  @param pInParams   The parameters which control the scrubbing
	*                      operation.
	*  @param pInProgressCallback  A callback which can be used to
	*                               give the user or developer a progress
	*                               report on the scrubbing operation.
	*/
	virtual IFXRESULT IFXAPI Scrub(
								IFXAuthorMesh* pInAuthorMesh, 
								IFXAuthorMesh** ppOutAuthorMesh, 
								IFXAuthorMeshMap** ppOutMeshMap,
								IFXAuthorMeshScrubParams* pInParams= NULL, 
								IFXProgressCallback* pInProgressCallback = NULL) = 0;
};


IFXINLINE IFXAuthorMeshScrubParams::IFXAuthorMeshScrubParams()
{
	bOperatateInPlace = FALSE;
	bInvalidIndices = TRUE; 
	bZeroAreaFaces = TRUE;
	ZeroAreaFaceTolerance = 100.0f * FLT_EPSILON;
	bNormalizeNormals = FALSE;
	bUnusedAttributes = TRUE;
	RemoveUnusedMaterials = TRUE;
	RemoveUnusedPositions = FALSE;
	RemoveUnusedNormals = FALSE;
	RemoveUnusedDiffuseColors = FALSE;
	RemoveUnusedSpecularColors = FALSE;
	RemoveUnusedTextureCoordinates = FALSE;
}


#endif
