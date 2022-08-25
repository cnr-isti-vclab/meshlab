//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file	CIFXAuthorMesh.cpp 
			
			Implementation of the CIFXAuthorMesh class.
*/

#include "CIFXAuthorMesh.h"
#include "IFXCoreCIDs.h"
#include <float.h>

const U32 NORMQUANT = 1024;

/** 
	@todo	When doing copies, should this include CLOD data or not?
			Should resizes/reallocates nuke CLOD data?
*/

template<class T>
void ReallocDataBlock(IFXAutoPtr<T>& in_Data, U32 in_CurSize, U32 in_NewSize )
{
	T* pNewData = NULL;
	T* pOldData = in_Data;

	if(in_NewSize)
	{
		pNewData = new T[in_NewSize];
	}

	if(pNewData && pOldData)
	{
		int blocksToCopy = in_CurSize<in_NewSize?in_CurSize:in_NewSize;
		memcpy(pNewData, pOldData, sizeof(T) * blocksToCopy);
	}

	in_Data = pNewData; // causes deletion of old data and AutoPtr takes ownership of new data
}

//---------------------------------------------------------------------------
//  CIFXAuthorMesh_Factory
//
//  This is the CIFXClassName component factory function.  The
//  CIFXClassName component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorMesh_Factory( IFXREFIID interfaceId,
								 void**   ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXAuthorMesh  *pComponent = new CIFXAuthorMesh;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// Construction/Destruction

CIFXAuthorMesh::CIFXAuthorMesh()
{
	m_MeshFlags = IFXMESH_UNLOCKED;
	m_refCount = 0;
	m_normalAlloc = 0;

	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; ++i)
		m_pTexCoordFaces[i] = NULL;
}

CIFXAuthorMesh::~CIFXAuthorMesh()
{
}


//---------------------------------------------------------------------------
//  CIFXAuthorMesh::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXAuthorMesh::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMesh::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXAuthorMesh::Release()
{
	if ( !( --m_refCount ) )
	{

		delete this;
		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMesh::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT CIFXAuthorMesh::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXAuthorMesh ||
			interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXAuthorMesh* ) this;
		}
		else if ( interfaceId == IID_IFXAuthorCLODMesh )
		{
			*ppInterface = ( IFXAuthorCLODMesh* ) this;
		}
		else if ( interfaceId == IID_IFXAuthorCLODAccess )
		{
			*ppInterface = ( IFXAuthorCLODAccess* ) this;
		}
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//---------------------------------------------------------
// IFXAuthorMesh Iterface Implementation
//---------------------------------------------------------

IFXRESULT CIFXAuthorMesh::Allocate(const IFXAuthorMeshDesc* pInMeshDesc)
{
	IFXRESULT ir = IFX_OK;
	if(!pInMeshDesc)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	// Should potentially Validate the Mesh description
	if(IFXSUCCESS(ir))
	{
		if(pInMeshDesc->NumMaterials < 1 || pInMeshDesc->NumPositions < 1)
		{
			ir = IFX_E_INVALID_RANGE;
		}
	}

	// clean up any preexisting memory allocation
	if(IFXSUCCESS(ir))
	{
		Deallocate();
	}

	if(IFXSUCCESS(ir))
	{
		// Set the cur mesh desc to have no attribs.
		m_CurMeshDesc = *pInMeshDesc;

		m_MaxMeshDesc = *pInMeshDesc;
		m_FinalMaxResolution = m_MaxMeshDesc.NumPositions;
		m_MaxResolution = 0;
		m_MinResolution = 0;
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumPositions)
	{
		m_pPositionFaces= new IFXAuthorFace[m_MaxMeshDesc.NumFaces];
		if(!m_pPositionFaces)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumNormals)
	{
		m_pNormalFaces= new IFXAuthorFace[m_MaxMeshDesc.NumFaces];
		if(!m_pNormalFaces)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumDiffuseColors)
	{
		m_pDiffuseFaces= new IFXAuthorFace[m_MaxMeshDesc.NumFaces];
		if(!m_pDiffuseFaces)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumSpecularColors)
	{
		m_pSpecularFaces= new IFXAuthorFace[m_MaxMeshDesc.NumFaces];
		if(!m_pSpecularFaces)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) )
	{
		m_pFaceMaterials = new U32[m_MaxMeshDesc.NumFaces];
		if(!m_pFaceMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	// TexFaces are allocated on demand
	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		if (m_pTexCoordFaces[i])
		{
			m_pTexCoordFaces[i] = NULL;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumBaseVertices)
	{
		m_pBaseVertices = new U32[m_MaxMeshDesc.NumBaseVertices];
		if(!m_pBaseVertices)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumPositions)
	{
		m_pPositions = new IFXVector3[m_MaxMeshDesc.NumPositions];
		if(!m_pPositions)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
		m_pUpdates = new IFXAuthorVertexUpdate[m_MaxMeshDesc.NumPositions];
		if(!m_pUpdates)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumNormals)
	{
		m_pNormals= new IFXVector3[m_MaxMeshDesc.NumNormals];
		m_normalAlloc = m_MaxMeshDesc.NumNormals;
		if(!m_pNormals)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumDiffuseColors)
	{
		m_pDiffuseColors= new IFXVector4[m_MaxMeshDesc.NumDiffuseColors];
		if(!m_pDiffuseColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumSpecularColors)
	{
		m_pSpecularColors = new IFXVector4[m_MaxMeshDesc.NumSpecularColors];
		if(!m_pSpecularColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)  && m_MaxMeshDesc.NumTexCoords)
	{
		m_pTexCoords = new IFXVector4[m_MaxMeshDesc.NumTexCoords];
		if(!m_pTexCoords)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumMaterials)
	{
		m_pMaterials = new IFXAuthorMaterial[m_MaxMeshDesc.NumMaterials];

		if(!m_pMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir))
	{
		// Set the cur mesh desc to have no attribs.
		m_CurMeshDesc.NumFaces = 0;
		m_CurMeshDesc.NumPositions = 0;
		m_CurMeshDesc.NumNormals = 0;
		m_CurMeshDesc.NumDiffuseColors = 0;
		m_CurMeshDesc.NumSpecularColors = 0;
		m_CurMeshDesc.NumTexCoords = 0;
		m_CurMeshDesc.NumBaseVertices = 0;
	}

	if(IFXFAILURE(ir))
	{
		Deallocate();
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::Reallocate(const IFXAuthorMeshDesc* pInMeshDesc)
{
	IFXRESULT ir = IFX_OK;
	if(!pInMeshDesc)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	// Should ideally Validate the Mesh description
	if(IFXSUCCESS(ir))
	{
		if( 0 == pInMeshDesc->NumFaces || 0 == pInMeshDesc->NumPositions )
			ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumFaces != pInMeshDesc->NumFaces)
	{
		ReallocDataBlock(
			m_pPositionFaces, m_MaxMeshDesc.NumFaces, pInMeshDesc->NumFaces );
		if(!m_pPositionFaces)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		if( 0 != m_MaxMeshDesc.NumNormals )
		{
		ReallocDataBlock(
			m_pNormalFaces, m_MaxMeshDesc.NumFaces, pInMeshDesc->NumFaces);
		if(!m_pNormalFaces)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
		}

		if( 0 != m_MaxMeshDesc.NumDiffuseColors ) 
		{
			ReallocDataBlock(
				m_pDiffuseFaces, m_MaxMeshDesc.NumFaces, pInMeshDesc->NumFaces);
			if(!m_pDiffuseFaces)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
		}

		if( 0 != m_MaxMeshDesc.NumSpecularColors )
		{
			ReallocDataBlock(
				m_pSpecularFaces, m_MaxMeshDesc.NumFaces, pInMeshDesc->NumFaces);
			if(!m_pSpecularFaces)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
		}

		if( 0 != m_MaxMeshDesc.NumMaterials ) 
		{
			ReallocDataBlock(
				m_pFaceMaterials, m_MaxMeshDesc.NumFaces, pInMeshDesc->NumFaces);
			if(!m_pFaceMaterials)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
		}

		if(m_MaxMeshDesc.NumTexCoords)
		{
			U32 j;

			GetNumAllocatedTexFaceLayers(&j);

			U32 i;
			for( i = 0; i < j; ++i)
			{
				if(m_pTexCoordFaces[i].IsValid())
				{
					ReallocDataBlock(
						m_pTexCoordFaces[i], 
						m_MaxMeshDesc.NumFaces, 
						pInMeshDesc->NumFaces);
					if(!m_pTexCoordFaces[i])
					{
						ir = IFX_E_OUT_OF_MEMORY;
					}
				}
			}
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumBaseVertices!= pInMeshDesc->NumBaseVertices)
	{
		ReallocDataBlock(
			m_pBaseVertices, 
			m_MaxMeshDesc.NumBaseVertices, 
			pInMeshDesc->NumBaseVertices);
		if(!m_pBaseVertices)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumPositions != pInMeshDesc->NumPositions)
	{
		ReallocDataBlock(
			m_pPositions, m_MaxMeshDesc.NumPositions, pInMeshDesc->NumPositions);
		if(!m_pPositions)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}

		if(m_pUpdates)
		{
			IFXAuthorVertexUpdate* pNewUpdates = 
				new IFXAuthorVertexUpdate[pInMeshDesc->NumPositions];
			if(!pNewUpdates)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
			else
			{
				U32 NumCpy = pInMeshDesc->NumPositions < m_MaxMeshDesc.NumPositions ? 
					pInMeshDesc->NumPositions : 
					m_MaxMeshDesc.NumPositions;
				U32 i;
				for( i = 0; i <NumCpy; ++i )
				{
					m_pUpdates[i].CopyTo(pNewUpdates);
				}
			}
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumNormals != pInMeshDesc->NumNormals)
	{
		ReallocDataBlock(m_pNormals, m_MaxMeshDesc.NumNormals, pInMeshDesc->NumNormals);
		m_normalAlloc = pInMeshDesc->NumNormals;
		if(!m_pNormals)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && 
		m_MaxMeshDesc.NumDiffuseColors != pInMeshDesc->NumDiffuseColors)
	{
		ReallocDataBlock(
			m_pDiffuseColors, 
			m_MaxMeshDesc.NumDiffuseColors, 
			pInMeshDesc->NumDiffuseColors);
		if(!m_pDiffuseColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && 
		m_MaxMeshDesc.NumSpecularColors != pInMeshDesc->NumSpecularColors)
	{
		ReallocDataBlock(
			m_pSpecularColors, 
			m_MaxMeshDesc.NumSpecularColors, 
			pInMeshDesc->NumSpecularColors);
		if(!m_pSpecularColors)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir)  && m_MaxMeshDesc.NumTexCoords != pInMeshDesc->NumTexCoords)
	{
		ReallocDataBlock(
			m_pTexCoords, m_MaxMeshDesc.NumTexCoords, pInMeshDesc->NumTexCoords);
		if(!m_pTexCoords)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumMaterials != pInMeshDesc->NumMaterials)
	{
		ReallocDataBlock(
			m_pMaterials, m_MaxMeshDesc.NumMaterials, pInMeshDesc->NumMaterials);
		if(!m_pMaterials)
		{
			ir = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(ir) )
	{
		m_MaxMeshDesc = *pInMeshDesc;
	}
	if(IFXFAILURE(ir))
	{
		Deallocate();
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::Deallocate()
{
	IFXRESULT ir = IFX_OK;
	U32 i = 0;

	m_MeshFlags     = IFXMESH_UNLOCKED; // The Mesh state
	m_pPositionFaces  = NULL;
	m_pNormalFaces    = NULL;
	m_pDiffuseFaces   = NULL;
	m_pSpecularFaces  = NULL;

	for(i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		if(m_pTexCoordFaces[i])
			m_pTexCoordFaces[i] = NULL;
	}

	m_pFaceMaterials  = NULL;

	m_pBaseVertices = NULL;

	m_pPositions    = NULL;
	m_pNormals      = NULL;
	m_pDiffuseColors  = NULL;
	m_pSpecularColors = NULL;
	m_pTexCoords    = NULL;
	m_pMaterials    = NULL;
	m_pUpdates      = NULL;

	IFXInitStruct(&m_CurMeshDesc);
	IFXInitStruct(&m_MaxMeshDesc);
	return ir;
}

IFXRESULT CIFXAuthorMesh::Copy(IFXREFIID interfaceID, void** ppOutNewAuthorMesh)
{
	IFXRESULT ir = IFX_OK;
	IFXAuthorCLODMesh* pNewAuthorMesh = NULL;

	if(!ppOutNewAuthorMesh)
		ir = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(ir))
		ir = IFXCreateComponent(
					CID_IFXAuthorMesh, IID_IFXAuthorCLODMesh, 
					(void**)&pNewAuthorMesh);

	if(IFXSUCCESS(ir))
		ir = pNewAuthorMesh->Allocate(&m_MaxMeshDesc);

	if(IFXSUCCESS(ir))
		ir = pNewAuthorMesh->SetMeshDesc(&m_CurMeshDesc);

	if(IFXSUCCESS(ir))
	{
#ifdef _DEBUG
		U32 uTemp = 
#endif
		pNewAuthorMesh->SetMaxResolution(m_MaxResolution);
		IFXASSERT(uTemp == m_MaxResolution);
	}

	if(IFXSUCCESS(ir))
	{
#ifdef _DEBUG
		U32 uTemp = 
#endif
		pNewAuthorMesh->SetMinResolution(m_MinResolution);
		IFXASSERT(uTemp == m_MinResolution);
	}

	{
		CIFXAuthorMeshLocker MeshLock(pNewAuthorMesh);
		IFXAuthorMaterial* pTmpMat = NULL;
		IFXAuthorFace* pTmpFace = NULL;
		U32* pTmpU32 = NULL;
		IFXVector3* pTmpV3 = NULL;
		IFXVector4* pTmpV4 = NULL;
		IFXAuthorVertexUpdate* pTmpUpdates = NULL;

		if(IFXSUCCESS(ir) && m_pMaterials)
		{
			ir = pNewAuthorMesh->GetMaterials(&pTmpMat);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpMat, m_pMaterials, 
					sizeof(IFXAuthorMaterial) * m_MaxMeshDesc.NumMaterials);
			}
		}

		if(IFXSUCCESS(ir) && m_pPositionFaces)
		{
			ir = pNewAuthorMesh->GetPositionFaces(&pTmpFace);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpFace, m_pPositionFaces, 
					sizeof(IFXAuthorFace) * m_MaxMeshDesc.NumFaces);
			}
		}

		if(IFXSUCCESS(ir) && m_pNormalFaces)
		{
			ir = pNewAuthorMesh->GetNormalFaces(&pTmpFace);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpFace, m_pNormalFaces, 
					sizeof(IFXAuthorFace) * m_MaxMeshDesc.NumFaces);
			}
		}

		if(IFXSUCCESS(ir) && m_pDiffuseFaces)
		{
			ir = pNewAuthorMesh->GetDiffuseFaces(&pTmpFace);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpFace, m_pDiffuseFaces, 
					sizeof(IFXAuthorFace) * m_MaxMeshDesc.NumFaces);
			}
		}

		if(IFXSUCCESS(ir) && m_pSpecularFaces)
		{
			ir = pNewAuthorMesh->GetSpecularFaces(&pTmpFace);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpFace, m_pSpecularFaces, 
					sizeof(IFXAuthorFace) * m_MaxMeshDesc.NumFaces);
			}
		}

		U32 j;

		GetNumAllocatedTexFaceLayers(&j);

		U32 i;
		for( i = 0; i < j; ++i)
		{
			if(IFXSUCCESS(ir) && m_pTexCoordFaces[i].IsValid())
			{
				ir = pNewAuthorMesh->GetTexFaces(i, &pTmpFace);
				if(IFXSUCCESS(ir))
				{
					memcpy(
						pTmpFace, m_pTexCoordFaces[i], 
						sizeof(IFXAuthorFace) * m_MaxMeshDesc.NumFaces);
				}
			}
		}

		if(IFXSUCCESS(ir) && m_pFaceMaterials)
		{
			ir = pNewAuthorMesh->GetFaceMaterials(&pTmpU32);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpU32, m_pFaceMaterials, 
					sizeof(U32) * m_MaxMeshDesc.NumFaces);
			}
		}

		if(IFXSUCCESS(ir) && m_pBaseVertices)
		{
			ir = pNewAuthorMesh->GetBaseVertices(&pTmpU32);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpU32, m_pBaseVertices, 
					sizeof(U32) * m_MaxMeshDesc.NumBaseVertices);
			}
		}

		if(IFXSUCCESS(ir) && m_pPositions)
		{
			ir = pNewAuthorMesh->GetPositions(&pTmpV3);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpV3, m_pPositions, 
					sizeof(IFXVector3) * m_MaxMeshDesc.NumPositions);
			}
		}
		if(IFXSUCCESS(ir) && m_pNormals)
		{
			ir = pNewAuthorMesh->GetNormals(&pTmpV3);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpV3, m_pNormals, 
					sizeof(IFXVector3) * m_MaxMeshDesc.NumNormals);
			}
		}

		if(IFXSUCCESS(ir) && m_pDiffuseColors)
		{
			ir = pNewAuthorMesh->GetDiffuseColors(&pTmpV4);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpV4, m_pDiffuseColors, 
					sizeof(IFXVector4) * m_MaxMeshDesc.NumDiffuseColors);
			}
		}
		if(IFXSUCCESS(ir) && m_pSpecularColors)
		{
			ir = pNewAuthorMesh->GetSpecularColors(&pTmpV4);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpV4, m_pSpecularColors, 
					sizeof(IFXVector4) * m_MaxMeshDesc.NumSpecularColors);
			}
		}
		if(IFXSUCCESS(ir) && m_pTexCoords)
		{
			ir = pNewAuthorMesh->GetTexCoords(&pTmpV4);
			if(IFXSUCCESS(ir))
			{
				memcpy(
					pTmpV4, m_pTexCoords, 
					sizeof(IFXVector4) * m_MaxMeshDesc.NumTexCoords);
			}
		}

		if(IFXSUCCESS(ir) && m_pUpdates)
		{
			ir = pNewAuthorMesh->GetUpdates(&pTmpUpdates);
			if(IFXSUCCESS(ir))
			{
				U32 i;
				for( i = 0; i < m_MaxMeshDesc.NumPositions; ++i)
				{
					pTmpUpdates[i] = m_pUpdates[i];
				}
			}
		}
	} // end of mesh locker scope

	if(IFXSUCCESS(ir))
	{
		if(pNewAuthorMesh) {
			ir = pNewAuthorMesh->QueryInterface(interfaceID,ppOutNewAuthorMesh);
		} else {
			*ppOutNewAuthorMesh = NULL;
		}
	}

	IFXRELEASE(pNewAuthorMesh);

	return ir;
}


const IFXAuthorMeshDesc* CIFXAuthorMesh::GetMeshDesc()
{
	return &m_CurMeshDesc;
}

IFXRESULT CIFXAuthorMesh::SetMeshDesc(const IFXAuthorMeshDesc* pInMeshDesc)
{
	if(pInMeshDesc->NumFaces > m_MaxMeshDesc.NumFaces
		|| pInMeshDesc->NumBaseVertices > m_MaxMeshDesc.NumBaseVertices
		|| pInMeshDesc->NumPositions > m_MaxMeshDesc.NumPositions
		|| pInMeshDesc->NumNormals > m_MaxMeshDesc.NumNormals
		|| pInMeshDesc->NumDiffuseColors > m_MaxMeshDesc.NumDiffuseColors
		|| pInMeshDesc->NumSpecularColors > m_MaxMeshDesc.NumSpecularColors
		|| pInMeshDesc->NumTexCoords > m_MaxMeshDesc.NumTexCoords
		|| pInMeshDesc->NumMaterials != m_MaxMeshDesc.NumMaterials )
	{
		return IFX_E_INVALID_RANGE;
	}

	m_CurMeshDesc = *pInMeshDesc;
	return IFX_OK;
}

const IFXAuthorMeshDesc* CIFXAuthorMesh::GetMaxMeshDesc()
{
	return &m_MaxMeshDesc;
}

IFXRESULT CIFXAuthorMesh::Lock()
{
	IFXRESULT ir = IFX_OK;

	if(m_MeshFlags & IFXMESH_LOCKED)
	{
		ir = IFX_E_AUTHOR_MESH_LOCKED;
	}

	if(IFXSUCCESS(ir))
	{
		m_MeshFlags |= IFXMESH_LOCKED;
		m_MeshFlags ^= IFXMESH_UNLOCKED;
	}
	return ir;
}

IFXRESULT CIFXAuthorMesh::Unlock()
{
	IFXRESULT ir = IFX_OK;

	if(m_MeshFlags & IFXMESH_UNLOCKED)
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(IFXSUCCESS(ir))
	{
		m_MeshFlags |= IFXMESH_UNLOCKED;
		m_MeshFlags ^= IFXMESH_LOCKED;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetNumAllocatedTexFaceLayers(U32* pOutLayers)
{
	IFXRESULT result = IFX_OK;
	U32 valid_Layers = 0;

	if(!pOutLayers)
		result = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(result))
	{
		U32 i;
		for( i = 0; i < IFX_MAX_TEXUNITS; ++i )
		{
			if( m_pTexCoordFaces[i].IsValid() )
				valid_Layers++;
		}

		*pOutLayers = valid_Layers;
	}

	return result;
}

IFXRESULT CIFXAuthorMesh::GetMaterials(IFXAuthorMaterial** ppOutMaterials)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutMaterials)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutMaterials = m_pMaterials;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetPositionFaces(IFXAuthorFace** ppOutPositionFaces)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutPositionFaces)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutPositionFaces = m_pPositionFaces;
	}
	return ir;
}

IFXRESULT CIFXAuthorMesh::GetNormalFaces(IFXAuthorFace** ppOutFaces)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutFaces)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumNormals == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutFaces = m_pNormalFaces;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetDiffuseFaces(IFXAuthorFace** ppOutFaces)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutFaces)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumDiffuseColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutFaces = m_pDiffuseFaces;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetSpecularFaces(IFXAuthorFace** ppOutFaces)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutFaces)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumSpecularColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutFaces = m_pSpecularFaces;
	}
	return ir;
}

IFXRESULT CIFXAuthorMesh::GetTexFaces(U32 index, IFXAuthorFace** ppOutTexFaces)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;

	//If there's a posibility that the user passes in multiple bad arg.
	//Then we need to look into additional checks, which can slow down
	//this at runtime.
	if(!ppOutTexFaces)
		ir = IFX_E_INVALID_POINTER;

	if(index >= IFX_MAX_TEXUNITS)
		ir = IFX_E_INVALID_RANGE;
	else
	{
		if(m_MaxMeshDesc.NumTexCoords == 0)
			ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir) && m_pTexCoordFaces[index].IsNull())
	{
		m_pTexCoordFaces[index] = new IFXAuthorFace[m_MaxMeshDesc.NumFaces];

		if(!m_pTexCoordFaces[index])
			ir = IFX_E_OUT_OF_MEMORY;
		else
			memset( 
				m_pTexCoordFaces[index], 0, 
				m_MaxMeshDesc.NumFaces * sizeof(IFXAuthorFace));
	}

	if(IFXSUCCESS(ir))
		*ppOutTexFaces = m_pTexCoordFaces[index];

	return ir;
}


IFXRESULT CIFXAuthorMesh::GetFaceMaterials(U32** ppOutFaceMaterials)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutFaceMaterials)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	//If Faces are not exist, then we expected that Materials
	//should not be there either.  We may not need this check.
	if(m_MaxMeshDesc.NumFaces == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutFaceMaterials = m_pFaceMaterials;
	}
	return ir;
}


IFXRESULT CIFXAuthorMesh::GetBaseVertices(U32** ppOutBaseVertices)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutBaseVertices)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	//If Faces are not exist, then we expected that Materials
	//should not be there either.  We may not need this check.
	if(m_MaxMeshDesc.NumBaseVertices == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutBaseVertices = m_pBaseVertices;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetPositions(IFXVector3** ppOutPositions)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutPositions)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumPositions == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutPositions = m_pPositions;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetNormals(IFXVector3** ppOutNormals)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutNormals)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumNormals == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutNormals = m_pNormals;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetDiffuseColors(IFXVector4** ppOutDiffuseColors)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutDiffuseColors)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumDiffuseColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutDiffuseColors = m_pDiffuseColors;
	}
	return ir;
}


IFXRESULT CIFXAuthorMesh::GetSpecularColors(IFXVector4** ppOutSpecularColor)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutSpecularColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumSpecularColors == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutSpecularColor = m_pSpecularColors;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetTexCoords(IFXVector4** ppOutTexCoords)
{
	IFXRESULT ir = IFX_OK;

	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		ir = IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutTexCoords)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(m_MaxMeshDesc.NumTexCoords == 0)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(ir))
	{
		*ppOutTexCoords = m_pTexCoords;
	}

	return ir;
}

//---------------------------------

IFXRESULT CIFXAuthorMesh::GetMaterial(U32  index, IFXAuthorMaterial* pOutMaterial)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumMaterials)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutMaterial = m_pMaterials[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::SetMaterial(U32  index, const IFXAuthorMaterial* pInMaterial)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxMeshDesc.NumMaterials)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (!pInMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		m_pMaterials[index] = *pInMaterial;
	}

	return ir;
}
//---------------------------------


IFXRESULT CIFXAuthorMesh::GetPositionFace(U32  index, IFXAuthorFace* pOutPositionFace)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutPositionFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutPositionFace = m_pPositionFaces[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetNormalFace(U32 index, IFXAuthorFace* pOutFace)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutFace = m_pNormalFaces[index];
		}
	}
	return ir;
}

IFXRESULT CIFXAuthorMesh::GetDiffuseFace(U32 index, IFXAuthorFace* pOutFace)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutFace= m_pDiffuseFaces[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetSpecularFace(U32 index, IFXAuthorFace* pOutFace)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutFace= m_pSpecularFaces[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetTexFace(U32 in_Layer, U32 index, IFXAuthorFace* pOutFace)
{
	IFXRESULT ir = IFX_OK;

	if (in_Layer > IFX_MAX_TEXUNITS)
		ir = IFX_E_INVALID_RANGE;

	if(!pOutFace)
		ir = IFX_E_INVALID_POINTER;

	if (index >= m_MaxMeshDesc.NumFaces || !m_MaxMeshDesc.NumTexCoords )
		ir = IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(ir) && m_pTexCoordFaces[in_Layer].IsNull())
	{
		m_pTexCoordFaces[in_Layer] = new IFXAuthorFace[m_MaxMeshDesc.NumFaces];

		if(!m_pTexCoordFaces[in_Layer])
			ir = IFX_E_OUT_OF_MEMORY;
	}

	if (IFXSUCCESS(ir))
		*pOutFace = m_pTexCoordFaces[in_Layer][index];

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetFaceMaterial(U32 index, U32* pOutFaceMaterial)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutFaceMaterial)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutFaceMaterial = m_pFaceMaterials[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetBaseVertex(U32 index, U32* pOutBaseVertex)
{
	IFXRESULT ir = IFX_OK;

	if(!pOutBaseVertex)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumBaseVertices)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutBaseVertex = m_pBaseVertices[index];
		}
	}
	return ir;
}



IFXRESULT CIFXAuthorMesh::SetPositionFace(U32 index, const IFXAuthorFace* pInPositionFace)
{
	IFXRESULT ir = IFX_OK;

	if (!pInPositionFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pPositionFaces[index] = *pInPositionFace;
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::SetNormalFace(U32 index, const IFXAuthorFace* pInFace)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxMeshDesc.NumFaces)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (!pInFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		m_pNormalFaces[index] = *pInFace;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::SetDiffuseFace(U32 index, const IFXAuthorFace* pInFace)
{
	IFXRESULT ir = IFX_OK;

	if (!pInFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pDiffuseFaces[index] = *pInFace;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetSpecularFace(U32 index, const IFXAuthorFace* pInFace)
{
	IFXRESULT ir = IFX_OK;

	if (!pInFace)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumFaces)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pSpecularFaces[index] = *pInFace;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetTexFace(U32 in_Layer, U32 index, const IFXAuthorFace* pInFace)
{
	IFXRESULT ir = IFX_OK;

	if (in_Layer > IFX_MAX_TEXUNITS || !m_MaxMeshDesc.NumTexCoords)
		ir = IFX_E_INVALID_RANGE;

	if (!pInFace)
		ir = IFX_E_INVALID_POINTER;

	if (index >= m_MaxMeshDesc.NumFaces)
		ir = IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(ir) && m_pTexCoordFaces[in_Layer].IsNull())
	{
		m_pTexCoordFaces[in_Layer] = new IFXAuthorFace[m_MaxMeshDesc.NumFaces];

		if(!m_pTexCoordFaces[in_Layer])
			ir = IFX_E_OUT_OF_MEMORY;
	}

	if (IFXSUCCESS(ir))
		m_pTexCoordFaces[in_Layer][index] = *pInFace;

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetFaceMaterial(U32 index, U32 faceMaterialID)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxMeshDesc.NumFaces)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (IFXSUCCESS(ir))
	{
		m_pFaceMaterials[index] = faceMaterialID;
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetBaseVertex(U32 index, U32 baseVertex)
{
	IFXRESULT ir = IFX_OK;

	if (index >= m_MaxMeshDesc.NumBaseVertices)
	{
		ir = IFX_E_INVALID_RANGE;
	}

	if (IFXSUCCESS(ir))
	{
		m_pBaseVertices[index] = baseVertex;
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetPosition(U32 index,IFXVector3* pOutVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pOutVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumPositions)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutVector3 = m_pPositions[index];
		}
	}

	return ir;
}

IFXRESULT CIFXAuthorMesh::GetNormal(U32 index, IFXVector3* pOutVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pOutVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumNormals)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutVector3 = m_pNormals[index];
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::GetDiffuseColor(U32 index,IFXVector4* pOutColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pOutColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumDiffuseColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutColor = m_pDiffuseColors[index];
		}
	}
	return ir;
}


IFXRESULT CIFXAuthorMesh::GetSpecularColor(U32 index, IFXVector4* pOutColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pOutColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumSpecularColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			*pOutColor = m_pSpecularColors[index];
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::GetTexCoord(U32 index,
									  IFXVector4* pOutVector4)
{
	IFXRESULT ir = IFX_OK;

	if (!pOutVector4)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir) && m_MaxMeshDesc.NumTexCoords)
	{
		*pOutVector4 = (m_pTexCoords)[index];
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetPosition(U32 index, const IFXVector3* pInVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pInVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{

		if (index >= m_MaxMeshDesc.NumPositions /*|| index < 0*/)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pPositions[index] = *pInVector3;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetNormal(U32 index, const IFXVector3* pInVector3)
{
	IFXRESULT ir = IFX_OK;

	if (!pInVector3)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_normalAlloc )
		{
			U32 oldSize = m_normalAlloc;
			m_normalAlloc = (index + 1) + NORMQUANT - ((index + 1) % NORMQUANT);
			ReallocDataBlock( m_pNormals, oldSize, m_normalAlloc );
			if(!m_pNormals)
			{
				ir = IFX_E_OUT_OF_MEMORY;
			}
		}
		if (IFXSUCCESS(ir))
		{
			if (index >= m_MaxMeshDesc.NumNormals )
			{
				m_MaxMeshDesc.NumNormals = index + 1;
			}
			m_pNormals[index] = *pInVector3;
		}
	}
	return ir;
}


IFXRESULT CIFXAuthorMesh::SetDiffuseColor(U32 index,IFXVector4* pInColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pInColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumDiffuseColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pDiffuseColors[index] = *pInColor;
		}
	}
	return ir;
}


IFXRESULT CIFXAuthorMesh::SetSpecularColor(U32 index, IFXVector4* pInColor)
{
	IFXRESULT ir = IFX_OK;

	if (!pInColor)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumSpecularColors)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pSpecularColors[index] = *pInColor;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetTexCoord(U32 index,
									  const IFXVector4* pInVector4)
{
	IFXRESULT ir = IFX_OK;

	if (!pInVector4)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(ir))
	{
		if(m_MaxMeshDesc.NumTexCoords)
		{
			(m_pTexCoords)[index] = *pInVector4;
		}
	}

	return ir;
}


IFXRESULT CIFXAuthorMesh::SetUpdates(IFXAuthorVertexUpdate* updates)
{
	m_pUpdates = updates;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::GetUpdates(IFXAuthorVertexUpdate** ppOutUpdates)
{
	if(!(m_MeshFlags & IFXMESH_LOCKED))
	{
		return IFX_E_AUTHOR_MESH_NOT_LOCKED;
	}

	if(!ppOutUpdates)
	{
		return IFX_E_INVALID_POINTER;
	}

	*ppOutUpdates = m_pUpdates;

	return IFX_OK;
}

// Retrieves a specific vertex update for this mesh.
IFXRESULT CIFXAuthorMesh::GetVertexUpdate(U32 index, const IFXAuthorVertexUpdate*& pOutVertexUpdate)
{
	IFXRESULT ir = IFX_OK;

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumPositions /*|| index < 0*/)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			pOutVertexUpdate = &(m_pUpdates[index]);
		}
	}

	return ir;
}

// Sets a specific vertex update for this mesh.
IFXRESULT CIFXAuthorMesh::SetVertexUpdate(
								U32 index, 
								const IFXAuthorVertexUpdate* pInVertexUpdate)
{
	IFXRESULT ir = IFX_OK;

	if (!pInVertexUpdate)
	{
		ir = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(ir))
	{
		if (index >= m_MaxMeshDesc.NumPositions)
		{
			ir = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_pUpdates[index] = *pInVertexUpdate;
		}
	}

	return ir;
}

U32 CIFXAuthorMesh::GetMaxResolution()
{
	return m_MaxResolution;
}

U32 CIFXAuthorMesh::SetMaxResolution(U32 r)
{
	return m_MaxResolution = r;
}

U32 CIFXAuthorMesh::SetFinalMaxResolution(U32 r)
{
	return m_FinalMaxResolution = r;
}

U32 CIFXAuthorMesh::GetFinalMaxResolution()
{
	return m_FinalMaxResolution;
}

U32 CIFXAuthorMesh::SetMinResolution(U32 r)
{
	// The static mesh compression cannot deal with minimum resolutions lower than 3.
	// A minimum resolution less than three is nonsensical anyway, because the smallest
	// visible mesh -- a single face -- has at least three vertices. If someone tries
	// to go below three, just set it down to zero.
	if (r < 3)
		r = 0;

	if (r > m_MaxResolution) 
	{
		r = m_MaxResolution;
		IFXASSERT(m_MaxResolution > 0);
	}

	return m_MinResolution = r;
}

U32 CIFXAuthorMesh::GetMinResolution()
{
	return m_MinResolution;
}



U32 CIFXAuthorMesh::SetResolution(U32 r)
{
	if(r < m_MinResolution)
	{
		r = m_MinResolution;
	}

	if(r > m_MaxResolution)
	{
		r = m_MaxResolution;
	}

	// the mesh is locked
	if(m_MeshFlags & IFXMESH_LOCKED)
	{
		return m_CurMeshDesc.NumPositions;
	}

	// if there are no updates bail
	if(!m_pUpdates)
	{
		return m_CurMeshDesc.NumPositions;
	}

	U32 result = r;

	IFXAuthorVertexUpdate *pu;  // pointer to update
	U32 nfu; // num face updates

	// increase resolution if needed
	while (r > m_CurMeshDesc.NumPositions)
	{
		pu = &m_pUpdates[m_CurMeshDesc.NumPositions];

		m_CurMeshDesc.NumPositions++;
		m_CurMeshDesc.NumFaces   += pu->NumNewFaces;
		m_CurMeshDesc.NumNormals += pu->NumNewNormals;// author CLOD
													  // might not do normals
		m_CurMeshDesc.NumDiffuseColors  += pu->NumNewDiffuseColors;
		m_CurMeshDesc.NumSpecularColors += pu->NumNewSpecularColors;
		m_CurMeshDesc.NumTexCoords    += pu->NumNewTexCoords;

		nfu = pu->NumFaceUpdates;
		while(nfu--)
		{
			IFXAuthorFaceUpdate *pfu = &pu->pFaceUpdates[nfu];
			if (((U32)pfu->Attribute >= (U32)IFXAuthorFaceUpdate::Tex0) &&
				((U32)pfu->Attribute < ((U32)IFXAuthorFaceUpdate::Tex0+IFX_MAX_TEXUNITS)))
			{
				m_pTexCoordFaces[pfu->Attribute - IFXAuthorFaceUpdate::Tex0]
								[pfu->FaceIndex].corner[pfu->Corner] = pfu->IncrValue;
			}
			else
			{
				switch (pfu->Attribute)
				{
				case IFXAuthorFaceUpdate::Position:
					m_pPositionFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->IncrValue;
					break;
				case IFXAuthorFaceUpdate::Normal:
					m_pNormalFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->IncrValue;
					break;
				case IFXAuthorFaceUpdate::Diffuse:
					m_pDiffuseFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->IncrValue;
					break;
				case IFXAuthorFaceUpdate::Specular:
					m_pSpecularFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->IncrValue;
					break;
				default:
					IFXASSERT(0);
				}
			}
		} // while nfu
	} // while r


	// decrease resolution if needed
	while (r < m_CurMeshDesc.NumPositions)
	{
		m_CurMeshDesc.NumPositions--;
		pu = &m_pUpdates[m_CurMeshDesc.NumPositions];

		m_CurMeshDesc.NumFaces	 -= pu->NumNewFaces;
		m_CurMeshDesc.NumNormals -= pu->NumNewNormals;// author CLOD 
													  // might not do normals
		m_CurMeshDesc.NumDiffuseColors  -= pu->NumNewDiffuseColors;
		m_CurMeshDesc.NumSpecularColors -= pu->NumNewSpecularColors;
		m_CurMeshDesc.NumTexCoords    -= pu->NumNewTexCoords;

		nfu = pu->NumFaceUpdates;
		while(nfu--)
		{
			IFXAuthorFaceUpdate *pfu = &pu->pFaceUpdates[nfu];
			if (((U32)pfu->Attribute >= (U32)IFXAuthorFaceUpdate::Tex0) &&
				((U32)pfu->Attribute < ((U32)IFXAuthorFaceUpdate::Tex0+IFX_MAX_TEXUNITS)))
			{
				m_pTexCoordFaces[pfu->Attribute - IFXAuthorFaceUpdate::Tex0]
								[pfu->FaceIndex].corner[pfu->Corner] = pfu->DecrValue;
			}
			else
				switch (pfu->Attribute)
			{
				case IFXAuthorFaceUpdate::Position:
					m_pPositionFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->DecrValue;
					break;
				case IFXAuthorFaceUpdate::Normal:
					m_pNormalFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->DecrValue;
					break;
				case IFXAuthorFaceUpdate::Diffuse:
					m_pDiffuseFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->DecrValue;
					break;
				case IFXAuthorFaceUpdate::Specular:
					m_pSpecularFaces[pfu->FaceIndex].corner[pfu->Corner] = 
						pfu->DecrValue;
					break;
				default:
					IFXASSERT(0);
			}
		} // while nfu
	} // while r
	return result;
}

U32 CIFXAuthorMesh::GetResolution()
{
	return m_CurMeshDesc.NumPositions;
}


IFXRESULT CIFXAuthorMesh::GenerateNormals(IFXAuthorMeshNormalGen normalType)
{
	IFXRESULT result = IFX_OK;
	U32 m = 0;
	if((m_MeshFlags & IFXMESH_LOCKED))
	{
		result = IFX_E_AUTHOR_MESH_LOCKED;
	}

	if(IFXSUCCESS(result))
	{
		switch(normalType)
		{
		case IFXAuthorMeshNormalGen_None:
			m_pNormals = NULL;
			m_MaxMeshDesc.NumNormals = 0;
			m_normalAlloc = 0;
			m_pNormalFaces = NULL;
			break;
		case IFXAuthorMeshNormalGen_Flat:
			{
				m_pNormals = new IFXVector3[m_MaxMeshDesc.NumFaces];
				m_normalAlloc = m_MaxMeshDesc.NumFaces;
				m_MaxMeshDesc.NumNormals = m_MaxMeshDesc.NumFaces;
				m_CurMeshDesc.NumNormals = IFXMIN(
												m_CurMeshDesc.NumFaces, 
												m_MaxMeshDesc.NumFaces);
				if(!m_pNormalFaces)
				{
					m_pNormalFaces = new IFXAuthorFace[m_MaxMeshDesc.NumFaces];
				}
				result = GenFlatNormals(m_pNormals, m_pNormalFaces);
				if(!IFXSUCCESS(result))
				{
					m_pNormals = NULL;
					m_CurMeshDesc.NumNormals = 0;
					m_MaxMeshDesc.NumNormals = 0;
					m_normalAlloc = 0;
				}
			}
			break;
		case IFXAuthorMeshNormalGen_Smooth:
			{
				GenSmoothNormals();
			}
			break;
		case IFXAuthorMeshNormalGen_SmoothByMaterial:
			{
				//        GenFlatNormals();
				for(m = 0; m < m_MaxMeshDesc.NumMaterials; ++m)
				{
					//        GenSmoothNormals();
				}
			}
			break;
		default:
			result = IFX_E_BAD_PARAM;
		}
	}

	return result;
}

IFXRESULT CIFXAuthorMesh::GenFlatNormals(
								IFXVector3* pOutFlatNormals, 
								IFXAuthorFace* pOutNormalFaces)
{
	IFXRESULT result = IFX_OK;

	if(IFXSUCCESS(result))
	{
		U32 numfaces = m_MaxMeshDesc.NumFaces;
		U32 f;
		for (f=0;f<numfaces;f++)
		{
			IFXVector3 left,right,normal;
			IFXVector3 *corners[3];

			corners[0] = &m_pPositions[m_pPositionFaces[f].VertexA()];
			corners[1] = &m_pPositions[m_pPositionFaces[f].VertexB()];
			corners[2] = &m_pPositions[m_pPositionFaces[f].VertexC()];

			left.Subtract(*corners[1],*corners[0]);
			right.Subtract(*corners[2],*corners[0]);
			normal.CrossProduct(left,right);
			if ((normal.X() == 0.0f)
				&& (normal.Y() == 0.0f)
				&& (normal.Z() == 0.0f))
			{
				normal.Z() = 1;
			}
			else
			{
				normal.Normalize();
			}
			pOutFlatNormals[f] = normal;
			if(pOutNormalFaces)
			{
				pOutNormalFaces[f].Set(f, f, f);
			}
		}
	}

	return result;
}

IFXRESULT CIFXAuthorMesh::GenSmoothNormals()
{
	IFXRESULT result = IFX_OK;
	U32 numFaces = m_MaxMeshDesc.NumFaces;
	U32 numPositions = m_MaxMeshDesc.NumPositions;

	IFXVector3* pNormals = new IFXVector3[m_MaxMeshDesc.NumFaces];

	if(!pNormals)
	{
		result = IFX_E_OUT_OF_MEMORY;
	}

	if(!m_pNormalFaces)
	{
		m_pNormalFaces = new IFXAuthorFace[m_MaxMeshDesc.NumFaces];
	}


	if(IFXSUCCESS(result))
	{
		result = GenFlatNormals(pNormals, NULL);
	}

	if(IFXSUCCESS(result))
	{

		m_pNormals = new IFXVector3[numPositions];
		m_normalAlloc = numPositions;
		if(!m_pNormals)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
		m_MaxMeshDesc.NumNormals = numPositions;
		m_CurMeshDesc.NumNormals = IFXMIN(m_CurMeshDesc.NumNormals,
			m_MaxMeshDesc.NumNormals);
	}


	if(IFXSUCCESS(result))
	{
		// for each vertex
		memset(m_pNormals, 0, sizeof(IFXVector3) * numPositions);

		U32 f = 0;
		for (;f<numFaces;f++)
		{
			m_pNormalFaces[f] = m_pPositionFaces[f];

			// get any of the flat normals for this face
			IFXVector3 *pNormal =  pNormals + f;

			// Add the normal to each vertex of the face
			m_pNormals[m_pPositionFaces[f].corner[0]].Add(*pNormal);
			m_pNormals[m_pPositionFaces[f].corner[1]].Add(*pNormal);
			m_pNormals[m_pPositionFaces[f].corner[2]].Add(*pNormal);
		}  // end loop over faces

		// Now, loop over the vertex positions again.  Set the
		// corresponding normals to the new smooth values.
		U32 p = 0;
		for (;p<numPositions;p++)
		{
			F32 magnitude = m_pNormals[p].CalcMagnitude();
			if (magnitude > 0)
			{
				m_pNormals[p].Scale(1.0f/magnitude);
			}
		}
	}

	IFXDELETE_ARRAY(pNormals);
	if(!IFXSUCCESS(result))
	{
		m_pNormals = NULL;
		m_MaxMeshDesc.NumNormals = 0;
		m_CurMeshDesc.NumNormals = 0;
		m_normalAlloc = 0;
	}

	return result;
}


IFXRESULT CIFXAuthorMesh::GenSmoothMatNormals()
{
	IFXRESULT result = IFX_OK;
	IFXVector3* pNormals = new IFXVector3[m_MaxMeshDesc.NumFaces];
	if(!IFXSUCCESS(result))
	{
		result = IFX_E_OUT_OF_MEMORY;
	}

	if(IFXSUCCESS(result))
	{
		result = GenFlatNormals(pNormals, NULL);
	}


	if(IFXSUCCESS(result))
	{

	}

	IFXDELETE_ARRAY(pNormals);
	if(!IFXSUCCESS(result))
	{
		m_pNormals = NULL;
		m_MaxMeshDesc.NumNormals = 0;
		m_CurMeshDesc.NumNormals = 0;
		m_normalAlloc = 0;
	}

	return result;
}


IFXRESULT CIFXAuthorMesh::SetPositionArray(IFXVector3* pData, U32 p)
{
	m_pPositions = pData;
	m_MaxMeshDesc.NumPositions = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetNormalArray(IFXVector3* pData, U32 p)
{
	m_pNormals = pData;
	m_MaxMeshDesc.NumNormals = p;
	m_normalAlloc = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetDiffuseColorArray(IFXVector4* pData, U32 p)
{
	m_pDiffuseColors = pData;
	m_MaxMeshDesc.NumDiffuseColors = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetSpecularColorArray(IFXVector4* pData, U32 p)
{
	m_pSpecularColors = pData;
	m_MaxMeshDesc.NumSpecularColors = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetTexCoordArray(IFXVector4* pData, U32 p)
{
	m_pTexCoords = pData;
	m_MaxMeshDesc.NumTexCoords = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetPositionFaces(IFXAuthorFace*p)
{
	m_pPositionFaces = p;
	return IFX_OK;
}
IFXRESULT CIFXAuthorMesh::SetNormalFaces(IFXAuthorFace*p)
{
	m_pNormalFaces = p;
	return IFX_OK;
}
IFXRESULT CIFXAuthorMesh::SetDiffuseFaces(IFXAuthorFace*p)
{
	m_pDiffuseFaces = p;
	return IFX_OK;
}
IFXRESULT CIFXAuthorMesh::SetSpecularFaces(IFXAuthorFace*p)
{
	m_pSpecularFaces = p;
	return IFX_OK;
}
IFXRESULT CIFXAuthorMesh::SetTexFaces(U32 layer, IFXAuthorFace*p)
{
	m_pTexCoordFaces[layer] = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetFaceMaterials(U32*p)
{
	m_pFaceMaterials = p;
	return IFX_OK;
}

IFXRESULT CIFXAuthorMesh::SetMaxNumFaces(U32 p)
{
	m_MaxMeshDesc.NumFaces = p;
	return IFX_OK;
}

IFXVector4 CIFXAuthorMesh::CalcBoundSphere()
{
	const float minFloats[] = { FLT_MAX, FLT_MAX, FLT_MAX, 0 };
	const float maxFloats[] = {-FLT_MAX,-FLT_MAX,-FLT_MAX, 0 };
	IFXVector4 vmin(minFloats);
	IFXVector4 vmax(maxFloats);

	IFXVector4 bound;

	// Determine the axis aligned bounding box and the number of verticies.
	IFXVector3* vertex;

	U32 uVertexCount = m_CurMeshDesc.NumPositions;
	if( !m_CurMeshDesc.NumFaces )
		uVertexCount = 0;

	U32 i;
	for(  i = 0; i < uVertexCount; i++ )
	{
		vertex = &m_pPositions[i];
		if ( vertex->X() < vmin.X() ) vmin.X() = vertex->X();
		if ( vertex->X() > vmax.X() ) vmax.X() = vertex->X();
		if ( vertex->Y() < vmin.Y() ) vmin.Y() = vertex->Y();
		if ( vertex->Y() > vmax.Y() ) vmax.Y() = vertex->Y();
		if ( vertex->Z() < vmin.Z() ) vmin.Z() = vertex->Z();
		if ( vertex->Z() > vmax.Z() ) vmax.Z() = vertex->Z();
	}

	// If there are any verticies, find the average position as the center,
	// and the distance to the furthest point as the radius.
	if ( uVertexCount )
	{
		vmin.Add(vmax);
		vmin.Scale3(0.5f);
		bound = vmin;
		F32 fMaxSquaredDistance = -FLT_MAX;
		F32 fSquaredDistance;
		IFXVector3 d;

		const U32 uVertexCount = m_CurMeshDesc.NumPositions;

		U32 i;
		for(  i = uVertexCount; i--; )
		{
			vertex = &m_pPositions[i];
			d.X() = vertex->X() - bound.X();
			d.Y() = vertex->Y() - bound.Y();
			d.Z() = vertex->Z() - bound.Z();
			fSquaredDistance = d.DotProduct(d);
			if ( fSquaredDistance > fMaxSquaredDistance )
				fMaxSquaredDistance = fSquaredDistance;
		}

		bound.A() = sqrtf( fMaxSquaredDistance );
	}
	else
		bound.Set( 0.0, 0.0, 0.0, 0.0 );

	return bound;
}
