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
	@file	CIFXAuthorMeshScrub.h

			Implementation of CIFXAuthorMeshScrub.
*/

#ifndef CIFXAuthorMeshScrub_H
#define CIFXAuthorMeshScrub_H

#include "IFXAuthorMeshScrub.h"
#include "IFXProgressCallback.h"
#include "IFXEnums.h"

class CIFXAuthorMeshScrub : public IFXAuthorMeshScrub
{
public:

	// Member functions:  IFXUnknown.
	U32 IFXAPI 		 AddRef ();
	U32 IFXAPI 		 Release ();
	IFXRESULT IFXAPI QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXAuthorMeshScrub
	IFXRESULT IFXAPI Scrub(
						IFXAuthorMesh*, IFXAuthorMesh**, 
						IFXAuthorMeshMap** ppOutMeshMap,
						IFXAuthorMeshScrubParams*, IFXProgressCallback*);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorMeshScrub_Factory( 
												IFXREFIID  interfaceId,
												void**   ppInterface );

private:
	CIFXAuthorMeshScrub();
	virtual ~CIFXAuthorMeshScrub();
	U32	m_refCount;	// Number of outstanding references to the object.

	// 
	IFXProgressCallback* m_pProgressCallback;
	F32 m_NumSteps;
	F32 m_CurStep;

	IFXAuthorMeshDesc m_ScrubbedMeshDesc;
	IFXAuthorMeshDesc m_OriginalMeshDesc;
	IFXAuthorMeshScrubParams* m_pParams;
	IFXAuthorMesh* m_pScrubMesh;
	U32* m_pTempBuffer;
	U32 m_TempBufferSize; // in # U32s

	BOOL m_bLocked;
	IFXAuthorMaterial* m_pMaterials;
	IFXVector3* m_pPositions;
	IFXVector3* m_pNormals;
	IFXVector4* m_pDiffuseColors;
	IFXVector4* m_pSpecularColors;
	IFXVector4* m_pTextureCoords;

	IFXAuthorFace* m_pPositionFaces;
	IFXAuthorFace* m_pNormalFaces;
	IFXAuthorFace* m_pDiffuseFaces;
	IFXAuthorFace* m_pSpecularFaces;
	IFXAuthorFace* m_pTexCoordFaces[IFX_MAX_TEXUNITS];
	U32* m_pFaceMaterials;
	U32* m_pBaseVertices;

	// Maps
	IFXAuthorMeshMap *m_pMeshMap;
	U32* m_pFaceMap;
	U32* m_pPositionMap;
	U32* m_pNormalMap;
	U32* m_pTextureCoordMap;
	U32* m_pDiffuseMap;
	U32* m_pSpecularMap;

	// 
	void Lock();
	void Unlock();
	
	void CalcProgressSteps();
	IFXRESULT UpdateProgress();
	
	void DetectUnusedAttrib(IFXAuthorFace* pInFace, U32* pUsage);
	void RemapFaces(IFXAuthorFace* pInFace, U32* pMap);
	void RemapBaseVertices(U32* pInBV, U32* pMap);
	void RemoveBaseVertices(U32* pRemoveBV);

	IFXRESULT RemoveInvalidIndices();
	IFXRESULT RemoveZeroAreaFaces();	
	IFXRESULT NormalizeNormals();
	IFXRESULT RemoveUnusedMaterials();		
	IFXRESULT RemoveUnusedPositions();		
	IFXRESULT RemoveUnusedNormals();		
	IFXRESULT RemoveUnusedDiffuseColors();		
	IFXRESULT RemoveUnusedSpecularColors();		
	IFXRESULT RemoveUnusedTexCoords();	

	void BuildOutputMeshMap();

	void RemoveFaces(U32* pInRemoveFace);
};

#endif
