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
/*
@file  CIFXViewResource.cpp                                                          
*/

#include "IFXSceneGraphPCH.h"
#include "CIFXViewResource.h"
#include "IFXLightSet.h"
#include "IFXPickObject.h"
#include "IFXRenderable.h"
#include "IFXModifierDataElementIter.h"
#include "IFXModifierDataPacket.h"
#include "IFXSimpleList.h"
#include "IFXSpatialSetQuery.h"

#include "IFXBoundSphereDataElement.h"
#include "IFXDids.h"

#include "IFXExportingCIDs.h"

CIFXViewResource::CIFXViewResource()
{
	m_uRefCount = 0;
	m_uQualityFactor = 0;

	m_layer = 0;

	m_uNumRenderPasses = 0;
	m_uCurrentPass = 0;
	m_ppRenderPass = NULL;
	AllocateRenderPasses();

	FogEnable(FALSE);
	SetStencilEnabled( FALSE );
	SetColorBufferEnabled( TRUE ); //currently does nothing
	GetRenderClear().SetColorCleared( TRUE );
	GetRenderClear().SetColorValue( IFXVector3( 0,0,0 ) );
	SetDepthTestEnabled( TRUE );
	SetDepthWriteEnabled( TRUE );
	GetRenderClear().SetDepthCleared( TRUE );
	GetRenderClear().SetDepthValue( (F32)1.0 );
}


CIFXViewResource::~CIFXViewResource()
{
}


IFXRESULT IFXAPI_CALLTYPE CIFXViewResource_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXViewResource *pView = new CIFXViewResource;

		if ( pView )
		{
			// Perform a temporary AddRef for our usage of the component.
			pView->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pView->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pView->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXViewResource::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXViewResource::Release()
{
	if (m_uRefCount == 1)
	{
		DeallocateRenderPasses();
		delete this ;
		return 0 ;
	}
	else
		return (--m_uRefCount);
}


IFXRESULT CIFXViewResource::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXViewResource )
			*ppInterface = ( IFXViewResource* ) this;
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

// IFXMarker
IFXRESULT CIFXViewResource::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	return CIFXMarker::SetSceneGraph( pInSceneGraph );
}


// IFXMarkerX
void CIFXViewResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXViewResourceEncoder, rpEncoderX);
}

IFXRESULT CIFXViewResource::GetRootNode(U32* pNodeIndex, U32* pNodeInstance)
{
	IFXRESULT result = IFX_OK;
	if ( pNodeIndex && pNodeInstance ) {
		*pNodeIndex = m_ppRenderPass[m_uCurrentPass]->m_nodeIndex;
		*pNodeInstance = m_ppRenderPass[m_uCurrentPass]->m_nodeInstance;
	} else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXViewResource::SetRootNode(U32 nodeIndex, U32 nodeInstance)
{
	IFXRESULT result = IFX_OK;

	result = m_ppRenderPass[m_uCurrentPass]->SetRootNode(nodeIndex, nodeInstance);

	// Make sure to set all rootnodes that are null
	U32 i;
	for( i = 0; IFXSUCCESS(result) && i < m_uNumRenderPasses; i++)
		if(m_ppRenderPass[i]->m_nodeSet == FALSE)
			result = m_ppRenderPass[i]->SetRootNode(nodeIndex, nodeInstance);

	return result;
}

void CIFXViewResource::ClearRootNode()
{
	m_ppRenderPass[m_uCurrentPass]->ClearRootNode();
}

IFXRenderClear& CIFXViewResource::GetRenderClear()
{
	return m_ppRenderPass[m_uCurrentPass]->m_Clear;
}


// IFXViewResource
IFXRESULT CIFXViewResource::AllocateRenderPasses(U32 uNumRenderPasses)
{
	IFXRESULT rc = IFX_OK;

	IFXRenderPass** ppRenderPasses = new IFXRenderPass*[uNumRenderPasses];

	if(NULL == ppRenderPasses)
	{
		rc = IFX_E_OUT_OF_MEMORY;
	}

	if(IFXSUCCESS(rc))
	{
		if(m_uNumRenderPasses)
		{
			U32 uNumToCopy = m_uNumRenderPasses;
			if(uNumToCopy > uNumRenderPasses)
				uNumToCopy = uNumRenderPasses;
			U32 i;
			for( i = 0; i < uNumToCopy; i++)
			{
				ppRenderPasses[i] = m_ppRenderPass[i];
				m_ppRenderPass[i] = 0;
			}
		}

		U32 i;
		for( i = m_uNumRenderPasses; i < uNumRenderPasses; i++)
		{
			ppRenderPasses[i] = new IFXRenderPass();
			ppRenderPasses[i]->SetDefaults(i);

			// Assign new passes to have the same rootnode as the first pass
			if(i && ppRenderPasses[0]->m_nodeSet)
				ppRenderPasses[i]->SetRootNode(
										ppRenderPasses[0]->m_nodeIndex, 
										ppRenderPasses[0]->m_nodeInstance);
		}

		IFXDELETE_ARRAY(m_ppRenderPass);
		m_ppRenderPass = ppRenderPasses;
		m_uNumRenderPasses = uNumRenderPasses;
	}

	return rc;
}

IFXRESULT CIFXViewResource::DeallocateRenderPasses()
{
	U32 i;
	for( i = 0; i < m_uNumRenderPasses; i++ )
		IFXDELETE( m_ppRenderPass[i] );

	IFXDELETE_ARRAY(m_ppRenderPass);

	return IFX_OK;
}


// IFXViewResource
IFXRESULT CIFXViewResource::GetFogEnableValue( BOOL* pbEnable )
{
	IFXRESULT result = IFX_OK;

	if ( NULL != pbEnable )
		*pbEnable = m_ppRenderPass[m_uCurrentPass]->m_bFogEnabled;
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXViewResource::FogEnable( BOOL bEnable )
{
	m_ppRenderPass[m_uCurrentPass]->m_bFogEnabled = bEnable;
	return IFX_OK;
}


IFXRenderFog& CIFXViewResource::GetRenderFog()
{
	return m_ppRenderPass[m_uCurrentPass]->m_Fog;
}

IFXRESULT CIFXViewResource::GetColorBufferEnabled(BOOL& bEnabled )
{
	bEnabled = m_ppRenderPass[m_uCurrentPass]->m_bColorBuffer;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::SetColorBufferEnabled(BOOL bEnabled )
{
	m_ppRenderPass[m_uCurrentPass]->m_bColorBuffer = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::GetDepthTestEnabled(BOOL& bEnabled )
{
	bEnabled = m_ppRenderPass[m_uCurrentPass]->m_bDepthTest;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::SetDepthTestEnabled(BOOL bEnabled )
{
	m_ppRenderPass[m_uCurrentPass]->m_bDepthTest = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::GetDepthWriteEnabled(BOOL& bEnabled )
{
	bEnabled = m_ppRenderPass[m_uCurrentPass]->m_bDepthWrite;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::SetDepthWriteEnabled(BOOL bEnabled )
{
	m_ppRenderPass[m_uCurrentPass]->m_bDepthWrite = bEnabled;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::GetDepthFunc(IFXenum & eDepthFunc )
{
	eDepthFunc = m_ppRenderPass[m_uCurrentPass]->m_eDepthFunc;

	return IFX_OK;
}

IFXRESULT CIFXViewResource::SetDepthFunc(IFXenum eDepthFunc )
{
	IFXRESULT rc = IFX_OK;

	switch(eDepthFunc)
	{
	case IFX_ALWAYS:
	case IFX_LESS:
	case IFX_LEQUAL:
	case IFX_GREATER:
	case IFX_GEQUAL:
	case IFX_EQUAL:
	case IFX_NOT_EQUAL:
	case IFX_NEVER:
		m_ppRenderPass[m_uCurrentPass]->m_eDepthFunc = eDepthFunc;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXViewResource::GetStencilEnabled( BOOL& bEnable )
{
	bEnable = m_ppRenderPass[m_uCurrentPass]->m_bStencilEnabled;
	return IFX_OK;
}


IFXRESULT CIFXViewResource::SetStencilEnabled( BOOL bEnable )
{
	m_ppRenderPass[m_uCurrentPass]->m_bStencilEnabled = bEnable;
	return IFX_OK;
}


IFXRenderStencil& CIFXViewResource::GetRenderStencil()
{
	return m_ppRenderPass[m_uCurrentPass]->m_Stencil;
}


// Multipass support
IFXRESULT CIFXViewResource::SetNumRenderPasses(U32 uNumPasses)
{
	IFXRESULT rc = IFX_E_INVALID_RANGE;

	IFXASSERTBOX( uNumPasses <= 32,
		"Trying to use too many rendering passes - use less than 33"  );

	if(uNumPasses <= 32)
	{
		rc = AllocateRenderPasses(uNumPasses);
	}

	return rc;
}

U32 CIFXViewResource::GetNumRenderPasses(void)
{
	return m_uNumRenderPasses;
}

IFXRESULT CIFXViewResource::SetCurrentRenderPass(U32 uRenderPass)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX( uRenderPass < m_uNumRenderPasses,
		"Setting invalid render pass number" );

	if(uRenderPass >= m_uNumRenderPasses)
		rc = IFX_E_INVALID_RANGE;
	else
		m_uCurrentPass = uRenderPass;

	return rc;
}

U32 CIFXViewResource::GetCurrentRenderPass()
{
	return m_uCurrentPass;
}

///
