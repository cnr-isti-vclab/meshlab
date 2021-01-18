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
	@file	CIFXMotionResourceEncoder.cpp

			Implementation of the CIFXMotionResourceEncoder.
			The CIFXMotionResourceEncoder contains motion resource encoding
			functionality that is used by the write manager.
*/

#include "CIFXMotionResourceEncoder.h"
#include "IFXACContext.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXMotionResource.h"
#include "IFXMetaDataX.h"
#include "IFXAutoRelease.h"
#include <float.h> // for FLT_MAX


// constructor
CIFXMotionResourceEncoder::CIFXMotionResourceEncoder()
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXMotionResourceEncoder::~CIFXMotionResourceEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXMotionResourceEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXMotionResourceEncoder::Release()
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

IFXRESULT CIFXMotionResourceEncoder::QueryInterface( IFXREFIID  interfaceId,
													void**   ppInterface )
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
void CIFXMotionResourceEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units)
{
	IFXDataBlockX*	pDataBlock = NULL;
	IFXKeyFrame*    pKeyFrameArray = NULL;
	IFXMarkerX*     pMarker = NULL;
	IFXMotionResource*  pMotionResource = NULL;
	U32         i = 0;
	U32         j = 0;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXMotionResource, (void**)&pMotionResource ) );

		U32 uQualityFactor = 0;
		IFXCHECKX( m_pObject->QueryInterface(IID_IFXMarkerX, (void**)&pMarker) );
		pMarker->GetQualityFactorX( uQualityFactor );
		IFXRELEASE( pMarker );

		// Write the motion track description data to the bitstream object
		// The motion description data block has the following sections:
		// 1. MotionName
		// 2. TrackCount
		// 3. InverseQuantTime
		// 4. InverseQuantRotation
		// 5. TrackName
		// 6. TimeCount
		// 7. InverseQuantDisplacement
		// 8. InverseQuantScale
		// 9. Time
		// 10. Displacement
		// 11. Rotation
		// 12. Scale

		// 1. MotionName
		m_pBitStream->WriteIFXStringX(rName);

		// 2. TrackCount
		U32 uTrackCount = 0;
		IFXCHECKX( pMotionResource->GetTrackCount(&uTrackCount) );
		m_pBitStream->WriteU32X(uTrackCount);

		// 3. InverseQuantTime
		F32 fQuantTime = 333.3f;
		F32 fInverseQuantTime = (F32) (1.0 / fQuantTime);
		m_pBitStream->WriteF32X(fInverseQuantTime);

		// 4. InverseQuantRotation
		F32 fQuantRotation = 3600.0f;
		if ( 1000 == uQualityFactor )
			fQuantRotation = (F32) pow(2.0,16.0);
		else
			fQuantRotation = (F32) pow(1.0027820076332643836073011110611,uQualityFactor+1995.0);

		F32 fInverseQuantRotation = (F32) (1.0 / fQuantRotation);

		m_pBitStream->WriteF32X(fInverseQuantRotation);

		// apply units scaling factor used for writing the file
		F32 invertedUSF = 1/(F32)units;

		for( i=0; i<uTrackCount; i++)
		{
			//  5. TrackName
			IFXString stringTrackName;
			IFXCHECKX( pMotionResource->GetTrackName(i,&stringTrackName) );
			m_pBitStream->WriteIFXStringX(stringTrackName);

			//  6. TimeCount
			U32 uTimeCount = 0;
			IFXCHECKX( pMotionResource->GetKeyFrameCount(i,&uTimeCount) );
			m_pBitStream->WriteU32X(uTimeCount);

			// Allocate space for the key frame data
			pKeyFrameArray = new IFXKeyFrame[uTimeCount];

			if ( NULL == pKeyFrameArray )
				throw IFXException( IFX_E_OUT_OF_MEMORY );

			// Get the key frame data
			IFXCHECKX( pMotionResource->GetKeyFrames(i,0,uTimeCount,pKeyFrameArray) );
			for( j = 0; j < uTimeCount; j++ )
			{
				pKeyFrameArray[j].Location().Scale( invertedUSF );
			}

			// 7. InverseQuantDisplacement
			F32 fQuantDisplacement = 300.0f;
			if ( 1000 == uQualityFactor )
				fQuantDisplacement = (F32) pow(2.0,15.0);
			else
				fQuantDisplacement = (F32) pow(1.0020857809803532823296165503372,uQualityFactor+1995.0);

			// Calculate a bounding volume for the displacement differences
			IFXVector3 vDiffMin(FLT_MAX,FLT_MAX,FLT_MAX);
			IFXVector3 vDiffMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			IFXVector3 vPredictedDisplacement(0.0f,0.0f,0.0f);
			IFXVector3 vDiffDisplacement(0.0f,0.0f,0.0f);
			IFXVector3 vDisplacement(0.0f,0.0f,0.0f);

			for ( j=0; j<uTimeCount; j++)
			{
				// Note that this is a rough calcualtion
				// since the differences are not quantized
				vDisplacement = pKeyFrameArray[j].Location();
				vDiffDisplacement.Subtract(vDisplacement,vPredictedDisplacement);

				if ( vDiffDisplacement.X() < vDiffMin.X() )
					vDiffMin.X() = vDiffDisplacement.X();

				if ( vDiffDisplacement.Y() < vDiffMin.Y() )
					vDiffMin.Y() = vDiffDisplacement.Y();

				if ( vDiffDisplacement.Z() < vDiffMin.Z() )
					vDiffMin.Z() = vDiffDisplacement.Z();

				if ( vDiffDisplacement.X() > vDiffMax.X() )
					vDiffMax.X() = vDiffDisplacement.X();

				if ( vDiffDisplacement.Y() > vDiffMax.Y() )
					vDiffMax.Y() = vDiffDisplacement.Y();

				if ( vDiffDisplacement.Z() > vDiffMax.Z() )
					vDiffMax.Z() = vDiffDisplacement.Z();

				vPredictedDisplacement = pKeyFrameArray[j].Location();
			}

			// Calculate the length of the diagonal of the bounding box.
			IFXVector3 vDiagonal;
			vDiagonal.Subtract(vDiffMax,vDiffMin);
			F32 fBoxLength = vDiagonal.CalcMagnitude();

			// Calculate the quantization factor based on the box length.
			if ( fBoxLength > 1.0f )
				fQuantDisplacement /= fBoxLength;

			if ( fQuantDisplacement < 1.0f )
				fQuantDisplacement = 1.0f;

			F32 fInverseQuantDisplacement = (F32) (1.0 / fQuantDisplacement);
			m_pBitStream->WriteF32X(fInverseQuantDisplacement);

			// 8. InverseQuantScale
			F32 fQuantScale = 100.0f;
			if ( 1000 == uQualityFactor )
				fQuantScale = (F32) pow(2.0,15.0);
			else
				fQuantScale = (F32) pow(1.0017378489449164679289500960562,uQualityFactor+1995.0);

			F32 fInverseQuantScale = (F32) (1.0 / fQuantScale);

			m_pBitStream->WriteF32X(fInverseQuantScale);


			IFXKeyFrame thePredictedKeyFrame;

			for( j=0; j<uTimeCount; j++)
			{
				//  9. Time
				F32 fTime = pKeyFrameArray[j].Time();
				F32 fPredictedTime = thePredictedKeyFrame.Time();
				F32 fDiffTime = fTime - fPredictedTime;

				if ( 0==j || (uTimeCount-1) == j )
				{
					m_pBitStream->WriteF32X(fDiffTime);
				}
				else
				{
					U32 uDiffTime = (U32) (0.5 + fQuantTime*fabs(fDiffTime));
					U8 u8SignTime = (0.0 > fDiffTime);
					m_pBitStream->WriteCompressedU8X(uACContextMotionSignTime,u8SignTime);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffTime,uDiffTime);

					fDiffTime = (fInverseQuantTime*uDiffTime);
					if ( u8SignTime & 1 )
						fDiffTime *= -1.0f;
				}

				fTime = fDiffTime + fPredictedTime;
				pKeyFrameArray[j].SetTime( fTime );


				//  10. Displacement
				IFXVector3 vDisplacement = pKeyFrameArray[j].Location();
				IFXVector3 vPredictedDisplacement = thePredictedKeyFrame.Location();
				IFXVector3 vDiffDisplacement(0.0f,0.0f,0.0f);
				vDiffDisplacement.Subtract(vDisplacement,vPredictedDisplacement);

				if ( 0==j || (uTimeCount-1) == j  )
				{
					m_pBitStream->WriteF32X(vDiffDisplacement.X());
					m_pBitStream->WriteF32X(vDiffDisplacement.Y());
					m_pBitStream->WriteF32X(vDiffDisplacement.Z());
				}
				else
				{
					U8 u8SignDisplacement = ((0.0 > vDiffDisplacement.X())) | ((0.0 > vDiffDisplacement.Y()) << 1) | ((0.0 > vDiffDisplacement.Z()) << 2);
					U32 udX = (U32) (0.5 + fQuantDisplacement*fabs(vDiffDisplacement.X()));
					U32 udY = (U32) (0.5 + fQuantDisplacement*fabs(vDiffDisplacement.Y()));
					U32 udZ = (U32) (0.5 + fQuantDisplacement*fabs(vDiffDisplacement.Z()));

					m_pBitStream->WriteCompressedU8X(uACContextMotionSignDisplacement,u8SignDisplacement);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffDisplacement,udX);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffDisplacement,udY);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffDisplacement,udZ);

					F32 fdX = fInverseQuantDisplacement*udX;
					F32 fdY = fInverseQuantDisplacement*udY;
					F32 fdZ = fInverseQuantDisplacement*udZ;

					if ( u8SignDisplacement & 1 )
						fdX *= -1.0;

					if ( u8SignDisplacement & 2 )
						fdY *= -1.0;

					if ( u8SignDisplacement & 4 )
						fdZ *= -1.0;

					IFXVector3 vDisplacementQ(0.0f,0.0f,0.0f);
					IFXVector3 vPredictedDisplacementQ = thePredictedKeyFrame.Location();
					IFXVector3 vDiffDisplacementQ(fdX,fdY,fdZ);
					vDisplacementQ.Add(vDiffDisplacementQ,vPredictedDisplacementQ);
					pKeyFrameArray[j].Location() = vDisplacementQ;
				}

				//  11. Rotation
				IFXQuaternion vRotation = pKeyFrameArray[j].Rotation();
				IFXQuaternion vPredictedRotation = thePredictedKeyFrame.Rotation();
				vPredictedRotation.Invert();
				IFXQuaternion vDiffRotation(1.0f,0.0f,0.0f,0.0f);
				vDiffRotation.Multiply(vPredictedRotation,vRotation);
				vPredictedRotation.Invert();
				F32 *pfRot = vDiffRotation.Raw();

				if ( NULL == pfRot )
				{
					IFXASSERT(0);
					throw IFXException( IFX_E_UNDEFINED );
				}

				U8 u8SignRot = ((0.0 > pfRot[0])) | ((0.0 > pfRot[1]) << 1) | ((0.0 > pfRot[2]) << 2) | ((0.0 > pfRot[3]) << 3);
				U32 uB = (U32) (0.5 + fQuantRotation*fabs(pfRot[1]));
				U32 uC = (U32) (0.5 + fQuantRotation*fabs(pfRot[2]));
				U32 uD = (U32) (0.5 + fQuantRotation*fabs(pfRot[3]));

				F32 fb = fInverseQuantRotation*uB;
				F32 fc = fInverseQuantRotation*uC;
				F32 fd = fInverseQuantRotation*uD;

				if(fb > 1.0f) fb = 1.0f;
				if(fc > 1.0f) fc = 1.0f;
				if(fd > 1.0f) fd = 1.0f;

				F32 fa = (F32) sqrt(fabs(1.0 - ((F64)fb)*((F64)fb) - ((F64)fc)*((F64)fc) - ((F64)fd)*((F64)fd)));

				if ( u8SignRot & 1 )
					fa *= -1.0;

				if ( u8SignRot & 2 )
					fb *= -1.0;

				if ( u8SignRot & 4 )
					fc *= -1.0;

				if ( u8SignRot & 8 )
					fd *= -1.0;


				vDiffRotation.Set( fa,fb,fc,fd );
				//vDiffRotation.Normalize();
				IFXQuaternion vRotationQ;
				vRotationQ.Multiply(vPredictedRotation,vDiffRotation);
				//vRotationQ.Normalize();
				pKeyFrameArray[j].Rotation() = vRotationQ;

				if( 0==j || (uTimeCount-1) == j )
				{
					pKeyFrameArray[j].Rotation() = vRotation;
					F32 *pfOrigRot = vRotation.Raw();

					m_pBitStream->WriteF32X(pfOrigRot[0]);
					m_pBitStream->WriteF32X(pfOrigRot[1]);
					m_pBitStream->WriteF32X(pfOrigRot[2]);
					m_pBitStream->WriteF32X(pfOrigRot[3]);
				}
				else
				{
					m_pBitStream->WriteCompressedU8X(uACContextMotionSignRotation,u8SignRot);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffRotation,uB);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffRotation,uC);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffRotation,uD);
				}

				pfRot = NULL;

				// 12. Scale
				IFXVector3 vScale = pKeyFrameArray[j].Scale();
				IFXVector3 vPredictedScale = thePredictedKeyFrame.Scale();
				IFXVector3 vDiffScale(0.0f,0.0f,0.0f);
				vDiffScale.Subtract(vScale,vPredictedScale);

				if ( 0==j || (uTimeCount-1) == j )
				{
					m_pBitStream->WriteF32X(vDiffScale.X());
					m_pBitStream->WriteF32X(vDiffScale.Y());
					m_pBitStream->WriteF32X(vDiffScale.Z());
				}
				else
				{
					U8 u8SignScale = ((0.0 > vDiffScale.X())) | ((0.0 > vDiffScale.Y()) << 1) | ((0.0 > vDiffScale.Z()) << 2);
					U32 udsX = (U32) (0.5 + fQuantScale*fabs(vDiffScale.X()));
					U32 udsY = (U32) (0.5 + fQuantScale*fabs(vDiffScale.Y()));
					U32 udsZ = (U32) (0.5 + fQuantScale*fabs(vDiffScale.Z()));

					m_pBitStream->WriteCompressedU8X(uACContextMotionSignScale,u8SignScale);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffScale,udsX);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffScale,udsY);
					m_pBitStream->WriteCompressedU32X(uACContextMotionDiffScale,udsZ);

					F32 fdsX = fInverseQuantScale*udsX;
					F32 fdsY = fInverseQuantScale*udsY;
					F32 fdsZ = fInverseQuantScale*udsZ;

					if ( u8SignScale & 1 )
						fdsX *= -1.0;

					if ( u8SignScale & 2)
						fdsY *= -1.0;

					if ( u8SignScale & 4 )
						fdsZ *= -1.0;

					IFXVector3 vScaleQ(0.0f,0.0f,0.0f);
					IFXVector3 vPredictedScaleQ = thePredictedKeyFrame.Scale();
					IFXVector3 vDiffScaleQ(fdsX,fdsY,fdsZ);
					vScaleQ.Add(vDiffScaleQ,vPredictedScaleQ);
					pKeyFrameArray[j].Scale() = vScaleQ;
				}

				// Set the prediction for the next key frame
				thePredictedKeyFrame = pKeyFrameArray[j];
			}

			// Clear the old key frame array from the motion resource
			IFXCHECKX( pMotionResource->ClearTrack(i) );

			// Set the key frame array to the motion resource
			IFXCHECKX( pMotionResource->InsertKeyFrames(i,uTimeCount,pKeyFrameArray) );

			IFXDELETE_ARRAY( pKeyFrameArray );
		}

		// get the block
		m_pBitStream->GetDataBlockX( pDataBlock );

		// set the data block type
		pDataBlock->SetBlockTypeX( BlockType_ResourceMotionU3D );

		// set the priority on the datablock
		pDataBlock->SetPriorityX( 0 );

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
		IFXRELEASE( pMotionResource );
	}
	catch ( ... )
	{
		IFXRELEASE( pDataBlock );
		IFXRELEASE( pMarker );
		IFXRELEASE( pMotionResource );

		IFXDELETE_ARRAY( pKeyFrameArray );

		throw;
	}
}

void CIFXMotionResourceEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{
		// latch onto the core services object passed in
		IFXRELEASE( m_pCoreServices )
			m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();

		// create a bitstream (compressed)
		IFXRELEASE( m_pBitStream );
		IFXCHECKX( IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamCompressedX, (void**)&m_pBitStream ) );
		U32 uProfile;
		m_pCoreServices->GetProfile(uProfile);
		m_pBitStream->SetNoCompressionMode((uProfile & IFXPROFILE_NOCOMPRESSION) ? TRUE : FALSE);

		m_bInitialized = TRUE;
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		throw;
	}
}

void CIFXMotionResourceEncoder::SetObjectX( IFXUnknown& rObject )
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
IFXRESULT IFXAPI_CALLTYPE CIFXMotionResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXMotionResourceEncoder *pComponent = new CIFXMotionResourceEncoder;

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
