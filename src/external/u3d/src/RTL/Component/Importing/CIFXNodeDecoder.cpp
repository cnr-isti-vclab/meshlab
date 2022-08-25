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
	@file	CIFXNodeBaseDecoder.cpp

			Implementation of the CIFXNodeBaseDecoder.
			The CIFXNodeBaseDecoder contains common node decoding functionality that
			is used by the individual specific node decoders (e.g. CIFXLightDecoder).

	@note	This class is intended to be used as an abstract base class
			for various types of node decoders. As such, both the
			constructor and destructor are declared as protected members.
			This does, in itself, suffice in keeping a stand-alone instance
			of this class from being created. Note that the destructor is
			also declared as pure virtual to further enforce the abstract
			nature of this class - but the destructor does still have an
			implementation.
*/

#include "IFXCoreCIDs.h"
#include "IFXModel.h"
#include "IFXView.h"
#include "CIFXNodeDecoder.h"
#include "IFXMatrix4x4.h"
#include "IFXCheckX.h"
#include "IFXBlockTypes.h"

// Constructor
CIFXNodeBaseDecoder::CIFXNodeBaseDecoder() :
IFXDEFINEMEMBER(m_pBitStreamX),
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pDataBlockQueueX),
IFXDEFINEMEMBER(m_pNode),
IFXDEFINEMEMBER(m_pSceneGraph)
{
	m_uRefCount = 0;
	m_uLoadId = 0;

	m_unitScale = 1.0f;
}

// Destructor
CIFXNodeBaseDecoder::~CIFXNodeBaseDecoder()
{
}

BOOL CIFXNodeBaseDecoder::CommonNodeReadU3DX( IFXDataBlockX &rDataBlockX )
{
	IFXDECLARELOCAL(IFXSceneGraph, pSG);
	if (IFXFAILURE(m_pNode->GetSceneGraph(&pSG)))
		return FALSE;

	// The common node operations are
	// A. Validiate the input data block
	// B. Get the scene graph
	// C. Get the node palette
	// D. Create a bitstream component and initialize it to decode the block
	// E. Read the common node fields

	// The common node fields are
	// 1. Node name (IFXString)
	// 2. Parent node count (U32)
	// 2.1. Parent node name (IFXString)
	// 2.2. Local matrix (16*F32)

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pNode->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	// A. Validiate the input data block
	CheckInitializedX();

	// B. Get the scene graph
	IFXRELEASE( m_pSceneGraph );
	IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&m_pSceneGraph ));

	// C. Get the node palette
	IFXDECLARELOCAL(IFXPalette,pNodePalette);
	IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pNodePalette ));

	// D. Create a bitstream component and initialize it to decode the block
	IFXRELEASE( m_pBitStreamX );
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStreamX ));
	m_pBitStreamX->SetDataBlockX( rDataBlockX );

	// E. Read the common node fields

	// 1. Node name (IFXString)
	IFXString stringNodeName;
	m_pBitStreamX->ReadIFXStringX( stringNodeName );

	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, stringNodeName));

	// 2. Parent node count (U32)
	U32 nParentCount = 0;
	m_pBitStreamX->ReadU32X(nParentCount);

	U32 i;
	for ( i = 0; i < nParentCount; i++) {
		// 2.1. Parent node name (IFXString)
		IFXString stringParentNodeName;
		m_pBitStreamX->ReadIFXStringX( stringParentNodeName );

		IFXString sWorldName;
		IFXCHECKX(pNameMap->GetWorldAlias(m_uLoadId, sWorldName));
		if (stringParentNodeName.IsEmpty())
			stringParentNodeName = sWorldName;
		else if (stringParentNodeName != sWorldName) /// @todo: leave only else
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, stringParentNodeName));

		U32 uParentId = 0;
		// Look for the parent node in the node palette
#ifdef _DEBUG
		IFXRESULT iFindResult = 
#endif
			pNodePalette->Find( &stringParentNodeName, &uParentId );
		IFXASSERT(IFXSUCCESS(iFindResult));

		// Link the node to the parent node
		IFXDECLARELOCAL(IFXNode,pParentNode);
		IFXCHECKX(pNodePalette->GetResourcePtr( uParentId, IID_IFXNode, (void**)&pParentNode ));
		IFXCHECKX(m_pNode->AddParent( pParentNode ));

		// 2.2. Local matrix (16*F32) (aka Position Matrix)
		IFXMatrix4x4 matrix;

		U32 j;
		for ( j = 0; j < 16; j++)
			m_pBitStreamX->ReadF32X(matrix[j]);

		IFXCHECKX(m_pNode->SetMatrix(i, &matrix));
	}
	return TRUE;
}

void CIFXNodeBaseDecoder::InitializeX(const IFXLoadConfig &lc)
{
	// Initialize the data block queue
	IFXRELEASE(m_pDataBlockQueueX);
	IFXCHECKX(IFXCreateComponent( CID_IFXDataBlockQueueX, IID_IFXDataBlockQueueX, (void**)&m_pDataBlockQueueX ));

	// Store the core services pointer
	lc.m_pCoreServices->AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = lc.m_pCoreServices;
	m_uLoadId = lc.m_loadId;
	m_bExternal = lc.m_external;
	if( lc.m_units > 0.0f )
		m_unitScale = lc.m_units;
}

void CIFXNodeBaseDecoder::CheckInitializedX( )
{
	if ( NULL == m_pCoreServices || NULL == m_pNode ) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}
}

void CIFXNodeBaseDecoder::CreateObjectX(IFXDataBlockX &rDataBlockX, IFXREFCID cid)
{
	if (NULL == m_pNode) {
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);

		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXCHECKX(IFXCreateComponent( cid, IID_IFXUnknown, (void**)&pObject ));
		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXCHECKX(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);
		pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);
		IFXCHECKX(pObject->QueryInterface( IID_IFXNode, (void**)&m_pNode));

		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		IFXString sBlockName;
		pBitStreamX->ReadIFXStringX( sBlockName );
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, sBlockName));

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pSGPalette ));

		U32 uResourceID;
		IFXRESULT iResultPaletteFind = pSGPalette->Find( &sBlockName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) {
			IFXCHECKX(pSGPalette->Add( &sBlockName, &uResourceID ));
		}

		ReplaceNodeX(uResourceID,*m_pNode);
		AddParentNodePlaceholderX(rDataBlockX);
	}
}

void CIFXNodeBaseDecoder::AddParentNodePlaceholderX( IFXDataBlockX &rDataBlockX )
{
	IFXDECLARELOCAL(IFXSceneGraph,pScenegraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pScenegraph ));

	IFXDECLARELOCAL(IFXPalette,pNodePalette);
	IFXCHECKX(pScenegraph->GetPalette( IFXSceneGraph::NODE, &pNodePalette ));

	// Note: If AddParentNodePlaceholder() encounters an existing node with the same
	// name as the placeholder node to be added, then we know either that's the real
	// parent node or a placeholder and thus AddParentNodePlaceholder will do nothing.
	// Note that linkage between parents and children occurs at transfer time.

	// get the name of a potential parent node
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	{
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	IFXString sParentNodeName;
	U32 uParentCount;
	// read the node name and ignore it
	pBitStreamX->ReadIFXStringX( sParentNodeName );
	pBitStreamX->ReadU32X(uParentCount);

	U32 i;
	for ( i = 0; i < uParentCount; i++) {
		// now read the parent node name and store it
		pBitStreamX->ReadIFXStringX( sParentNodeName );
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		IFXCHECKX(m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap));
		IFXString sWorldName;
		IFXCHECKX(pNameMap->GetWorldAlias(m_uLoadId, sWorldName));
		if (sParentNodeName.IsEmpty())
			sParentNodeName = sWorldName;
		else if (sParentNodeName != sWorldName) /// @todo: leave only else
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, sParentNodeName));

		// attempt to find parent node entry in node palette
		U32 uResourceId = 0;
		IFXRESULT iFindResult = pNodePalette->Find( &sParentNodeName, &uResourceId );

		// if parent node entry not found, then add it
		if ( IFX_E_CANNOT_FIND == iFindResult )
			IFXCHECKX(pNodePalette->Add( &sParentNodeName, &uResourceId ));

		// if the palette entry was found or, alternately, it was added, attempt now to find the resource
		IFXDECLARELOCAL(IFXNode,pParentNode);
		IFXRESULT iResultGet = pNodePalette->GetResourcePtr( uResourceId, IID_IFXNode, (void**)&pParentNode );

		// if placeholder node is not found, then add the placeholder node
		if (IFX_E_PALETTE_NULL_RESOURCE_POINTER == iResultGet) {
			IFXCHECKX(IFXCreateComponent( CID_IFXGroup, IID_IFXNode, (void**)&pParentNode ));

			IFXCHECKX(pParentNode->SetSceneGraph( pScenegraph ));
			pParentNode->SetExternalFlag(m_bExternal);

			IFXDECLARELOCAL(IFXUnknown,pUnknown);
			IFXCHECKX(pParentNode->QueryInterface( IID_IFXUnknown, (void**)&pUnknown ));
			IFXCHECKX(pNodePalette->SetResourcePtr( uResourceId, pUnknown ));
		}

		F32 tmp;
		U32 j;
		for ( j = 0; j < 16; j++)
			pBitStreamX->ReadF32X(tmp);
	}
}

void CIFXNodeBaseDecoder::ReplaceNodeX( U32 uResourceId, IFXUnknown &rUnknownNode )
{
	// Get new node interface
	IFXDECLARELOCAL(IFXNode,pNewNode);
	IFXCHECKX(rUnknownNode.QueryInterface(IID_IFXNode,(void**)&pNewNode));

	// get node palette
	IFXDECLARELOCAL(IFXSceneGraph,pScenegraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pScenegraph ));
	IFXDECLARELOCAL(IFXPalette,pNodePalette);
	IFXCHECKX(pScenegraph->GetPalette( IFXSceneGraph::NODE, &pNodePalette ));

	// attempt to get old node from uResourceId
	IFXDECLARELOCAL(IFXNode,pOldNode);
	IFXRESULT iResultGetResource = pNodePalette->GetResourcePtr( uResourceId, IID_IFXNode, (void**)&pOldNode );

	// If we got the old node
	if(IFXSUCCESS(iResultGetResource)) {
		// transfer children from old node to new node
		IFXNode* pChildNR = NULL;
		U32 count, i;
		pChildNR = pOldNode->GetChildNR( 0 );
		while ( NULL != pChildNR) 
		{
			count = pChildNR->GetNumberOfParents();
			for (i = 0; i < count; i++) 
			{
				if (pOldNode == pChildNR->GetParentNR(i))
					IFXCHECKX(pChildNR->RemoveParent(i));
			}
			IFXCHECKX(pChildNR->AddParent( pNewNode ));
			pChildNR = pOldNode->GetChildNR( 0 );
		}

		// Prune the old node
		IFXCHECKX(pOldNode->Prune());

		/**
			@todo	What do we do about the old node's modifiers? My guess is that we
					would not copy them to the new node unless they are of the same 
					type. Consider the replacement of a model node with a light node -
					it would not make sense to copy model node modifiers to a 
					light node....
		*/

		// here is a start...
		IFXCHECKX(pOldNode->SetSceneGraph( NULL ));
	}

	// set palette to refer to new node
	IFXCHECKX(pNodePalette->SetResourcePtr( uResourceId, &rUnknownNode ));
}
