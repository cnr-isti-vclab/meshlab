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
	@file	CIFXBTTHash.h

			The BTT hash is responsible for deleting all of the entries
			in the hash.  This includes the helper IFXHashU32Link structs and
			the IFXBTTHashEntry structs.  Pointers to IFXBTTHashEntry structs
			passed into the hash through the IFXBTTHash interface are NOT
			retained by the hash.  New structs are always created to retain
			the provided entry data.
*/


//-------------------------------------------------------------------
//  Includes
//-------------------------------------------------------------------

#include "IFXBlockTypes.h"
#include "CIFXBTTHash.h"
#include "IFXCoreCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXImportingCIDs.h"
#include "IFXDebug.h"

//-------------------------------------------------------------------
//-------------------------------------------------------------------

IFXBTTHash::IFXBTTHashEntry CIFXBTTHash::m_DefaultBlockTypes[] =
{
	// Nodes
	IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_NodeGroupU3D,
		IFXSceneGraph::NODE,
		0,
		&CID_IFXGroupDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_NodeModelU3D,
		IFXSceneGraph::NODE,
		0,
		&CID_IFXModelDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_NodeLightU3D,
		IFXSceneGraph::NODE,
		0,
		&CID_IFXLightDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_NodeViewU3D,
		IFXSceneGraph::NODE,
		0,
		&CID_IFXViewDecoder
		),
		// Generators
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorCLODMeshU3D,
		IFXSceneGraph::GENERATOR,
		0,
		&CID_IFXAuthorCLODDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorCLODBaseMeshContinuationU3D,
		IFXSceneGraph::GENERATOR,
		IFXBTTHash::CONTINUATION,
		&CID_IFXAuthorCLODDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorCLODProgressiveMeshContinuationU3D,
		IFXSceneGraph::GENERATOR,
		IFXBTTHash::CONTINUATION,
		&CID_IFXAuthorCLODDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorPointSetU3D,
		IFXSceneGraph::GENERATOR,
		0,
		&CID_IFXPointSetDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorPointSetContinuationU3D,
		IFXSceneGraph::GENERATOR,
		IFXBTTHash::CONTINUATION,
		&CID_IFXPointSetDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorLineSetU3D,
		IFXSceneGraph::GENERATOR,
		0,
		&CID_IFXLineSetDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_GeneratorLineSetContinuationU3D,
		IFXSceneGraph::GENERATOR,
		IFXBTTHash::CONTINUATION,
		&CID_IFXLineSetDecoder
		),
		// Modifiers
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_Modifier2DGlyphU3D,
		IFXSceneGraph::NODE,
		IFXBTTHash::CHAIN,
		&CID_IFXGlyphGeneratorDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ModifierSubdivisionU3D,
		IFXSceneGraph::NODE,
		IFXBTTHash::CHAIN,
		&CID_IFXSubdivisionModifierDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ModifierAnimationU3D,
		IFXSceneGraph::NODE,
		IFXBTTHash::CHAIN,
		&CID_IFXAnimationModifierDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ModifierBoneWeightsU3D,
		IFXSceneGraph::NODE,
		IFXBTTHash::CHAIN,
		&CID_IFXBoneWeightsModifierDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ModifierShadingU3D,
		IFXSceneGraph::NODE,
		IFXBTTHash::CHAIN,
		&CID_IFXShadingModifierDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ModifierCLODU3D,
		IFXSceneGraph::NODE,
		IFXBTTHash::CHAIN,
		&CID_IFXCLODModifierDecoder
		),
		// Resources
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceLightU3D,
		IFXSceneGraph::LIGHT,
		0,
		&CID_IFXLightResourceDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceViewU3D,
		IFXSceneGraph::VIEW,
		0,
		&CID_IFXViewResourceDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceLitTextureShaderU3D,
		IFXSceneGraph::SHADER,
		0,
		&CID_IFXShaderLitTextureDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceMaterialU3D,
		IFXSceneGraph::MATERIAL,
		0,
		&CID_IFXMaterialDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceTextureU3D,
		IFXSceneGraph::TEXTURE,
		0,
		&CID_IFXTextureDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceTextureContinuationU3D,
		IFXSceneGraph::TEXTURE,
		IFXBTTHash::CONTINUATION,
		&CID_IFXTextureDecoder
		),
		IFXBTTHash::IFXBTTHashEntry
		(
		BlockType_ResourceMotionU3D,
		IFXSceneGraph::MOTION,
		0,
		&CID_IFXMotionDecoder
		)
};


U32 CIFXBTTHash::m_uDefaultBlockTypeCount =
sizeof(CIFXBTTHash::m_DefaultBlockTypes) / sizeof(IFXBTTHash::IFXBTTHashEntry);

//---------------------------------------------------------------------------
//  CIFXBTTHash_Factory (non-singleton)
//
//  This is the CIFXBTTHash component factory function.  The
//  CIFXBTTHash component is NOT a singleton.  This function creates the
//  BTT Hash object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXBTTHash_Factory( IFXREFIID interfaceId,
							  void**    ppInterface )
{
	IFXRESULT rc = IFX_OK ;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXBTTHash *pComponent = new CIFXBTTHash();

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			rc = IFX_E_OUT_OF_MEMORY;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	return rc;
} // end CIFXBTTHash_Factory
//-------------------------------------------------------------------


//---------------------------------------------------------------------------
//  CIFXBTTHash::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXBTTHash::AddRef(void)
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXBTTHash::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXBTTHash::Release(void)
{
	// This implementation of Release()
	// prevents double destruction.  Basically -
	// the component is still fully functional (allowing
	// AddRef's and Release's) while it's being
	// destroyed.  Caveat: This prevents double destruction
	// only when the component is correctly refcounted.  You
	// can still cause double destruction by having extra Release()'s
	// but you won't cause it within a legitimate destruction.

	if(m_uRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
		return (--m_uRefCount);
}

//---------------------------------------------------------------------------
//  CIFXBTTHash::QueryInterface
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
IFXRESULT CIFXBTTHash::QueryInterface( IFXREFIID interfaceId,
									  void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXBTTHash )
			*ppInterface = ( IFXBTTHash* ) this;
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( rc ) )
			AddRef();
	}
	else
		rc = IFX_E_INVALID_POINTER;

	return rc;
} // end QueryInterface()
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE CIFXBTTHash::CIFXBTTHash()
{
	m_uRefCount     = 0;
	m_pHashTable    = NULL;
	m_uHashTableSize  = 0;
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE CIFXBTTHash::~CIFXBTTHash()
{
	// Delete the hash table:
	ClearHash();
} // end ~CIFXBTTHash
//-------------------------------------------------------------------


//-------------------------------------------------------------------
IFXINLINE void CIFXBTTHash::ClearHash()
{
	U32 h = 0 ;
	IFXHashU32Link* pTmpLink1 = NULL;
	IFXHashU32Link* pTmpLink2 = NULL;

	U32 uNumLinksDeleted = 0;
	U32 uNumEntriesDeleted = 0 ;

	// keep in mind that each entry in m_pHashTable is a lightweight
	// struct (IFXHashU32Link) which in turn owns a pointer to a
	// BTTHashEntry struct.  The lightweight structs will get
	// destroyed when m_pHashTable itself is deleted, but the
	// contained BTTHashEntry structs, as well as any IFXHashU32Links
	// that are 'linked' as a chain for a given hash entry, must be
	// manually deleted.  That's what the for() is doing.

	if (m_pHashTable)
	{
		// Walk the hash table:
		for (h = 0; h < m_uHashTableSize; h++)
		{
			if (m_pHashTable[h].pEntry)
			{
				// delete the contained BTT entry:
				delete m_pHashTable[h].pEntry ;
				uNumEntriesDeleted++ ;
			}

			// Walk the overflow chain if it exists:
			pTmpLink1 = m_pHashTable[h].pNext;

			while (pTmpLink1)
			{
				pTmpLink2 = pTmpLink1->pNext;

				if (pTmpLink1->pEntry)
				{
					// delete the contained BTT entry:
					delete pTmpLink1->pEntry ;
					uNumEntriesDeleted++ ;
				}

				// now delete the link struct:
				delete pTmpLink1;
				uNumLinksDeleted++;

				pTmpLink1 = pTmpLink2;
			} // traversing link chain for a given position in hash
		} // for (every position in the hash)
	} // if (m_pHashTable)

	delete m_pHashTable;
	// note: that deleted all of the base link stucts at each position
	// (element in the array)

	uNumLinksDeleted += m_uHashTableSize ;

	m_pHashTable = NULL;
	m_uHashTableSize = 0 ;

	return ;

} // end ClearHash()
//-------------------------------------------------------------------


//-------------------------------------------------------------------
IFXINLINE IFXRESULT CIFXBTTHash::Initialize()
{
	IFXRESULT rc = IFX_OK;
	//U32 i = 0;
	U32 uInitialHashSize = 2*m_uDefaultBlockTypeCount ;

	// to accommodate reinitialization, we must deal with any
	// existing hash:
	ClearHash();

	rc = Initialize( uInitialHashSize,        // hash size
		m_uDefaultBlockTypeCount,    // # of entries for init
		m_DefaultBlockTypes );    // initial entries

	return rc;
} // end Initialize()
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE IFXRESULT CIFXBTTHash::Initialize( U32  uHashSize,
											U32  numEntries,
											const IFXBTTHashEntry* pEntryList)
{
	IFXRESULT rc = IFX_OK;
	U32 i = 0;

	// to accommodate reinitialization, we must deal with any
	// existing hash:
	ClearHash();

	// only bail if uHashSize == 0.  It's ok if the hashmap isn't
	// initialized with a set of key:data pairs.
	if (0 == uHashSize)
		rc = IFX_E_NOT_INITIALIZED;

	// allocate the base link structs for the hash array:
	if (IFXSUCCESS(rc))
	{
		m_uHashTableSize = uHashSize;
		m_pHashTable = new IFXHashU32Link[m_uHashTableSize];
		if (!m_pHashTable)
			rc = IFX_E_OUT_OF_MEMORY ;
	}

	// Initialize all of the bucket overflow ptrs to NULL:
	if (IFXSUCCESS(rc))
	{
		for (i = 0; i < m_uHashTableSize; i++)
		{
			m_pHashTable[i].pEntry  = NULL;
			m_pHashTable[i].pNext = NULL;
		}
	}

	// Load the provided data into the hash table:
	if (IFXSUCCESS(rc))
	{
		if ( (numEntries > 0) && (NULL == pEntryList) )
			rc = IFX_E_INVALID_POINTER ;

		if (IFXSUCCESS(rc))
		{
			for (i = 0; (i < numEntries) && (IFXSUCCESS(rc)); i++)
				rc = Add(&pEntryList[i]);
		}
	}

	return rc;
} // end Initialize(...)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE U32 CIFXBTTHash::HashFunction (U32 uKey)
{
	U32 hashValue = 0;

	// lazy hash function for U32s:
	hashValue = (uKey) % m_uHashTableSize;

	return hashValue;
} // end HashFunction()
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE CIFXBTTHash::IFXHashU32Link*
CIFXBTTHash::FindHashLink( U32 uKey, IFXHashU32Link** ppPrevLink)
{
	IFXRESULT rc = IFX_OK ;

	U32 uHashIndex = 0 ;
	IFXHashU32Link* pLinkUsed = NULL ;
	IFXHashU32Link* pHashLink = NULL ;

	if (NULL == ppPrevLink)
		rc = IFX_E_INVALID_POINTER ;
	else
		*ppPrevLink = NULL;

	// First, hash the key:
	if (IFXSUCCESS(rc))
		uHashIndex = HashFunction(uKey);

	pHashLink = &m_pHashTable[uHashIndex];


	// Have we used the header bucket yet?
	if (pHashLink->pEntry)
	{
		// Is this key already in the hash?
		if (pHashLink->pEntry->blockType == uKey)
			pLinkUsed = pHashLink;

		else
		{ // Loop over buckets until we either null or we find the key:
			do
			{
				*ppPrevLink = pHashLink;
				pHashLink = pHashLink->pNext;
			}
			while (pHashLink && (pHashLink->pEntry) && (pHashLink->pEntry->blockType != uKey) );

			pLinkUsed = pHashLink;
		}
	}
	else  // the original hash entry was unused, so we'll use it
		pLinkUsed = pHashLink;

	return pLinkUsed;
} // end FindHashLink()
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE IFXRESULT CIFXBTTHash::Add( const IFXBTTHashEntry* pEntry)
{
	IFXRESULT rc = IFX_OK;
	IFXHashU32Link* pHashLink   = NULL;
	IFXHashU32Link* pPrvHashLink  = NULL;

	if ( (NULL == pEntry) || (NULL == m_pHashTable))
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
		pHashLink = FindHashLink(pEntry->blockType, &pPrvHashLink);

	// If it wasn't found, then create a new bucket and attach it to the table:
	if ((IFXSUCCESS(rc)) && (NULL == pHashLink))
	{
		// note: if pHashLink == NULL, then it's NOT ok if
		// pPrrvHashLink == NULL.  Each position in the hash is
		// populated with a link structure, and if that struct
		// hasn't been used yet, then it would come back as pHashLink.
		// If it's been used, then at the very least, pPrevHashLink
		// would be that base struct, and pHashLink would either be the end
		// of the chain, or NULL if there is no chain.
		if (NULL == pPrvHashLink)
			rc = IFX_E_UNDEFINED;
		else
		{
			pHashLink = new IFXHashU32Link;

			if (NULL == pHashLink)
				rc = IFX_E_OUT_OF_MEMORY ;
			else
			{
				// hook up the new hash link to the chain for the
				// given hash position:
				pHashLink->pNext  = pPrvHashLink->pNext;
				pHashLink->pEntry = NULL;
				pPrvHashLink->pNext = pHashLink;
			}
		}
	}

	if (IFXSUCCESS(rc))
	{
		// Note: if this assert fires, then someone is adding a
		// U32 that is already in the Hash table.   Currently -- the behavior
		// is to assert, but do the Add() anyway.

		/// @todo: it might be desirable to have a better 'official' reporting mechanism
		// for when duplicate blocktypes get registered in the hash,  i.e. a warning
		// return code of IFX_W_DUPLICATE_BLOCKTYPES or something.

		if (pHashLink->pEntry)
		{
			//const U32 uKeyExisting  = pHashLink->pEntry->blockType ;
			//const U32 uKeyNew   = pEntry->blockType ;
			IFXASSERT (0);
		}

		// note: we're not keeping any BTTHashEntry pointers
		// handed to us, we're alwasy allocating our own
		// has entry structs.  If this proves problematic
		// (because the entry struct becomes some huge beast
		// or something), then it's easy enough to just do
		// pointer adoption instead of struct allocation.  In
		// that case, the hash entry should probably become a
		// first class component and be refcounted.
		pHashLink->pEntry = new IFXBTTHashEntry ;

		if (NULL == pHashLink->pEntry)
			rc = IFX_E_OUT_OF_MEMORY;
		else
		{
			pHashLink->pEntry->blockType  = pEntry->blockType ;
			pHashLink->pEntry->paletteType  = pEntry->paletteType ;
			pHashLink->pEntry->flags    = pEntry->flags ;
			pHashLink->pEntry->decoderCID = pEntry->decoderCID ;
		}
	}

	return rc;
} // end Add()
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE IFXRESULT CIFXBTTHash::Delete (U32 uKey)
{
	IFXRESULT rc = IFX_OK;
	IFXHashU32Link* pHashLink   = NULL;
	IFXHashU32Link* pPrvHashLink  = NULL;

	if (!m_pHashTable)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS (rc))
		pHashLink = FindHashLink(uKey, &pPrvHashLink);

	if (IFXSUCCESS(rc))
	{
		// If we found it, then delete it:
		if (pHashLink && pHashLink->pEntry && (uKey == pHashLink->pEntry->blockType))
		{
			// Is this the first hash link or further down the overflow chain?
			if (pPrvHashLink)
			{ // it's not the first link, so remove it from the chain:
				pPrvHashLink->pNext = pHashLink->pNext;

				// delete the contained entry:
				delete pHashLink->pEntry ;

				// delete the link struct:
				delete pHashLink;
				pHashLink = NULL;
			}

			else // it is the first link
			{
				// Is there an overflow chain?
				if (pHashLink->pNext)
				{
					IFXHashU32Link* pTmpLink = pHashLink->pNext;

					// Bubble the 1st overflow link back:
					delete pHashLink->pEntry ;
					pHashLink->pEntry = pTmpLink->pEntry ;
					pHashLink->pNext  = pTmpLink->pNext ;
					delete pTmpLink ;
					pTmpLink = NULL ;
				}

				else // No overflow links, just zero out the link:
				{
					delete pHashLink->pEntry ;
					pHashLink->pNext = NULL ;
				}
			} // end else (it's the first link)
		}
		else
			rc = IFX_E_HASHMAPU32_ID_NOT_FOUND;
	}

	return rc;
} // end Delete()
//-------------------------------------------------------------------

//-------------------------------------------------------------------
IFXINLINE IFXRESULT CIFXBTTHash::Find ( U32 uKey,
									   IFXBTTHashEntry* pEntry)
{
	IFXRESULT rc = IFX_OK;
	IFXHashU32Link* pHashLink   = NULL;
	IFXHashU32Link* pPrvHashLink  = NULL;

	if (!pEntry || !m_pHashTable)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS (rc))
		pHashLink = FindHashLink(uKey, &pPrvHashLink);

	if (IFXSUCCESS(rc))
	{
		// note: this function doesn't 'hand back' an IFXBTTHashEntry,
		// but rather copies the contents of the found hash entry to
		// the struct provided.  This helps insulate the hash from
		// unintended or malicious changes to an entry outside of the
		// official Add(), Delete() methods.  As always, this is
		// appropriate so long as the IFXBTTHashEntry stays lightweight.
		// If it balloons, other policies should be investigated
		// to avoid the 'copy' while still maintaining protection.

		if (pHashLink && pHashLink->pEntry && (uKey == pHashLink->pEntry->blockType))
		{
			//      pHashLink->pEntry->blockType  = pEntry->blockType ;
			//      pHashLink->pEntry->paletteType  = pEntry->paletteType ;
			//      pHashLink->pEntry->flags    = pEntry->flags ;
			//      pHashLink->pEntry->objectCID  = pEntry->objectCID ;
			//      pHashLink->pEntry->decoderCID = pEntry->decoderCID ;

			pEntry->blockType = pHashLink->pEntry->blockType;
			pEntry->paletteType = pHashLink->pEntry->paletteType;
			pEntry->flags   = pHashLink->pEntry->flags;
			pEntry->decoderCID  = pHashLink->pEntry->decoderCID;
		}

		else
			rc = IFX_E_HASHMAPU32_ID_NOT_FOUND;
	}

	return rc;
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
