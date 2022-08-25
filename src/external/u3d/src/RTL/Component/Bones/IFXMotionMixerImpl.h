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
	@file IFXMotionMixerImpl.h
*/

#ifndef IFXMOTIONMIXERIMPL_H
#define IFXMOTIONMIXERIMPL_H

#include "IFXMotionMixer.h"
#include "IFXCoreNode.h"
#include "IFXVoidStar.h"
#include "IFXMotionReader.h"

class IFXMotionMixerImpl : public IFXMotionMixer
{
public:
	virtual U32 IFXAPI AddRef( void );
	virtual U32 IFXAPI Release( void );

	IFXMotionMixerImpl(void);
	virtual ~IFXMotionMixerImpl(void);

virtual IFXRESULT IFXAPI Reset(void);

virtual IFXRESULT IFXAPI AssociateWithMotionManager(IFXMotionManager *set);
/// alternative to pick motions without a motion manager
virtual IFXRESULT IFXAPI UseCustomMotionArray(I32 length,IFXMotion *motion);

virtual IFXRESULT IFXAPI AssociateWithBonesManager(IFXBonesManager *set);
virtual IFXRESULT IFXAPI MapFullMotionToCharacter(I32 motionid);
virtual IFXRESULT IFXAPI SubMapMotionToCharacter(I32 motionid,I32 fromboneid);
virtual IFXRESULT IFXAPI MapMotionTrackToBone(I32 motionid,const IFXString& rTrackName,
																I32 boneid);

virtual IFXRESULT IFXAPI GetMotionTimeLimits(F32 *min,F32 *max);

virtual IFXRESULT IFXAPI SetPositionExplicitly(
	I32 boneid,
	const IFXVector3 *location,
	const IFXQuaternion *rotation,
	const IFXVector3 *scale
	);

virtual IFXRESULT IFXAPI GetBonePosition(
	I32 boneid,
	IFXVector3 *location,
	IFXQuaternion *rotation,
	IFXVector3 *scale);

virtual IFXRESULT IFXAPI GetAbsoluteBoneMatrix(I32 boneid, IFXMatrix4x4* pMatrix);
virtual IFXRESULT IFXAPI ActivateIK(I32 boneid, bool on);

virtual IFXRESULT IFXAPI ApplyMotion(F32 time,bool toblend);
virtual IFXRESULT IFXAPI ApplyIK(void);
virtual IFXRESULT IFXAPI GetMotionReader(I32 boneid,IFXMotionReader **reader);

virtual IFXRESULT IFXAPI IncrementTimeOffset(F32 deltaOffset)
{
	m_timeOffset += deltaOffset;
	return IFX_OK;
};

virtual IFXRESULT IFXAPI SetTimeOffset(F32 offset)
{
	m_timeOffset=offset;
	return IFX_OK;
};
virtual IFXRESULT IFXAPI GetTimeOffset(F32 *offset) const
{
	*offset = m_timeOffset;
	return IFX_OK;
};
virtual IFXRESULT IFXAPI CalcBonePositionAtTime(
	I32 boneid,
	F32 time,
	IFXVector3 *location,
	IFXQuaternion *rotation,
	IFXVector3 *scale);

/// transform access is copy-based, not direct access
virtual IFXRESULT IFXAPI GetRelativeTransform(U32 boneID,F32 *pMatrix16);
virtual IFXRESULT IFXAPI SetRelativeTransform(U32 boneID,F32 *pMatrix16);

/// EventState: 0=normal 1=awaiting-start 2=awaiting-end
virtual IFXSX_EventState& EventState(void) { return m_eventState; };

virtual BOOL        &DefaultOffset(void)        { return m_defaultOffset; };

virtual F32         &QueueOffset(void)          { return m_queueOffset; };
virtual F32         &LocalStartTime(void)       { return m_localStartTime; };
virtual F32         &LocalEndTime(void)         { return m_localEndTime; };
virtual F32         &TimeScale(void)            { return m_timeScale; };
virtual BOOL        &Loop(void)                 { return m_loop; };
virtual BOOL        &Sync(void)                 { return m_sync; };
virtual BOOL        &Running(void)              { return m_running; };
virtual BOOL        &DelayMap(void)             { return m_delaymap; };
virtual BOOL        &Started(void)              { return m_started; };
virtual IFXString IFXAPI    GetPrimaryMotionName(void)  { return m_primaryMotion; };
virtual void IFXAPI SetPrimaryMotionName(const IFXString& name)
{ m_primaryMotion=name; };

// not exposed:

IFXRESULT   VerifyCharacter(IFXVariant vset);

private:
	U32 m_uRefCount;

	static  BOOL        MapBone(IFXCoreNode &node,IFXVariant state);

	IFXMotion   *LookupMotion(I32 motionid);
	void        ResizeReaderArray(I32 size);

	// use manager or custom, not both
	IFXMotionManager            *m_motionmanager;
	F32                         m_timeOffset;

	// custom array
	U32                         m_customlength;
	IFXMotion                   *m_motionarray;

	IFXCharacter                *m_character;
	IFXArray<IFXMotionReader>   m_readerarray;


	// older
	F32 m_queueOffset;  // was m_timeOffset conflict
	F32 m_localStartTime;
	F32 m_localEndTime;
	F32 m_timeScale;

	BOOL m_loop;
	BOOL m_sync;
	BOOL m_running;
	BOOL m_delaymap;
	BOOL m_started;
	BOOL m_defaultOffset;
	
	IFXString m_primaryMotion;
	
	IFXSX_EventState m_eventState;
};

#endif
