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
	@file	IFXMotionResource.h

			Declaration of IFXMotionResource interface.
			The IFXMotionResource interface is used to get to animation data.
			The animation data is stored in separate tracks.
			Each track generally corresponds to the animation of one bone.
*/
#ifndef IFXMOTIONRESOURCE_H__
#define IFXMOTIONRESOURCE_H__

#include "IFXUnknown.h"
#include "IFXInstant.h"
#include "IFXKeyFrame.h"
#include "IFXMarkerX.h"

class IFXMotion;

// {5CC4FF83-1794-11d4-8ED6-009027A6D7B4}
IFXDEFINE_GUID(IID_IFXMotionResource,
0x5cc4ff83, 0x1794, 0x11d4, 0x8e, 0xd6, 0x0, 0x90, 0x27, 0xa6, 0xd7, 0xb4);

/**
The IFXMotionResource interface is used to get to animation data.
The animation data is stored in separate tracks.Each track generally corresponds to the animation of one bone.
*/
class IFXMotionResource : virtual public IFXMarkerX
{
	public:
		//*			`Track operations'

		/// Gets the number of tracks.
virtual IFXRESULT IFXAPI 	GetTrackCount(U32 *puTrackCount)						=0;

		/// Finds track index, given the name.
virtual IFXRESULT IFXAPI 	FindTrack(IFXString *pTrackName, U32 *puTrackID)		=0;

		/// Adds a new track by name.  The track index is returned.
virtual IFXRESULT IFXAPI 	AddTrack(IFXString *pTrackName, U32 *puTrackID)			=0;

		/// Gets a name of a track.
virtual IFXRESULT IFXAPI 	GetTrackName(U32 uTrackID, IFXString *pTrackName)		=0;

		//*			`Key frame operations'

		/// Removes all key frames from a track.
virtual IFXRESULT IFXAPI 	ClearTrack(U32 uTrackID)								=0;

		/// Removes any troublesome or redundant data
virtual IFXRESULT IFXAPI 	FilterAndCompress(F32 deltatime=0.01f,
						F32 deltalocation=0.01f,F32 deltarotation=0.001f,
						F32 deltascale=0.01f)								=0;

		/// Adds key frames to track.
virtual IFXRESULT IFXAPI 	InsertKeyFrames(U32 uTrackID,U32 uKeyFrameCount,
										IFXKeyFrame *pKeyFrameArray)		=0;
		/// Gets the number of key frames from track.
virtual IFXRESULT IFXAPI 	GetKeyFrameCount(U32 uTrackID, U32 *puKeyFrameCount)	=0;
		/** Gets a specified number of key frames from track starting with
			a specified index.  The preallocated size of the array should
			equal the requested number of key frames. */
virtual IFXRESULT IFXAPI 	GetKeyFrames(U32 uTrackID,U32 uKeyFrameStart,
						U32 uKeyFrameCount,IFXKeyFrame *pKeyFrameArray)		=0;

		/** Gets a calculated frame from track (not necessarily a key frame).
			The frame which is returned will have an interpolated location,
			rotation, and scale if there is no exact key frame for the
			input time. */
virtual IFXRESULT IFXAPI 	GetInterpolatedFrame(U32 uTrackID,F32 fTime,
												IFXKeyFrame *pKeyFrame)		=0;

		/// Gets the limiting overall duration of all the tracks in the motion.
virtual IFXRESULT IFXAPI 	GetDuration(F32* pOutDuration)							=0;
		/// Gets the duration of a single indexed track.
virtual IFXRESULT IFXAPI 	GetDuration(U32 uTrackID, F32* pOutDuration)			=0;

		/// Internal: Accesses the IFXMotion directly.
virtual IFXMotion*	IFXAPI GetMotionRef(void)										=0;
};

#endif
