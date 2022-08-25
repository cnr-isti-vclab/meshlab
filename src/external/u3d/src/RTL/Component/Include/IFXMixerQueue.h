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

#ifndef IFXMIXERQUEUE_H
#define IFXMIXERQUEUE_H

#include "IFXTransform.h"
#include "IFXMotionMixer.h"

/**
	This class maintains a queue of motions.
*/
class IFXMixerQueue
{
	public:
	virtual ~IFXMixerQueue() {}
	enum LockAxis
	{
		TranslationX=	0x0001,
		TranslationY=	0x0002,
		TranslationZ=	0x0004,
		TranslationAll= 0x0007
	};

	enum Cat
	{
		CatTx=			0x0001,	// push/pop transitions
		CatTy=			0x0002,
		CatTz=			0x0004,
		CatLocation=	0x0007,
		CatRx=			0x0010,
		CatRy=			0x0020,
		CatRz=			0x0040,
		CatRotation=	0x0070,
		CatSx=			0x0100,
		CatSy=			0x0200,
		CatSz=			0x0400,
		CatScale=		0x0700,
		CatAll=			0x0777
	};

						/** Pauses the current motion at the head of the
							queue if it is playing. */
virtual	IFXRESULT	 IFXAPI 	Pause(void)											=0;
						/** Continues the current motion at the head of the
							queue if it has been paused. */
virtual	IFXRESULT	 IFXAPI 	Play(void)											=0;

						/** Pushes a new motion onto the head of the stack.
							If a motion is found by the given name, is
							is activate with the given offset, cropped
							time bounds, scaling, loop flag, and sync flag.
							If delaymap is true, the system will wait until
							the latest moment possible to actually do the
							motion lookup in case it is not available yet.
							If tail is true, the motion is placed at the
							tail of the queue. */
virtual	IFXRESULT	 IFXAPI 	Push(const IFXString& name, F32 *localOffset,
								F32 *localStartTime, F32 *localEndTime,
								F32 *timeScale, BOOL *loop,BOOL *sync,
								BOOL delaymap = false, BOOL tail=false)		=0;
						/// Queue() is just Play() with delaymap set to true.
virtual	IFXRESULT	 IFXAPI 	Queue(const IFXString& name,F32 *localOffset,
								F32 *localStartTime,F32 *localEndTime,
								F32 *timeScale, BOOL *loop,BOOL *sync,
								BOOL delayMap = false)						=0;

						/** Pops the motion on the head of the queue.
							If tail is true, the motion is taken from the
							tail of the queue. */
virtual	IFXRESULT	 IFXAPI 	Pop(BOOL tail = false)								=0;
						/// UnQueue() is just Pop() with delaymap set to true.
virtual	IFXRESULT	 IFXAPI 	UnQueue(void)										=0;

						/** Gets the time from the perspective of indexed motion,
							accounting for the offset and time-scaling. */
virtual	IFXRESULT	 IFXAPI 	GetLocalTime(U32 index, F32 *localtime)				=0;
						/** Adjusts the time from the perspective of indexed
							motion, accounting for time-scaling, and modifying
							the offset. */
virtual	IFXRESULT	 IFXAPI 	SetLocalTime(U32 index, F32 localtime)				=0;

						/** Gets a queued IFXMotionMixer indexed relative to
							the head of queue. */
virtual	IFXMotionMixer	*GetMotionMixerNR(U32 index)						=0;
						/** Gets a queued IFXMotionMixer indexed relative to
							the head of queue, but do not force the name
							mapping if it hasn't occurred yet.  This form
							also does not affect the mixer's reference count
							so the pointer should not be stored for use
							later (immediate use only). */
virtual	IFXMotionMixer	*UseMotionMixerNoMap(U32 index)						=0;
						/** Gets a mixer recent popped from the head of the
							queue, indexed starting from the most recently
							popped mixer.  The mixers will eventually be
							deleted altogether by some internal discretion,
							so a NULL return value is a strong possibility. */
virtual	IFXMotionMixer	*UseHistoricalMixer(U32 index)						=0;

virtual	IFXTransform	&RootTransform(void)								=0;
virtual	IFXTransform	&DefaultTransform(void)								=0;

						/** Accesses the overall time scaling the queue.
							This affects all motions on this queue by
							effectively multiplying by every queued motion's
							specific time scale. */
virtual	F32				&TimeScale(void)									=0;
						/** Accesses the rootlock flag.  This prevents any
							additional translation while it is on. */
virtual	BOOL			&RootLock(void)										=0;
						/** Accesses the flag for playing the motions.
							Zero indicates that all motion is paused. */
virtual	BOOL			&Playing(void)										=0;
						/** Accesses the concatenation setting.  A bit field
							indicates that transitions between motions are
							aligned in space.  See IFXMixerQueue.h for
							constants use to specify the bits field. */
virtual	U32				&Concatenation(void)								=0;
						/** The three least significant bits are used to
							axis lock for X, Y, and Z translation.
							This forces all translation to be relative to
							the axis.  This can be used to eliminate
							cumulative error, either from simple data
							resolution or from uneven motion data. */
virtual	U32				&AxisLock(void)										=0;
						/// unused?
virtual	IFXVector3		&AxisOffset(void)									=0;

						/// Accesses the flag controlling auto-blending
virtual	BOOL			&AutoBlend(void)									=0;
						/// Accesses the time over which motion blending occurs.
virtual	F32				&BlendTime(void)									=0;
						/** Accesses the fractional setting of the current blend.
							This can be quickly overwritten if auto-blending
							is on. */
virtual	F32				&BlendFraction(void)								=0;

						/// Returns the number of motions currently queued.
virtual	U32			 IFXAPI 	GetNumberQueued(void)								=0;
						/** Returns the number of popped motions currently
							retained.  These can be arbitrarily deleted
							during any Advance() call. */
virtual	U32			 IFXAPI 	GetNumberHistory(void)								=0;
						/** Gets the current transform of an indexed bone
							relative to its parent bone. */
virtual IFXRESULT	 IFXAPI 	GetRelativeTransform(U32 boneID,F32 *pMatrix16)		=0;
						/** Sets the current transform of an indexed bone
							relative to its parent bone. */
virtual IFXRESULT	 IFXAPI 	SetRelativeTransform(U32 boneID,F32 *pMatrix16)		=0;

						/** Updates the queue based on any change to the
							current world time stored with the bones manager. */
virtual IFXRESULT	 IFXAPI 	Advance(void)										=0;

						// unused flag?
virtual	U32				&Updated(void)										=0;
};

#endif
