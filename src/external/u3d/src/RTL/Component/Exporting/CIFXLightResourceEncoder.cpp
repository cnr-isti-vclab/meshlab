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
//	CIFXLightResourceEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXLightResourceEncoder.
//		The CIFXLightResourceEncoder contains light node encoding functionality 
//		that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXLightResourceEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXPalette.h"
#include "IFXLight.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"
#include "IFXRenderContext.h"

// constructor
CIFXLightResourceEncoder::CIFXLightResourceEncoder()
{
	m_pLightRes = NULL;
	m_bInitialized = FALSE;
	m_uRefCount = 0;
}

// destructor
CIFXLightResourceEncoder::~CIFXLightResourceEncoder()
{
	IFXRELEASE( m_pLightRes );
}



// IFXUnknown
U32 CIFXLightResourceEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXLightResourceEncoder::Release()
{
	if ( !( --m_uRefCount ) ) 
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

IFXRESULT CIFXLightResourceEncoder::QueryInterface( IFXREFIID	interfaceId, 
											    void**		ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
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
		rc = IFX_E_INVALID_POINTER;


	IFXRETURN(rc);
}






// IFXEncoderX
void CIFXLightResourceEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// initialize base class(es)
		CIFXNodeBaseEncoder::Initialize( rCoreServices );

		// initialize locally
		if ( TRUE == CIFXNodeBaseEncoder::m_bInitialized )
			CIFXLightResourceEncoder::m_bInitialized = TRUE;
		else
			CIFXLightResourceEncoder::m_bInitialized = FALSE;

		return;
	}

	catch ( ... )
	{
		throw;
	}
}

void CIFXLightResourceEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX* pDataBlock = NULL;

	try
	{
		
		// use node base class to encode shared node data (i.e. data that all node
		// types possess)

		// check for initialization
		if ( FALSE == CIFXLightResourceEncoder::m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );

		if ( NULL == m_pLightRes )
			throw IFXException( IFX_E_CANNOT_FIND );

		// Process the light resource data block

		// The light resource block has the following sections:
		// 0. Resource Name	  (IFXString)
		// 1. LightAttributes (U32)
		// 2. LightType       (U8)
		// 3. Color           (3*F32)
		// 4. Attenuation     (3*F32)
		// 5. SpotAngle       (F32)
		// 6. Intensity       (F32)

		// 0. Name (IFXString)
		m_pBitStream->WriteIFXStringX(rName);

		// 1. LightAttributes (U32)
		m_pBitStream->WriteU32X((U32)m_pLightRes->GetAttributes());

		// 2. LightType (U8)
		m_pBitStream->WriteU8X((U8)m_pLightRes->GetType());

		// 3. Color (4*F32)
		IFXVector4 vColor = m_pLightRes->GetColor();
		m_pBitStream->WriteF32X(vColor.R());
		m_pBitStream->WriteF32X(vColor.G());
		m_pBitStream->WriteF32X(vColor.B());
		m_pBitStream->WriteF32X(vColor.A());

		// 4. Attenuation (3*F32)
		F32 fAttenuation[3];
		IFXCHECKX( m_pLightRes->GetAttenuation(fAttenuation) );
		m_pBitStream->WriteF32X(fAttenuation[IFXLightResource::CONSTANT]);
		m_pBitStream->WriteF32X(fAttenuation[IFXLightResource::LINEAR]);
		m_pBitStream->WriteF32X(fAttenuation[IFXLightResource::QUADRADIC]);

		// 5. SpotAngle (F32)
		m_pBitStream->WriteF32X(m_pLightRes->GetSpotAngle());

		// 6. Intensity
		m_pBitStream->WriteF32X(m_pLightRes->GetRenderLight().GetIntensity());

		// get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// set the data block type
		pDataBlock->SetBlockTypeX( BlockType_ResourceLightU3D );

		// set the priority on the datablock
		pDataBlock->SetPriorityX( 0 ); // returns void

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pLightRes->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX( *pDataBlock );

		// clean up
		IFXRELEASE( pDataBlock );
	}

	catch ( ... )
	{
		// release IFXCOM objects
		IFXRELEASE( pDataBlock );

		throw;
	}
}

void CIFXLightResourceEncoder::SetObjectX( IFXUnknown& rObject )
{
	try
	{
		// get the IFXNode interface
		IFXCHECKX( rObject.QueryInterface( IID_IFXLightResource, (void**)&m_pLightRes ) );

		if ( NULL == m_pLightRes )
			throw IFXException( IFX_E_INVALID_POINTER );
	}

	catch ( ... )
	{
		throw;
	}
}



// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXLightResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXLightResourceEncoder *pComponent = new CIFXLightResourceEncoder;

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

	IFXRETURN( rc );
}
