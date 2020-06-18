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
//
//	CIFXGroupDecoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXGroupDecoder.
//		The CIFXGroupDecoder is used by the CIFXLoadManager to load
//		group nodes into the scene graph. CIFXGroupDecoder exposes the
//		IFXDecoderX interface to the	CIFXLoadManager for this purpose.
//
//	NOTES
//
//*****************************************************************************

#include "CIFXGroupDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXGroupDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXGroupDecoder *pComponent = new CIFXGroupDecoder;

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

// Constructor
CIFXGroupDecoder::CIFXGroupDecoder()
{
}

// Destructor
CIFXGroupDecoder::~CIFXGroupDecoder()
{
}


// IFXUnknown
U32 CIFXGroupDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXGroupDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXGroupDecoder::QueryInterface( IFXREFIID	interfaceId,
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
void CIFXGroupDecoder::InitializeX(const IFXLoadConfig &lc)
{
	CIFXNodeBaseDecoder::InitializeX(lc);
}

void CIFXGroupDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXGroup);
}

void CIFXGroupDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	CheckInitializedX();

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if (pDataBlockX) {
			CommonNodeReadU3DX(*pDataBlockX);
		}
	}

	rWarningPartialTransfer = IFX_OK;
}
