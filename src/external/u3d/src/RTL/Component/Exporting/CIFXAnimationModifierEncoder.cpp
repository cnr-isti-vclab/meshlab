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

/**
	@file	CIFXAnimationModifierEncoder.cpp

			Implementation of the CIFXAnimationModifierEncoder.
			The CIFXAnimationModifierEncoder contains skeleton modifier param 
			encoding functionality that is used by the write manager.
*/

#include "CIFXAnimationModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXAnimationModifier.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"

// constructor
CIFXAnimationModifierEncoder::CIFXAnimationModifierEncoder()
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXAnimationModifierEncoder::~CIFXAnimationModifierEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXAnimationModifierEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXAnimationModifierEncoder::Release()
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

IFXRESULT CIFXAnimationModifierEncoder::QueryInterface( IFXREFIID	interfaceId, 
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
void CIFXAnimationModifierEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXDataBlockX*		pDataBlock = NULL;
	IFXAnimationModifier*	pBonesModifier = NULL;
	IFXModifier*		pModifier = NULL;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		// get the required interfaces
		IFXCHECKX( m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier ) );
		IFXCHECKX( m_pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&pBonesModifier ) );

		// The SkeletonModifierParam block has the following sections:
		// 1. Parent node name (CIFXString)
		// 2. Modifier chain position
		// 3. Attibutes flag (U8)
		//		a) isPlaying
		//		b) isRootLock
		//		c) isSingleTrack
		//		d) AutoBlend

		// 4. Time Scaling value (F32)
		// 5. Mixer count (U32)
		//		6) Motion name (CIFXString)
		//		7) Attributes flag (U8)...holds isLooped, isSync, and isSingleTrack
		//		8) Time Offset (F32) ...milliseconds
		//		9) Time Scaling value (F32)
		// 10.	Blendtime (F32)
		// 11.	AxisLock (U32)
		// 12.	Concatenation (U32)

		// 1. Encode the parent node name
		m_pBitStream->WriteIFXStringX(rName);

		// 2. Encode the modifier chain position
		U32 uIndex = 0;
		IFXCHECKX( pModifier->GetModifierChainIndex( uIndex ) );
		m_pBitStream->WriteU32X( uIndex );

		// 3. encode attributes flags:
		U32 attributes		= 0x00000000;
		U32 isPlaying		= 0x00000001;
		U32 isRootLock		= 0x00000002;
		U32 isSingleTrack	= 0x00000004;
		U32 autoblend		= 0x00000008;

		if (pBonesModifier->Playing())
			attributes |= isPlaying;
		if (pBonesModifier->RootLock())
			attributes |= isRootLock;
		if ( pBonesModifier->IsKeyframe() == TRUE )
			attributes |= isSingleTrack;
		if (pBonesModifier->AutoBlend())
			attributes |= autoblend;

		m_pBitStream->WriteU32X(attributes);

		// 4. Time Scaling value:
		m_pBitStream->WriteF32X(pBonesModifier->TimeScale());

		// 5. Mixer count:
		U32 mixerCount = pBonesModifier->GetNumberQueued();
		m_pBitStream->WriteU32X(mixerCount);

		// Write out mixer stack (the playlist)
		U32 m;
		for (m = 0; m < mixerCount; m++)
		{
			IFXMotionMixer* pMixer = pBonesModifier->GetMotionMixerNR( m );

			if ( pMixer )
			{
				//	6) Motion name (CIFXString)
				IFXString mixerName = pMixer->GetPrimaryMotionName();

				m_pBitStream->WriteIFXStringX(mixerName);

				//	7) Attributes flag (U8)
				U32 attributes = 0x00;
				if (pMixer->Loop())
					attributes |= 0x01;
				if (pMixer->Sync())
					attributes |= 0x02;

				m_pBitStream->WriteU32X(attributes);

				//	8) Time Offset (F32) ...milliseconds
				F32 fTimeOffset	= 0.0f;
				pMixer->GetTimeOffset( &fTimeOffset );
				m_pBitStream->WriteF32X(fTimeOffset);

				//	9) Time Scaling value (F32)
				F32 fTimeScaling = pMixer->TimeScale();
				m_pBitStream->WriteF32X(fTimeScaling);
			}
			else
				throw IFXException( IFX_E_NOT_INITIALIZED );
		} // end for (m = mixerCount-1; m >= 0; m--)

		// 10.	Blendtime (F32)
		m_pBitStream->WriteF32X(pBonesModifier->BlendTime());

		// Get the block
		m_pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX(BlockType_ModifierAnimationU3D);

		// Set the Priority on the Datablock
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		pBonesModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX(*pDataBlock);

		// clean up
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pModifier );
		IFXRELEASE( pBonesModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pBonesModifier );
		IFXRELEASE( pModifier );

		throw;
	}
}

void CIFXAnimationModifierEncoder::InitializeX( IFXCoreServices& rCoreServices )
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
		throw;
	}
}

void CIFXAnimationModifierEncoder::SetObjectX( IFXUnknown& rObject )
{
	try
	{
		// set the object
		IFXRELEASE( m_pObject );		
		m_pObject = &rObject;
		m_pObject->AddRef();
	}
	catch ( ... )
	{
		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXAnimationModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) 
	{
		// Create the CIFXLoadManager component.
		CIFXAnimationModifierEncoder *pComponent = new CIFXAnimationModifierEncoder;

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
