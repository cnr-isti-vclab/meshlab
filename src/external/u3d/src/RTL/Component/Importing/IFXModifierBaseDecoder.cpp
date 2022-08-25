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
	@note	IFXModifierBaseDecoder.cpp

			Implementation of the IFXModifierBaseDecoder.
			The IFXModifierBaseDecoder contains common modifier decoding
			functionality that  is used by the individual specific modifier
			(and generator) decoders (e.g. CIFXGlyphGeneratorDecoder).

	@note	This class is intended to be used as an abstract base class
			for various types of modifier decoders. As such, both the
			constructor and destructor are declared as protected members.
			This does, in itself, suffice in keeping a stand-alone instance
			of this class from being created. Note that the destructor is
			also declared as pure virtual to further enforce the abstract
			nature of this class - but the destructor does still have an
			implementation.
*/

#include "IFXModifierBaseDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXPalette.h"
#include "IFXCheckX.h"
#include "IFXModifierChain.h"
#include "IFXBlockTypes.h"
#include "IFXModifier.h"
#include "IFXNameMap.h"
#include "IFXBitStreamX.h"

// Constructor
IFXModifierBaseDecoder::IFXModifierBaseDecoder() :
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pDataBlockQueueX),
IFXDEFINEMEMBER(m_pObject)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
}

// Destructor
IFXModifierBaseDecoder::~IFXModifierBaseDecoder()
{
}

void IFXModifierBaseDecoder::InitializeX(const IFXLoadConfig &rLoadConfig)
{
	// Initialize the data block queue
	IFXRELEASE(m_pDataBlockQueueX);
	IFXCHECKX(IFXCreateComponent( 
						CID_IFXDataBlockQueueX, 
						IID_IFXDataBlockQueueX, 
						(void**)&m_pDataBlockQueueX ));

	// Store the core services pointer
	rLoadConfig.m_pCoreServices->AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = rLoadConfig.m_pCoreServices;
	m_uLoadId = rLoadConfig.m_loadId;
	m_bExternal = rLoadConfig.m_external;
	m_ePalette = rLoadConfig.m_palette;
}

void IFXModifierBaseDecoder::ProcessChainX( IFXDataBlockX &rDataBlockX )
{
	U32 resourceID;

	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph( 
									IID_IFXSceneGraph, 
									(void**)&pSceneGraph ));

	IFXDECLARELOCAL(IFXPalette,pSGPalette);
	IFXCHECKX(pSceneGraph->GetPalette( m_ePalette, &pSGPalette ));

	IFXRESULT iResultPaletteFind = 
		pSGPalette->Find( &m_stringObjectName, &resourceID );

	if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) 
	{
		IFXCHECKX(pSGPalette->Add( &m_stringObjectName, &resourceID ));
	}

	// If this is a chain object
	if (m_uChainPosition > 0) 
	{
		// get the modifier at the head of the chain
		IFXDECLARELOCAL(IFXModifier,pHeadModifier);
		U32 iResultGetResource = pSGPalette->GetResourcePtr(
													resourceID,
													IID_IFXModifier,
													(void**)&pHeadModifier);

		if(IFXFAILURE(iResultGetResource)) 
		{
			// If the head of the chain was missing, then create a placeholder
			if( IFXSceneGraph::GENERATOR == m_ePalette ) 
			{
				// Create a line set generator for generator palette
				IFXCHECKX(IFXCreateComponent( 
								CID_IFXAuthorLineSet, 
								IID_IFXModifier, 
								(void**)&pHeadModifier));
			} 
			else if ( IFXSceneGraph::NODE == m_ePalette ) 
			{
				// Create a group node for the node palette
				IFXCHECKX(IFXCreateComponent( 
								CID_IFXGroup, 
								IID_IFXModifier, 
								(void**)&pHeadModifier));
			} 
			else 
			{
				throw IFXException( IFX_E_INVALID_RANGE );
			}

			IFXCHECKX(pHeadModifier->SetSceneGraph(pSceneGraph));

			if (m_bExternal)
				pHeadModifier->SetExternalFlag(TRUE);

			IFXDECLARELOCAL(IFXUnknown,pPlaceholderAsUnknown);
			IFXCHECKX(pHeadModifier->QueryInterface( 
											IID_IFXUnknown, 
											(void**)&pPlaceholderAsUnknown ));
			IFXCHECKX(pSGPalette->SetResourcePtr( resourceID, pPlaceholderAsUnknown ));
		}

		// Get the modifier chain from the head modifier
		IFXDECLARELOCAL(IFXModifierChain,pModifierChain);
		IFXCHECKX(pHeadModifier->GetModifierChain(&pModifierChain));

		// Put the object (the new modifier) into the modifier chain
		IFXDECLARELOCAL(IFXModifier,pObjectAsModifier);
		IFXCHECKX(m_pObject->QueryInterface( 
									IID_IFXModifier, 
									(void**)&pObjectAsModifier ));
		IFXCHECKX(pModifierChain->AddModifier( 
									*pObjectAsModifier, 
									m_uChainPosition, 
									FALSE ));
	} 
	else 
	{
		IFXASSERT(IFXSceneGraph::GENERATOR == m_ePalette);
		IFXCHECKX(pSGPalette->SetResourcePtr(resourceID, m_pObject));
	}
}

void IFXModifierBaseDecoder::CreateObjectX(IFXDataBlockX &rDataBlockX, IFXREFCID cid)
{
	if (NULL == m_pObject) 
	{
		U32 blockType;
		rDataBlockX.GetBlockTypeX(blockType);

		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( 
							CID_IFXBitStreamX, 
							IID_IFXBitStreamX, 
							(void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		pBitStreamX->ReadIFXStringX( m_stringObjectName );
		pBitStreamX->ReadU32X( m_uChainPosition );

		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

		IFXCHECKX(IFXCreateComponent( cid, IID_IFXUnknown, (void**)&m_pObject ));
		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXCHECKX(m_pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);

		ProcessChainX(rDataBlockX);
	}
}
