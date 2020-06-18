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
	@file	IFXCoincidentVertexMap.cpp

			This module defines the IFXCoincidentVertexMap class.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXMesh.h"
#include "IFXVectorHasher.h"
#include "IFXCoincidentVertexMap.h"
#include "IFXBoundingBox.h"

IFXCoincidentVertexMap::IFXCoincidentVertexMap()
{
	m_ppCoincidentVertexID = NULL;
	m_numMeshes = 0;
	m_numVertexID = 0;
}


IFXCoincidentVertexMap::~IFXCoincidentVertexMap()
{
	if (m_ppCoincidentVertexID)
	{
		U32 meshIndex;
		U32* pList;
		for (meshIndex = 0; meshIndex < m_numMeshes; ++meshIndex)
		{
			pList = m_ppCoincidentVertexID[meshIndex];
			IFXDELETE(pList);
		}
	}

	IFXDELETE_ARRAY(m_ppCoincidentVertexID);
}


IFXRESULT IFXCoincidentVertexMap::Initialize(IFXMeshGroup* pMeshGroup)
{
	IFXRESULT result = IFX_OK;

	U32 numMeshes, meshIndex;
	U32 numVerts, vertIndex;
	U32 numMeshVertices = 0;
	IFXMesh* pMesh = 0;
	IFXVector3Iter iter;
	IFXVectorHasher hasher;

	if (NULL == pMeshGroup)
	{
		result = IFX_E_INVALID_POINTER;
	}
	
	if ( IFXSUCCESS( result ) )
	{
		numMeshes = pMeshGroup->GetNumMeshes();
		m_numMeshes = numMeshes;
	
		//
		// Build vector position hash table to find duplicate position vectors
		//

		// Compute bounding box
		IFXBoundingBox boundingBox;
		for (meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
		{
			pMeshGroup->GetMesh(meshIndex, pMesh);
			// must have position attribute
			IFXASSERT(pMesh->GetAttributes().m_uData.m_bHasPositions);
			pMesh->GetPositionIter(iter);

			numVerts = pMesh->GetNumVertices();
			numMeshVertices += numVerts;
			for (vertIndex = 0; vertIndex < numVerts; ++vertIndex)
			{
				boundingBox.AddVector(iter.Next());
			}

			IFXRELEASE(pMesh);
		}

		IFXVector3 minPos;
		IFXVector3 maxPos;
		boundingBox.GetBox(&minPos, &maxPos);

		// Initialize hash table
		result = hasher.Initialize(numMeshVertices, &minPos, &maxPos);
	}

	if ( IFXSUCCESS( result ) )
	{
		// Add vectors to hash table
		for ( meshIndex = 0; meshIndex < numMeshes; ++meshIndex )
		{
			result = pMeshGroup->GetMesh(meshIndex, pMesh);
			
			if( IFXSUCCESS( result ) )
			{
				// must have position attribute
				IFXASSERT(pMesh->GetAttributes().m_uData.m_bHasPositions);
				result = pMesh->GetPositionIter(iter);
				
				if( IFXSUCCESS( result ) )
				{
					numVerts = pMesh->GetNumVertices();
					for (vertIndex = 0; vertIndex < numVerts; ++vertIndex)
					{
						result = hasher.AddVector(iter.Next(), meshIndex, vertIndex);
						if( IFXFAILURE( result ) ) break;
					}
				}

				IFXRELEASE(pMesh);
			}

			if( IFXFAILURE( result ) ) break;
		}
	}

	//
	// Allocate memory for mapping
	//

	if ( IFXSUCCESS( result ) )
	{
		m_ppCoincidentVertexID = new U32*[numMeshes];
		
		if (NULL == m_ppCoincidentVertexID)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if ( IFXSUCCESS( result ) )
	{
		for ( meshIndex = 0; 
			  meshIndex < numMeshes; 
			  ++meshIndex )
		{
			result = pMeshGroup->GetMesh(meshIndex, pMesh);
			
			if( IFXSUCCESS( result ) )			
			{
				m_ppCoincidentVertexID[meshIndex] = new U32[pMesh->GetNumVertices()];
				
				if (NULL == m_ppCoincidentVertexID[meshIndex])
				{
					result = IFX_E_OUT_OF_MEMORY;
				}

				IFXRELEASE(pMesh);
			}

			if( IFXFAILURE( result ) ) break;
		}
	}

	//
	// Walk through hash table and build mapping
	//
	U32 coincidentVertexID = 0;
	if ( IFXSUCCESS( result ) )
	{
		
		IFXVectorHasherBinIterator binIter;
		IFXVectorHasherNodeIterator nodeIter;
		BOOL ret = hasher.GetBinIterator(&binIter);
		if (ret)
		{
			do
			{
				binIter.GetNodeIterator(&nodeIter);
				do
				{
					nodeIter.Get(&meshIndex, &vertIndex);
					m_ppCoincidentVertexID[meshIndex][vertIndex] = 
						coincidentVertexID;
				} while (nodeIter.Next());	// next node in this bin

				++coincidentVertexID;

			} while (binIter.Next());	// next bin
		}

	}

	m_numVertexID = coincidentVertexID;
	return result;
}


IFXRESULT IFXCoincidentVertexMap::Initialize(IFXMeshGroup* pMeshGroup, 
											IFXVertexMapGroup* pVertexMapGroup)
{
	IFXRESULT result = IFX_OK;
	U32 meshIndex;

	if (NULL == pMeshGroup)
	{
		result = IFX_E_INVALID_POINTER;
	}
	
	if ( IFXSUCCESS( result ) )
	{
		//
		// Allocate memory for mapping
		//

		m_numMeshes = pMeshGroup->GetNumMeshes();
		m_ppCoincidentVertexID = new U32*[m_numMeshes];

		if (NULL == m_ppCoincidentVertexID)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS( result ) )
	{	
		for ( meshIndex = 0; meshIndex < m_numMeshes; ++meshIndex )
		{
			IFXMesh* pMesh = 0;
			result = pMeshGroup->GetMesh(meshIndex, pMesh);
			
			if ( IFXSUCCESS( result ) )
			{
				m_ppCoincidentVertexID[meshIndex] = new U32[pMesh->GetNumVertices()];
				
				if (NULL == m_ppCoincidentVertexID[meshIndex])
				{
					result = IFX_E_OUT_OF_MEMORY;
				}

				IFXRELEASE(pMesh);
			}

			if( IFXFAILURE( result ) ) break;
		}

		//
		// Walk through vertex map and build reverse map
		//
		// We want to map IFXMeshGroup vertex indices to a single
		// unqiue vertex index, thus we don't care about the multiple
		// source meshes.
		if( IFXSUCCESS( result ) )
		{
			m_numVertexID = 0;
			U32 origMeshIndex;
			U32 numMaps = pVertexMapGroup->GetNumMaps();
			for (origMeshIndex = 0; origMeshIndex < numMaps; ++origMeshIndex)
			{
				IFXVertexMap* pVertexMap = pVertexMapGroup->GetMap(origMeshIndex);

				U32 origVertexIndex;
				U32 numEntry = pVertexMap->GetNumMapEntries();
				for ( origVertexIndex = 0; 
					origVertexIndex < numEntry; 
					++origVertexIndex )
				{
					U32 numCopies=pVertexMap->GetNumVertexCopies(origVertexIndex);
					U32 copyIndex;
					for ( copyIndex = 0; copyIndex < numCopies; ++copyIndex)
					{
						U32 meshIndex = 0, vertexIndex = 0;
						pVertexMap->GetVertexCopy( origVertexIndex, copyIndex,
												&meshIndex, &vertexIndex);

						// Verify that vertex map values are valid
						IFXASSERT(meshIndex < m_numMeshes);

						m_ppCoincidentVertexID[meshIndex][vertexIndex] = 
							m_numVertexID + origVertexIndex;
					}
				}

				m_numVertexID += numEntry;
			}
		}
	}

	return result;
}
