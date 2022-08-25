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
	@file	IFXAuthorMeshMap.cpp
*/
#ifndef __IFXAuthorMeshMap_H__
#define __IFXAuthorMeshMap_H__


#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXAuthorMesh.h"

// {6103473B-E29D-4f30-BCAA-5ABCC24C787A}
IFXDEFINE_GUID(IID_IFXAuthorMeshMap,
0x6103473b, 0xe29d, 0x4f30, 0xbc, 0xaa, 0x5a, 0xbc, 0xc2, 0x4c, 0x78, 0x7a);
 
#define IFX_NULL_INDEX32 0xffffffff

/**
*  Describes the mapping between an author mesh and a modifier version of
*  the mesh.  This class is most commonly used to determine how the mesh
*  has changed after it is scrubbed or compressed, or if a multi-resolution 
*  version of the mesh has been created (at the very least, these operations
*  reorder the mesh elements.\n
*  For each attribute there is an array of U32.  The array specifies the 
*  mapping.  For example, if vertex #7 in the input  mesh moves to 
*  position #3 in the output mesh, then array[7] = 3.  If vertex #7 is 
*  not mapped to the output, then array[7] = IFX_NULL_INDEX32
*/
class IFXAuthorMeshMap : public IFXUnknown
{
public:
	
	/**
	*  Allocates a map of sufficient size to fully describe the mapping of 
	*  pMesh.  Initialized to the identity mapping.
	*
	*  @param pMesh     The mesh for which to allocate the mapping.
	*
	*  @return	An IFXRESULT value
	*  @retval	IFX_OK	   No error.
	*  @retval IFX_E_OUT_OF_MEMORY   Not enough memory to allocate map.
	*/
	virtual IFXRESULT IFXAPI Allocate(IFXAuthorMesh *pMesh) = 0;

	/**
	*  Constructs a full copy of this map.
	*
	*  @return A pointer to an object of type IFXAuthorMeshMap
	*  @retval NULL   The clone operation failed, most likely due to
	*                 insufficient memory.
	*  @retval non-NULL   A pointer to the cloned IFXAuthorMeshMap.
	*/
	virtual IFXAuthorMeshMap* IFXAPI Clone() = 0;  

	/**
	*  Combines two IFXAuthorMeshMaps into one equivalent map.  The map is
	*  modified in-place.\n
	*  Before: inputMesh --> thisMap --> IntermediateMesh --> MapB --> resultMesh\n
	*  After : inputMesh --> thisMap --> resultMesh
	*
	*  @param  in_pMapB  The IFXAuthorMeshMap to merge into the current
	*                    IFXAuthorMeshMap.
	*
	*  @return	An IFXRESULT value
	*  @retval	IFX_OK	   No error.
	*/
	virtual IFXRESULT IFXAPI Concatenate(IFXAuthorMeshMap *in_pMapB) = 0;  // each map in pmesh map must be less than or equal to corresponding map

	/**
	*  Describes the mapping of the faces between the original mesh and
	*  this mesh.
	*
    *  @return An array of U32
	*  @retval A valid pointer to a mapping array
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetFaceMap() = 0;
	/**
	*  Describes the mapping of the vertex positions between the original 
	*  mesh and this mesh.
	*
    *  @return An array of U32
	*  @retval A valid pointer to a mapping array
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetPositionMap() = 0;
	/**
	*  Describes the mapping of the normals between the original mesh and
	*  this mesh.
	*
    *  @return An array of U32
	*  @retval A valid pointer to a mapping array
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetNormalMap() = 0;
	/**
	*  Describes the mapping of the texture coordinates between the original 
	*  mesh and this mesh.
	*
    *  @return An array of U32
	*  @retval A valid pointer to a mapping array
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetTextureMap() = 0;
	/**
	*  Describes the mapping of the diffuse vertex colors between the  
	*  original mesh and this mesh.
	*
    *  @return An array of U32
	*  @retval A valid pointer to a mapping array
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetDiffuseMap() = 0;
	/**
	*  Describes the mapping of the specular vertex colors between the  
	*  original mesh and this mesh.
	*
    *  @return An array of U32
	*  @retval A valid pointer to a mapping array
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetSpecularMap() = 0;

	/**
	*  Recovers a mapping between the original mesh and this mesh by index.
	*
	*  @param mapIndex  The index of the map to return:  0=faces, 1=positions,
	*  2=normals, 3=texure coordinates, 4=diffuse vertex colors, 5=specular
	*  vertex colors
	*
    *  @return An array of U32
	*  @retval A valid pointer
	*  @retval NULL if there is no mapping for the faces.
	*/
	virtual U32* IFXAPI GetMap(U32 mapIndex) = 0;
	/**
	*  Returns the size of the mapping between the original mesh and 
	*  this mesh by index.
	*
	*  @param mapIndex  The index of the map whose size to return:  0=faces, 
	*  1=positions, 2=normals, 3=texure coordinates, 4=diffuse vertex colors,
	*  5=specular vertex colors
	*
    *  @return A U32
	*  @retval The size of the map
	*  @retval 0 if the map does not exist.
	*/
	virtual U32 IFXAPI GetMapSize(U32 mapIndex) = 0;

	/**
	*  Sets the size of the mapping between the original mesh and 
	*  this mesh by index.
	*
	*  @param mapIndex  The index of the map whose size to return:  0=faces, 
	*  1=positions, 2=normals, 3=texure coordinates, 4=diffuse vertex colors,
	*  5=specular vertex colors
	*  @param value  Actual size of the mapping
	*
	*  @return void
	*/
	virtual void IFXAPI SetMapSize(U32 mapIndex, U32 value) = 0;

	virtual IFXRESULT IFXAPI AllocateMaps() = 0;
};

#endif
