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
	@file	CIFXLightDecoder.cpp

			Implementation of the CIFXLightDecoder.
			The CIFXLightDecoder is used by the CIFXLoadManager to load
			light nodes into the scenegraph. CIFXLightDecoder exposes the
			IFXDecoderX interface to the CIFXLoadManager for this purpose.
*/

#include "IFXCoreCIDs.h"
#include "CIFXLightDecoder.h"
#include "IFXLight.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"

// Constructor
CIFXLightDecoder::CIFXLightDecoder()
{
}

// Destructor
CIFXLightDecoder::~CIFXLightDecoder()
{
}

// IFXUnknown
U32 CIFXLightDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXLightDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXLightDecoder::QueryInterface( IFXREFIID	interfaceId,
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
void CIFXLightDecoder::InitializeX(const IFXLoadConfig &lc)
{
	CIFXNodeBaseDecoder::InitializeX(lc);
}

void CIFXLightDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	// create light node
	CreateObjectX(rDataBlockX, CID_IFXLight);
}

void CIFXLightDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	CheckInitializedX();

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) 
	{
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX ) 
		{
			U32 uBlockType = 0;
			pDataBlockX->GetBlockTypeX( uBlockType );

			// read U3D light node //
			if ( uBlockType == BlockType_NodeLightU3D )
			{
				if (CommonNodeReadU3DX(*pDataBlockX) == FALSE) continue;

				if( m_unitScale > 0.0f && m_unitScale != 1.0f )
				{
					IFXMatrix4x4 matrix;

					U32 i;
					for( i = 0; i < m_pNode->GetNumberOfParents(); ++i )
					{
						matrix = m_pNode->GetMatrix(i);

						// Apply units scaling factor to TM's translation and scale.
						matrix[0] *= (F32)m_unitScale;
						matrix[5] *= (F32)m_unitScale;
						matrix[10] *= (F32)m_unitScale;
						matrix[12] *= (F32)m_unitScale;
						matrix[13] *= (F32)m_unitScale;
						matrix[14] *= (F32)m_unitScale;

						IFXCHECKX(m_pNode->SetMatrix(i, &matrix));
					}
				}

				// Read Light Resource name
				IFXString sResName;
				m_pBitStreamX->ReadIFXStringX(sResName);

				IFXDECLARELOCAL(IFXNameMap, pNameMap);
				m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
				IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::LIGHT, sResName));

				// try to find light resource

				// Get the light resource palette
				IFXDECLARELOCAL( IFXPalette, pLightResourcePalette );
				IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
				IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
				IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette ));

				U32 uLightResourceId = 0;
				// Look for the light resource in the light resource palette
				IFXRESULT rc = pLightResourcePalette->Find( &sResName, &uLightResourceId );

				// If the light resource was not found
				if (IFXFAILURE(rc))	
				{
					IFXLightResource* pLightResource = NULL;
					// create a light resource and add the light resource to the palette
					rc = pLightResourcePalette->Add( &sResName, &uLightResourceId );

					if (IFXSUCCESS(rc)) 
					{
						IFXCHECKX( IFXCreateComponent( CID_IFXLightResource, IID_IFXLightResource, (void**)&pLightResource ) );
						IFXDECLARELOCAL( IFXUnknown, pUnk );
						IFXCHECKX( pLightResource->QueryInterface( IID_IFXUnknown, (void**)&pUnk ) );

						IFXDECLARELOCAL( IFXMarker, pMarker );
						IFXCHECKX(pUnk->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
						IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
						IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
						IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
						pMarker->SetExternalFlag(m_bExternal);
						pMarker->SetPriority(pDataBlockX->GetPriorityX(), FALSE, FALSE);

						IFXCHECKX( pLightResourcePalette->SetResourcePtr( uLightResourceId, pUnk ) );
						IFXRELEASE(pUnk);
					}
					IFXRELEASE(pLightResource);
				}

				// Link the light node to the light resource pallete entry
				IFXDECLARELOCAL( IFXLight, pLight );
				IFXCHECKX( m_pNode->QueryInterface(IID_IFXLight,(void**)&pLight) );
				IFXCHECKX( pLight->SetLightResourceID(uLightResourceId) );
			}
			else
			{
				throw IFXException( IFX_E_UNSUPPORTED );
			}
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXLightDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXLightDecoder *pComponent = new CIFXLightDecoder;

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
