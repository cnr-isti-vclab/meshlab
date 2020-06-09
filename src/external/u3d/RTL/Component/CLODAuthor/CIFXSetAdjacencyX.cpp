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
@file CIFXSetAdjacencyX.cpp
	Implementation of CIFXSetAdjacencyX class.  
	This class is used by the progressive geometry compression and decompression.
*/
//*****************************************************************************
#include "CIFXSetAdjacencyX.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"

//-----------------------------------------------------------------------------
//	Public methods
//-----------------------------------------------------------------------------


// IFXUnknown
U32 CIFXSetAdjacencyX::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXSetAdjacencyX::Release()
{
	if ( !( --m_uRefCount )) 
	{
		delete this;
		return 0;
	}
	return m_uRefCount;
}

IFXRESULT CIFXSetAdjacencyX::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;
	if(ppInterface) {
		if (IID_IFXUnknown == interfaceId) 
		{
			*ppInterface = (IFXUnknown*) this;
			this->AddRef();
		} 
		else if (IID_IFXSetAdjacencyX == interfaceId) 
		{
			*ppInterface = (IFXSetAdjacencyX*) this;
			this->AddRef();
		}
		else 
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} 
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}


// IFXSetAdjacencyX
void CIFXSetAdjacencyX::InitializeX(IFXAuthorCLODMesh* pAuthorCLODMesh)
{
	// Check for already initialized
	if(NULL != m_pAuthorCLODMesh) 
	{
		IFXCHECKX(IFX_E_ALREADY_INITIALIZED);
	}
	// Check for null input pointer
	if(NULL == pAuthorCLODMesh) 
	{
		IFXCHECKX(IFX_E_INVALID_POINTER);
	}

	// Store the author mesh pointer
	m_pAuthorCLODMesh = pAuthorCLODMesh;
	m_pAuthorCLODMesh->AddRef();

	// Allocate the position set array
	m_uPositionSetArraySize = m_pAuthorCLODMesh->GetFinalMaxResolution();
	m_ppPositionSetArray = new IFXSetX*[m_uPositionSetArraySize];
	if(NULL == m_ppPositionSetArray) 
	{
		IFXCHECKX(IFX_E_OUT_OF_MEMORY);
	}
	memset(m_ppPositionSetArray,0,m_uPositionSetArraySize*sizeof(IFXSetX*));
}

void CIFXSetAdjacencyX::AddX(U32 uPositionIndex, U32 uFaceIndex) 
{
	// check input position index range
	if(uPositionIndex > m_uPositionSetArraySize) 
	{
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}
	// Allocate the set if necessary
	if(NULL == m_ppPositionSetArray[uPositionIndex]) 
	{
		IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX, 
			(void**)(m_ppPositionSetArray+uPositionIndex)));
	}
	// Add the face index to the set
	m_ppPositionSetArray[uPositionIndex]->AddX(uFaceIndex);
}

void CIFXSetAdjacencyX::RemoveX(U32 uPositionIndex, U32 uFaceIndex) 
{
	// check input position index range
	if(uPositionIndex > m_uPositionSetArraySize) 
	{
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}
	// If the position set exists
	if(NULL != m_ppPositionSetArray[uPositionIndex]) 
	{
		// Remove the face index from the set
		m_ppPositionSetArray[uPositionIndex]->RemoveX(uFaceIndex);
	}
}

void CIFXSetAdjacencyX::GetFaceSetX(U32 uPositionIndex, IFXSetX*& rpFaceSet) 
{
	// check input position index range
	if(uPositionIndex > m_uPositionSetArraySize) 
	{
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}
	// Allocate the set if necessary
	if(NULL == m_ppPositionSetArray[uPositionIndex]) 
	{
		IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX, (void**)(m_ppPositionSetArray+uPositionIndex)));
	}
	// Set the output face set for the position
	IFXRELEASE(rpFaceSet);
	rpFaceSet = m_ppPositionSetArray[uPositionIndex];
	rpFaceSet->AddRef();
}

void CIFXSetAdjacencyX::GetPositionSetX(IFXSetX& rFaceSet, IFXSetX*& rpPositionSet) 
{
	// Create the output position set
	IFXRELEASE(rpPositionSet);
	IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&rpPositionSet));

	// For each face in the face set
	U32 uSize = 0;
	U32 i = 0;
	rFaceSet.GetSizeX(uSize);
	IFXAuthorFace face;
	for(i = 0; i < uSize; i++) 
	{
		// Get the face
		U32 uFaceIndex = 0;
		rFaceSet.GetMemberX(i,uFaceIndex);
		m_pAuthorCLODMesh->GetPositionFace(uFaceIndex, &face);
		// Add each corner of the face to the output set
		rpPositionSet->AddX(face.VertexA());
		rpPositionSet->AddX(face.VertexB());
		rpPositionSet->AddX(face.VertexC());
	}
}

void CIFXSetAdjacencyX::GetNormalSetX(U32 uPositionIndex, U32 uNumFaces, IFXSetX*& rpNormalSet) 
{
	// Create the output normal set
	IFXRELEASE(rpNormalSet);
	IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&rpNormalSet));

	// Get the set of faces using that position
	IFXDECLARELOCAL(IFXSetX,pFaceSet);
	GetFaceSetX(uPositionIndex,pFaceSet);
		
	// For each face in the face set
	U32 uSize = 0;
	U32 i = 0;
	pFaceSet->GetSizeX(uSize);
	IFXAuthorFace facePositionFace, faceNormalFace;
	for(i = 0; i < uSize; i++) 
	{
		// Get the position face and the normal face
		U32 uFaceIndex = 0;
		pFaceSet->GetMemberX(i,uFaceIndex);
		if(uFaceIndex < uNumFaces) 
		{
			m_pAuthorCLODMesh->GetPositionFace(uFaceIndex, &facePositionFace);
			m_pAuthorCLODMesh->GetNormalFace(uFaceIndex, &faceNormalFace);
			// Find which corner of the face is using the position
			// and add the normal at that corner to the output set
			if(uPositionIndex == facePositionFace.VertexA()) 
			{
				rpNormalSet->AddX(faceNormalFace.VertexA());
			} 
			else if(uPositionIndex == facePositionFace.VertexB()) 
			{
				rpNormalSet->AddX(faceNormalFace.VertexB());
			} 
			else if(uPositionIndex == facePositionFace.VertexC()) 
			{
				rpNormalSet->AddX(faceNormalFace.VertexC());
			} 
			else 
			{
				IFXASSERT(0); 
			}
		} 
	}
}

void CIFXSetAdjacencyX::GetDiffuseColorSetX(U32 uPositionIndex, IFXSetX*& rpDiffuseColorSet)
{
	// Create the output diffuse color set
	IFXRELEASE(rpDiffuseColorSet);
	IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&rpDiffuseColorSet));

	// Get the set of faces using that position
	IFXDECLARELOCAL(IFXSetX,pFaceSet);
	GetFaceSetX(uPositionIndex,pFaceSet);
		
	// For each face in the face set
	U32 uSize = 0;
	U32 i = 0;
	pFaceSet->GetSizeX(uSize);
	IFXAuthorFace facePositionFace, faceDiffuseColorFace;
	for(i = 0; i < uSize; i++) 
	{
		// Get the position face and the diffuse color face
		U32 uFaceIndex = 0;
		pFaceSet->GetMemberX(i,uFaceIndex);
		U32 uMaterialID = 0;
		IFXCHECKX(m_pAuthorCLODMesh->GetFaceMaterial(uFaceIndex, &uMaterialID));

		IFXAuthorMaterial material;
		IFXCHECKX(m_pAuthorCLODMesh->GetMaterial(uMaterialID,&material));

		if(material.m_uDiffuseColors) 
		{
			IFXCHECKX(m_pAuthorCLODMesh->GetPositionFace(uFaceIndex, &facePositionFace));
			IFXCHECKX(m_pAuthorCLODMesh->GetDiffuseFace(uFaceIndex, &faceDiffuseColorFace));
			// Find which corner of the face is using the position
			// and add the diffuse color at that corner to the output set
			if(uPositionIndex == facePositionFace.VertexA()) 
			{
				rpDiffuseColorSet->AddX(faceDiffuseColorFace.VertexA());
			} 
			else if(uPositionIndex == facePositionFace.VertexB()) 
			{
				rpDiffuseColorSet->AddX(faceDiffuseColorFace.VertexB());
			} 
			else if(uPositionIndex == facePositionFace.VertexC()) 
			{
				rpDiffuseColorSet->AddX(faceDiffuseColorFace.VertexC());
			} 
			else 
			{
				IFXASSERT(0); 
			}
		}
	} 
}

void CIFXSetAdjacencyX::GetSpecularColorSetX(U32 uPositionIndex, IFXSetX*& rpSpecularColorSet)
{
	// Create the output specular color set
	IFXRELEASE(rpSpecularColorSet);
	IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&rpSpecularColorSet));

	// Get the set of faces using that position
	IFXDECLARELOCAL(IFXSetX,pFaceSet);
	GetFaceSetX(uPositionIndex,pFaceSet);
		
	// For each face in the face set
	U32 uSize = 0;
	U32 i = 0;
	pFaceSet->GetSizeX(uSize);
	IFXAuthorFace facePositionFace, faceSpecularColorFace;
	for(i = 0; i < uSize; i++) 
	{
		// Get the position face and the specular color face
		U32 uFaceIndex = 0;
		pFaceSet->GetMemberX(i,uFaceIndex);

		U32 uMaterialID = 0;
		IFXCHECKX(m_pAuthorCLODMesh->GetFaceMaterial(uFaceIndex, &uMaterialID));

		IFXAuthorMaterial material;
		IFXCHECKX(m_pAuthorCLODMesh->GetMaterial(uMaterialID,&material));

		if(material.m_uSpecularColors ) 
		{
			IFXCHECKX(m_pAuthorCLODMesh->GetPositionFace(uFaceIndex, &facePositionFace));
			IFXCHECKX(m_pAuthorCLODMesh->GetSpecularFace(uFaceIndex, &faceSpecularColorFace));
			// Find which corner of the face is using the position
			// and add the diffuse color at that corner to the output set
			if(uPositionIndex == facePositionFace.VertexA()) 
			{
				rpSpecularColorSet->AddX(faceSpecularColorFace.VertexA());
			}
			else if(uPositionIndex == facePositionFace.VertexB()) 
			{
				rpSpecularColorSet->AddX(faceSpecularColorFace.VertexB());
			} 
			else if(uPositionIndex == facePositionFace.VertexC()) 
			{
				rpSpecularColorSet->AddX(faceSpecularColorFace.VertexC());
			} 
			else 
			{
				IFXASSERT(0); 
			}
		} 
	} 
}

void CIFXSetAdjacencyX::GetTexCoordSetX(U32 uLayer, U32 uPositionIndex, IFXSetX*& rpTexCoordSet)
{
	// Create the output tex coord set
	IFXRELEASE(rpTexCoordSet);
	IFXCHECKX(IFXCreateComponent(CID_IFXSetX,IID_IFXSetX,(void**)&rpTexCoordSet));

	// Get the set of faces using that position
	IFXDECLARELOCAL(IFXSetX,pFaceSet);
	GetFaceSetX(uPositionIndex,pFaceSet);
		
	// For each face in the face set
	U32 uSize = 0;
	U32 i = 0;
	pFaceSet->GetSizeX(uSize);
	IFXAuthorFace facePositionFace, faceTexCoordFace;
	for(i = 0; i < uSize; i++) 
	{
		// Get the position face and the tex coord face
		U32 uFaceIndex = 0;
		pFaceSet->GetMemberX(i,uFaceIndex);
		U32 uMaterialID = 0;
		IFXCHECKX(m_pAuthorCLODMesh->GetFaceMaterial(uFaceIndex, &uMaterialID));

		IFXAuthorMaterial material;
		IFXCHECKX(m_pAuthorCLODMesh->GetMaterial(uMaterialID,&material));

		if(material.m_uNumTextureLayers > uLayer) 
		{

			IFXCHECKX(m_pAuthorCLODMesh->GetPositionFace(uFaceIndex, &facePositionFace));
			IFXCHECKX(m_pAuthorCLODMesh->GetTexFace(uLayer,uFaceIndex, &faceTexCoordFace));
			// Find which corner of the face is using the position
			// and add the diffuse color at that corner to the output set
			if(uPositionIndex == facePositionFace.VertexA()) 
			{
				rpTexCoordSet->AddX(faceTexCoordFace.VertexA());
			} 
			else if(uPositionIndex == facePositionFace.VertexB()) 
			{
				rpTexCoordSet->AddX(faceTexCoordFace.VertexB());
			} 
			else if(uPositionIndex == facePositionFace.VertexC()) 
			{
				rpTexCoordSet->AddX(faceTexCoordFace.VertexC());
			} 
			else 
			{
				IFXASSERT(0); 
			}
		}
	} 
}


//-----------------------------------------------------------------------------
// Factory function
//-----------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXSetAdjacencyX_Factory(IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXSetX component.
		CIFXSetAdjacencyX *pComponent = new CIFXSetAdjacencyX;

		if ( pComponent ) 
		{
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} 
		else 
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}
	else 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

//-----------------------------------------------------------------------------
//	Private methods
//-----------------------------------------------------------------------------
CIFXSetAdjacencyX::CIFXSetAdjacencyX() :
	IFXDEFINEMEMBER(m_pAuthorCLODMesh)
{
	m_uRefCount = 0;
	m_ppPositionSetArray = NULL;
	m_uPositionSetArraySize = 0;
}

CIFXSetAdjacencyX::~CIFXSetAdjacencyX()
{
	if(NULL != m_ppPositionSetArray)
	{
		U32 i;
		for ( i = 0 ; i < m_uPositionSetArraySize; i++) 
		{
			IFXRELEASE(m_ppPositionSetArray[i]);
		}
		IFXDELETE_ARRAY(m_ppPositionSetArray);
	}
}
