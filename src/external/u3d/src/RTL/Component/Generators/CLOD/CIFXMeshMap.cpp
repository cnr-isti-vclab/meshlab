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
	@file	CIFXMeshMap.cpp
*/

#include "IFXAuthorMeshMap.h"
#include "CIFXMeshMap.h"
#include "IFXVertexMap.h"

IFXRESULT CIFXMeshMap::Construct( U32* pMapSizes )
{
	IFXRESULT result = IFX_OK;

	U32 i, j;
	for( i = 0; i < 6 && IFXSUCCESS(result); ++i )
	{
		if( 0 != pMapSizes[i] )
		{
			IFXDELETE( m_pMaps[i] );
			m_pMaps[i] = new IFXVertexMap;
			if( NULL != m_pMaps[i] )
			{
				m_pMaps[i]->AllocateMap(pMapSizes[i]);
			}
			else
			{
				result = IFX_E_OUT_OF_MEMORY;

				// delete all previously allocated maps
				for( j = 0; j < i; ++j )
					IFXDELETE(m_pMaps[i]);
			}
		}
	}
	
	if( IFXFAILURE( result ) )
	{
		for( i = 0; i < 6; ++i )
			pMapSizes[i] = 0;
	}

	return result;
}


IFXRESULT CIFXMeshMap::Allocate( IFXAuthorMesh* pMesh )
{	
	U32 mapSizes[6]; 
	mapSizes[0] = pMesh->GetMaxMeshDesc()->NumFaces;
	mapSizes[1] = pMesh->GetMaxMeshDesc()->NumPositions;
	mapSizes[2] = pMesh->GetMaxMeshDesc()->NumNormals;
	mapSizes[3] = pMesh->GetMaxMeshDesc()->NumTexCoords;
	mapSizes[4] = pMesh->GetMaxMeshDesc()->NumDiffuseColors;
	mapSizes[5] = pMesh->GetMaxMeshDesc()->NumSpecularColors;
	
	return Construct( mapSizes );
}

IFXRESULT CIFXMeshMap::Allocate( IFXAuthorLineSet* pLineSet )
{	
	U32 mapSizes[6]; 
	mapSizes[0] = pLineSet->GetMaxLineSetDesc()->m_numLines;
	mapSizes[1] = pLineSet->GetMaxLineSetDesc()->m_numPositions;
	mapSizes[2] = pLineSet->GetMaxLineSetDesc()->m_numNormals;
	mapSizes[3] = pLineSet->GetMaxLineSetDesc()->m_numTexCoords;
	mapSizes[4] = pLineSet->GetMaxLineSetDesc()->m_numDiffuseColors;
	mapSizes[5] = pLineSet->GetMaxLineSetDesc()->m_numSpecularColors;
	
	return Construct( mapSizes );
}

IFXRESULT CIFXMeshMap::Allocate( IFXAuthorPointSet* pPointSet )
{	
	U32 mapSizes[6]; 
	mapSizes[0] = pPointSet->GetMaxPointSetDesc()->m_numPoints;
	mapSizes[1] = pPointSet->GetMaxPointSetDesc()->m_numPositions;
	mapSizes[2] = pPointSet->GetMaxPointSetDesc()->m_numNormals;
	mapSizes[3] = pPointSet->GetMaxPointSetDesc()->m_numTexCoords;
	mapSizes[4] = pPointSet->GetMaxPointSetDesc()->m_numDiffuseColors;
	mapSizes[5] = pPointSet->GetMaxPointSetDesc()->m_numSpecularColors;
	
	return Construct( mapSizes );
}

IFXRESULT CIFXMeshMap::Allocate( IFXMeshMap* pMeshMap )
{	
	U32 mapSizes[6] = {0};
	U32 i;
	for( i = 0; i < 6; ++i )
	{
		IFXVertexMap* pVertexMap = pMeshMap->GetMap( i );
		if( NULL != pVertexMap )
			mapSizes[i] = pVertexMap->GetNumMapEntries();
		else
			mapSizes[i] = 0;
	}

	return Construct( mapSizes );
}


// Combines this map with an AuthorMeshMap to provide a complete
// mapping from an orginal author mesh to the final IFXMesh.
// This map is modified in place.
IFXRESULT CIFXMeshMap::Concatenate(IFXAuthorMeshMap *pAuthorMap)
{
	IFXRESULT r = IFX_OK;
	U32 i;
	for( i = 0; i < 6; i++)
	{
		if(m_pMaps[i])
		{
			r = m_pMaps[i]->Concatenate(
								pAuthorMap->GetMap(i), 
								pAuthorMap->GetMapSize(i));
			if(r != IFX_OK)
				return r;
		}
	}

	return r;
}

IFXRESULT CIFXMeshMap::ConcatenateMeshMap( 
							IFXMeshMap* pMeshMap, 
							IFXMeshMap** ppOutMap )
{
	IFXRESULT result = IFX_OK;
	IFXVertexMap* pResultVertexMap = NULL;

	if( NULL != *ppOutMap )
	{
		U32 i;
		for( i = 0; i < 6 && IFXSUCCESS( result ); ++i )
		{
			if( NULL != m_pMaps[i] )
			{
				pResultVertexMap = (*ppOutMap)->GetMap(i);
				result = m_pMaps[i]->ConcatenateVertexMap( 
											pMeshMap->GetMap(i), 
											&pResultVertexMap );
			}
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXMeshMap::PopulateMeshMap(IFXAuthorMeshMap *pAuthorMeshMap)
{
	IFXRESULT		result = IFX_OK;
	IFXVertexMap	*pOutputMap;
	U32				*pInputMap;
	U32				mapSize = 0;
	U32				i, j;

	if (!pAuthorMeshMap)
		result = IFX_E_INVALID_POINTER;

	// Populate one map at a time.
	for (i = 0; IFXSUCCESS(result) && (i < 6); i++)
	{
		pInputMap = pAuthorMeshMap->GetMap(i);
		mapSize = pAuthorMeshMap->GetMapSize(i);
		pOutputMap = m_pMaps[i];

		if (pOutputMap)
		{
			// Before we proceed, do a range check
			if (mapSize > pOutputMap->GetNumMapEntries())
				result = IFX_E_INVALID_RANGE;
			else
			{
				// Copy the IFXAuthorMeshMap to the IFXMeshMap
				for (j = 0; IFXSUCCESS(result) && (j < mapSize); j++)
				{
					//  Index j became this index (including, possibly, IFX_NULL_INDEX32)
					U32 remappedIndex = pInputMap[j];

					// Only replicate this mapping forward if the original
					// vertex actually ended up in the remapped mesh.
					if (IFX_NULL_INDEX32 != remappedIndex)
						result = pOutputMap->AddVertex(j, 0, remappedIndex);
				}
			}
		}
	}

	return result;
}

IFXRESULT CIFXMeshMap::AddIdentityMappingToMap(U32 mapIndex)
{
	IFXRESULT		result = IFX_OK;
	IFXVertexMap	*pMap = NULL;
	U32				i;

	if (mapIndex >= 6)
		result = IFX_E_INVALID_RANGE;

	if (IFXSUCCESS(result))
	{
		pMap = m_pMaps[mapIndex];
	}

	// Note - if pMap is NULL this function is a NOOP.
	if (pMap && IFXSUCCESS(result))
	{
		for (i = 0; IFXSUCCESS(result) && (i < pMap->GetNumMapEntries()); i++)
		{
			result = pMap->AddVertex(i, 0, i);
		}
	}

	return result;
}

IFXRESULT CIFXMeshMap::AddMappingToMap(
							const U32 mapIndex, const U32 origVertexIndex, 
							const U32 indexMesh, U32 indexVertex)
{
	IFXRESULT result = IFX_OK;

	if (mapIndex < 6)
	{
		if (NULL != m_pMaps[mapIndex])
		{
			result = m_pMaps[mapIndex]->AddVertex(
											origVertexIndex, 
											indexMesh, 
											indexVertex);
		}
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_INVALID_RANGE;

	return result;
}


IFXVertexMap* CIFXMeshMap::GetFaceMap()
{	
	return m_pMaps[0];
}
IFXVertexMap* CIFXMeshMap::GetPositionMap()
{	
	return m_pMaps[1];
}
IFXVertexMap* CIFXMeshMap::GetNormalMap()
{	
	return m_pMaps[2];
}
IFXVertexMap* CIFXMeshMap::GetTextureMap()
{	
	return m_pMaps[3];
}
IFXVertexMap* CIFXMeshMap::GetDiffuseMap()
{	
	return m_pMaps[4];
}
IFXVertexMap* CIFXMeshMap::GetSpecularMap()
{	
	return m_pMaps[5];
}

IFXVertexMap* CIFXMeshMap::GetMap(U32 i)
{	
	IFXASSERT(i < 6);  // map index too large
	return m_pMaps[i];
}
	
CIFXMeshMap::CIFXMeshMap()
{
	m_refCount = 0;
	U32 i;
	for( i = 0; i < 6; i++)
		m_pMaps[i] = NULL;
}


CIFXMeshMap::~CIFXMeshMap()
{
	U32 i;
	for( i = 0; i < 6; i++)
		IFXDELETE(m_pMaps[i]);
}

//---------------------------------------------------------------------------
//	CIFXMeshMap::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXMeshMap::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
//	CIFXMeshMap::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXMeshMap::Release()
{
	if ( 1 == m_refCount )
	{
		delete this;
		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_refCount;
}

//---------------------------------------------------------------------------
//	CIFXMeshMap::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------

IFXRESULT CIFXMeshMap::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if(interfaceId == IID_IFXUnknown)
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		if ( interfaceId == IID_IFXMeshMap )
		{
			*ppInterface = ( IFXMeshMap* ) this;
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
//	CIFXMeshMap_Factory
//
//	This is the CIFXClassName component factory function.  The
//	CIFXClassName component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXMeshMap_Factory( IFXREFIID	interfaceId,
								 void**		ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXMeshMap	*pComponent	= new CIFXMeshMap;

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
