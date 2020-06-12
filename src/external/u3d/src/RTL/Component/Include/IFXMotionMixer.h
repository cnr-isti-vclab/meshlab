//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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
	@file IFXMotionMixer.h 
*/

#ifndef IFXMOTIONMIXER_H
#define IFXMOTIONMIXER_H

class IFXBonesManager;
class IFXVector3;
class IFXQuaternion;
class IFXMotionReader;
class IFXMotionManager;

#include "IFXMatrix4x4.h"

/**
	This class selects motions from multiple tracks or IK. It uses a single IFXMotionManager
	and is used for a specific character.

	@note
	This is a motion mixer, not a motion blender.
	It selects from multiple tracks or IK.
	It does not blend from one motion to another over time.

	Each IFXCharacter can have multiple IFXMotionMixers from which it selects.
	Each mixer uses a single IFXMotionManager for a specific
	character.  You must Reset() before associating a different motion manager
	or bones manager.

	Multiple mixers can draw from the same IFXMotionManager.
	Multiple mixers can be applied by the same IFXBonesManager.
	A specific IFXMotionMixer can only be used on a single IFXMotionManager
	and a single IFXBonesManager.
*/
class IFXMotionMixer
{
	public:
virtual U32		 IFXAPI 	AddRef( void ) = 0;
virtual U32		 IFXAPI 	Release( void ) = 0;
	virtual ~IFXMotionMixer() {}

	public:
		enum		IFXSX_EventState
					{ IFXSX_Normal,IFXSX_WaitStart,IFXSX_WaitEnd};

					// Initialization

					/// Resets manager as though just instantiated.
virtual	IFXRESULT IFXAPI 	Reset(void)												=0;
					/// Chooses the single IFXMotionManager for this mixer.
virtual	IFXRESULT IFXAPI 	AssociateWithMotionManager(IFXMotionManager *set)		=0;
					/// Chooses the single IFXBonesManager for this mixer.
virtual	IFXRESULT IFXAPI 	AssociateWithBonesManager(IFXBonesManager *set)			=0;

					// Setup

					/** Maps the entire indexed motion into mixer,
					assuming matching track and bone names. */
virtual	IFXRESULT IFXAPI 	MapFullMotionToCharacter(I32 motionid)					=0;
					/// Maps a sub-hierarchy starting from a specified bone.
virtual IFXRESULT IFXAPI 	SubMapMotionToCharacter(I32 motionid,I32 fromboneid)	=0;
					/// Maps a specific, named track to a specific, indexed bone. 
virtual	IFXRESULT IFXAPI 	MapMotionTrackToBone(I32 motionid,
										const IFXString& rTrackName,I32 boneid)	=0;
					/** Sets specific, indexed bone to an explicit displacement
					and rotation. */
virtual	IFXRESULT IFXAPI 	SetPositionExplicitly(I32 boneid,
										const IFXVector3 *location,
										const IFXQuaternion *rotation=NULL,
										const IFXVector3 *scale=NULL)		=0;
					/// Gets min and max time of all currently mapped tracks.
virtual IFXRESULT IFXAPI 	GetMotionTimeLimits(F32 *min,F32 *max)				=0;
					/// Turns on or off IK for specific indexed bone.
virtual	IFXRESULT IFXAPI 	ActivateIK(I32 boneid,bool on)							=0;

virtual IFXRESULT IFXAPI 	IncrementTimeOffset(F32 deltaOffset)					=0;
virtual IFXRESULT IFXAPI 	SetTimeOffset(F32 offset)								=0;
virtual IFXRESULT IFXAPI 	GetTimeOffset(F32 *offset) const						=0;

					// Per frame update

					/** Gets current displacement and rotation of specific,
					indexed bone.*/
virtual	IFXRESULT IFXAPI 	GetBonePosition(I32 boneid,IFXVector3 *location,
												IFXQuaternion *rotation=NULL,
												IFXVector3 *scale=NULL)		=0;
					/// Gets model-relative rotation of last deformation.
virtual IFXRESULT IFXAPI 	GetAbsoluteBoneMatrix(I32 boneid,IFXMatrix4x4 *matrix)	=0;
//*	internal:
					/// Internal access only: Gets a bone's position at a specified time.
virtual	IFXRESULT IFXAPI 	CalcBonePositionAtTime(I32 boneid,F32 time,
												IFXVector3 *location,
												IFXQuaternion *rotation=NULL,
												IFXVector3 *scale=NULL)		=0;

					/** Internal access only:
					Applies motion in all mapped tracks at specified time.*/
virtual IFXRESULT IFXAPI 	ApplyMotion(F32 time,bool toblend)						=0;
					/** Internal access only:
					Applies IK to all IK-activated bones. */
virtual IFXRESULT IFXAPI 	ApplyIK(void)											=0;
					/** Internal access only:
					For a given bone ID, gets the associated IFXMotionReader.
					You must be able to declare an IFXMotionReader to extract the data.*/
virtual IFXRESULT IFXAPI 	GetMotionReader(I32 boneid,IFXMotionReader **reader)	=0;

					//	From Rushmore
					/// Marks initialization default for timeOffset.
virtual BOOL		&DefaultOffset(void)									=0;
					/** Accesses the mixer's time alignment, 
						continuously changing frame to frame */
virtual F32			&QueueOffset(void)										=0;
					/// Accesses modified start time.
virtual F32			&LocalStartTime(void)									=0;
					/// Accesses modified end time (<0 uses default).
virtual F32			&LocalEndTime(void)										=0;
					/// Delayed trigger to send events to scheduler.
virtual IFXSX_EventState &EventState(void)									=0;
					/// Accesses how time changes are scaled for this mixer
virtual F32			&TimeScale(void)										=0;
					/** Accesses whether this mixer repeats or just ends
						when finished. */
virtual BOOL		&Loop(void)												=0;
					/** Accesses whether this mixer attempts to align with
						adjacent mixers using percent completed. */
virtual BOOL		&Sync(void)												=0;
					/// Gets whether this mixer has started running yet.
virtual BOOL		&Running(void)											=0;
					/** Gets whether this mixer is waiting for a modifier
						to complete loading. */
virtual BOOL		&DelayMap(void)											=0;
					/** Gets whether this mixer has trigger a start event
						but not yet a corresponding end event. */
virtual BOOL		&Started(void)											=0;

					/** Gets a bone's transform relative to it's parent.
					(All transform access is copy-based, not direct access.)*/
virtual IFXRESULT IFXAPI 	GetRelativeTransform(U32 boneID,F32 *pMatrix16)			=0;
					/// Sets a bone's transform relative to it's parent.
virtual IFXRESULT IFXAPI 	SetRelativeTransform(U32 boneID,F32 *pMatrix16)			=0;

					/** Gets the name of the motion that was first applied, probably
						due to MapFullMotion. */
virtual IFXString IFXAPI 	GetPrimaryMotionName(void)								=0;
					/** Sets the name of the motion that was first applied.
						(Intended for internal use, but can be used to
						force a motion name.) */
virtual void	 IFXAPI 	SetPrimaryMotionName(const IFXString& rName)			=0;
};

#endif
