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
//	CIFXMaterialResourceEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXMaterialResourceEncoder.
//		The CIFXMaterialResourceEncoder contains material resource encoding 
//		functionality that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXMaterialResourceEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXMaterialResource.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"




// constructor
CIFXMaterialResourceEncoder::CIFXMaterialResourceEncoder()
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXMaterialResourceEncoder::~CIFXMaterialResourceEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}






// IFXUnknown
U32 CIFXMaterialResourceEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXMaterialResourceEncoder::Release()
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

IFXRESULT CIFXMaterialResourceEncoder::QueryInterface( IFXREFIID	interfaceId, 
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
void CIFXMaterialResourceEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*			pDataBlock = NULL;
	IFXMaterialResource*	pMaterialResource = NULL;


	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXMaterialResource, 
											  (void**)&pMaterialResource ) );

		// The material resource block has the following sections:
		// 1.  MaterialName (ICHAR)
		// 2.  MaterialAttributes (U32)
		// 3.  Ambient color (3*F32)
		// 4.  Diffuse color (3*F32)
		// 5.  Specular color (3*F32)
		// 6.  Emissive color (3*F32)
		// 7.  Reflectivity (F32)
		// 8.  Opacity (F32)

		// 1. Write the material name
		m_pBitStream->WriteIFXStringX( rName );

		// 2. MaterialAttributes (U32)
		U32 uAttributes = pMaterialResource->GetAttributes();
		uAttributes &= 0x0000003F; // Not using Attributes enum since it could get out of sync with the spec.
		m_pBitStream->WriteU32X(uAttributes);

		// 3.  Ambient color (3*F32)
		IFXVector4 vColor;
		IFXCHECKX( pMaterialResource->GetAmbient(&vColor) );
		m_pBitStream->WriteF32X(vColor.R());
		m_pBitStream->WriteF32X(vColor.G());
		m_pBitStream->WriteF32X(vColor.B());

		// 4.  Diffuse color (3*F32)
		IFXCHECKX( pMaterialResource->GetDiffuse(&vColor) );
		m_pBitStream->WriteF32X(vColor.R());
		m_pBitStream->WriteF32X(vColor.G());
		m_pBitStream->WriteF32X(vColor.B());
		
		// 5.  Specular color (3*F32)
		IFXCHECKX( pMaterialResource->GetSpecular(&vColor) );
		m_pBitStream->WriteF32X(vColor.R());
		m_pBitStream->WriteF32X(vColor.G());
		m_pBitStream->WriteF32X(vColor.B());

		// 6.  Emissive color (3*F32)
		IFXCHECKX( pMaterialResource->GetEmission(&vColor) );
		m_pBitStream->WriteF32X(vColor.R());
		m_pBitStream->WriteF32X(vColor.G());
		m_pBitStream->WriteF32X(vColor.B());

		// 7.  Reflectivity (F32)
		F32 fReflectivity;
		IFXCHECKX( pMaterialResource->GetReflectivity(&fReflectivity) );
		m_pBitStream->WriteF32X(fReflectivity);

		// 8.  Opacity (F32)
		F32 fOpacity;
		IFXCHECKX( pMaterialResource->GetOpacity(&fOpacity) );
		m_pBitStream->WriteF32X(fOpacity);

		// get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// set the data block type
		pDataBlock->SetBlockTypeX( BlockType_ResourceMaterialU3D );

		// set the priority on the datablock
		pDataBlock->SetPriorityX( 0 ); // returns void

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX( *pDataBlock );

		// clean up
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pMaterialResource );
		
		return;
	}

	catch ( ... )
	{
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pMaterialResource );

		throw;
	}
}

void CIFXMaterialResourceEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{	
		// latch onto the core services object passed in
		IFXRELEASE( m_pCoreServices )
		m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();
		
		// create a bitstream
		IFXRELEASE( m_pBitStream );
		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX,
									   (void**)&m_pBitStream ) );

		m_bInitialized = TRUE;

		return;
	}

	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		throw;
	}
}

void CIFXMaterialResourceEncoder::SetObjectX( IFXUnknown& rObject )
{
	try
	{
		// set the object
		IFXRELEASE( m_pObject );		
		m_pObject = &rObject;
		m_pObject->AddRef();

		return;
	}

	catch ( ... )
	{
		throw;
	}
}






// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXMaterialResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXMaterialResourceEncoder *pComponent = new CIFXMaterialResourceEncoder;

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


