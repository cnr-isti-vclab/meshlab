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
	@file	IFXSkeletonMixer.h

			Declaration of IFXSkeletonMixer interface.
			Used to map motions to bones.
*/

#ifndef IFXSKELETONMIXER_H
#define IFXSKELETONMIXER_H

#include "IFXUnknown.h"
#include "IFXBones.h"
class IFXSkeletonModifierParam;
class IFXModel;

// {A1A6F423-2BE7-11d4-BE4C-00A0C9D6AB25}
IFXDEFINE_GUID(IID_IFXSkeletonMixer,
0xa1a6f423, 0x2be7, 0x11d4, 0xbe, 0x4c, 0x0, 0xa0, 0xc9, 0xd6, 0xab, 0x25);

/**
	The motion mixer maps tracks from motions to the character.
	The simplest method maps all tracks in one motion to bones with
	matching names.

	In addition, you can map specific tracks by name to specific bones,
	either individually or by sub-hierarchy.

	Note that the potential "mixing capabilities" of taking weighted parts of
	tracks (a non-temporal blend) inside the mixer is not presently supported,
	so the mixer is really just a mapper.
*/
class IFXSkeletonMixer : virtual public IFXUnknown
{
public:
	enum IFXSX_EventState
	{ IFXSX_Normal,IFXSX_WaitStart,IFXSX_WaitEnd};

	virtual	IFXRESULT IFXAPI 	MapFullMotion(U32 motionId)								=0;
					/// Maps sub-hierarchy of motion under bone.
	virtual IFXRESULT IFXAPI 	MapSubMotion(U32 motionId,long fromboneid)				=0;
					/// Maps a specific named motion track to an indexed bone.
	virtual	IFXRESULT IFXAPI 	MapMotionTrackToBone(U32 motionId,const IFXString& trackname,
																U32 boneID)	=0;
					/// Marks that initialization should default for timeOffset.
	virtual	BOOL&	IFXAPI DefaultOffset(void)									=0;
					/** Accesses the mixer's time alignment, potentially
						continuously changing frame to frame. */
	virtual	F32&	IFXAPI TimeOffset(void)										=0;
					/// Accesses the modified start time.
	virtual	F32&	IFXAPI LocalStartTime(void)									=0;
					/// Accesses the modified end time (<0 uses default).
	virtual	F32&	IFXAPI LocalEndTime(void)										=0;
					/// Delayed trigger to send events to scheduler.
	virtual	IFXSX_EventState &EventState(void)									=0;
					/// Accesses how time changes are scaled for this mixer.
	virtual	F32&	IFXAPI TimeScale(void)										=0;
					/** Determines whether this mixer repeats or just ends
						when finished. */
	virtual	BOOL&	IFXAPI Loop(void)												=0;
					/** Determines whether this mixer attempts to align with
						adjacent mixers using percent completed. */
	virtual	BOOL&	IFXAPI Sync(void)												=0;
					/// Determines whether this mixer has started running.
	virtual	BOOL&	IFXAPI Running(void)											=0;
					/** Determines whether this mixer is waiting for a modifier
						to complete loading. */
	virtual	BOOL&	IFXAPI DelayMap(void)											=0;
					/** Determines whether this mixer has triggered a start event
						but not yet a corresponding end event. */
	virtual	BOOL&	IFXAPI Started(void)											=0;
					/// Gets the collective time bounds of tracks used by mixer.
	virtual IFXRESULT IFXAPI 	GetMotionTimeLimits(F32 *pMin,F32 *pMax)				=0;

					/// Gets a bone's transform relative to it's parent.
					/// (All transform access is copy-based, not direct access.)
	virtual	IFXRESULT IFXAPI 	GetRelativeTransform(U32 boneID,F32 *pMatrix16)			=0;
					/// Sets a bone's transform relative to it's parent.
	virtual	IFXRESULT IFXAPI 	SetRelativeTransform(U32 boneID,F32 *pMatrix16)			=0;
					/// Sets a bone's transform in absolute world space.
	virtual	IFXRESULT IFXAPI 	GetWorldTransform(U32 boneID,F32 *pMatrix16)			=0;

					/** Gets the name of the motion first applied, probably
						due to MapFullMotion. */
	virtual	IFXString IFXAPI 	GetPrimaryMotionName(void)								=0;
					/** Sets the name of the motion first applied.
						(intended for internal use, but can be used to
						force a motion name). */
	virtual	void	 IFXAPI 	SetPrimaryMotionName(const IFXString& name)					=0;

					/// Intended for internal use by IFXAnimationModifierParam.
	virtual void	 IFXAPI 	SetModel(IFXModel *pModel)								=0;
					/// Intended for internal use by IFXAnimationModifierParam.
	virtual void	 IFXAPI 	SetParam(IFXSkeletonModifierParam *pParam)				=0;
					/// Intended for internal use by IFXAnimationModifierParam.
	virtual	IFXMotionMixer*	IFXAPI GetMotionMixer(void)								=0;
};


#endif
