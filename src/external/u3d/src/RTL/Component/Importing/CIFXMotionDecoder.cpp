//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	CIFXMotionDecoder.cpp

			Implementation of the CIFXMotionDecoder.
			The CIFXMotionDecoder is used by the CIFXLoadManager to load
			motion resources into the scene graph. CIFXMotionDecoder exposes a
			IFXDecoder interface to the CIFXLoadManager for this purpose.
*/

#include "CIFXMotionDecoder.h"
#include "IFXMotion.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXBlockTypes.h"
#include "IFXMotionResource.h"
#include "IFXACContext.h"
#include "IFXCheckX.h"
#include "IFXMixerConstruct.h"


CIFXMotionDecoder::CIFXMotionDecoder() :
IFXDEFINEMEMBER(m_pObject),
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pDataBlockQueueX)
{
	m_uRefCount = 0;
	m_uLoadId = 0;
}

CIFXMotionDecoder::~CIFXMotionDecoder()
{
}

U32 CIFXMotionDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXMotionDecoder::Release( )
{
	if( 1  == m_uRefCount )
	{
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXMotionDecoder::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if (ppInterface) {
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

// Initialize and get a reference to the core services object
void CIFXMotionDecoder::InitializeX( const IFXLoadConfig &lc )
{
	// Initialize the data block queue
	IFXRELEASE(m_pDataBlockQueueX);
	IFXCHECKX(IFXCreateComponent( CID_IFXDataBlockQueueX, IID_IFXDataBlockQueueX, (void**)&m_pDataBlockQueueX ));

	// Store the core services pointer
	lc.m_pCoreServices->AddRef();
	IFXRELEASE(m_pCoreServices);
	m_pCoreServices = lc.m_pCoreServices;
	m_uLoadId = lc.m_loadId;
	m_bExternal = lc.m_external;
}

void CIFXMotionDecoder::ProcessMotionCompressedX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pCoreServices) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// Get the scene graph
	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph,( void** )&pSceneGraph ));

	// Process the motion description data block
	// The motion description block has the following sections:
	// 1. MotionName
	// 2. TrackCount
	// 3. InverseQuantTime
	// 4. InverseQuantRotation
	//    5. TrackName
	//    6. TimeCount
	//    7. InverseQuantDisplacement
	//    8. InverseQuantScale
	//      9. Time
	//      10. Displacement
	//      11. Rotation
	//      12. Scale

	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	// Create a bitstream component and initialize it to decode the block
	IFXDECLARELOCAL(IFXBitStreamCompressedX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamCompressedX, ( void** )&pBitStreamX ));
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	U32 uProfile;
	pNameMap->GetProfile(m_uLoadId, uProfile);
	pBitStreamX->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

	U32 uBlockType = 0;
	rDataBlockX.GetBlockTypeX(uBlockType);
	pBitStreamX->SetDataBlockX( rDataBlockX );

	// 1. MotionName
	IFXString stringMotionName;
	// read the motion name
	pBitStreamX->ReadIFXStringX( stringMotionName );
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MOTION, stringMotionName));

	// Access the appropriate interface of the shader that was handed in via SetObject().
	IFXDECLARELOCAL(IFXMotionResource,pMotionResource);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXMotionResource, (void**)&pMotionResource ));

	pMotionResource->GetMotionRef()->SetName(stringMotionName);

	// 2. TrackCount
	U32 uTrackCount = 0;
	pBitStreamX->ReadU32X( uTrackCount );
	// 3. InverseQuantTime
	F32 fInverseQuantTime = 1.0;
	pBitStreamX->ReadF32X( fInverseQuantTime );
	// 4. InverseQuantRotation
	F32 fInverseQuantRotation = 1.0;
	pBitStreamX->ReadF32X( fInverseQuantRotation );

	U32 i = 0;
	for( i=0; i < uTrackCount; i++ )
	{
		// 5. TrackName
		IFXString trackName;
		pBitStreamX->ReadIFXStringX( trackName );
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MOTION, trackName));
		// Find the track id from the track name
		U32 uTrackID = 0;
		IFXRESULT iFindResult = pMotionResource->FindTrack( &trackName, &uTrackID );
		if( IFXFAILURE( iFindResult ) ) {
			IFXCHECKX(pMotionResource->AddTrack( &trackName, &uTrackID ));
		}

		// 6. TimeCount
		U32 uTimeCount = 0;
		pBitStreamX->ReadU32X( uTimeCount );
		IFXKeyFrame *pKeyFrameArray = NULL;
		pKeyFrameArray = new IFXKeyFrame[uTimeCount];
		if( NULL == pKeyFrameArray ) {
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}

		// 7. InverseQuantDisplacement
		F32 fInverseQuantDisplacement = 1.0;
		pBitStreamX->ReadF32X(fInverseQuantDisplacement);

		// 8. InverseQuantScale
		F32 fInverseQuantScale = 1.0;
		pBitStreamX->ReadF32X(fInverseQuantScale);

		IFXKeyFrame thePredictedKeyFrame;
		U32 j = 0;
		for( j=0;  j < uTimeCount; j++ )
		{
			//  9. Time
			F32 fDiffTime = 0.0f;
			if(0==j || (uTimeCount-1) == j)
			{
				pBitStreamX->ReadF32X(fDiffTime);
			}
			else
			{
				U32 uDiffTime = 0;
				U8 u8SignTime = 0;
				pBitStreamX->ReadCompressedU8X(uACContextMotionSignTime,u8SignTime);
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffTime, uDiffTime );
				fDiffTime = (fInverseQuantTime*uDiffTime);
				if(u8SignTime & 1) {
					fDiffTime *= -1.0f;
				}
			}

			F32 fPredictedTime = thePredictedKeyFrame.Time();
			F32 fTime = fDiffTime + fPredictedTime;
			pKeyFrameArray[j].SetTime( fTime );

			//  10. Displacement
			F32 fdX=0.0f, fdY=0.0f, fdZ=0.0f;
			if(0==j || ((uTimeCount-1) == j && BlockType_ResourceMotionU3D == uBlockType ))
			{
				pBitStreamX->ReadF32X(fdX);
				pBitStreamX->ReadF32X(fdY);
				pBitStreamX->ReadF32X(fdZ);
			}
			else
			{
				U8 u8SignDisplacement = 0;
				U32 udX = 0;
				U32 udY = 0;
				U32 udZ = 0;
				pBitStreamX->ReadCompressedU8X(uACContextMotionSignDisplacement, u8SignDisplacement);
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffDisplacement, udX );
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffDisplacement, udY );
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffDisplacement, udZ );
				fdX = fInverseQuantDisplacement*udX;
				fdY = fInverseQuantDisplacement*udY;
				fdZ = fInverseQuantDisplacement*udZ;
				if(u8SignDisplacement & 1) {
					fdX *= -1.0;
				}
				if(u8SignDisplacement & 2) {
					fdY *= -1.0;
				}
				if(u8SignDisplacement & 4) {
					fdZ *= -1.0;
				}
			}

			IFXVector3 vDisplacement(0.0f,0.0f,0.0f);
			IFXVector3 vPredictedDisplacement = thePredictedKeyFrame.Location();
			IFXVector3 vDiffDisplacement(fdX,fdY,fdZ);
			vDisplacement.Add(vDiffDisplacement,vPredictedDisplacement);
			pKeyFrameArray[j].Location() = vDisplacement;

			//  11. Rotation
			U8 u8SignRot = 0;
			U32 uB=0,uC=0,uD=0;
			if(0==j || ((uTimeCount-1) == j && BlockType_ResourceMotionU3D == uBlockType ))
			{
				F32 fao,fbo,fco,fdo;
				pBitStreamX->ReadF32X(fao);
				pBitStreamX->ReadF32X(fbo);
				pBitStreamX->ReadF32X(fco);
				pBitStreamX->ReadF32X(fdo);
				IFXQuaternion vRotation(1.0f,0.0f,0.0f,0.0f);
				vRotation.Set(fao,fbo,fco,fdo);
				pKeyFrameArray[j].Rotation() = vRotation;
			} else {
				pBitStreamX->ReadCompressedU8X(uACContextMotionSignRotation,u8SignRot);
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffRotation, uB );
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffRotation, uC );
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffRotation, uD );
				F32 fb = fInverseQuantRotation*uB;
				F32 fc = fInverseQuantRotation*uC;
				F32 fd = fInverseQuantRotation*uD;
				if(fb > 1.0f) fb = 1.0f;
				if(fc > 1.0f) fc = 1.0f;
				if(fd > 1.0f) fd = 1.0f;
				F32 fa = (F32) sqrt(fabs(1.0 - ((F64)fb)*((F64)fb) - ((F64)fc)*((F64)fc) - ((F64)fd)*((F64)fd)));
				if(u8SignRot & 1) {
					fa *= -1.0;
				}
				if(u8SignRot & 2) {
					fb *= -1.0;
				}
				if(u8SignRot & 4) {
					fc *= -1.0;
				}
				if(u8SignRot & 8) {
					fd *= -1.0;
				}

				// Set the key frame values
				IFXQuaternion vDiffRotation( fa,fb,fc,fd );
				IFXQuaternion vPredictedRotation = thePredictedKeyFrame.Rotation();
				IFXQuaternion vRotationQ(1.0f,0.0f,0.0f,0.0f);
				vRotationQ.Multiply(vPredictedRotation,vDiffRotation);
				pKeyFrameArray[j].Rotation() = vRotationQ;
			}

			// 12. Scale
			F32 fdsX=0.0f, fdsY=0.0f, fdsZ=0.0f;
			if(0==j || ((uTimeCount-1) == j && BlockType_ResourceMotionU3D == uBlockType ))
			{
				pBitStreamX->ReadF32X(fdsX);
				pBitStreamX->ReadF32X(fdsY);
				pBitStreamX->ReadF32X(fdsZ);
			} else {
				U8 u8SignScale = 0;
				U32 udsX = 0;
				U32 udsY = 0;
				U32 udsZ = 0;
				pBitStreamX->ReadCompressedU8X(uACContextMotionSignScale, u8SignScale);
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffScale, udsX );
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffScale, udsY );
				pBitStreamX->ReadCompressedU32X(uACContextMotionDiffScale, udsZ );
				fdsX = fInverseQuantScale*udsX;
				fdsY = fInverseQuantScale*udsY;
				fdsZ = fInverseQuantScale*udsZ;
				if(u8SignScale & 1) {
					fdsX *= -1.0;
				}
				if(u8SignScale & 2) {
					fdsY *= -1.0;
				}
				if(u8SignScale & 4) {
					fdsZ *= -1.0;
				}
			}

			IFXVector3 vScaleQ(0.0f,0.0f,0.0f);
			IFXVector3 vPredictedScaleQ = thePredictedKeyFrame.Scale();
			IFXVector3 vDiffScaleQ(fdsX,fdsY,fdsZ);
			vScaleQ.Add(vDiffScaleQ,vPredictedScaleQ);
			pKeyFrameArray[j].Scale() = vScaleQ;

			// Set the predicted frame for the next key frame
			thePredictedKeyFrame = pKeyFrameArray[j];
		}

		// Set the key frame array to the motion resource
		IFXCHECKX(pMotionResource->ClearTrack( uTrackID ));
		IFXCHECKX(pMotionResource->InsertKeyFrames( uTrackID,uTimeCount,pKeyFrameArray ));
		IFXDELETE_ARRAY( pKeyFrameArray );
	}

	// Now, add a fully-mapped mixer to the mixer palette for this motion
	{
		U32 uMixerID = 0;
		IFXDECLARELOCAL(IFXPalette,pMixerPalette);
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette ));
		IFXRESULT iFindResult = pMixerPalette->Find( &stringMotionName,&uMixerID );
		// if not found, add it to the palette
		if ( IFXFAILURE( iFindResult ) ) {
			IFXCHECKX(pMixerPalette->Add( &stringMotionName,&uMixerID ));
		}

		// now, try to get the motion resource pointer
		IFXDECLARELOCAL(IFXMixerConstruct, pMixerConstruct );
		IFXRESULT iGetResourceResult = pMixerPalette->GetResourcePtr(uMixerID,IID_IFXMixerConstruct,(void**)&pMixerConstruct);
		// if there was no resource pointer, then create one
		if (IFXFAILURE(iGetResourceResult)) {
			IFXCHECKX(IFXCreateComponent( CID_IFXMixerConstruct, IID_IFXMixerConstruct,( void** )&pMixerConstruct ));
			IFXASSERT(pMotionResource);
			pMixerConstruct->SetMotionResource(pMotionResource);

			// Set the scenegraph
			IFXCHECKX(pMixerConstruct->SetSceneGraph( pSceneGraph ));
			pMixerConstruct->SetExternalFlag(m_bExternal);
			pMixerConstruct->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

			// set the resource pointer in the palette
			IFXDECLARELOCAL(IFXUnknown,pUnknown);
			IFXCHECKX(pMixerConstruct->QueryInterface(IID_IFXUnknown, (void**)&pUnknown));
			IFXCHECKX(pMixerPalette->SetResourcePtr( uMixerID,pUnknown ));
		}
	}
}

// Provide next block of data to the loader
void CIFXMotionDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

	if (NULL == m_pObject) {
		U32 uBlockType;
		rDataBlockX.GetBlockTypeX(uBlockType);

		IFXDECLARELOCAL(IFXUnknown,pObject);
		IFXCHECKX(IFXCreateComponent( CID_IFXMotionResource, IID_IFXUnknown, (void**)&pObject ));
		IFXDECLARELOCAL(IFXMarker,pMarker );
		IFXCHECKX(pObject->QueryInterface( IID_IFXMarker, (void**)&pMarker ));
		IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pSceneGraph ));
		IFXCHECKX(pMarker->SetSceneGraph( pSceneGraph ));
		pMarker->SetExternalFlag(m_bExternal);
		pMarker->SetPriority(rDataBlockX.GetPriorityX(), FALSE, FALSE);

		IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );

		IFXString sBlockName;
		pBitStreamX->ReadIFXStringX( sBlockName );
		IFXDECLARELOCAL(IFXNameMap, pNameMap);
		m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
		IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::MOTION, sBlockName));

		IFXDECLARELOCAL(IFXPalette,pSGPalette);
		IFXCHECKX(pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pSGPalette ));

		U32 uResourceID;
		IFXRESULT iResultPaletteFind = pSGPalette->Find( &sBlockName, &uResourceID );
		if ( IFX_E_CANNOT_FIND == iResultPaletteFind  ) {
			IFXCHECKX(pSGPalette->Add( &sBlockName, &uResourceID ));
		}
		IFXCHECKX(pSGPalette->SetResourcePtr( uResourceID, pObject ));

		pObject->AddRef();
		IFXRELEASE(m_pObject);
		m_pObject = pObject;
	}
}

// Update the motion resource palette
void CIFXMotionDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) {

			// Determine the block type
			U32 uBlockType = 0;
			pDataBlockX->GetBlockTypeX( uBlockType );

			// Process the data block
			switch ( uBlockType ) {
				case BlockType_ResourceMotionU3D:
					ProcessMotionCompressedX( *pDataBlockX );
					break;

				default:
					IFXCHECKX(IFX_E_UNSUPPORTED);
					break;
			}
		}
	}

	rWarningPartialTransfer = IFX_OK;
}

// Factory function.
IFXRESULT IFXAPI_CALLTYPE CIFXMotionDecoder_Factory(  IFXREFIID interfaceId, void** ppInterface  )
{
	IFXRESULT rc = IFX_OK;

	if (  ppInterface  )
	{
		// Create the CIFXMotionLoader component.
		CIFXMotionDecoder *pComponent = new CIFXMotionDecoder;

		if (  pComponent  )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface(  interfaceId, ppInterface  );

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

	IFXRETURN( rc );
}
