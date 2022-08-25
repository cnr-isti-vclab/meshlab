//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
//	CIFXShadingModifierEncoder.cpp
//
//	DESCRIPTION:
//		Implementation of the CIFXShadingModifierEncoder.
//		The CIFXShadingModifierEncoder contains SDS (surface subdivision) modifier 
//		encoding functionality that is used by the write manager.
//	
//*****************************************************************************


#include "CIFXShadingModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXShadingModifier.h"
#include "IFXSceneGraph.h"

// constructor
CIFXShadingModifierEncoder::CIFXShadingModifierEncoder() :
	IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXShadingModifierEncoder::~CIFXShadingModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXShadingModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXShadingModifierEncoder::Release()
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

IFXRESULT CIFXShadingModifierEncoder::QueryInterface( IFXREFIID interfaceId, 
												  void**	ppInterface )
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
void CIFXShadingModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXShadingModifier*	pShadingModifier = NULL;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXShadingModifier,
											  (void**)&pShadingModifier ) );

		// The following elements are common to all Modifier blocks 
		// and are encoded in EncodeCommonElements():
			// 1. ModelGenerator Name (IFXString)
			// 2. Index for modifier chain (U32)

		m_pBitStream->WriteIFXStringX( rName );

		U32 uTemp = 0;
		IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
		m_pBitStream->WriteU32X( uTemp );

		// Shading Modifiers have the following parameters:
		// 1. Shading Attributes (U32)
		// 2. Shader List Count (U32)
		//		2.1 Shader Count (U32)
		//			2.1.1 Shader Name (IFXString)
		U32 uShadingAttributes = 0;
		U32 uShaderCnt;
		U32 uShaderListCnt;
		IFXDECLARELOCAL( IFXPalette, pShaderPalette );
		IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
		U32 uShaderID;

		m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void **)&pSceneGraph );
		IFXASSERT( pSceneGraph );
		pSceneGraph->GetPalette( IFXSceneGraph::SHADER, &pShaderPalette );

		pShadingModifier->GetAttributes( &uShadingAttributes );
		m_pBitStream->WriteU32X( uShadingAttributes );

		pShadingModifier->GetSize( &uShaderListCnt );
		m_pBitStream->WriteU32X( uShaderListCnt );

		U32 i;
		for(  i = 0; i < uShaderListCnt; i++ )
		{
			IFXDECLARELOCAL( IFXShaderList, pShaderList );

			pShadingModifier->GetElementShaderList( i, &pShaderList );
			uShaderCnt = pShaderList->GetNumShaders();
			m_pBitStream->WriteU32X( uShaderCnt );

			U32 j;
			IFXString sShaderName;
			for(  j = 0; j < uShaderCnt; j++ )
			{
				pShaderList->GetShader( j, &uShaderID );
				pShaderPalette->GetName( uShaderID, &sShaderName );
				m_pBitStream->WriteIFXStringX( sShaderName );
			}
		}

		IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

		// Get a data block from the bitstream
		m_pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX(BlockType_ModifierShadingU3D);

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX(*pDataBlock);

		// clean up
		IFXRELEASE( pShadingModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( pShadingModifier );

		throw;
	}
}

void CIFXShadingModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
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
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		IFXRELEASE( m_pBitStream );
		
		throw;
	}
}

void CIFXShadingModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;
	
	try
	{
		// set the object
		IFXRELEASE( m_pObject );		
		m_pObject = &rObject;
		m_pObject->AddRef();

	
		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

//		CIFXModifierBaseEncoder::SetModifier( *pModifier );
		pModifier->AddRef();
		IFXRELEASE( m_pModifier );
		m_pModifier = pModifier;

		IFXRELEASE( pModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter
		IFXRELEASE( pModifier );

		throw;
	}
}



// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXShadingModifierEncoder *pComponent = new CIFXShadingModifierEncoder;

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


