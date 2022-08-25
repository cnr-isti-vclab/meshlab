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
	@file IFXMixerQueueImpl.cpp
*/

#include <string.h>
#include <wchar.h>
#include "IFXMixerQueueImpl.h"
#include "IFXBonesManagerImpl.h"
#include "IFXMotionManagerImpl.h"
#include "IFXMotionMixerImpl.h"
#include "IFXEuler.h"

#define IFXMQ_BLEND       TRUE

#define IFXMQ_QUEUEDEBUG    FALSE
#define IFXMQ_LOOPDEBUG     FALSE
#define IFXMQ_VERIFYDEBUG   FALSE

#define IFXMQ_TRACKVIEW     FALSE

#define IFXMQ_HISTORY     TRUE
#define IFXMQ_HISTORY_LIFE    (10.0f)
#define IFXMQ_LOOP_THRESHOLD  0.001f  //* in time units


IFXMixerQueueImpl::IFXMixerQueueImpl(void)
{
	//  m_singleTrack= -1;
	m_rootTransform.Reset();
	m_defaultTransform.Reset();
	m_lastSync=0.0f;
	m_lastTime=0.0f;
	m_timeScale=1.0f;
	m_autoBlend=true;
	m_concatenation=0x0;  // (CatTx|CatTy|CatTz)  (CatSx|CatSy|CatSz)
	m_axisLock=0x0000;
	m_axisOffset.Reset();
	m_blendTime=0.5f;
	m_blendFraction=0.0f;
	m_rootLock=false;
	m_playing=true;
	m_pBonesManager=NULL;
	m_pMotionManager=NULL;
	m_initialized=false;
	m_updating=false;
	m_updated=0;
	m_noloop=false;
	m_noadjust=false;
}


IFXMixerQueueImpl::~IFXMixerQueueImpl(void)
{
}

IFXRESULT IFXMixerQueueImpl::Pause(void)
{
	m_playing = false;
	return IFX_OK;
}

IFXRESULT IFXMixerQueueImpl::Play(void)
{
	m_playing = true;
	return IFX_OK;
}

IFXRESULT IFXMixerQueueImpl::Queue(const IFXString& rName,F32 *localOffset,
								   F32 *localStartTime,F32 *localEndTime,
								   F32 *timeScale,BOOL *loop,BOOL *sync,BOOL delayMap)
{
	return Push(rName,localOffset,localStartTime,localEndTime,
		timeScale,loop,sync,delayMap,true);
}

IFXRESULT IFXMixerQueueImpl::Push( const IFXString& rName,F32 *localOffset,
								  F32 *localStartTime,F32 *localEndTime,
								  F32 *timeScale,BOOL *loop,BOOL *sync,BOOL delayMap,BOOL tail)
{
	IFXMixerWrap *oldwrap=NULL;
	if(!tail && GetNumberQueued())
	{
		oldwrap=&((tail)?m_queue.ElementFromTail(0):m_queue.ElementFromHead(0));

		oldwrap->m_pMotionMixer->EventState()=IFXMotionMixer::IFXSX_WaitEnd;
	}

	IFXMixerWrap &wrap= (tail)? m_queue.PushTail(): m_queue.PushHead();

	wrap.m_pMotionMixer->AssociateWithBonesManager(m_pBonesManager);
	wrap.m_pMotionMixer->AssociateWithMotionManager(m_pMotionManager);

	IFXRESULT iResult=IFX_OK;

	if(!delayMap)
		iResult=m_pMotionManager->PopulateMotionMixer(rName, wrap.m_pMotionMixer);

#if IFXMQ_QUEUEDEBUG
	IFXTRACE_GENERIC(L"Push/Queue \"%ls localOffset=%.6G localStartTime=%.6G localEndTime=%.6G\n timeScale=%.6G loop=%d sync=%d m_lastTime=%.6G worldTime=%.6G\n",
		rName.Raw(),
		localOffset ? *localOffset: 0.0f,
		localStartTime ? *localStartTime: 0.0f,
		localEndTime ? *localEndTime: 0.0f,
		timeScale ? *timeScale: 0.0f,
		loop ? *loop: 0,
		sync ? *sync: 0,
		m_lastTime, m_pBonesManager ? GetWorldTime() : 0);
#endif

	wrap.m_pMotionMixer->DefaultOffset()=(localOffset==NULL);
	F32 offset=0.0f;
	if(localOffset)
		offset= *localOffset+
		(GetWorldTime()-m_lastTime)*(timeScale? *timeScale: 1.0f);

	wrap.Set(rName,&offset,localStartTime,localEndTime,timeScale,loop,sync);
	if(!delayMap && IFXSUCCESS(iResult))
	{
		/// @todo: investigate if usage
		//    if(GetModifierNR())
		iResult=MapPush(tail);
	}
	else if(iResult==IFX_E_NOT_INITIALIZED)
	{
		iResult=IFX_OK;
		delayMap=true;
	}

	if(!IFXSUCCESS(iResult))
	{
		IFXTRACE_GENERIC(L"Push/Queue \"%ls\" failed\n",rName.Raw());
		(tail)? m_queue.PopTail(): m_queue.PopHead();

		if(oldwrap)
			oldwrap->m_pMotionMixer->EventState()=
			IFXMotionMixer::IFXSX_Normal;
	}
	else if(delayMap || m_noloop || m_noadjust
		/// @todo: investigate removed modifier access usage
		//      || !GetModifierNR() || UseModifier()->GetNumberBones()==0
		)
	{
		IFXTRACE_GENERIC(L"Push/Queue \"%ls\" delayed mapping\n",rName.Raw());
		GetMotionMixerNR(0)->DelayMap()=true;
	}

	IFXRETURN(iResult);
}

IFXRESULT IFXMixerQueueImpl::SanitizeBounds( U32 index )
{
	IFXMixerWrap &wrap = m_queue.ElementFromHead(index);
	IFXMotionMixer *mixer = wrap.m_pMotionMixer;
	F32 min=0.0f,max=0.0f;
	if(mixer)
		mixer->GetMotionTimeLimits(&min,&max);

	if(mixer->LocalStartTime()<min)
		mixer->LocalStartTime()=min;

	if(mixer->DefaultOffset())
		mixer->QueueOffset()= -mixer->LocalStartTime();

	// NOTE might be redundant with no_adjust Advance() in TransitionHead()
	if(mixer->Loop())
	{
		U32 steps;

		mixer->QueueOffset()= -mixer->QueueOffset();
		if(max<min)
		{
			F32 temp=max;
			max=min;
			min=temp;
		}
		if(mixer->QueueOffset()>max)
		{
			steps=U32((mixer->QueueOffset()-max)/(max-min)+1);
			mixer->QueueOffset()-=steps*(max-min);
		}
		if(mixer->QueueOffset()<min)
		{
			steps=U32((min-mixer->QueueOffset())/(max-min)+1);
			mixer->QueueOffset()+=steps*(max-min);
		}
		mixer->QueueOffset()= -mixer->QueueOffset();
	}

	if(mixer->LocalEndTime()<0)
		mixer->LocalEndTime()=max;
	if(mixer->LocalEndTime()<mixer->LocalStartTime())
		mixer->LocalEndTime()=mixer->LocalStartTime();
	if(mixer->LocalEndTime()<min)
		mixer->LocalEndTime()=min;

	return IFX_OK;
}


IFXRESULT IFXMixerQueueImpl::MapPush(BOOL tail)
{
	IFXRESULT iResult=IFX_OK;

	ActivateHead();

	if(!tail || GetNumberQueued()==1)
	{
		iResult = TransitionToMixer( GetMotionMixerNR( 0 ) );

		F32 worldTime=GetWorldTime();

		if(IFXSUCCESS(iResult) && GetNumberQueued()>1)
			m_queue.ElementFromHead(1).Stop(worldTime);

		if(IFXSUCCESS(iResult))
			TransitionHead(tail!=0);
	}

	IFXRETURN(iResult);
}


IFXRESULT IFXMixerQueueImpl::UnQueue(void)
{
	IFXRETURN(Pop(true));
}

IFXRESULT IFXMixerQueueImpl::Pop(BOOL tail)
{
	if(!GetNumberQueued())
		return IFX_E_INVALID_RANGE;
#if IFXMQ_QUEUEDEBUG
	IFXTRACE_GENERIC(L"Pop \"%ls\" %.6G\n",
		GetMotionMixerNR(0)->GetPrimaryMotionName().Raw(),
		m_lastTime);
#endif
	const IFXMixerWrap &oldwrap=(tail)?
		m_queue.ElementFromTailConst(0): m_queue.ElementFromHeadConst(0);

#if IFXMQ_HISTORY
	IFXMixerWrap &wrap=m_history.PushTail();
	wrap=oldwrap; // copy
	wrap.m_pMotionMixer->EventState()=IFXMotionMixer::IFXSX_WaitEnd;
#endif

	(tail)? m_queue.PopTail(): m_queue.PopHead();

	ActivateHead();

	IFXRESULT iResult=IFX_OK;
	if(!GetNumberQueued())
		iResult = TransitionToMixer(NULL);
	else
		iResult = TransitionToMixer( GetMotionMixerNR( 0 ) );

	TransitionHead(false);
	IFXRETURN(iResult);
}



void IFXMixerQueueImpl::ActivateHead(void)
{
	if(GetNumberQueued())
	{
		SanitizeBounds(0);
		I32 intsync=(I32)((m_lastSync<0.0f)? m_lastSync-1.0f: m_lastSync);
		m_lastSync-=(F32)intsync;

		m_queue.ElementFromHead(0).Run(m_lastTime,m_lastSync);
	}
}


void IFXMixerQueueImpl::TransitionHead(bool push)
{
	IFXVector3 location;
	IFXVector3 scale;
	IFXQuaternion rotation;
	IFXRESULT iResult=IFX_OK;

	IFXCharacter *pCharacter;
	IFXASSERT(m_pBonesManager);
	m_pBonesManager->GetCharacter(&pCharacter);
	IFXASSERT(pCharacter);

	if(GetNumberQueued())
	{
		//* adjust local time to resolve looping
		IFXASSERT(!m_noadjust);
		m_noadjust=true;
		Advance();
		m_noadjust=false;

		//* does not use use blended result
		iResult=GetMotionMixerNR(0)->CalcBonePositionAtTime(0,m_lastTime,
			&location,&rotation,&scale);
	}
	else
	{
		RootTransform().CalcTRS(&location,&rotation,&scale);
		DefaultTransform()=RootTransform();

		m_lastSync=0.0f;
	}

	if(IFXSUCCESS(iResult))
	{
		IFXVector3 old_location,old_scale;
		IFXQuaternion old_rotation;
		IFXTransform clearance;
		m_pBonesManager->GetRootClearance( &clearance );
		clearance.CalcTRS(&old_location,&old_rotation,&old_scale);

#if IFXMQ_QUEUEDEBUG
		if(GetNumberQueued())
		{
			IFXTRACE_GENERIC(L"Transition 0x%x %ls %.6G\n",m_concatenation,
				GetMotionMixerNR(0)->GetPrimaryMotionName().Raw(),
				m_lastTime);
			IFXTRACE_GENERIC(L"old %ls new %ls\n",old_location.Out().Raw(),
				location.Out().Raw());
		}
#endif

		if(!(m_concatenation&CatTx))
			location[0]=old_location[0];
		if(!(m_concatenation&CatTy))
			location[1]=old_location[1];
		if(!(m_concatenation&CatTz))
			location[2]=old_location[2];

		if(AxisLock() & IFXMixerQueueImpl::TranslationX)
			location[0]=old_location[0];
		if(AxisLock() & IFXMixerQueueImpl::TranslationY)
			location[1]=old_location[1];
		if(AxisLock() & IFXMixerQueueImpl::TranslationZ)
			location[2]=old_location[2];

		if(!(m_concatenation&CatRotation))
			rotation=old_rotation;
		else if((m_concatenation&CatRotation) != CatRotation)
		{
			IFXEuler old_euler=old_rotation;
			IFXEuler euler=rotation;
			if(!(m_concatenation&CatRz))
				euler[2]=old_euler[2];
			if(!(m_concatenation&CatRy))
				euler[1]=old_euler[1];
			if(!(m_concatenation&CatRx))
				euler[0]=old_euler[0];

			rotation.MakeIdentity();
			rotation.Rotate(euler[2],IFX_Z_AXIS);
			rotation.Rotate(euler[1],IFX_Y_AXIS);
			rotation.Rotate(euler[0],IFX_X_AXIS);
		}

		if(!(m_concatenation&CatSx))
			scale[0]=old_scale[0];
		if(!(m_concatenation&CatSy))
			scale[1]=old_scale[1];
		if(!(m_concatenation&CatSz))
			scale[2]=old_scale[2];

		IFXVector3 change;
		change.Subtract(location,old_location);

		//    IFXTRACE_GENERIC(L"%p %.6G change %ls\n",this,GetWorldTime(),change.Out().Raw());

		IFXQuaternion qchange,inverse=old_rotation;
		inverse.Invert();
		qchange.Multiply(rotation,inverse);

		//* when translation_cat=off and rotation_cat=on, need a tweak
		/// @todo: tweak for scale too?
		IFXVector3 translation=location;
		qchange.RotateVector(location,translation);
		if(m_concatenation&CatTx)
			translation[0]=location[0];
		if(m_concatenation&CatTy)
			translation[1]=location[1];
		if(m_concatenation&CatTz)
			translation[2]=location[2];

		//* meaningless?
		RootTransform().Reset();
		RootTransform().SetTranslation(translation);
		RootTransform().Rotate(rotation);
		RootTransform().Scale(scale);

		IFXVector3 schange(1.0f,1.0f,1.0f);
		schange.Divide(scale,old_scale);

		schange.Reciprocate();
		m_pBonesManager->SetAutoScale(schange,0);
		schange.Reciprocate();

		qchange.Invert();
		m_pBonesManager->SetAutoRotate(qchange,0);
		qchange.Invert();

		change.Negate();
		m_pBonesManager->SetAutoTranslate(change,0);
		change.Negate();
	}
	else
		RootTransform().Reset();

	VerifyQueue();

	//* make sure changes are flushed through, but prevent loop adjustments
	IFXASSERT(!m_noloop);
	m_noloop=true;
	//  UseModifier(true)->UpdateSkeleton(false,GetModelNR(),this);

	//* just in case someone pops it before we get a chance to render
	/// @todo: investigate usage
#if FALSE
	if(!Updated())
	{
		UseModifier(true)->UpdateSkeleton(true,GetModelNR(),this);
	}
#endif
	m_noloop=false;
}



IFXRESULT IFXMixerQueueImpl::TransitionToMixer(IFXMotionMixer *pSkelMixer)
{
	IFXASSERT(m_pBonesManager);
	// NOTE AxisLock() defeats concatenation (per axis)
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoTranslationX,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoTranslationY,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoTranslationZ,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoRotationX,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoRotationY,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoRotationZ,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoScaleX,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoScaleY,TRUE);
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoScaleZ,TRUE);

	IFXMotionMixer *pMotionMixer=NULL;
	if(pSkelMixer)
		pMotionMixer=pSkelMixer;

#if IFXMQ_BLEND
	F32 worldTime=GetWorldTime();

	F32 blendTime=m_blendTime;
	if(pSkelMixer && !pSkelMixer->Loop())
	{
		F32 min=0.0f,max=0.0f;
		CalcMotionLimits(pSkelMixer,&min,&max);

		F32 maxblendTime=max-min;
		if(blendTime>maxblendTime)
			blendTime=maxblendTime;

		//    IFXTRACE_GENERIC(L"blendTime=%.6G/%.6G min=%.6G max=%.6G\n",
		//                      blendTime,m_blendTime,min,max);
	}


	/// @todo: consider more control over settings
	IFXBlendParam blendparam;
	blendparam.SetStopLast(false);
	blendparam.SetIncremental(false);
	blendparam.SetRealignment(0.0f);
	blendparam.SetBlendTime(blendTime);
	blendparam.SetExponent(1.0f);
	blendparam.SetStartTime(0.0f + worldTime);

	IFXRESULT iResult=m_pBonesManager->BlendToMotionMixer(pMotionMixer,
		&blendparam);
	IFXRETURN(iResult);
#else
	IFXRESULT iResult=m_pBonesManager->SelectMotionMixer(pMotionMixer);
	IFXRETURN(iResult);
#endif
}



void IFXMixerQueueImpl::VerifyQueue(void)
{
	// probe mixers queue to verify validity
#if IFXMQ_VERIFYDEBUG
	I32 m=0,length=GetNumberQueued();
	for(m=0;m<length;m++)
	{
		IFXMotionMixer *pMixer = GetMotionMixerNR(m);
		//    IFXTRACE_GENERIC(L"SkelMixer[%d]=%p\n",m,pMixer);

		pMixer = GetMotionMixerNR(m);
		if(pMixer)
		{
			/// @todo: investigate disabled (may leak)
			//      IFXRELEASE(pMixer);
		}
		else
			IFXTRACE_GENERIC(L"Warning: SkelMixer[%d]=NULL on SkelParam %p",
			m,this);
	}
#endif
}

F32 IFXMixerQueueImpl::GetWorldTime(void)
{
	F32 worldTime=0.0f;
	if(m_pBonesManager)
		m_pBonesManager->GetFloat(IFXBonesManager::Time,&worldTime);

	return worldTime;
}


IFXRESULT IFXMixerQueueImpl::GetLocalTime(U32 index,F32 *localTime)
{

	if(index>=GetNumberQueued())
		IFXRETURN(IFX_E_INVALID_RANGE);

	F32 worldTime=0.0f;
	GetMotionMixerNR(0)->GetTimeOffset(&worldTime);
	*localTime=m_lastTime-worldTime;

	VerifyQueue();
	return IFX_OK;
}

IFXRESULT IFXMixerQueueImpl::SetLocalTime(U32 index,F32 localTime)
{

	if(index>=GetNumberQueued())
		IFXRETURN(IFX_E_INVALID_RANGE);

	GetMotionMixerNR(0)->SetTimeOffset(m_lastTime-localTime);

	VerifyQueue();
	return IFX_OK;
}

IFXRESULT IFXMixerQueueImpl::Advance()
{
	U32 i, n = GetNumberQueued();
	for (i = 0; i < n; i++)
		if (GetMotionMixerNR(i) == NULL)
			IFXRETURN(IFX_E_RESOURCE_NOT_AVAILABLE);

	bool advancing = false;
	F32 worldTime = GetWorldTime();
	F32 timeDiff = worldTime - m_lastTime;
	U32 history = 0;
	BOOL playing = m_playing;

	m_pBonesManager->SetBool(IFXBonesManagerImpl::RootLock,(m_rootLock != 0));
	m_pBonesManager->SetBool(IFXBonesManagerImpl::AutoBlend,(AutoBlend()!=0));
	if(!AutoBlend())
		m_pBonesManager->SetFloat(IFXBonesManagerImpl::BlendFraction,
		BlendFraction());

#if IFXMQ_HISTORY
	if(!m_noloop && !m_noadjust && GetNumberHistory())
	{
		F32 localTime=0.0f;
		while( GetNumberHistory() )
		{
			IFXMotionMixer *pSkelMixer = UseHistoricalMixer(0);
			pSkelMixer->GetTimeOffset(&localTime);
			localTime=worldTime-localTime;

			if( fabs(localTime)>IFXMQ_HISTORY_LIFE )
			{
				if(pSkelMixer->EventState()==
					IFXMotionMixer::IFXSX_WaitEnd &&
					pSkelMixer->Started())
				{
					IFXTRACE_GENERIC(L"Warning: mixer %p \"%ls\" did not end cleanly"
						L" -> event forced\n",pSkelMixer,
						pSkelMixer->GetPrimaryMotionName().Raw());
					pSkelMixer->Started()=false;
				}
				m_history.PopHead();
			}
			else
				break;
		}
	}

	history=GetNumberHistory();
#endif
	//* delay blending when paused
	if( !playing )
	{
		F32 startTime=0.0f;
		m_pBonesManager->GetFloat(IFXBonesManagerImpl::BlendStartTime,
			&startTime);
		startTime += timeDiff;
		m_pBonesManager->SetFloat(IFXBonesManagerImpl::BlendStartTime,
			startTime);
	}

	IFXMotionMixer *pBlendMixer=NULL;
	m_pBonesManager->GetMotionMixer(1,&pBlendMixer);

	F32 lastMixerTime[3];
	lastMixerTime[0] = 0.0f;  //* default shouldn't actually be ever read
	lastMixerTime[1] = 0.0f;
	lastMixerTime[2] = 0.0f;
	if(history)
		UseHistoricalMixer(history-1)->GetTimeOffset(&lastMixerTime[0]);
	if(GetNumberQueued())
		GetMotionMixerNR(0)->GetTimeOffset(&lastMixerTime[1]);
	if(GetNumberQueued()>1)
		GetMotionMixerNR(1)->GetTimeOffset(&lastMixerTime[2]);

	// NOTE scaling is before looping
	I32 m, length = history + GetNumberQueued();

	/// @todo: check timescale effect on newly started motions
	for(m=0; m<length; m++)
	{
		//* increment time offset on all mixers not running
		F32 modscale = 1.0f;

		//* scale adjustment
		IFXMotionMixer* pSkelMixer = (U32(m) < history) ?
			UseHistoricalMixer(m): GetMotionMixerNR(m-history);

		if(playing && pSkelMixer->Running())
			modscale -= TimeScale() * pSkelMixer->TimeScale();

		pSkelMixer->IncrementTimeOffset(modscale*timeDiff);
	}

	F32 time = GetWorldTime();
	for(m=1;m<I32(GetNumberQueued());m++)
	{
		if( GetMotionMixerNR(m) != pBlendMixer )
			m_queue.ElementFromHead(m).Stop(time);
	}

	for(m=0;m<2 && m<I32(GetNumberQueued());m++)
	{
		IFXMotionMixer* pSkelMixer=(m<0)?
			UseHistoricalMixer(history-1): GetMotionMixerNR(m);

		if(!pSkelMixer->Running())
			continue;

		bool loop=(pSkelMixer->Loop() != 0);
		if(m && !loop)
			continue;

		F32 overallScale=TimeScale()*pSkelMixer->TimeScale();

		F32 min=0.0f, max=0.0f;
		CalcMotionLimits( pSkelMixer, &min, &max );

		F32 duration = max - min;
		F32 mixerTime = 0.0f;
		pSkelMixer->GetTimeOffset(&mixerTime);
		F32 localTime = worldTime - mixerTime;
		//F32 lastLocal = m_lastTime - lastMixerTime[m+1];

		//* when in use, trigger a startEvent on any WaitStart
		if( !m_noloop && !m_noadjust && playing &&
			pSkelMixer->EventState() == IFXMotionMixer::IFXSX_WaitStart )
		{
			pSkelMixer->EventState()=IFXMotionMixer::IFXSX_Normal;
			if(pSkelMixer->Started())
			{
				IFXTRACE_GENERIC(L"Warning: mixer \"%ls\" submitting"
					L" second start event without appropriate end event\n");
			}
#if IFXMQ_QUEUEDEBUG
			IFXTRACE_GENERIC(L"On Mixer %p->%p",pSkelMixer,
				pSkelMixer);
			IFXTRACE_GENERIC(L" min,max %.6G,%.6G mixerTime=%.6G\n"
				L" start=%.6G end=%.6G offset=%.6G"
				L" scale=%.6G loop=%d\n",
				min,max,mixerTime,
				pSkelMixer->LocalStartTime(),
				pSkelMixer->LocalEndTime(),
				pSkelMixer->QueueOffset(),
				pSkelMixer->TimeScale(),
				pSkelMixer->Loop());
#endif
			pSkelMixer->Started()=true;
		}

		if(pSkelMixer->Sync() && duration!=0.0f)
			m_lastSync=(localTime-min)/duration;
		else
			m_lastSync=0.0f;


		//* pop while approaching a blend if this is not the last on queue
		F32 precognition=(GetNumberQueued()>1)? BlendTime(): 0.0f;
		precognition*=overallScale;
		if(precognition>duration-IFXMQ_LOOP_THRESHOLD)
			precognition=duration-IFXMQ_LOOP_THRESHOLD;

		if((localTime-max)>IFXMQ_LOOP_THRESHOLD && loop)
		{
			/// @todo: iterations>1 need to be represented in concatentation

			if(!m_noloop)
			{
				//* looping forwards
#if IFXMQ_LOOPDEBUG
				//        IFXTRACE_GENERIC(L"%p Loop Forward %.6G>%.6G world=%.6G mixer=%.6G\n",
				//          this,localTime,max,worldTime,mixerTime);
#endif
				I32 iterations=0;
				if(duration<=IFXMQ_LOOP_THRESHOLD)
					pSkelMixer->SetTimeOffset(worldTime);
				else
				{
					iterations=(U32)((localTime-max)/duration)+1;
					F32 loopSlide=iterations*duration;
					pSkelMixer
						->IncrementTimeOffset(loopSlide);
					m_lastSync-=iterations;
					if(iterations!=1)
						IFXTRACE_GENERIC(L"%d Loop forward %d iterations\n",m,iterations);
					if(iterations>0 && !m_noadjust)
						AdjustForGap(m,min,max);
				}
			}
		}
		else if(localTime>(max-precognition) && !loop && overallScale>0.0f)
		{
			//* will not occur when m!=0 since !loop continue'd above
			if(!m_noloop)
				advancing=true;

			//* tweak offset so that motion is exactly at end
			if(localTime>max)
			{
#if IFXMQ_QUEUEDEBUG
				IFXTRACE_GENERIC(L"Tweak Pop() %.6G-%.6G world=%.6G\n",
					localTime,max,worldTime);
#endif
				pSkelMixer->SetTimeOffset(worldTime-max);
			}
		}
		else if( (min-localTime) > IFXMQ_LOOP_THRESHOLD && loop )
		{
			if( !m_noloop )
			{
				//* looping backwards
#if IFXMQ_LOOPDEBUG
				IFXTRACE_GENERIC(L"%p Loop Backward %.6G<%.6G world=%.6G mixer=%.6G\n",
					this,localTime,min,worldTime,mixerTime);
#endif

				I32 iterations=0;
				if(duration<=IFXMQ_LOOP_THRESHOLD)
					pSkelMixer->SetTimeOffset(worldTime);
				else
				{
					iterations=(I32)((localTime-min)/duration-0.99999f);
					F32 loopSlide=iterations*duration;
					pSkelMixer->IncrementTimeOffset(loopSlide);
					m_lastSync-=iterations;
					if(iterations!= -1)
						IFXTRACE_GENERIC(L"%d Loop back %d iterations\n",m,-iterations);
					if(iterations<0 && !m_noadjust)
						AdjustForGap(m,max,min);
				}
			}
		}
		else if(localTime<(min-precognition) && !loop && overallScale<0.0f)
		{
			//* will not occur when m!=0 since !loop continue'd above
			if(!m_noloop)
				advancing=true;

			//* tweak offset so that motion is exactly at start
			if(localTime<min)
			{
#if IFXMQ_QUEUEDEBUG
				IFXTRACE_GENERIC(L"Tweak Pop() %.6G-%.6G world=%.6G\n",
					localTime,min,worldTime);
#endif
				pSkelMixer->SetTimeOffset(worldTime-min);
			}
		}
	}

	if((advancing || m_updating) && !m_noadjust)
	{
		if( m_updating )
		{
			m_updating = false;
			Pop();
#if IFXMQ_QUEUEDEBUG
			IFXTRACE_GENERIC(L"DO Pop()\n");
#endif
		}
		else
		{
			m_updating = true;
#if IFXMQ_QUEUEDEBUG
			IFXTRACE_GENERIC(L"Wait to Pop()\n");
#endif
		}
	}

	m_pBonesManager->GetMotionMixer(1, &pBlendMixer);
	for(m=0; m < (I32)((history=GetNumberHistory())+GetNumberQueued());m++)
	{
		//* scale adjustment
		IFXMotionMixer *pSkelMixer=(U32(m)<history)?
			UseHistoricalMixer(m): GetMotionMixerNR(m-history);
		IFXASSERT(pSkelMixer);

		//* when no longer in a blend, trigger an endEvent on any WaitEnd
		if(!m_noloop && !m_noadjust && (!GetNumberQueued() ||
			pSkelMixer!=pBlendMixer))
		{
			F32 mixerTime=0.0f;
			pSkelMixer->GetTimeOffset(&mixerTime);
			//F32 localTime=worldTime-mixerTime;

			if(pSkelMixer->EventState()==IFXMotionMixer::IFXSX_WaitEnd)
			{
				pSkelMixer->EventState()=IFXMotionMixer::IFXSX_Normal;
				if(pSkelMixer->Started())
				{
#if IFXMQ_QUEUEDEBUG
					IFXTRACE_GENERIC(L"On Mixer %p->%p\n",pSkelMixer,
						pSkelMixer);
#endif
					pSkelMixer->Started()=false;
				}
			}
		}
	}

	m_lastTime = GetWorldTime();

	VerifyQueue();
	return advancing? IFX_TRUE: IFX_OK;
}


void IFXMixerQueueImpl::CalcMotionLimits(IFXMotionMixer *pSkelMixer,
										 F32 *pMin,F32 *pMax)
{
	F32 min=0.0f, max=0.0f;

	pSkelMixer->GetMotionTimeLimits( &min, &max );
	
	if(min<pSkelMixer->LocalStartTime())
		min=pSkelMixer->LocalStartTime();

	if(pSkelMixer->LocalEndTime()>=0.0f)
		max=pSkelMixer->LocalEndTime();

	if( min > max )
	{
		F32 temp=min;
		min = max;
		max = temp;
	}

	if(pMin)
		*pMin=min;
	if(pMax)
		*pMax=max;
}


void IFXMixerQueueImpl::AdjustForGap(I32 index, F32 time1, F32 time2)
{
	//* attach next loop to end of previous
	IFXQuaternion rotation[2];
	IFXVector3 scale[2];
	IFXVector3 firstframe,lastframe,change;
	F32 offset=0.0f;
	IFXMotionMixer* pSkelMixer=(index<0)?
		UseHistoricalMixer(GetNumberHistory()+index):
	GetMotionMixerNR(index);

	pSkelMixer->GetTimeOffset(&offset);
	pSkelMixer->CalcBonePositionAtTime(0,offset+time1,
		&firstframe,&rotation[0],&scale[0]);

	pSkelMixer->CalcBonePositionAtTime(0,offset+time2,
		&lastframe,&rotation[1],&scale[1]);

	IFXVector3 old_location,old_scale;
	IFXQuaternion old_rotation;
	RootTransform().CalcTRS(&old_location,&old_rotation,&old_scale);

	change.Subtract(firstframe,lastframe);
	if(!(m_concatenation&CatTx))
		change[0]=0.0f;
	if(!(m_concatenation&CatTy))
		change[1]=0.0f;
	if(!(m_concatenation&CatTz))
		change[2]=0.0f;

	if(AxisLock() & IFXMixerQueueImpl::TranslationX)
		change[0]=0.0f;
	if(AxisLock() & IFXMixerQueueImpl::TranslationY)
		change[1]=0.0f;
	if(AxisLock() & IFXMixerQueueImpl::TranslationZ)
		change[2]=0.0f;

	//* don't rotate translation mod
	if(!index)
	{
		old_location.Add(change);
#if IFXMQ_LOOPDEBUG
		IFXTRACE_GENERIC(L"Gap Translation %ls to %ls\n  (%ls to %ls)\n",
			change.Out().Raw8(),old_location.Out().Raw8(),
			firstframe.Out().Raw8(),lastframe.Out().Raw8());
#endif
	}

	if(index)
	{
		change.Negate();
		m_pBonesManager->AddToAutoTranslate(change,true);
		change.Negate();
	}
	else
	{
		change.Negate();
		m_pBonesManager->AddToAutoTranslate(change,false);
		change.Negate();
	}

	IFXQuaternion qchange;
	qchange.MakeIdentity();

	if(!(m_concatenation&CatRotation))
		rotation[0]=rotation[1];
	else
	{
		if((m_concatenation&CatRotation) != CatRotation)
		{
			IFXEuler old_euler=rotation[1];
			IFXEuler euler=rotation[0];
			if(!(m_concatenation&CatRz))
				euler[2]=old_euler[2];
			if(!(m_concatenation&CatRy))
				euler[1]=old_euler[1];
			if(!(m_concatenation&CatRx))
				euler[0]=old_euler[0];
			euler.ConvertToQuaternion(rotation[0]);
		}

		qchange.Multiply(rotation[1].Invert(),rotation[0]);
		if(index)
		{
			qchange.Invert();
			m_pBonesManager->AddToAutoRotate(qchange,true);
			qchange.Invert();
		}
		else
		{
			qchange.Invert();
			m_pBonesManager->AddToAutoRotate(qchange,false);
			qchange.Invert();
		}
	}

	if(!index)
		old_rotation.Rotate(qchange);

	//Since the result of division doesn't depend on value of elements
	//of arrays we assign 1.0f value to them to avoid possible division by zero
	if(!(m_concatenation&CatSx))
		scale[0][0]=scale[1][0]=1.0f;
	if(!(m_concatenation&CatSy))
		scale[0][1]=scale[1][1]=1.0f;
	if(!(m_concatenation&CatSz))
		scale[0][2]=scale[1][2]=1.0f;
	scale[0].Divide(scale[1]);

	if(index)
	{
		scale[0].Reciprocate();
		m_pBonesManager->AddToAutoScale(scale[0],true);
		scale[0].Reciprocate();
	}
	else
	{
		scale[0].Reciprocate();
		m_pBonesManager->AddToAutoScale(scale[0],false);
		scale[0].Reciprocate();
	}

	if(!index)
	{
		old_scale.Multiply(scale[0]);

		//* meaningless?
		RootTransform().Reset();
		RootTransform().SetTranslation(old_location);
		RootTransform().Rotate(old_rotation);
		RootTransform().Scale(old_scale);

#if IFXMQ_LOOPDEBUG
		IFXTRACE_GENERIC(L"%p AdjustForGap index=%d\n",this,index);
		IFXTRACE_GENERIC(L"T %ls\n",old_location.Out().Raw());
		IFXTRACE_GENERIC(L"  %ls\n",firstframe.Out().Raw());
		IFXTRACE_GENERIC(L"  %ls\n",lastframe.Out().Raw());
		IFXTRACE_GENERIC(L"R %ls\n",old_rotation.Out().Raw());
		IFXTRACE_GENERIC(L"  %ls\n",rotation[0].Out().Raw());
		IFXTRACE_GENERIC(L"  %ls\n",rotation[1].Out().Raw());
		IFXTRACE_GENERIC(L"S %ls\n",old_scale.Out().Raw());
		IFXTRACE_GENERIC(L"  %ls\n",scale[0].Out().Raw());
		IFXTRACE_GENERIC(L"  %ls\n",scale[1].Out().Raw());
#endif
	}

	VerifyQueue();
}



IFXMotionMixer* IFXMixerQueueImpl::UseMotionMixerNoMap(U32 index)
{
	if(index>=GetNumberQueued())
		return NULL;

	return m_queue.ElementFromHead(index).m_pMotionMixer;
}



IFXMotionMixer* IFXMixerQueueImpl::GetMotionMixerNR(U32 index)
{
	if( index>=GetNumberQueued() )
		return NULL;

	IFXMotionMixer *pMixer = m_queue.ElementFromHead(index).m_pMotionMixer;

	if( m_pBonesManager && pMixer && pMixer->DelayMap() )
	{
		if (IFXFAILURE(m_pMotionManager->PopulateMotionMixer(pMixer->GetPrimaryMotionName().Raw(), pMixer)))
			return NULL;
		SanitizeBounds( index );
		pMixer->DelayMap()=false;
		MapPush( false );
	}

	return pMixer;
}

IFXMotionMixer *IFXMixerQueueImpl::UseHistoricalMixer(U32 index)
{
	if(index>=GetNumberHistory())
		return NULL;

	return m_history.ElementFromHead(index).m_pMotionMixer;
}

IFXRESULT IFXMixerQueueImpl::GetRelativeTransform(U32 boneID,F32 *pMatrix16)
{
	if(!pMatrix16)
		IFXRETURN(IFX_E_INVALID_POINTER);

	IFXVector3 location,scale;
	IFXQuaternion rotation;
	IFXCharacter *pCharacter;
	IFXASSERT(m_pBonesManager);
	m_pBonesManager->GetCharacter(&pCharacter);
	IFXASSERT(pCharacter);

	IFXCoreNode *node=pCharacter;
	//  if(boneID>=0)
	{
		IFXBoneNode *bonenode=pCharacter->LookupBoneIndex(boneID);
		if(!bonenode)
			IFXRETURN(IFX_E_INVALID_HANDLE);

		node=bonenode;
	}

	location=node->DisplacementConst();
	rotation=node->RotationConst();
	scale=node->ScaleConst();

	IFXMatrix4x4 matrix=rotation;
	matrix.Scale(scale);
	matrix.SetTranslation(location);

	I32 m;
	for(m=0;m<16;m++)
		pMatrix16[m]=matrix[m];

	return IFX_OK;
}

IFXRESULT IFXMixerQueueImpl::SetRelativeTransform(
	U32 boneID,F32 *pMatrix16)
{
	if(!pMatrix16)
		IFXRETURN(IFX_E_INVALID_POINTER);

	//* use mixer when available (common case)
	if(GetNumberQueued())
		IFXRETURN(GetMotionMixerNR(0)->SetRelativeTransform(boneID,pMatrix16));

	IFXMatrix4x4 matrix=pMatrix16;

	IFXQuaternion rotation;
	IFXVector3 location,scale;

	matrix.CalcTRS(&location,&rotation,&scale);

	//* fallback: twiddle inanimate skeleton
	IFXCharacter *pCharacter;
	IFXASSERT(m_pBonesManager);
	m_pBonesManager->GetCharacter(&pCharacter);
	IFXASSERT(pCharacter);

	IFXCoreNode *node=pCharacter;
	IFXBoneNode *bonenode=NULL;
	//  if(boneID>=0)
	{
		bonenode=pCharacter->LookupBoneIndex(boneID);
		if(!bonenode)
			IFXRETURN(IFX_E_INVALID_HANDLE);

		node=bonenode;
	}

	if(bonenode && bonenode->IsEffected() )
	{
		bonenode->Effector()=location;
	}
	else
	{
		node->Displacement()=location;
		node->Rotation()=rotation;
		node->Scale()=scale;
	}

	return IFX_OK;
}



IFXMixerQueueImpl::IFXMixerWrap::IFXMixerWrap(void)
{
	m_pMotionMixer = NULL;
	Reset();
}

IFXMixerQueueImpl::IFXMixerWrap::~IFXMixerWrap(void)
{
	Destruct(); 
}

void IFXMixerQueueImpl::IFXMixerWrap::Reset(void)
{
	Destruct();
	IFXRELEASE( m_pMotionMixer );
	m_pMotionMixer = new IFXMotionMixerImpl();
}

const IFXMixerQueueImpl::IFXMixerWrap &IFXMixerQueueImpl::IFXMixerWrap::operator=(
	const IFXMixerWrap &other)
{
	if( m_pMotionMixer )
		m_pMotionMixer->Release();

	m_pMotionMixer = other.m_pMotionMixer;

	if( m_pMotionMixer )
		m_pMotionMixer->AddRef();

	return *this;
}

void IFXMixerQueueImpl::IFXMixerWrap::Destruct(void)
{
	IFXRELEASE( m_pMotionMixer );
}

void IFXMixerQueueImpl::IFXMixerWrap::Set(const IFXString& rPrimaryName,
										  const F32 *pOffset,const F32 *pLocalStartTime,
										  const F32 *pLocalEndTime,const F32 *pTimeScale,
										  const BOOL *pLoop,const BOOL *pSync)
{
	m_pMotionMixer->SetPrimaryMotionName(rPrimaryName);

	if(pOffset)
		m_pMotionMixer->QueueOffset()= *pOffset;
	if(pLocalStartTime)
	{
		m_pMotionMixer->LocalStartTime()= *pLocalStartTime;

		if(!pOffset)
			m_pMotionMixer->QueueOffset()= *pLocalStartTime;
	}

	if(pLocalEndTime)
		m_pMotionMixer->LocalEndTime()= *pLocalEndTime;

	if(pTimeScale)
		m_pMotionMixer->TimeScale()= *pTimeScale;

	if(pLoop)
		m_pMotionMixer->Loop()= *pLoop;

	if(pSync)
		m_pMotionMixer->Sync()= *pSync;
}

void IFXMixerQueueImpl::IFXMixerWrap::Run(F32 worldTime,F32 fsync)
{
	IFXMotionMixer *motionmixer = m_pMotionMixer;

	if(!m_pMotionMixer->Running())
	{
#if IFXMQ_QUEUEDEBUG
		IFXTRACE_GENERIC(L"Run() \"%ls\" %.6G+%.6G\n",
			m_pMotionMixer->GetPrimaryMotionName().Raw(),
			m_pMotionMixer->QueueOffset(),worldTime);
#endif
		motionmixer->SetTimeOffset(m_pMotionMixer->QueueOffset()+worldTime);
		m_pMotionMixer->EventState()=IFXMotionMixer::IFXSX_WaitStart;
	}

	m_pMotionMixer->Running() = true;

	if( m_pMotionMixer->Sync() )
	{
		F32 min=0.0f,max=0.0f;
		motionmixer->GetMotionTimeLimits(&min,&max);

		motionmixer->SetTimeOffset( m_pMotionMixer->QueueOffset() +
			worldTime - (max-min)*fsync );
	}
}

void IFXMixerQueueImpl::IFXMixerWrap::Stop(F32 worldTime)
{
	IFXMotionMixer *motionmixer = m_pMotionMixer;
	if(m_pMotionMixer->Running())
	{
		if(m_pMotionMixer->Sync())
			m_pMotionMixer->QueueOffset()=0.0f;
		else
		{
			F32 offset;
			motionmixer->GetTimeOffset(&offset);
			m_pMotionMixer->QueueOffset()=offset-worldTime;
		}
	}

	m_pMotionMixer->Running()=false;
}


IFXMixerQueueImpl::IFXMixerSummary::IFXMixerSummary()
{
	m_name      = "<no motion>";
	m_localOffset = 0.0f;
	m_localStartTime= 0.0f;
	m_localEndTime  = 0.0f;
	m_timeScale   = 0.0f;
	m_loop      = TRUE;
	m_sync      = TRUE;

	return;
}


IFXMixerQueueImpl::IFXMixerSummary::~IFXMixerSummary()
{
}


IFXMixerQueueImpl::IFXPlaylistSummary::IFXPlaylistSummary()
{
	m_uMixerCount=0;
	m_MixerSummaryArray=NULL;

	return;
}

IFXMixerQueueImpl::IFXPlaylistSummary::~IFXPlaylistSummary()
{
	if(NULL != m_MixerSummaryArray)
	{
		delete[] m_MixerSummaryArray;
		m_MixerSummaryArray=NULL;
	}

	m_uMixerCount=0;

	return;
}


IFXRESULT IFXMixerQueueImpl::IFXPlaylistSummary::SetSize(U32 uCount)
{
	IFXRESULT rc=IFX_OK;

	if( uCount != m_uMixerCount)
	{
		if(NULL != m_MixerSummaryArray)
		{
			delete[] m_MixerSummaryArray;
			m_MixerSummaryArray=NULL;
		}

		if(uCount != 0)
		{
			m_MixerSummaryArray=new IFXMixerSummary[ uCount ];

			if(NULL == m_MixerSummaryArray)
				rc=IFX_E_OUT_OF_MEMORY;
		}

		m_uMixerCount=uCount;
	}

	IFXRETURN(rc);
}


IFXRESULT IFXMixerQueueImpl::GetPlaylistSummary(
	IFXPlaylistSummary* pPlaylistSummary)
{
	IFXRESULT rc=IFX_OK;

	U32 i=0;
	U32 uNumMixers=GetNumberQueued();
	IFXMotionMixer* pMixer=NULL;

	if(NULL == pPlaylistSummary)
		rc=IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(rc))
	{
		// get the playlist:
		for(i=0;(i<uNumMixers) && (i<pPlaylistSummary->m_uMixerCount);i++)
		{
			pMixer = GetMotionMixerNR(i);

			if(IFXSUCCESS(rc) && (NULL != pMixer))
			{
				pPlaylistSummary->m_MixerSummaryArray[i].m_name=
					pMixer->GetPrimaryMotionName();
				pPlaylistSummary->m_MixerSummaryArray[i].m_localOffset=
					pMixer->QueueOffset();
				pPlaylistSummary->m_MixerSummaryArray[i].m_localStartTime=0.0f;
				pPlaylistSummary->m_MixerSummaryArray[i].m_localEndTime= -1.0f;
				pPlaylistSummary->m_MixerSummaryArray[i].m_timeScale=
					pMixer->TimeScale();
				pPlaylistSummary->m_MixerSummaryArray[i].m_loop=pMixer->Loop();
				pPlaylistSummary->m_MixerSummaryArray[i].m_sync=pMixer->Sync();

			}
			else
				rc=IFX_E_NOT_INITIALIZED;

			/// @todo: investigate if need to release
			//      IFXRELEASE(pMixer);
		}
	}

	IFXRETURN(rc);
}


IFXRESULT IFXMixerQueueImpl::RestorePlaylist(
	const IFXPlaylistSummary* pPlaylistSummary)
{
	IFXRESULT rc=IFX_OK;

	U32 i=0;

	if(NULL == pPlaylistSummary)
		rc=IFX_E_INVALID_POINTER;

	// Reinstate the playlist:
	for(i=0; (i < pPlaylistSummary->m_uMixerCount) && (IFXSUCCESS(rc)); i++)
	{
		// queue up the next one:
		rc=Queue( pPlaylistSummary->m_MixerSummaryArray[i].m_name,
			NULL,
			NULL,
			NULL,
			&pPlaylistSummary->m_MixerSummaryArray[i].m_timeScale,
			&pPlaylistSummary->m_MixerSummaryArray[i].m_loop,
			&pPlaylistSummary->m_MixerSummaryArray[i].m_sync );
	}

	IFXRETURN(rc);
}
