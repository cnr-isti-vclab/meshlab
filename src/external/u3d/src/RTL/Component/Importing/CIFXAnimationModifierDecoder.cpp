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
	@file	CIFXAnimationModifierDecoder.cpp

			Implementation of the CIFXAnimationModifierDecoder.
			The CIFXAnimationModifierDecoder is used by the CIFXLoadManager
			to load skeleton modifier parameterization components into the scene
			graph. CIFXAnimationModifierDecoder exposes the IFXDecoderX
			interface to the CIFXLoadManager for this purpose.
*/

#include "CIFXAnimationModifierDecoder.h"
#include "IFXCoreCIDs.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXBlockTypes.h"
#include "IFXAnimationModifier.h"
#include "IFXModifierChain.h"
#include "IFXBitStreamX.h"
#include "IFXCheckX.h"

CIFXAnimationModifierDecoder::CIFXAnimationModifierDecoder() :
	IFXDEFINEMEMBER(m_pBonesModifier)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
}

CIFXAnimationModifierDecoder::~CIFXAnimationModifierDecoder()
{
}

// IFXUnknown
U32 CIFXAnimationModifierDecoder::AddRef( void )
{
	return ++m_uRefCount;
}

U32 CIFXAnimationModifierDecoder::Release( void )
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXAnimationModifierDecoder::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX)
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
void CIFXAnimationModifierDecoder::InitializeX( const IFXLoadConfig &lc )
{
	if( lc.m_units > 0.0f )
		m_unitScale = lc.m_units;
	IFXModifierBaseDecoder::InitializeX(lc);
}

void CIFXAnimationModifierDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	if (NULL == m_pObject) {
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);

		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		// 1. Model Name
		pBitStreamX->ReadIFXStringX( m_stringObjectName);
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

		// 2. Index for modifier chain
		pBitStreamX->ReadU32X( m_uChainPosition );

		BOOL bIsKeyframe = FALSE;
		U32 uAttributes = 0;
		U32 isSingleTrack = 0x00000004;
		pBitStreamX->ReadU32X(uAttributes);
		if (uAttributes & isSingleTrack)
			bIsKeyframe = TRUE;

		IFXCHECKX(IFXCreateComponent( CID_IFXAnimationModifier, IID_IFXUnknown, (void**)&m_pObject ));
		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXCHECKX(m_pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);
		pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

		IFXRELEASE(m_pBonesModifier);
		IFXCHECKX(m_pObject->QueryInterface( IID_IFXAnimationModifier, (void**)&m_pBonesModifier));
		if( bIsKeyframe == TRUE )
			m_pBonesModifier->SetAsKeyframe();
		else
			m_pBonesModifier->SetAsBones();

		ProcessChainX(rDataBlockX);
	}
}

void CIFXAnimationModifierDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	// Get the scene graph
	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));

	// Get the node palette
	IFXDECLARELOCAL(IFXPalette,pNodePalette);
	IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::NODE, &pNodePalette ));

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX ) {

			// set metadata
			IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
			IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
			pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
			m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
			pObjectMD->AppendX(pBlockMD);

			// Create a bitstream component and initialize it to decode the block
			IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
			{
				IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
				pBitStreamX->SetDataBlockX( *pDataBlockX );
			}

			// The SkeletonModifierParam block has the following sections:
			// 1.  Parent node name (IFXString)
			// 1b. Index if applicable
			// 2.  Attibutes flag (U8)
			//		a) isPlaying
			//		b) isRootLock
			//		c) isSingleTrack
			//		d) AutoBlend
			// 3.  Time Scaling value (F32)
			// 4.  Mixer count (U32)
			//		5. Motion name (IFXString)
			//		6) Attributes flag (U8)...holds isLooped, isSync, and isSingleTrack
			//		7) Time Offset (F32) ...milliseconds
			//		8) Time Scaling value (F32)
			// 9.  Blendtime (F32)
			// 10. AxisLock (U32)
			// 11. Concatenation (U32)

			// Process the SkeletonModifierParam data block
			// 0. Model Name
			pBitStreamX->ReadIFXStringX( m_stringObjectName);
			IFXDECLARELOCAL(IFXNameMap, pNameMap);
			m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
			IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

			// 1. Index for modifier chain
			pBitStreamX->ReadU32X( m_uChainPosition );

			// 2. Attibutes flag (U32)
			U32 attributes		= 0x00000000;
			U32 isPlaying		= 0x00000001;
			U32 isRootLock		= 0x00000002;
			//U32 isSingleTrack	= 0x00000004;
			U32 autoblend		= 0x00000008;

			pBitStreamX->ReadU32X(attributes);

			BOOL bTemp = FALSE ;

			bTemp = (attributes & isPlaying) ? TRUE : FALSE ;
			m_pBonesModifier->Playing() = bTemp;

			bTemp = (attributes & isRootLock) ? TRUE : FALSE ;
			m_pBonesModifier->RootLock() = bTemp;

			bTemp = (attributes & autoblend) ? TRUE : FALSE ;
			m_pBonesModifier->AutoBlend() = bTemp;

			// 3. Time Scaling value (F32)
			F32 timeScaling = 0.0;
			pBitStreamX->ReadF32X(timeScaling);
			m_pBonesModifier->TimeScale() = timeScaling;

			// 4. Mixer count (U32)
			U32 uMixerCount = 0;
			pBitStreamX->ReadU32X(uMixerCount);

			// for each animation being loaded
			// check to see if it is in the mixer palette.
			IFXDECLARELOCAL(IFXPalette,pMixerPalette);
			IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette ));

			U32 m;
			for (m = 0; m < uMixerCount ; m++) {
				//	5. Mixer name (IFXString)
				IFXString mixerName;
				pBitStreamX->ReadIFXStringX(mixerName);
				IFXDECLARELOCAL(IFXNameMap, pNameMap);
				m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
				IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MOTION, mixerName));

				// If the mixer has not arrived yet, we know it is coming
				// so put a placeholder in the mixer palette so the queue
				// can succeed.
				U32 id = 0;
					// Find() will return IFX_E_CANNOT_FIND if
					// the mixer is not found
				IFXRESULT iResultFind = pMixerPalette->Find( &mixerName, &id );
				if (IFXFAILURE(iResultFind))	{
					IFXCHECKX(pMixerPalette->Add( &mixerName, &id ));
				}

				//	6) Attributes flag (U8)...holds isLooped and isSync
				U32 attributes = 0x00;
				U32 isLooped = 0x01;
				U32 isSync   = 0x02;
				BOOL bLoop = FALSE;
				BOOL bSync = FALSE;
				pBitStreamX->ReadU32X(attributes);
				if (attributes & isLooped)
					bLoop = TRUE;
				if (attributes & isSync)
					bSync = TRUE;

				//	7) Time Offset (F32) ...milliseconds
				F32 fTimeOffset;
				pBitStreamX->ReadF32X(fTimeOffset);

				//	8) Time Scaling value (F32)
				F32 fTimeScaling;
				pBitStreamX->ReadF32X(fTimeScaling);

				// modified Duration (for "end time")
				//F32 fStartTime= 0.0f;
				//F32 fEndTime= -0.001f;

				// Queue up the mixer
				// To avoid needing re-exporting, ignore exported offset/start/end
				// which probably always 0,0,-1 anyhow

				BOOL bDelayMap = TRUE;
				IFXCHECKX(m_pBonesModifier->Queue(mixerName, NULL, NULL, NULL,&fTimeScaling,&bLoop, &bSync,bDelayMap));

			} // end for (m = 0; m < uMixerCount; m++)

			// 9.	Blendtime (F32)
			F32 fTemp = 0 ;
			pBitStreamX->ReadF32X(fTemp);
			m_pBonesModifier->BlendTime() = fTemp;
		}
	} // end while (there are blocks in the queue)

	rWarningPartialTransfer = IFX_OK;
}

IFXRESULT IFXAPI_CALLTYPE CIFXAnimationModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		CIFXAnimationModifierDecoder *pComponent = 	new CIFXAnimationModifierDecoder;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}
