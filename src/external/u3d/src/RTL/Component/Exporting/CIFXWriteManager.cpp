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
	@file	CIFXWriteManager.cpp
			
			This module defines the CIFXWriteManager component.

	@todo: usage
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "CIFXWriteManager.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXBitStreamX.h"
#include "IFXMarkerX.h"
#include "IFXModifierChain.h"
#include "IFXExportingCIDs.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXCheckX.h"
#include "IFXBlockTypes.h"
#include "IFXModifier.h"

//---------------------------------------------------------------------------
//  CIFXWriteManager::CIFXWriteManager
//
//  This is the constructor.  It sets up the object's initial state.
//---------------------------------------------------------------------------
CIFXWriteManager::CIFXWriteManager() :
	m_uRefCount(0),
	IFXDEFINEMEMBER(m_pScenegraph),
	IFXDEFINEMEMBER(m_pCoreServices),
	IFXDEFINEMEMBER(m_pPriorityQueue),
	IFXDEFINEMEMBER(m_pBlockWriter),
	m_ExportOptions(IFXEXPORT_NONE)
{
	m_newObjectBlockType = 0x00000100;
	m_extCIDs.Clear();
}

//---------------------------------------------------------------------------
//  CIFXWriteManager::~CIFXWriteManager
//
//  This is the destructor.  It performs any necessary cleanup activities.
//---------------------------------------------------------------------------
CIFXWriteManager::~CIFXWriteManager()
{
	// Note:  smart pointer releases for
	//  m_pScenegraph
	//  m_pBlockWriter,
	//  m_pCoreServices,
	//  m_pPriorityQueue
	m_extCIDs.Clear();
}

//---------------------------------------------------------------------------
//  CIFXWriteManager_Factory
//
//  This is the CIFXWriteManager component factory function.  The
//  CIFXWriteManager component can be instaniated multiple times.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXWriteManager_Factory( IFXREFIID interfaceId,
								   void**    ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXWriteManager component.
		CIFXWriteManager *pComponent = new CIFXWriteManager;

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
}


//---------------------------------------------------------------------------
//  PUBLIC IFXUnknown::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXWriteManager::AddRef()
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  PUBLIC IFXUnknown::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXWriteManager::Release()
{
	if ( !(--m_uRefCount) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

//---------------------------------------------------------------------------
//  PUBLIC IFXUnknown::QueryInterface
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
IFXRESULT CIFXWriteManager::QueryInterface( IFXREFIID interfaceId,
										   void**    ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXWriteManager )
		{
			*ppInterface = (IFXWriteManager*) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
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

	return rc;
}

//---------------------------------------------------------------------------
//  PUBLIC IFXWriteManager::Initialize
//
//  Initialize the component
//---------------------------------------------------------------------------
IFXRESULT CIFXWriteManager::Initialize( IFXCoreServices *pCoreServices )
{
	IFXRESULT rc = IFX_OK;

	try
	{
		// check inputs
		if ( NULL == pCoreServices )
			IFXCHECKX( IFX_E_INVALID_POINTER );

		// Check to see if already initialized
		if ( NULL != m_pCoreServices )
			IFXCHECKX( IFX_E_ALREADY_INITIALIZED );

		// Store the core services pointer
		m_pCoreServices = pCoreServices;
		m_pCoreServices->AddRef();

		// get the scene graph
		IFXCHECKX( m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**) &m_pScenegraph ) );
	}

	catch ( IFXException e )
	{
		rc = e.GetIFXResult();
	}

	IFXRETURN( rc );
}

//---------------------------------------------------------------------------
//  PUBLIC IFXWriteManager::Write
//
//  This method is the main driver for the IFXWriteManager.
//---------------------------------------------------------------------------
IFXRESULT CIFXWriteManager::Write( IFXWriteBuffer* pWriteBuffer, IFXExportOptions eExportOptions )
{
	IFXRESULT rc = IFX_OK;
	IFXDECLARELOCAL(IFXWriteBufferX,pWriteBufferX);

	try
	{
		// check inputs
		if ( NULL == pWriteBuffer )
			IFXCHECKX( IFX_E_INVALID_POINTER );

		// check for initialization
		if ( NULL == m_pCoreServices )
			IFXCHECKX( IFX_E_NOT_INITIALIZED );

		// create and initialize a block writer
		IFXRELEASE( m_pBlockWriter );
		IFXCHECKX( IFXCreateComponent( CID_IFXBlockWriterX, IID_IFXBlockWriterX, (void**) &m_pBlockWriter ) );
		IFXCHECKX( pWriteBuffer->QueryInterface( IID_IFXWriteBufferX, (void**)&pWriteBufferX ) );
		m_pBlockWriter->InitializeX( *m_pCoreServices, *pWriteBufferX );

		// create a priority queue
		IFXRELEASE( m_pPriorityQueue );
		IFXCHECKX(IFXCreateComponent(CID_IFXBlockPriorityQueueX, IID_IFXDataBlockQueueX, (void**)&m_pPriorityQueue));

		m_ExportOptions = eExportOptions;

		// For all "selected" resources, create an appropriate encoder based on the resource
		// type and give the selected resource to that encoder. These encoders will immediately
		// encode their resource into datablock(s) and then give these blocks to the the priority
		// queue.
		if ( eExportOptions & IFXEXPORT_NODE_HIERARCHY )
		{
			PutPalette( IFXSceneGraph::NODE );
		}
		if ( eExportOptions & IFXEXPORT_GEOMETRY )
		{
			PutPalette( IFXSceneGraph::GENERATOR );
		}
		if ( eExportOptions & IFXEXPORT_TEXTURES )
		{
			PutPalette( IFXSceneGraph::TEXTURE );
		}
		if ( eExportOptions & IFXEXPORT_ANIMATION )
		{
			PutPalette( IFXSceneGraph::MOTION );
		}
		if ( eExportOptions & IFXEXPORT_SHADERS )
		{
			PutPalette( IFXSceneGraph::SHADER );
		}
		if ( eExportOptions & IFXEXPORT_MATERIALS )
		{
			PutPalette( IFXSceneGraph::MATERIAL );
		}
		if ( eExportOptions & IFXEXPORT_LIGHTS )
		{
			PutPalette( IFXSceneGraph::LIGHT );
		}
		if ( eExportOptions & IFXEXPORT_VIEWS )
		{
			PutPalette( IFXSceneGraph::VIEW );
		}
		if ( eExportOptions & IFXEXPORT_FILEREFERENCES ) 
		{
			PutPalette( IFXSceneGraph::FILE_REFERENCE );
		}

		// queue is fully loaded, so write contents to the write buffer
		WriteQueueX();

		m_ExportOptions = IFXEXPORT_NONE;

		// clean up
		IFXRELEASE( m_pPriorityQueue );
		IFXRELEASE( m_pBlockWriter );
	}
	catch(IFXException e)
	{
		rc = e.GetIFXResult();

		IFXRELEASE( m_pPriorityQueue );
		IFXRELEASE( m_pBlockWriter );
	}
	catch(...)
	{
		IFXRELEASE( m_pPriorityQueue );
		IFXRELEASE( m_pBlockWriter );

		throw;
	}

	IFXRETURN( rc );
}


IFXRESULT CIFXWriteManager::SetQualityFactor( U32       uQualityFactor,
											 IFXExportOptions  ePalettes )
{
	IFXRESULT rc = IFX_OK;

	try
	{
		if ( ePalettes & IFXEXPORT_ANIMATION )
		{
			SetQualityOnPalette( uQualityFactor, IFXSceneGraph::MOTION );
		}

		if ( ePalettes & IFXEXPORT_GEOMETRY )
		{
			SetQualityOnPalette( uQualityFactor, IFXSceneGraph::GENERATOR );
		}

		if ( ePalettes & IFXEXPORT_MATERIALS )
		{
			SetQualityOnPalette( uQualityFactor, IFXSceneGraph::MATERIAL );
		}

		if ( ePalettes & IFXEXPORT_NODE_HIERARCHY )
		{
			SetQualityOnPalette( uQualityFactor, IFXSceneGraph::NODE );
		}

		if ( ePalettes & IFXEXPORT_SHADERS )
		{
			SetQualityOnPalette( uQualityFactor, IFXSceneGraph::SHADER );
		}

		if ( ePalettes & IFXEXPORT_TEXTURES )
		{
			SetQualityOnPalette( uQualityFactor, IFXSceneGraph::TEXTURE );
		}
	}
	catch ( IFXException e )
	{
		rc = e.GetIFXResult();
	}
	catch ( ... )
	{
		throw;
	}

	IFXRETURN( rc );
}

void CIFXWriteManager::SetQualityOnPalette( U32             uQualityFactor,
										   IFXSceneGraph::EIFXPalette  ePalette )
{
	IFXRESULT rc = IFX_OK;
	IFXPalette* pPalette = NULL;
	IFXMarkerX* pResource = NULL;
	U32     uPaletteIndex = 0;

	try
	{
		if ( NULL == m_pScenegraph )
			IFXCHECKX( IFX_E_NOT_INITIALIZED );

		IFXCHECKX( m_pScenegraph->GetPalette( ePalette, &pPalette ) );

		// for each entry in the palette, set the quality factor
		IFXCHECKX( pPalette->First( &uPaletteIndex ) );

		while ( IFXSUCCESS(rc) )
		{
			if ( IFXSUCCESS(rc) )
				rc = pPalette->GetResourcePtr( uPaletteIndex, IID_IFXMarkerX, (void**)&pResource );

			if ( IFXSUCCESS(rc) && pResource )
				pResource->SetQualityFactorX( uQualityFactor );

			IFXRELEASE( pResource );

			if ( IFXSUCCESS(rc) || IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc )
				rc = pPalette->Next( &uPaletteIndex );
		}

		if ( IFX_E_PALETTE_INVALID_ENTRY == rc )
			rc = IFX_OK;

		IFXRELEASE( pPalette );
	}

	catch ( ... )
	{
		IFXRELEASE( pResource );
		IFXRELEASE( pPalette );

		throw;
	}
}


//***************************************************************************
//  Private methods
//***************************************************************************

/** 
	@todo	This function should be in a general utilities section, since the write
			manager isn't the only component that might want to know this.
*/
void CIFXWriteManager::ComponentIsMarkedForWrite( IFXUnknown* pInUnknown,
												 BOOL*     pbOutMarked )
{
	IFXMarker*  pMarker = NULL;

	try
	{
		// check inputs
		if ( NULL == pInUnknown || NULL == pbOutMarked )
			IFXCHECKX( IFX_E_INVALID_POINTER );

		IFXCHECKX( pInUnknown->QueryInterface( IID_IFXMarker, (void**)&pMarker ) );

		if ( pMarker )
		{
			IFXCHECKX( pMarker->Marked( pbOutMarked ) );
			if (m_ExportOptions & IFXEXPORT_FILEREFERENCES) {
				BOOL bExternalFlag;
				pMarker->GetExternalFlag(&bExternalFlag);
				*pbOutMarked &= !bExternalFlag;
			}
			IFXRELEASE( pMarker );
		}
	}

	catch ( ... )
	{
		IFXRELEASE( pMarker );
		throw;
	}
}

void CIFXWriteManager::HandleExtensionsX( IFXEncoderX *pEncoder, IFXDataBlockQueueX *pDBQueue )
{
	// check if encoder supports IFXExtensionEncoderX interface
	if( pEncoder )
	{
		IFXDECLARELOCAL( IFXExtensionEncoderX, pEncExt );
		if( IFXSUCCESS( pEncoder->QueryInterface( IID_IFXExtensionEncoderX, (void**)&pEncExt ) ) )
		{
			IFXExtensionData extData;
			pEncExt->GetExtensionDataX( extData );

			IFXCID cid = extData.m_cid;
			BOOL isRegistered = FALSE;

			U32 i, f, limit = m_extCIDs.GetNumberElements();

			// assume the array is collected
			for( f = 0; f < limit && !isRegistered; f++ )
			{
				if( cid == m_extCIDs.GetElement(f).m_cid )
					isRegistered = TRUE;
			}

			U32 totalBlocksCount = extData.m_continuationBlockTypesCount + 1;

			if( isRegistered )
			{
				// CID was found - this means we already put NODB to the file
				pEncExt->SetBlockTypesX( m_extCIDs.GetElement(f-1).m_blockTypes, totalBlocksCount );
			}
			else
			{
				// if CID wasn't found store data and say to extension encoder to write NODB
				IFXExtBlocktype &extBT = m_extCIDs.CreateNewElement();
				extBT.m_cid = cid;
				extBT.m_blocksCount = totalBlocksCount;

				// just in case
				if( extBT.m_blockTypes != NULL )
					delete extBT.m_blockTypes;

				extBT.m_blockTypes = new U32[totalBlocksCount];

				for( i = 0; i < totalBlocksCount; i++ )
					extBT.m_blockTypes[i] = m_newObjectBlockType++;

				pEncExt->SetBlockTypesX( extBT.m_blockTypes, totalBlocksCount );

				CreateNewObjectDeclarationBlockX( extBT.m_blockTypes, &extData, *pDBQueue );
			}
		}
	}
}

void CIFXWriteManager::CreateNewObjectDeclarationBlockX( U32 *blockTypes, IFXExtensionData *pExtData, 
														IFXDataBlockQueueX& rDataBlockQueue )
{
	IFXDECLARELOCAL( IFXDataBlockX, pDataBlock );
	IFXDECLARELOCAL( IFXBitStreamX, pBitStream );

	try
	{
		if( blockTypes == NULL || pExtData == NULL )
			IFXCHECKX( IFX_E_INVALID_POINTER );

		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream ) );
		pBitStream->WriteIFXStringX( pExtData->m_extensionName );

		// write modifier type ( NODE = 0, RESOURCE = 1, TEXTURE = 2 )
		pBitStream->WriteU32X( pExtData->m_modifierType );

		// write decoder CID
		IFXCID cid = pExtData->m_cid;

		pBitStream->WriteU32X( cid.A );
		pBitStream->WriteU16X( cid.B );
		pBitStream->WriteU16X( cid.C );

		U32 f;
		for( f = 0; f < 8; f++ )
		{
			pBitStream->WriteU8X( cid.D[f] );
		}

		// write new blocktype
		if( blockTypes[0] < 0x00000100 || blockTypes[0] > 0x00FFFFFF )
			IFXCHECKX( IFX_E_NOT_INITIALIZED );

		pBitStream->WriteU32X( blockTypes[0] );

		// write cont. blocks count and cont. blocktypes
		pBitStream->WriteU32X( pExtData->m_continuationBlockTypesCount );

		for( f = 0; f < pExtData->m_continuationBlockTypesCount; f++ )
		{
			if( blockTypes[f+1] < 0x00000100 || blockTypes[f+1] > 0x00FFFFFF )
				IFXCHECKX( IFX_E_NOT_INITIALIZED );
			pBitStream->WriteU32X( blockTypes[f+1] );
		}

		// vendor name
		pBitStream->WriteIFXStringX( pExtData->m_vendorName );

		// write urls count & urls data
		pBitStream->WriteU32X( pExtData->m_extensionURLCount );

		for( f = 0; f < pExtData->m_extensionURLCount; f++ )
		{
			pBitStream->WriteIFXStringX( pExtData->m_extensionURLs[f] );
		}

		pBitStream->WriteIFXStringX( pExtData->m_extensionInfo );

		// APPEND BLOCK

		// Get a data block from the bitstream
		pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX( BlockType_FileNewObjectTypeU3D );

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX(*pDataBlock);
	}
	catch ( ... )
	{
		throw;
	}
}

// retrieve datablocks from resources and give them to the priority queue
void CIFXWriteManager::PutPalette( IFXSceneGraph::EIFXPalette ePalette )
{
	IFXRESULT     rc = IFX_OK;
	IFXDECLARELOCAL(IFXPalette,pPalette);
	IFXDECLARELOCAL(IFXMarkerX,pObject);
	IFXDECLARELOCAL(IFXEncoderX,pEncoder);
	IFXDECLARELOCAL(IFXModifierChain,pModChain);
	IFXDECLARELOCAL(IFXModifier,pModifier);
	IFXDECLARELOCAL(IFXModifier,pModifierInChain);
	IFXDECLARELOCAL(IFXMarkerX,pModAsMarker);
	IFXDECLARELOCAL(IFXEncoderX,pModEncoder);

	if (IFXSceneGraph::FILE_REFERENCE == ePalette)
		IFXCHECKX( m_pCoreServices->GetFileReferencePalette( &pPalette ) );
	else
		IFXCHECKX( m_pScenegraph->GetPalette( ePalette, &pPalette ) );

	F64 units;
	IFXCHECKX(m_pCoreServices->GetUnits(units));

	U32 i = 0;
	IFXString sName;

	rc = pPalette->First( &i ); // get first palette entry

	while ( IFXSUCCESS(rc) )
	{
		IFXCHECKX( pPalette->GetName( i, &sName ) );

		rc = pPalette->GetResourcePtr( i, IID_IFXMarkerX, (void**)&pObject );

		if ( IFXSUCCESS(rc) )
		{
			BOOL bMarked = FALSE;
			if (IFXSceneGraph::FILE_REFERENCE == ePalette)
				bMarked = m_ExportOptions & IFXEXPORT_FILEREFERENCES;
			else
				ComponentIsMarkedForWrite( pObject, &bMarked );

			if ((i != 0)&& bMarked)  // Don't write the default object (index 0)
			{
				pObject->GetEncoderX( pEncoder ); // this also gives the object to the encoder
				if (pEncoder) {
					// attempt to get the modifier chain
					U32 uModCount = 0;
					pModChain = NULL;
					pModifier = NULL;
					rc = pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );
					if ( IFXSUCCESS(rc) && pModifier ) {
						rc = pModifier->GetModifierChain( &pModChain );
						if ( IFX_E_CANNOT_FIND == rc ) // it's okay if a modchain wasn't found...
							rc = IFX_E_UNSUPPORTED;
						if (IFXSUCCESS(rc))
							IFXCHECKX( pModChain->GetModifierCount( uModCount ) );
						if (uModCount <= 0)
							rc = IFX_E_UNSUPPORTED;
					}

					if (IFXSUCCESS(rc)) {
						BOOL done;
						IFXDECLARELOCAL(IFXDataBlockQueueX, pModifierDeclarationQueue);
						IFXDECLARELOCAL(IFXBlockWriterX, pBlockWriterX);
						IFXDECLARELOCAL(IFXBitStreamX, pBitStreamX);
						IFXDECLARELOCAL(IFXDataBlockX, pModChainBlockX);
						IFXDECLARELOCAL(IFXWriteBufferX, pWriteBufferX);

						IFXCHECKX(IFXCreateComponent(CID_IFXBlockPriorityQueueX, IID_IFXDataBlockQueueX, (void**)&pModifierDeclarationQueue));
						IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));

						// 1. Modifier chain name
						U32 uPosition = 0, length = 0;
						IFXString sModChainName = sName;
						pBitStreamX->WriteIFXStringX(sModChainName);
						IFXCHECKX( sModChainName.GetLengthU8( &length ) );

						uPosition += 2 + length;

						// 2. Modifier chain type
						U32 uChainType = (U32)-1;
						if (ePalette == IFXSceneGraph::NODE) uChainType = IFXModifierChain::NODE;
						if (ePalette == IFXSceneGraph::GENERATOR) uChainType = IFXModifierChain::RESOURCE;
						if (ePalette == IFXSceneGraph::TEXTURE) uChainType = IFXModifierChain::TEXTURE;
						pBitStreamX->WriteU32X(uChainType);
						uPosition += 4;

						// 3. Attributes
						U32 uAttributes = 0; /// @todo: Do actual query
						pBitStreamX->WriteU32X(uAttributes);
						uPosition += 4;
						if (uAttributes & 0x00000001) {
							// 3.1. Write bounding sphere information
							F32 tmp = 0;
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							uPosition += 4*4;
						}
						if (uAttributes & 0x00000002) {
							// 3.2. Write axis-aligned bounding box
							F32 tmp = 0;
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							pBitStreamX->WriteF32X(tmp);
							uPosition += 4*6;
						}

						// 4. Modifier chain padding
						U32 uPadding = (4-((2 + length) & 3)) & 3;
						uPosition += uPadding;
						while (uPadding-- > 0)
							pBitStreamX->WriteU8X(0);

						// 5. Modifier count (would be filled later)
						pBitStreamX->WriteU32X(0);
						uPosition += 4;

						pBitStreamX->GetDataBlockX(pModChainBlockX);
						IFXCHECKX(pModChainBlockX->QueryInterface(IID_IFXWriteBufferX, (void**)&pWriteBufferX));

						IFXCHECKX(IFXCreateComponent(CID_IFXBlockWriterX, IID_IFXBlockWriterX, (void**)&pBlockWriterX));
						pBlockWriterX->InitializeX(*m_pCoreServices, *pWriteBufferX, uPosition);

						// Encode object
						U32 uModifierCount = 0;

						HandleExtensionsX( pEncoder, m_pPriorityQueue );

						pEncoder->EncodeX( sName, *pModifierDeclarationQueue, units );

						// Split declaration and all other blocks to separate queues
						done = FALSE;
						while (!done) {
							IFXDECLARELOCAL(IFXDataBlockX, pBlockX);
							pModifierDeclarationQueue->GetNextBlockX(pBlockX, done);
							if (pBlockX != NULL) {
								U32 bt = 0;
								pBlockX->GetBlockTypeX(bt);
								if (bt == BlockType_FilePriorityUpdateU3D) continue;
								if (pBlockX->GetPriorityX() > 0)
									m_pPriorityQueue->AppendBlockX(*pBlockX);
								else {
									pBlockWriterX->WriteBlockX(*pBlockX);
									uModifierCount++;
								}
							}
						};

						// Encoder all modifiers from mod. chain
						U32 uModIndex;
						for (  uModIndex = 1; IFXSUCCESS(rc) && uModIndex < uModCount; uModIndex++ )
						{
							IFXCHECKX( pModChain->GetModifier( uModIndex, pModifierInChain ) );
							IFXCHECKX( pModifierInChain->QueryInterface( IID_IFXMarkerX, (void**)&pModAsMarker ) );

							pModAsMarker->GetEncoderX( pModEncoder );
							if (pModEncoder) {
								IFXDECLARELOCAL(IFXDataBlockX, pBlockX);
								pModEncoder->InitializeX( *m_pCoreServices );

								HandleExtensionsX( pModEncoder, pModifierDeclarationQueue );

								pModEncoder->EncodeX( sName, *pModifierDeclarationQueue, units );
								U32 bt = 0;
								do {
									IFXRELEASE(pBlockX);
									pModifierDeclarationQueue->GetNextBlockX(pBlockX, done);
									pBlockX->GetBlockTypeX(bt);
									if( bt == BlockType_FileNewObjectTypeU3D )
										m_pPriorityQueue->AppendBlockX(*pBlockX);

									if( bt != BlockType_FilePriorityUpdateU3D 
										&& bt != BlockType_FileNewObjectTypeU3D && !done )
										pBlockWriterX->WriteBlockX(*pBlockX);

								} while ( bt == BlockType_FilePriorityUpdateU3D 
									|| bt == BlockType_FileNewObjectTypeU3D || !done );

								pBlockWriterX->WriteBlockX(*pBlockX);
								uModifierCount++;
							}

							IFXRELEASE( pModifierInChain );
							IFXRELEASE( pModAsMarker );
							IFXRELEASE( pModEncoder );
						}

						/// Write actual mod. declaration blocks count
						SWAP32_IF_BIGENDIAN(uModifierCount);
						pWriteBufferX->WriteX((U8*)&uModifierCount, uPosition-4, 4);
						SWAP32_IF_BIGENDIAN(uModifierCount);
						pModChainBlockX->SetBlockTypeX(BlockType_FileModifierChainU3D);
						pModChainBlockX->SetPriorityX(0);
						m_pPriorityQueue->AppendBlockX(*pModChainBlockX);

					}
					else if ( IFX_E_UNSUPPORTED == rc ) 
					{
						// it's okay if the modifier interface wasn't supported
						HandleExtensionsX( pEncoder, m_pPriorityQueue );

						pEncoder->EncodeX( sName, *m_pPriorityQueue, units );
						rc = IFX_OK;
					}

					IFXRELEASE( pModifier );
					IFXRELEASE( pModChain );
					IFXRELEASE( pEncoder );
				}
			}
		}

		IFXRELEASE( pObject );

		if ( IFXSUCCESS(rc) || IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc )
			rc = pPalette->Next( &i ); // get next palette entry
	} // while ( IFXSUCCESS(rc) )

	if ( rc == IFX_E_PALETTE_INVALID_ENTRY )
		rc = IFX_OK;

	IFXRELEASE( pPalette );
}

void CIFXWriteManager::WriteQueueX()
{
	IFXDECLARELOCAL(IFXDataBlockX, pBlockX);
	BOOL done = FALSE;

	// get each block from the queue and write it out
	while (!done)
	{
		m_pPriorityQueue->GetNextBlockX(pBlockX, done);

		if (NULL != pBlockX) {
			// write the block to the BlockWriter
			// (even if we are "done" we still need to write out that last block)
			m_pBlockWriter->WriteBlockX(*pBlockX);
		}

		IFXRELEASE(pBlockX);
	}
}
