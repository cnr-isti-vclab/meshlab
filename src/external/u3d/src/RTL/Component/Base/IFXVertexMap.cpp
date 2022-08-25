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
	@file	IFXVertexMap.cpp

			This module defines the IFXVertexMap class.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXVertexMap.h"
#include "IFXAuthorMesh.h"
#include <memory.h>
 
IFXVertexMap::IFXVertexMap()
{
	m_mapSize = 0;
	m_pCounts = NULL;
	m_pBlockSizes = NULL;
	m_ppCopies =  NULL;
}

IFXVertexMap::~IFXVertexMap()
{
	IFXDELETE_ARRAY(m_pCounts);
	IFXDELETE_ARRAY(m_pBlockSizes);
	
	if (m_ppCopies)
	{
		U32 i = 0;
		for (i = 0; i < m_mapSize; ++i)
		{
			IFXDELETE_ARRAY(m_ppCopies[i]);
		}
	}
	IFXDELETE_ARRAY(m_ppCopies);
}

U32 IFXVertexMap::GetNumMapEntries(void)
{
	return m_mapSize;
}

// These are used to build the map.
IFXRESULT IFXVertexMap::AllocateMap(U32 numOrigVertices)
{
	IFXRESULT result = IFX_OK;

	// Only AllocateMap once per instance
	IFXASSERT(m_mapSize == 0  &&  m_pCounts == NULL  &&  m_ppCopies == NULL);

	if ( !(m_mapSize == 0  &&  m_pCounts == NULL  &&  m_ppCopies == NULL) )
	{
		result = IFX_E_ALREADY_INITIALIZED;
	}

	if( IFXSUCCESS( result ) )
	{
		m_mapSize = numOrigVertices;

		m_ppCopies = new MeshVert*[numOrigVertices];
		
		if (NULL == m_ppCopies)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		memset(m_ppCopies, 0, sizeof(MeshVert*) * numOrigVertices);
		
		m_pBlockSizes = new U32[numOrigVertices];

		if (NULL == m_pBlockSizes) 
		{
			IFXDELETE_ARRAY(m_ppCopies);
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		m_pCounts = new U32[numOrigVertices];
		
		if (NULL == m_pCounts) 
		{
			IFXDELETE_ARRAY(m_pBlockSizes);
			IFXDELETE_ARRAY(m_ppCopies);
			result = IFX_E_OUT_OF_MEMORY;
		}
	}
	
	if( IFXSUCCESS( result ) )
	{
		memset(m_pCounts, 0, sizeof(U32) * numOrigVertices);
		memset(m_pBlockSizes, 0, sizeof(U32) * numOrigVertices);
	}

	return result;
}

// ASSUMPTION:  IFXVertexMap contains no entries with more than one entry
IFXRESULT IFXVertexMap::Concatenate(U32 *pMapArray, U32 arraySize)
{
	IFXRESULT result = IFX_OK;
	U32 i, mapFrom, mapTo, mapToMerge, remappedIndex;
	MeshVert **ppNewMV = NULL;
	U32 *pNewCounts = NULL;
	U32 *pReverseMap = NULL;
	U32 *pDataMap = NULL;

	// Range check
	if (arraySize > m_mapSize)
		result = IFX_E_INVALID_RANGE;

	if( IFXSUCCESS( result ) )
	{
		// Allocate memory for the output map
		ppNewMV = new MeshVert*[m_mapSize];

		if(ppNewMV == NULL) 
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		pNewCounts = new U32[m_mapSize];

		if(pNewCounts == NULL)
		{
			IFXDELETE_ARRAY(ppNewMV);
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		// Allocate our working structures
		pReverseMap = new U32[arraySize];

		if(pReverseMap == NULL)
		{
			IFXDELETE_ARRAY(ppNewMV);
			IFXDELETE_ARRAY(pNewCounts);
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		pDataMap = new U32[arraySize];

		if(pDataMap == NULL)
		{
			IFXDELETE_ARRAY(ppNewMV);
			IFXDELETE_ARRAY(pNewCounts);
			IFXDELETE_ARRAY(pReverseMap);
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	// Some final setup
	mapTo = 0;
	for (i = 0; i < m_mapSize; i++)
	{
		// Initialize the output map
		pNewCounts[i] = 0;
		ppNewMV[i] = NULL;

		// Build our working mappings.
		if (NULL != m_ppCopies[i])
		{
			if (mapTo >= arraySize)
			{
				IFXDELETE_ARRAY(ppNewMV);
				IFXDELETE_ARRAY(pNewCounts);
				IFXDELETE_ARRAY(pReverseMap);
				IFXDELETE_ARRAY(pDataMap);
				result = IFX_E_INVALID_RANGE;

				break;
			}

			// Record the indices of any input map entries 
			// that have something in them.
			pDataMap[mapTo] = i;

			// Build a reverse map, mapping values to indices
			pReverseMap[m_ppCopies[i]->vert] = i;

			mapTo++;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		// Now copy the records forward, based on the mappings
		// in the author vertex map
		for (i = 0; i < arraySize; i++)
		{
			// Index of cell whose value to replace
			mapTo = pDataMap[i];  

			// Value of cell (what index maps to)
			mapToMerge = m_ppCopies[mapTo]->vert; 

			// Value remapped by author vertex map
			remappedIndex = pMapArray[mapToMerge];
			
			if (IFX_BAD_INDEX32 != remappedIndex)
			{
				// Where these data come from out of the original array.
				mapFrom = pReverseMap[remappedIndex];  

				// Apply new mapping of vertex, found in author
				// map.  Carry the vertex data foward in that new
				// location.
				pNewCounts[mapTo] = m_pCounts[mapFrom];
				ppNewMV[mapTo] = m_ppCopies[mapFrom];
			}
		}

		// Clear out old maps
		IFXDELETE_ARRAY(pReverseMap);
		IFXDELETE_ARRAY(pDataMap);
		IFXDELETE_ARRAY(m_pCounts);
		for (i = 0; i < m_mapSize; ++i) {
			IFXDELETE_ARRAY(m_ppCopies[i]);
		}
		IFXDELETE_ARRAY(m_ppCopies);

		// And replace with new
		m_pCounts = pNewCounts;
		m_ppCopies = ppNewMV;
	}

	return result;
}

// pVMap is the upstream map (from compressor).
IFXRESULT IFXVertexMap::ConcatenateVertexMap(IFXVertexMap* pVMap, 
											 IFXVertexMap** ppNewMap)
{
	IFXRESULT result = IFX_OK;
	BOOL duplicate;
	U32 i,j,k,l, numTemp, numCopies, numCopies2, meshIndex = 0, vertIndex = 0;
	U32 MaxTemp = 256;
	MeshVert* pTemp = NULL;
	
	if( NULL == ppNewMap )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
	{
		if( NULL == pVMap || NULL == *ppNewMap )
			result = IFX_E_INVALID_POINTER;
	}

	if( IFXSUCCESS( result ) )
	{
		pTemp = new MeshVert[MaxTemp];

		if( NULL == pTemp )
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		U32 numMapEntries = pVMap->GetNumMapEntries();
		IFXVertexMap* pNewMap = *ppNewMap;
		MeshVert* pPotential = NULL;

		// iterate over all entries in the upstream map (map from compressor).
		for(i = 0; i < numMapEntries; i++)
		{
			numTemp = 0;
			numCopies = pVMap->GetNumVertexCopies(i);
			for(j = 0; j < numCopies; j++)
			{
				pVMap->GetVertexCopy(i,j, &meshIndex, &vertIndex);
				IFXASSERT(meshIndex == 0);
				IFXASSERT(vertIndex < m_mapSize);
				numCopies2 = m_pCounts[vertIndex];

				// make sure we are not putting duplicate entries into the map.
				for(k=0; k < numCopies2; k++)
				{
					duplicate = FALSE;
					pPotential = &m_ppCopies[vertIndex][k];

					for(l=0; l < numTemp; l++) 
					{
						IFXASSERT(l < MaxTemp);
						if( pTemp[l].vert == pPotential->vert && 
							pTemp[l].mesh == pPotential->mesh )
						{
							duplicate = TRUE;
							break;
						}
					}

					if(!duplicate)
					{
						IFXASSERT(numTemp < MaxTemp);
						pTemp[numTemp] = *pPotential;
						numTemp++;
						// handle very rare temp buffer overflow.
						if(numTemp >= MaxTemp)
						{  
							U32 curMaxTemp = MaxTemp * 2;
							MeshVert* pTemp2 = new MeshVert[curMaxTemp];

							if( NULL == pTemp2 )
							{
								result = IFX_E_OUT_OF_MEMORY;

								break;
							}

							memset(pTemp2,0,sizeof(MeshVert)*curMaxTemp);
							memcpy(pTemp2, pTemp, numTemp * sizeof(MeshVert));
							IFXDELETE_ARRAY(pTemp);
							pTemp = pTemp2;
							MaxTemp = curMaxTemp;
						}
					}
				}

				if( !IFXSUCCESS( result ) )
				{
					break;
				}
			}

			if( !IFXSUCCESS( result ) )
			{
				break;
			}

			// allocate buffer of the correct length 
			// and move the temp buf contents to it.
			if( numTemp )
			{
				MeshVert* pFinal = NULL;

				IFXASSERT( i < pNewMap->GetNumMapEntries() );

				pFinal = new MeshVert[numTemp];
				
				if( NULL == pFinal )
				{									
					result = IFX_E_OUT_OF_MEMORY;

					break;
				}


				memcpy(pFinal, pTemp, numTemp * sizeof(MeshVert));

				// fill in the new map.
				pNewMap->m_ppCopies[i] = pFinal;
				pNewMap->m_pCounts[i] = numTemp;
				pNewMap->m_pBlockSizes[i] = numTemp; 
			}
		}
	}

	IFXDELETE_ARRAY(pTemp);

	return result;
}

IFXRESULT IFXVertexMap::AddVertex(U32 origVertexIndex, U32 indexIFXMesh, 
								  U32 indexIFXVertex)
{
	IFXRESULT result = IFX_OK;

	if( origVertexIndex >= m_mapSize )
	{
		// vertex index exceed array boundary - prevent memory overrun
		result = IFX_E_INVALID_RANGE;
		IFXASSERT( FALSE );
	}

	if( IFXSUCCESS( result ) )
	{
		U32 numExisting = m_pCounts[origVertexIndex];
	
		// create new block if overflowed existing
		if((numExisting + 1) > m_pBlockSizes[origVertexIndex])
		{
			if(m_pBlockSizes[origVertexIndex])
				m_pBlockSizes[origVertexIndex] *= 2;  // double the size
			else
				m_pBlockSizes[origVertexIndex] = 1;   // if it was 0 make it 1
		
			MeshVert* pNewMapEntries=new MeshVert[m_pBlockSizes[origVertexIndex]];

			if( NULL != pNewMapEntries )
			{
				U32 i;
				for(i = 0; i < numExisting; i++)
					pNewMapEntries[i] = m_ppCopies[origVertexIndex][i];

				IFXDELETE_ARRAY(m_ppCopies[origVertexIndex]);
				m_ppCopies[origVertexIndex] = pNewMapEntries;
			}
			else
			{
				result = IFX_E_OUT_OF_MEMORY;
				IFXASSERT( FALSE );
			}
		}
		
		if( IFXSUCCESS( result ) )
		{
			m_pCounts[origVertexIndex] = numExisting + 1;
			MeshVert& rMapEntry = m_ppCopies[origVertexIndex][numExisting];
			rMapEntry.mesh = indexIFXMesh;
			rMapEntry.vert = indexIFXVertex;
		}
	}

	return result;
}
