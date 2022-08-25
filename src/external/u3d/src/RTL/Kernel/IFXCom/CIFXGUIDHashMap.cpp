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
	@file	CIFXComponentName.cpp

			This module defines the CIFXGuidHashMap class.
*/


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXPlugin.h"
#include "CIFXGuidHashMap.h"
#include "IFXResultComponentEngine.h"
#include <stdlib.h>

//***************************************************************************
//	Defines
//***************************************************************************

// IFXGUIDHASHMAPSIZE should be a prime number to ensure good hash map 
// distribution:
const U32 IFXGUIDHASHMAPSIZE = 127;

//***************************************************************************
//	Constants
//***************************************************************************


//***************************************************************************
//	Enumerations
//***************************************************************************


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//***************************************************************************
//	Global data
//***************************************************************************


//***************************************************************************
//	Local data
//***************************************************************************


//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Public methods
//***************************************************************************

CIFXGuidHashMap::CIFXGuidHashMap()
{
	m_pHashTable		= NULL;
	m_uHashTableSize	= 0;
}


CIFXGuidHashMap::~CIFXGuidHashMap()
{
	// Delete the hash table:
	IFXGUIDHashBucket* pTmpBucket1 = NULL;
	IFXGUIDHashBucket* pTmpBucket2 = NULL;
	U32 uNumBucketsDeleted = 0;
	
	if (m_pHashTable)
	{
		// Walk the hash table:
		U32 h;
		for ( h = 0; h < m_uHashTableSize; h++)
		{
			if (m_pHashTable[h].pCompDesc)
				uNumBucketsDeleted++;

			// Walk the overflow chain if it exists:
			pTmpBucket1 = m_pHashTable[h].pNext;
			while (pTmpBucket1)
			{
				pTmpBucket2 = pTmpBucket1->pNext;
				delete pTmpBucket1;
				pTmpBucket1 = pTmpBucket2;
				
				uNumBucketsDeleted++;
			}
		}
	}

	delete m_pHashTable;
	m_pHashTable = NULL;
}



IFXRESULT CIFXGuidHashMap::Initialize( 
							const U32 numComponents, 
							const IFXComponentDescriptor *pCompDescList )
{
	IFXRESULT result = IFX_OK;

	if (IFXSUCCESS(result))
	{
		m_uHashTableSize = IFXGUIDHASHMAPSIZE;
		m_pHashTable = new IFXGUIDHashBucket[m_uHashTableSize];
		if (!m_pHashTable)
			result = IFX_E_OUT_OF_MEMORY;
	}

	// Initialize all of the bucket overflow ptrs to NULL:
	if (IFXSUCCESS(result))
	{
		U32 uHe;
		for (  uHe = 0; uHe < m_uHashTableSize; ++uHe )
		{
			m_pHashTable[uHe].pCompDesc = 0;
			m_pHashTable[uHe].pNext = 0;
		}
	}
		
	// Load all of the component descriptors into the hash table:
	if (IFXSUCCESS(result))
	{
		U32 c;
		for ( c = 0; c < numComponents; c++)
		{
			// Not clear, cut and paste error or Add return code is ignored by design
			// comment out result to avoid warning and keep the existing functionality
			// IFXRESULT result = IFX_OK;
			/* result = */ Add( &pCompDescList[c] );
		}
	}

	return result;
}


IFXRESULT CIFXGuidHashMap::Add( const IFXComponentDescriptor* pCompDesc )
{
	IFXRESULT result = IFX_OK;
	IFXGUIDHashBucket* pHashBucket		= NULL;
	IFXGUIDHashBucket* pPrvHashBucket	= NULL;

	if ( !pCompDesc || !m_pHashTable)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS (result))
	{
		pHashBucket = FindHashBucket(*(pCompDesc->pComponentId), &pPrvHashBucket);
	}

	// If it wasn't found, then create a new bucket and attach it to the table:
	if (IFXSUCCESS(result) && !pHashBucket)
	{
		if (!pPrvHashBucket)
			result = IFX_E_UNDEFINED;
		else
		{
			pHashBucket				= new IFXGUIDHashBucket;
			pHashBucket->pNext		= pPrvHashBucket->pNext;
			pHashBucket->pCompDesc	= NULL;
			pPrvHashBucket->pNext	= pHashBucket;
		}
	}
	
	if (IFXSUCCESS(result))
	{
		if ( 0 == pHashBucket->pCompDesc )
			pHashBucket->pCompDesc = pCompDesc;
		else
		{
			// versioning support
			// before we can overwrite hash bucket we should compare
			// versions of components
			if ( abs(pHashBucket->pCompDesc->Version) < abs(pCompDesc->Version) )
				pHashBucket->pCompDesc = pCompDesc;
		}
	}
	
	return result;
}


IFXRESULT CIFXGuidHashMap::Delete (const IFXCID& compID)
{	
	IFXRESULT result = IFX_OK;
	IFXGUIDHashBucket* pHashBucket		= NULL;
	IFXGUIDHashBucket* pPrvHashBucket	= NULL;

	if (!m_pHashTable)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS (result))
	{
		pHashBucket = FindHashBucket(compID, &pPrvHashBucket);
	}

	if (IFXSUCCESS(result))
	{
		// If we found it, then delete it:
		if (pHashBucket && 
			pHashBucket->pCompDesc && 
			GUIDsEquivalent(*(pHashBucket->pCompDesc->pComponentId), compID))
		{
			// Is this the first hash bucket or further down the overflow chain?
			if (pPrvHashBucket)
			{
				pPrvHashBucket->pNext = pHashBucket->pNext;
				delete pHashBucket;
				pHashBucket = NULL;
			}
			else
			{
				// Is there an overflow chain?
				if (pHashBucket->pNext)
				{
					IFXGUIDHashBucket* pTmpBucket = pHashBucket->pNext;

					// Bubble the 1st overflow bucket back:
					pHashBucket->pCompDesc	= pTmpBucket->pCompDesc;
					pHashBucket->pNext		= pTmpBucket->pNext;
					delete pTmpBucket;
					pTmpBucket = NULL;
				}
				else
				// No overflow buckets, just the comp factory ptr to NULL:
				{
					pHashBucket->pCompDesc = 0;
				}
			}
		}
		else
        {
			result = IFX_E_GUIDHASHMAP_ID_NOT_FOUND;
        }
	}

	return result;
}


IFXRESULT CIFXGuidHashMap::Find ( const IFXCID& compID, const IFXComponentDescriptor** ppCompDesc) const
{
	IFXRESULT result = IFX_OK;
	IFXGUIDHashBucket* pHashBucket		= NULL;
	IFXGUIDHashBucket* pPrvHashBucket	= NULL;

	if (!ppCompDesc || !m_pHashTable)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS (result))
	{
		pHashBucket = FindHashBucket(compID, &pPrvHashBucket);
	}

	if (IFXSUCCESS(result))
	{
		if (pHashBucket && 
			pHashBucket->pCompDesc && 
			GUIDsEquivalent (*pHashBucket->pCompDesc->pComponentId, compID))
		{
            *ppCompDesc = pHashBucket->pCompDesc;
		}
		else
		{
			result = IFX_E_GUIDHASHMAP_ID_NOT_FOUND;
		}
	}

	return result;
}

#ifdef IFXGUIDDEBUG

#include <stdio.h>
/*
void CIFXGuidHashMap::Print()
{
	U32 h;
	IFXGUIDHashBucket* pTmpBucket1 = NULL;


	if (m_pHashTable)
	{
		// Walk the hash table:
		for (h = 0; h < m_uHashTableSize; h++)
		{
			// Walk the overflow chain if it exists:
			pTmpBucket1 = m_pHashTable[h].pNext;
			
			char buf[256];
			sprintf(buf, "hash [%4d]: ", h);
			//OutputDebugString(buf);
			puts(buf);

			// If there's a header entry, print it out:
			//if (m_pHashTable[h].pCompDesc)
			//{
			//	sprintf(buf, "%s  ", m_pHashTable[h].pCompDesc->pName);
				//OutputDebugString(buf);
			//	puts(buf);
			//}
			
			while (pTmpBucket1)
			{
				
				sprintf(buf, "%s  ", pTmpBucket1->pCompDesc->pName);
				//OutputDebugString(buf);
				puts(buf);

				pTmpBucket1 = pTmpBucket1->pNext;
			}
			//OutputDebugString("\n");
			puts("\n");
		}
	}
}
*/

void CIFXGuidHashMap::Print( )
{
	IFXGUIDHashBucket* pTmpBucket1 = NULL;

	if (m_pHashTable)
	{
		// Walk the hash table:
		U32 h;
		for ( h = 0; h < m_uHashTableSize; h++)
		{
			// If there's a header entry, print it out:
			if (m_pHashTable[h].pCompDesc)
			{
				printf("hash [%4d]: ", h);

				const IFXCID* pCID = m_pHashTable[h].pCompDesc->pComponentId;
				printf("%.8x ", pCID->A);
				printf("%.4x ", pCID->B);
				printf("%.4x ", pCID->C);
				U32 i;
				for( i = 0; i < 8; ++i ) printf("%.2x ", pCID->D[i]);

				printf("\t%.8x", m_pHashTable[h].pCompDesc->pFactoryFunction );
				printf("\t%.8x", m_pHashTable[h].pCompDesc->pPluginProxy );

				printf("\n\n");
			}
			
			// Walk the overflow chain if it exists:
			pTmpBucket1 = m_pHashTable[h].pNext;

			while (pTmpBucket1)
			{
				const IFXCID* pCID = pTmpBucket1->pCompDesc->pComponentId;
				printf("%.8x ", pCID->A);
				printf("%.4x ", pCID->B);
				printf("%.4x ", pCID->C);
				U32 i;
				for( i = 0; i < 8; ++i ) printf("%.2x ", pCID->D[i]);

				//printf("\t%.8x", pTmpBucket1->pCompDesc->pFactoryFunction );
				//printf("\t%.8x", pTmpBucket1->pCompDesc->pPluginProxy );

				pTmpBucket1 = pTmpBucket1->pNext;
			}
			//printf("\n");
		}
		printf("\n\n");
	}
}

#endif

//***************************************************************************
//	Protected methods
//***************************************************************************


//***************************************************************************
//	Private methods
//***************************************************************************

CIFXGuidHashMap::IFXGUIDHashBucket* CIFXGuidHashMap::FindHashBucket( 
									const IFXGUID& rCompId, 
									IFXGUIDHashBucket** ppPrevBucket) const
{
	U32 uHashIndex = HashFunction(rCompId);

	IFXGUIDHashBucket* pRetValue	= NULL;
	IFXGUIDHashBucket* pHashBucket	= &m_pHashTable[uHashIndex];
	*ppPrevBucket					= NULL;

	// Have we used the header bucket yet?
	if (pHashBucket->pCompDesc)
	{
		// Compare GUID structure values:
		if (GUIDsEquivalent(*(pHashBucket->pCompDesc->pComponentId), rCompId))
		{
			pRetValue = pHashBucket;
		}
		else
		{	// Loop over buckets until we either null or we find the compID:
			do 
			{
				IFXASSERT (pHashBucket->pCompDesc);

				*ppPrevBucket = pHashBucket;
				pHashBucket   = pHashBucket->pNext;
			}
			while (pHashBucket && 
				   !GUIDsEquivalent( *(pHashBucket->pCompDesc->pComponentId),
									rCompId ) );

			pRetValue = pHashBucket;
		}
	}
	else
    {
		pRetValue = pHashBucket;
    }

	return pRetValue;
}


BOOL CIFXGuidHashMap::GUIDsEquivalent ( const IFXCID& compID1, 
									    const IFXCID& compID2 ) const
{
	return ((compID1.A == compID2.A) &&
			(compID1.B == compID2.B) &&
			(compID1.C == compID2.C) &&
			(compID1.D[0] == compID2.D[0]) &&
			(compID1.D[1] == compID2.D[1]) &&
			(compID1.D[2] == compID2.D[2]) &&
			(compID1.D[3] == compID2.D[3]) &&
			(compID1.D[4] == compID2.D[4]) &&
			(compID1.D[5] == compID2.D[5]) &&
			(compID1.D[6] == compID2.D[6]) &&
			(compID1.D[7] == compID2.D[7]) );
}


U32 CIFXGuidHashMap::HashFunction ( const IFXCID& compID ) const
{
	U32 hashValue = 0;

	// Reformat the Guid:
	IFXHashableGUID  hashableGUID;
	IFXHashableGUID* pTmp;
	pTmp			= (IFXHashableGUID *) &compID;
	hashableGUID	= *pTmp;

	// Hokey mod bashed hash function for 128 bit GUIDs:
	hashValue = (hashableGUID.g1 +
		hashableGUID.g2  +
		hashableGUID.g3  +
		hashableGUID.g4) % m_uHashTableSize;

	return hashValue;
}


//***************************************************************************
//	Global functions
//***************************************************************************


//***************************************************************************
//	Local functions
//***************************************************************************


