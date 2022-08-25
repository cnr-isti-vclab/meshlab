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
#include "CIFXAuthorMeshMap.h"
#include "IFXAuthorMesh.h"
#include "IFXCoreCIDs.h"

/** 
This map will be modified in place so that it performs both mappings
with a single map.
Before: inputMesh --> thisMap --> IntermediateMesh --> MapB --> resultMesh
After : inputMesh --> thisMap --> resultMesh
*/
IFXRESULT CIFXAuthorMeshMap::Concatenate(IFXAuthorMeshMap* pMapB)
{
	U32 i, j, stop, *mapA, *mapB;

	for(i = 0; i < 6; i++)
	{
		// a one to one or a many to one mapping can not have more entries on the ouput.
		IFXASSERT(pMapB->GetMapSize(i) <= GetMapSize(i));
		stop = GetMapSize(i);
		mapA = GetMap(i);
		mapB = pMapB->GetMap(i);
		for(j = 0; j<stop; j++)
		{
			if(mapA[j] != IFX_BAD_INDEX32)
				mapA[j] = mapB[ mapA[j] ];
		}
	}

	return IFX_OK;
}

/**
@todo This method does not properly create a new mesh map and should be
changed to use IFXCreateComponent.
*/
IFXAuthorMeshMap* CIFXAuthorMeshMap::Clone()
{
	U32 i;
	IFXRESULT result;
	IFXAuthorMeshMap* pClone = NULL;
	result = IFXCreateComponent(CID_IFXAuthorMeshMap, IID_IFXAuthorMeshMap, (void**)&pClone);
	if (IFXFAILURE(result))
		return NULL;

	for(i = 0; i < 6; i++)
		pClone->SetMapSize(i, GetMapSize(i));

	result = pClone->AllocateMaps();
	if (result != IFX_OK) 
	{
		pClone->Release();
		return NULL;
	}

	// copy map data.
	for(i = 0; i < 6; i++)
		memcpy(pClone->GetMap(i), GetMap(i), sizeof(U32)*GetMapSize(i));

	return pClone;
}

CIFXAuthorMeshMap::CIFXAuthorMeshMap()
{
	m_refCount = 0;
	U32 i;
	for( i = 0; i < 6; i++)
	{
		m_pMaps[i] = NULL;
		m_MapSizes[i] = 0;
	}
}

CIFXAuthorMeshMap::~CIFXAuthorMeshMap()
{
	U32 i;
	for( i = 0; i < 6; i++)
	{
		IFXDELETE_ARRAY(m_pMaps[i]);
		m_MapSizes[i] = 0;
	}
}

IFXRESULT CIFXAuthorMeshMap::Allocate(IFXAuthorMesh* pMesh)
{
	m_MapSizes[0] = pMesh->GetMaxMeshDesc()->NumFaces;
	m_MapSizes[1] = pMesh->GetMaxMeshDesc()->NumPositions;
	m_MapSizes[2] = pMesh->GetMaxMeshDesc()->NumNormals;
	m_MapSizes[3] = pMesh->GetMaxMeshDesc()->NumTexCoords;
	m_MapSizes[4] = pMesh->GetMaxMeshDesc()->NumDiffuseColors;
	m_MapSizes[5] = pMesh->GetMaxMeshDesc()->NumSpecularColors;

	return AllocateMaps();
}

IFXRESULT CIFXAuthorMeshMap::AllocateMaps()
{
	U32 i;
	for( i = 0; i < 6; i++)
	{
		IFXDELETE_ARRAY(m_pMaps[i]);

		if( 0 != m_MapSizes[i] )
		{
			m_pMaps[i] = new U32[m_MapSizes[i]];
			if(m_pMaps[i] == NULL)
			{
				// clean up and exit with error
				U32 j;
				for ( j = 0; j < i; j++)
					IFXDELETE_ARRAY(m_pMaps[j]);

				return IFX_E_OUT_OF_MEMORY;
			}

			// init with identity mapping.
			U32 j;
			for( j=0; j < m_MapSizes[i]; j++)
				m_pMaps[i][j] = j;
		}
	}

	return IFX_OK;
}

U32* CIFXAuthorMeshMap::GetFaceMap()
{
	return m_pMaps[0];
}
U32* CIFXAuthorMeshMap::GetPositionMap()
{
	return m_pMaps[1];
}
U32* CIFXAuthorMeshMap::GetNormalMap()
{
	return m_pMaps[2];
}
U32* CIFXAuthorMeshMap::GetTextureMap()
{
	return m_pMaps[3];
}
U32* CIFXAuthorMeshMap::GetDiffuseMap()
{
	return m_pMaps[4];
}
U32* CIFXAuthorMeshMap::GetSpecularMap()
{
	return m_pMaps[5];
}
U32* CIFXAuthorMeshMap::GetMap(U32 mapIndex)
{
	IFXASSERT(mapIndex < 6);
	return m_pMaps[mapIndex];
}
U32 CIFXAuthorMeshMap::GetMapSize(U32 mapIndex)
{
	IFXASSERT(mapIndex < 6);
	return m_MapSizes[mapIndex];
}
void CIFXAuthorMeshMap::SetMapSize(U32 mapIndex, U32 value)
{
	IFXASSERT(mapIndex < 6);
	m_MapSizes[mapIndex] = value;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMeshMap::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXAuthorMeshMap::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMeshMap::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXAuthorMeshMap::Release()
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
//  CIFXAuthorMeshMap::QueryInterface
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

IFXRESULT CIFXAuthorMeshMap::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if(interfaceId == IID_IFXUnknown)
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		if ( interfaceId == IID_IFXAuthorMeshMap )
		{
			*ppInterface = ( IFXAuthorMeshMap* ) this;
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}

//---------------------------------------------------------------------------
//  CIFXAuthorMeshMap_Factory
//
//  This is the CIFXClassName component factory function.  The
//  CIFXClassName component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorMeshMap_Factory( IFXREFIID  interfaceId,
									void**   ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXAuthorMeshMap *pComponent = new CIFXAuthorMeshMap;

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
