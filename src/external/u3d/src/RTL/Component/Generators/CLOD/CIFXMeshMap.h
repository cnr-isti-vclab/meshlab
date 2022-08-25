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
	@file	CIFXMeshMap.h
*/

#ifndef CIFXMeshMap_H
#define CIFXMeshMap_H

#include "IFXAuthorMesh.h"
#include "IFXMeshMap.h"

class CIFXMeshMap : public IFXMeshMap
{
public:
	// IFXUnknown.
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );
	
	// IFXMeshMap
	IFXRESULT IFXAPI 	Allocate(IFXAuthorMesh* pMesh);
	IFXRESULT IFXAPI 	Allocate(IFXAuthorLineSet* pLineSet);
	IFXRESULT IFXAPI 	Allocate(IFXAuthorPointSet* pPointSet);

	IFXRESULT IFXAPI 	Allocate(IFXMeshMap* pMeshMap);
	/**
	@note each map in pmesh map must be less than or equal to corresponding map
	*/
	IFXRESULT IFXAPI 	Concatenate(IFXAuthorMeshMap* pMapB);
	IFXRESULT IFXAPI 	ConcatenateMeshMap(IFXMeshMap* pMeshMap,IFXMeshMap** ppOutMap);
	IFXRESULT IFXAPI 	PopulateMeshMap(IFXAuthorMeshMap* pAuthorMeshMap);
	IFXRESULT IFXAPI 	AddIdentityMappingToMap(U32 mapIndex);

	IFXRESULT IFXAPI	AddMappingToMap(
							const U32 mapIndex, const U32 origVertexIndex, 
							const U32 indexMesh, U32 indexVertex);

	IFXVertexMap* IFXAPI GetFaceMap();
	IFXVertexMap* IFXAPI GetPositionMap();
	IFXVertexMap* IFXAPI GetNormalMap();
	IFXVertexMap* IFXAPI GetTextureMap();
	IFXVertexMap* IFXAPI GetDiffuseMap();
	IFXVertexMap* IFXAPI GetSpecularMap();
	IFXVertexMap* IFXAPI GetMap(U32 i);

    // Factory function.
    friend IFXRESULT IFXAPI_CALLTYPE CIFXMeshMap_Factory( 
										IFXREFIID interfaceId, void** ppInterface );

private:
	CIFXMeshMap();
	virtual ~CIFXMeshMap();

	IFXRESULT Construct( U32* pMapSizes );

	/** 
	For each attribute there is an IFXVertexMap that describes
	the mapping of that particular attribute.  The VertexMap name
	is to maintain compatability with the previous release.
	*/
	IFXVertexMap* m_pMaps[6];
	U32 m_refCount;
};

#endif
