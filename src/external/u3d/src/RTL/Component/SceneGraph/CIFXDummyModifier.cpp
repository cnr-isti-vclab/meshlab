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
@file  CIFXDummyModifier.cpp

The implementation file of the CIFXDummyModifier component.      
*/

#include "CIFXDummyModifier.h"
#include "IFXModifierDataPacket.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXEnums.h"

#include "IFXExportingCIDs.h"

CIFXDummyModifier::CIFXDummyModifier()
{
	m_uRefCount = 0;
	m_pDataBlockQueue = NULL;
}

CIFXDummyModifier::~CIFXDummyModifier()
{
	IFXRELEASE( m_pDataBlockQueue );
}

U32 CIFXDummyModifier::AddRef(void)
{
	return ++m_uRefCount;
}

U32 CIFXDummyModifier::Release(void)
{
	if ( 1 == m_uRefCount )
	{
		PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXDummyModifier::QueryInterface( IFXREFIID interfaceId,
										   void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXModifier)
			*ppInterface = ( IFXModifier* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXDummyModifier )
			*ppInterface = ( IFXDummyModifier* ) this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifier_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXDummyModifier *pDummyModifier = new CIFXDummyModifier;

		if ( pDummyModifier )
		{
			// Perform a temporary AddRef for our usage of the component.
			pDummyModifier->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pDummyModifier->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pDummyModifier->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXMarkerX
void CIFXDummyModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXDummyModifierEncoder, rpEncoderX);
}

static const IFXGUID* s_pDummyOutputDIDs[] =
{
	&DID_IFXNullOutput
};

IFXRESULT CIFXDummyModifier::GetOutputs( IFXGUID**& rpOutOutputs,
									   U32&       rOutNumberOfOutputs,
									   U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = 1;
	rpOutOutputs = (IFXGUID**)&s_pDummyOutputDIDs;
	rpOutOutputDepAttrs = NULL;
	return IFX_OK;
}


IFXRESULT CIFXDummyModifier::GetDependencies(
	IFXGUID*   pInOutputDID,
	IFXGUID**& rppOutInputDependencies,
	U32&       rOutNumberInputDependencies,
	IFXGUID**& rppOutOutputDependencies,
	U32&       rOutNumberOfOutputDependencies,
	U32*&      rpOutOutputDepAttrs )
{
	rOutNumberInputDependencies = 0;
	rOutNumberOfOutputDependencies = 0;
	rpOutOutputDepAttrs = 0;
	rppOutInputDependencies = 0;
	rppOutOutputDependencies = 0;

	return IFX_OK;
}


IFXRESULT CIFXDummyModifier::GenerateOutput( U32    inOutputDataElementIndex,
										   void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	return result;
}


IFXRESULT CIFXDummyModifier::SetDataPacket(
	IFXModifierDataPacket* pInInputDataPacket,
	IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInDataPacket && pInInputDataPacket)
	{
		pInDataPacket->AddRef();
		m_pModifierDataPacket = pInDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	} else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXDummyModifier::Notify( IFXModifierMessage eInMessage,
								   void*               pMessageContext )

{
	return IFX_OK;
}

IFXRESULT CIFXDummyModifier::SetDataBlock( IFXDataBlockQueueX *pDataBlockQueue )
{
	IFXRESULT res = IFX_OK;

	if( NULL == pDataBlockQueue )
		res = IFX_E_INVALID_POINTER;
	else
	{
		if( m_pDataBlockQueue )
			IFXRELEASE( m_pDataBlockQueue );

		m_pDataBlockQueue = pDataBlockQueue;
		pDataBlockQueue->AddRef();
	}

	return res;
}

IFXDataBlockQueueX *CIFXDummyModifier::GetDataBlock()
{
	if( m_pDataBlockQueue )
		m_pDataBlockQueue->AddRef();

	return m_pDataBlockQueue;
}

