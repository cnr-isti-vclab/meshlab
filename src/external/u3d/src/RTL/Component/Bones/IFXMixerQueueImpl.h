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
	@file IFXMixerQueueImpl.h 
*/

#ifndef IFXMIXERQUEUEIMPL_H
#define IFXMIXERQUEUEIMPL_H

#include "IFXDeque.h"
#include "IFXMixerQueue.h"

class IFXMotionManager;

class IFXMixerQueueImpl : virtual public IFXMixerQueue
{
public:
	BOOL    m_playing;

	IFXMixerQueueImpl(void);
	virtual ~IFXMixerQueueImpl(void);

	void AssociateWithBonesManager( IFXBonesManager *pBonesManager )
	{   m_pBonesManager=pBonesManager; }

	void AssociateWithMotionManager( IFXMotionManager *pMotionManager )
	{   m_pMotionManager=pMotionManager; }

	virtual IFXRESULT IFXAPI    Pause(void);
	virtual IFXRESULT IFXAPI    Play(void);
	virtual IFXRESULT IFXAPI    Push(const IFXString& rName,F32 *localOffset,
									F32 *localStartTime,F32 *localEndTime,
									F32 *timeScale,BOOL *loop,BOOL *sync,
									BOOL delaymap=false,BOOL tail=false);
	virtual IFXRESULT IFXAPI    Queue(const IFXString& rName,F32 *localOffset,
									F32 *localStartTime,F32 *localEndTime,
									F32 *timeScale,BOOL *loop,BOOL *sync,
									BOOL delayMap=false);
	virtual IFXRESULT IFXAPI    Pop(BOOL tail=false);
	virtual IFXRESULT IFXAPI    UnQueue(void);
	virtual IFXRESULT IFXAPI    GetLocalTime(U32 index,F32 *localtime);
	virtual IFXRESULT IFXAPI    SetLocalTime(U32 index,F32 localtime);

	/// get IFXMotionMixer relative to head of queue
	virtual IFXMotionMixer  *GetMotionMixerNR(U32 index);

	/// Use does not add a reference (immediate use only)
	virtual IFXMotionMixer  *UseMotionMixerNoMap(U32 index);
	virtual IFXMotionMixer  *UseHistoricalMixer(U32 index);

	virtual IFXTransform    &RootTransform(void)
	{
		return m_rootTransform;
	}
	virtual IFXTransform    &DefaultTransform(void)
	{
		return m_defaultTransform;
	}
	virtual F32         &TimeScale(void)        { return m_timeScale; }
	virtual BOOL        &RootLock(void)         { return m_rootLock; }
	virtual BOOL        &Playing(void)          { return m_playing; }
	virtual U32         &Concatenation(void)    { return m_concatenation; }
	virtual U32         &AxisLock(void)         { return m_axisLock; }
	virtual IFXVector3  &AxisOffset(void)       { return m_axisOffset; }

	virtual BOOL        &AutoBlend(void)        { return m_autoBlend; }
	virtual F32         &BlendTime(void)        { return m_blendTime; }
	virtual F32         &BlendFraction(void)    { return m_blendFraction; }

	virtual U32 IFXAPI  GetNumberQueued(void)
	{
		return m_queue.GetNumberQueued();
	}
	virtual U32 IFXAPI  GetNumberHistory(void)
	{
		return m_history.GetNumberQueued();
	}

	virtual IFXRESULT IFXAPI GetRelativeTransform(U32 boneID,F32 *pMatrix16);
	virtual IFXRESULT IFXAPI SetRelativeTransform(U32 boneID,F32 *pMatrix16);

	// for immediate use only
	virtual IFXBonesManager *UseBonesManager(void)
	{
		return m_pBonesManager;
	}

	virtual IFXRESULT    IFXAPI     Advance(void);
	virtual U32             &Updated(void)
	{
		return m_updated;
	}

	class IFXMixerWrap
	{
	public:
				IFXMixerWrap(void);
				~IFXMixerWrap(void);
		void    Reset(void);
		void    Destruct(void);

		// NULL pointer = ignore argument
		void    Set(
					const IFXString& rPrimaryName,
					const F32 *pOffset=NULL,
					const F32 *pLocalEndTime=NULL,
					const F32 *pLocalStartTime=NULL,
					const F32 *pTimeScale=NULL,
					const BOOL *pLoop=NULL,
					const BOOL *pSync=NULL);

		void    Run(F32 worldTime,F32 fsync);
		void    Stop(F32 worldTime);

		const IFXMixerWrap &operator=(const IFXMixerWrap &other);

		IFXMotionMixer* m_pMotionMixer;
	};

protected:

	F32 GetWorldTime(void);

private:
	IFXRESULT   SanitizeBounds(U32 index);
	IFXRESULT   MapPush(BOOL tail);
	IFXRESULT   TransitionToMixer(IFXMotionMixer *pSkelMixer);
	void        VerifyQueue(void);
	void        CalcMotionLimits(IFXMotionMixer *pSkelMixer,
									F32 *pMin,F32 *pMax);

	void        ActivateHead(void);
	void        TransitionHead(bool push);
	void        AdjustForGap(I32 index,F32 time1,F32 time2);

	class IFXMixerWrapDeque: public IFXDeque<IFXMixerWrap>
	{
		virtual void ResetElement(IFXMixerWrap &wrap)   
		{ wrap.Reset(); }
	};

	class IFXMixerSummary
	{
	public:
		IFXMixerSummary(void);
		~IFXMixerSummary(void);

		IFXString   m_name;
		F32         m_localOffset;
		F32         m_localStartTime;
		F32         m_localEndTime;
		F32         m_timeScale;
		BOOL        m_loop;
		BOOL        m_sync;
	};

	class IFXPlaylistSummary
	{
	public:
		IFXPlaylistSummary(void);
		~IFXPlaylistSummary(void);

		IFXRESULT SetSize( U32 uCount );
		U32                 m_uMixerCount;
		IFXMixerSummary*    m_MixerSummaryArray;
	};

	IFXRESULT GetPlaylistSummary(IFXPlaylistSummary* pPlaylistSummary);
	IFXRESULT RestorePlaylist(const IFXPlaylistSummary* pPlaylistSummary);

	IFXBonesManager*    m_pBonesManager;
	IFXMotionManager*   m_pMotionManager;
	bool                m_initialized;
	U32                 m_updated;  // first update occurred

	IFXMixerWrapDeque   m_queue;
	IFXMixerWrapDeque   m_history;
	IFXTransform        m_rootTransform;
	IFXTransform        m_defaultTransform;
	F32                 m_lastSync;
	F32                 m_lastTime;
	F32                 m_timeScale;
	BOOL                m_autoBlend;
	F32                 m_blendTime;
	F32                 m_blendFraction;
	BOOL                m_rootLock;
	U32                 m_concatenation;
	U32                 m_axisLock;
	IFXVector3          m_axisOffset;
	BOOL                m_updating;
	BOOL                m_noloop;
	BOOL                m_noadjust;
};

#endif
