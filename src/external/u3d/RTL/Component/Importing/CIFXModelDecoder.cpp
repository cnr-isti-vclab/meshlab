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
	@file	CIFXModelDecoder.cpp

			Implementation of the CIFXModelDecoder.
			The CIFXModelDecoder is used by the CIFXLoadManager to load
			model nodes into the scenegraph. CIFXModelDecoder exposes the
			IFXDecoderX interface to the	CIFXLoadManager for this purpose.
*/

#include "IFXCoreCIDs.h"
#include "IFXModel.h"
#include "CIFXModelDecoder.h"
#include "IFXBlockTypes.h"
#include "IFXModifierChain.h"
#include "IFXCheckX.h"

// Constructor
CIFXModelDecoder::CIFXModelDecoder()
{
}

// Destructor
CIFXModelDecoder::~CIFXModelDecoder()
{
}

// IFXUnknown
U32 CIFXModelDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXModelDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXModelDecoder::QueryInterface( IFXREFIID	interfaceId,
										    void**		ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = ( IFXDecoderX* ) this;
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
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

// IFXDecoderX
void CIFXModelDecoder::InitializeX(const IFXLoadConfig &lc)
{
	CIFXNodeBaseDecoder::InitializeX(lc);
}

void CIFXModelDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXModel);
}

void CIFXModelDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	CheckInitializedX();

	// For each data block in the list
	BOOL bDone = FALSE;
	while ( IFX_OK == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if (pDataBlockX) {
			// 1 - 4. common node data
			if (CommonNodeReadU3DX(*pDataBlockX) == FALSE) continue;

			// Get the IFXModel interface from the node
			IFXDECLARELOCAL(IFXModel,pModelNode);
			IFXCHECKX(m_pNode->QueryInterface(IID_IFXModel,(void**)&pModelNode));

			// 5. Generator name (IFXString)
			IFXString stringGeneratorName;
			m_pBitStreamX->ReadIFXStringX(stringGeneratorName);
			IFXDECLARELOCAL(IFXNameMap, pNameMap);
			m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
			IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::GENERATOR, stringGeneratorName));

			// Get the model resource palette
			IFXDECLARELOCAL(IFXPalette,pGeneratorPalette);
			IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR, &pGeneratorPalette ));

			// find or create a entry in the generator palette corresponding to the
			// generator name:
			U32 uGeneratorId = 0;
			if ( !stringGeneratorName.IsEmpty()) {
				IFXRESULT iFindResult = pGeneratorPalette->Find( &stringGeneratorName, &uGeneratorId);
				if(IFXFAILURE(iFindResult)) {
					IFXCHECKX(pGeneratorPalette->Add( &stringGeneratorName, &uGeneratorId));
				}
			}

			// Link the model node to the palette entry:
			IFXDECLARELOCAL(IFXResourceClient,pResourceClient);
			IFXCHECKX(pModelNode->QueryInterface( IID_IFXResourceClient, (void**)&pResourceClient ));
			IFXCHECKX(pResourceClient->SetResourceIndex( uGeneratorId ));

  			// 7. Model attributes (U32)
			U32 uModelAttributes = 0;
			m_pBitStreamX->ReadU32X(uModelAttributes);
			pModelNode->SetVisibility( uModelAttributes & 3 );
			// 0=None, 1=Front, 2=Back, 3=FrontAndBack
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXModelDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXModelDecoder *pComponent = new CIFXModelDecoder;

		if ( pComponent ) {
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}
