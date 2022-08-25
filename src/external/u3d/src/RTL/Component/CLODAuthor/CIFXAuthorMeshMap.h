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
#ifndef __CIFXAuthorMeshMap_H__
#define __CIFXAuthorMeshMap_H__

#include "IFXAuthorMeshMap.h"
#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXAuthorMesh.h"

/// Describes the mapping from one author mesh to another.
 class CIFXAuthorMeshMap : IFXAuthorMeshMap
{
public:
	CIFXAuthorMeshMap();
	virtual ~CIFXAuthorMeshMap();
	// Member functions:  IFXUnknown.
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );
	
	/** Allocates a map of sufficient size to fully describe the mapping of pMesh. 
	Initialized to the identity mapping.*/
	IFXRESULT IFXAPI 	Allocate(IFXAuthorMesh *pMesh);
	
	IFXRESULT IFXAPI 	AllocateMaps();

	/// Constructs a full copy of this map, returns null if out of memory. 
	IFXAuthorMeshMap* IFXAPI Clone();   

	/** Combines two maps into one equivalent map.
	This map is modified in place.
	Before: inputMesh --> thisMap --> IntermediateMesh --> MapB --> resultMesh
	After : inputMesh --> thisMap --> resultMesh
	each map in pmesh map must be less than or equal to corresponding map*/
	IFXRESULT IFXAPI 	Concatenate(IFXAuthorMeshMap *in_pMapB);

	// For each attribute there is an array of U32.
	// The array specifies the mapping, for example if 
	// vertex #7 moves to position #3 then array[7] = 3.
	// If vertex #7 is not mapped to the output then
	// array[7] = IFX_NULL_INDEX32

	// These return a pointer to an array of U32, which describes the mapping.
	// Simply modify the array to change the mapping.
	U32* IFXAPI 	GetFaceMap();  	
	U32* IFXAPI 	GetPositionMap();
	U32* IFXAPI 	GetNormalMap();
	U32* IFXAPI 	GetTextureMap();
	U32* IFXAPI 	GetDiffuseMap();
	U32* IFXAPI 	GetSpecularMap();

	U32* IFXAPI 	GetMap(U32 mapIndex);
	U32  IFXAPI 	GetMapSize(U32 mapIndex);
	void IFXAPI 	SetMapSize(U32 mapIndex, U32 value);

private:
	U32 *m_pMaps[6];  ///< one map for each mesh attribute (faces, positions, normals, textures, diffuse color, specular color)
	U32 m_MapSizes[6]; ///< indicates size of each attribute map.
	U32 m_refCount;

	IFXRESULT internalAlloc();
};

#endif
