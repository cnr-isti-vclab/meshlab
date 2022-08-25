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
	@file	CIFXMotionResource.h

			Implementation of IFXMotionResource interface.
			The IFXMotionResource interface is used to get to animation data.
			The animation data is stored in separate tracks.
			Each track generally corresponds to the animation of one bone.
*/

#ifndef CIFXMOTIONRESOURCE_H__
#define CIFXMOTIONRESOURCE_H__

#include "IFXMotionResource.h"
#include "CIFXSubject.h"
#include "CIFXMarker.h"
#include "IFXListContext.h"

class IFXMotion;

class CIFXMotionResource : private CIFXSubject,
                           private CIFXMarker,
                   virtual public  IFXMotionResource
{
public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// IFXMarker
	IFXRESULT IFXAPI SetSceneGraph( IFXSceneGraph* pInSceneGraph );

	// IFXMarkerX interface...
	void IFXAPI 		GetEncoderX (IFXEncoderX*& rpEncoderX);

	/// @name Track operations
	//@{
	// Get number of tracks
	virtual IFXRESULT IFXAPI  GetTrackCount(U32 *puTrackCount);
	
	/// Find track id
	virtual IFXRESULT IFXAPI  FindTrack(IFXString *pTrackName, U32 *puTrackID);
	
	/// Add a new track
	virtual IFXRESULT IFXAPI  AddTrack(IFXString *pTrackName, U32 *puTrackID);

	/// Get name of track
	virtual IFXRESULT IFXAPI  GetTrackName(U32 uTrackID, IFXString *pTrackName);
	//@}

	///	@name Key frame operations
	//@{
	/// remove all frames from a track
	virtual IFXRESULT IFXAPI  ClearTrack(U32 uTrackID);

	/// Remove any troublesome or redundant data
	virtual IFXRESULT IFXAPI  FilterAndCompress(F32 deltatime=0.01f,
							F32 deltalocation=0.01f,F32 deltarotation=0.001f,
							F32 deltascale=0.01f);

	/// Add key frames to track
	virtual IFXRESULT IFXAPI  InsertKeyFrames(U32 uTrackID, U32 uKeyFrameCount, IFXKeyFrame *pKeyFrameArray);

	/// Get key frames from track
	virtual IFXRESULT IFXAPI  GetKeyFrameCount(U32 uTrackID, U32 *puKeyFrameCount);
	virtual IFXRESULT IFXAPI  GetKeyFrames(U32 uTrackID,U32 uKeyFrameStart,
							U32 uKeyFrameCount,IFXKeyFrame *pKeyFrameArray);

	/** 
		Get frame from track (not necessarily a key frame)
		The frame which is returned will have an interpolated rotation and 
		displacement if there is no key frame for the input time.
	*/
	virtual IFXRESULT IFXAPI  GetInterpolatedFrame(U32 uTrackID, F32 fTime, IFXKeyFrame *pKeyFrame);

	virtual IFXRESULT IFXAPI  GetDuration(U32 uTrackID, F32* pOutDuration);

	virtual IFXRESULT IFXAPI  GetDuration(F32* pOutDuration);

	virtual IFXMotion* IFXAPI GetMotionRef( void );
	//@}

private:
	          CIFXMotionResource();
	virtual  ~CIFXMotionResource();
	friend 
	IFXRESULT IFXAPI_CALLTYPE CIFXMotionResource_Factory( IFXREFIID interfaceId, 
	                                      void**    ppInterface );
	// IFXUnknown 
	U32            m_refCount;

	// IFXMotionResource 
	IFXMotion*     m_pMotion;
	IFXListContext m_interpolateContext;
	IFXListContext m_insertContext;
};

#endif 
