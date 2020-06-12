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
	@file	CIFXMeshCompiler.h

			Implementation of CIFXMeshCompiler.
*/

#ifndef CIFXMeshCompiler_H
#define CIFXMeshCompiler_H

#include "IFXMeshCompiler.h"
#include "IFXUpdatesGroup.h"
#include "IFXMeshMap.h"
#include "IFXEnums.h" 

class CIFXMeshCompiler : public IFXMeshCompiler
{
public:
	CIFXMeshCompiler();
	virtual ~CIFXMeshCompiler();
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );
	
	virtual IFXRESULT IFXAPI  PreviewCompile(	IFXAuthorCLODMesh*pInputMesh);

	virtual IFXRESULT IFXAPI  InitCompile(IFXAuthorCLODMesh*, U32 numOutputSizes,
												IFXMeshSize* OutputSizeArray);	
	virtual IFXRESULT IFXAPI  Compile();
	
	virtual IFXMeshGroup* IFXAPI GetMeshGroup();
	virtual IFXUpdatesGroup* IFXAPI GetUpdatesGroup();
	virtual IFXMeshMap* IFXAPI GetMeshMap();
	virtual IFXRESULT IFXAPI  GetIFXMeshSizes(
									U32* pNumSizes, 
									IFXMeshSize** ppMeshSizes);
	virtual void IFXAPI  SetCosMaxNormalError(F32 fCosMaxNormalError);
	virtual F32 IFXAPI  GetCosMaxNormalError();

	class VertexDescriptor 
	{
	public:
		VertexDescriptor();
		~VertexDescriptor();
		VertexDescriptor* Clone();
		BOOL EqualAttributes(VertexDescriptor*);
	
		/**
			this will go unused for static compile, keep it since
			this struct is nice 32 byte cache line friendly.
		*/
		IFXVector3 Normal;
		U32 AuthorIndex;
		U32	IFXIndex;
		U32 NumAttributes;
		U32 *pAttributes;    ///< array of indicies for each attribute
		VertexDescriptor *pNext;
	};
	
private:

	IFXRESULT StreamCompile();
	IFXRESULT StaticCompile();

	class VertexHash
	{
	public:
		VertexHash();
		~VertexHash();
		IFXRESULT Allocate(U32 numAuthorVerts);
		void Insert(VertexDescriptor*);
		VertexDescriptor* FindBestMatch(VertexDescriptor*);
		VertexDescriptor* FindExactMatch(VertexDescriptor*); ///< used by static compile.
	private:
		VertexDescriptor** m_ppArray;
		U32 m_Size;
	};

	IFXRESULT allocateOutputs(U32 numSizes, IFXMeshSize*);
	IFXRESULT commonInit();
	IFXRESULT allocateQueryVerts();
	IFXRESULT compileFace(U32 face);
	IFXRESULT compileUpdate(IFXAuthorFaceUpdate* pFU);
	IFXRESULT findOrBuildVertex(U32 authorFaceCorner, U32 authorFaceIndex, 
								U32 mat, IFXResolutionChange *pRC, 
								U32* pOutIFXVertexIndex);
	
	U32 m_refCount;
	U32 m_numMaterials;
	IFXAuthorCLODMesh*	m_pAM;  ///< input author mesh
	IFXMeshGroup*		m_pMG;      ///< output mesh 
	IFXUpdatesGroup*	m_pUG;      ///< output CLOD updates.
	IFXMeshSize*		m_pMeshSizes;

	/// array of flags, one for each material in the mesh. 
	BOOL*			m_MaterialUpdated;

	IFXMeshMap*		m_pMeshMap;
	VertexHash*		m_pVertexHash;

	/// array of pre-allocated query verts, one for each material.
	VertexDescriptor*	m_pQV;

	/// true if compiling for streaming mode, false for static mode.
	BOOL m_Stream;

	/// true if InitCompile has been called properly.
	BOOL m_InitComplete;

	/// true once the static portion of compile has completed;
	BOOL m_StaticCompileComplete;

	/// resolution that we were at after previous call to compile.
	U32 m_LastResolution;

	/// used for fast compare against dot prod of normals.
	F32 m_CosMaxNormalError;

	// These are for fast direct access to the author mesh, avoids interface overhead.
	IFXAuthorVertexUpdate *m_pUpdates;
	IFXAuthorMaterial *m_pMaterials;
	IFXVector3		*m_pPositions;
	IFXVector3		*m_pNormals;
	IFXVector4		*m_pTextures;
	IFXVector4		*m_pSpeculars;
	IFXVector4		*m_pDiffuses;
	U32				*m_pFaceMaterials;

	IFXAuthorFace	*m_pPositionFaces;
	IFXAuthorFace	*m_pNormalFaces;
	IFXAuthorFace	*m_pTextureFaces[IFX_MAX_TEXUNITS];
	IFXAuthorFace	*m_pSpecularFaces;
	IFXAuthorFace	*m_pDiffuseFaces;

	/// pointer to array of iters, one for each material in output ifx mesh.
	IFXVertexIter	*m_pIteratorCache;
	
	// fast access to maps for each attribute
	IFXVertexMap *m_pFaceMap;
	IFXVertexMap *m_pPositionMap;
	IFXVertexMap *m_pNormalMap;
	IFXVertexMap *m_pTextureMap;
	IFXVertexMap *m_pDiffuseMap;
	IFXVertexMap *m_pSpecularMap;
};

IFXINLINE CIFXMeshCompiler::VertexDescriptor::VertexDescriptor()
{
	NumAttributes = 0;
	pAttributes = NULL;
	pNext = NULL;
	AuthorIndex = 0;
	IFXIndex = 0;
}

IFXINLINE CIFXMeshCompiler::VertexDescriptor::~VertexDescriptor()
{
	IFXDELETE_ARRAY(pAttributes);
	if(pNext) delete pNext;   // delete the chain
}

IFXINLINE CIFXMeshCompiler::VertexDescriptor*
CIFXMeshCompiler::VertexDescriptor::Clone()
{
		VertexDescriptor *pNew = new VertexDescriptor;
		if(pNew == NULL) return pNew;

		*pNew = *this;
		U32 *pAttributesNew = new U32[this->NumAttributes];
		if(pAttributesNew == NULL) 
		{ 
			delete pNew;
			pNew = NULL;
			return pNew;
		}
		pNew->pAttributes = pAttributesNew;

		U32 i;
		for(i=0; i < this->NumAttributes;i++)
			pNew->pAttributes[i] = this->pAttributes[i];

		return pNew;
}

IFXINLINE BOOL 
CIFXMeshCompiler::VertexDescriptor::EqualAttributes(VertexDescriptor* v)
{
	U32 i;
	for(i = 0; i < NumAttributes; i++)
		if(v->pAttributes[i] != this->pAttributes[i])
			return FALSE;

	return TRUE;
}

CIFXMeshCompiler::VertexHash::VertexHash()
{
	m_ppArray = NULL;
	m_Size = 0;
}

CIFXMeshCompiler::VertexHash::~VertexHash()
{
	U32 i;
	for(i=0; i<m_Size; i++)
		IFXDELETE(m_ppArray[i]);
	m_Size = 0;
	IFXDELETE_ARRAY(m_ppArray);
}

IFXRESULT CIFXMeshCompiler::VertexHash::Allocate(U32 numAuthorPositions)
{
	m_ppArray = new VertexDescriptor*[numAuthorPositions];
	if(m_ppArray) 
	{
		memset(m_ppArray,0,numAuthorPositions*sizeof(VertexDescriptor*));
		m_Size = numAuthorPositions;
		return IFX_OK;
	}
	else
		return IFX_E_OUT_OF_MEMORY;
}

IFXINLINE void CIFXMeshCompiler::VertexHash::Insert(VertexDescriptor *pVD)
{
	pVD->pNext = m_ppArray[pVD->AuthorIndex];
	m_ppArray[pVD->AuthorIndex] = pVD;
}

// find best match on normal and exact match on all required attributes
// such as position, material, texture coord, colors.  
// This is used by the streaming CLOD compile.
CIFXMeshCompiler::VertexDescriptor* 
CIFXMeshCompiler::VertexHash::FindBestMatch(VertexDescriptor *pVD)
{
	VertexDescriptor *pCurrentVD, *pBestVD = NULL;
	F32 CosineBestAngle = -2.0; // cos(180) = -1, so -2 guarantees that any 
								// dot product done below will be assigned to this.

	F32 CosineCurrentAngle;

	pCurrentVD = m_ppArray[pVD->AuthorIndex];
	
	while(pCurrentVD)
	{
		// match exactly all attributes except normal
		if( pVD->EqualAttributes(pCurrentVD) )
		{
			CosineCurrentAngle = pVD->Normal.DotProduct(pCurrentVD->Normal);
			if(CosineCurrentAngle > CosineBestAngle)
			{
				pBestVD = pCurrentVD;
				CosineBestAngle = CosineCurrentAngle;
			}
		}
		pCurrentVD = pCurrentVD->pNext;
	}
	return pBestVD; 
}

IFXINLINE CIFXMeshCompiler::VertexDescriptor*
CIFXMeshCompiler::VertexHash::FindExactMatch(VertexDescriptor *pVD)
{
	VertexDescriptor *pCurrentVD;
	
	pCurrentVD = m_ppArray[pVD->AuthorIndex];
	
	while(pCurrentVD)
	{
		if( pVD->EqualAttributes(pCurrentVD) )  // match exactly all attributes
			return pCurrentVD;
		else
			pCurrentVD = pCurrentVD->pNext;
	}

	return pCurrentVD; 
}

#endif
