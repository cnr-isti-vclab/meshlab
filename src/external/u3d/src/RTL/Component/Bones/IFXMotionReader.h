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
	@file IFXMotionReader.h
*/

#ifndef IFXMOTIONREADER_H
#define IFXMOTIONREADER_H

#include "IFXMotion.h"

/**
	connection from IFXCoreNode to potentially-shared single track on IFXMotion
*/
class IFXMotionReader
{
public:
	IFXMotionReader(void)       { Reset(); }
	virtual             ~IFXMotionReader(void)
	{ m_context.SetCurrent(NULL); }

	void        Reset(void)
	{
		m_motion=NULL;
		m_track=0;

		m_timescale=1.0f;
		m_offset=0.0f;
		m_start=0.0f;
		m_length=1.0f;
		m_loop=false;
		m_oscillate=false;
		m_ik=false;
		m_reference=false;
	}

	void        SetScale(F32 set)           { m_timescale=set; }
	F32         GetScale(void)              { return m_timescale; }

	void        SetOffset(F32 set)      { m_offset=set; }
	F32         GetOffset(void)             { return m_offset; }

	void        SetStart(F32 set)           { m_start=set; }
	F32         GetStart(void)              { return m_start; }

	void        SetLength(F32 set)      { m_length=set; }
	F32         GetLength(void)             { return m_length; }

	void        SetIK(bool set)             { m_ik=set; }
	bool        GetIK(void)                 { return m_ik; }

	void        SetReference(bool set)      { m_reference=set; }
	bool        GetReference(void)          { return m_reference; }

	bool        GetLoop(void)               { return m_loop; }
	void        SetLoop(bool set)
	{
		m_loop=set;
		if(m_loop)
			m_oscillate=false;
	}

	bool        GetOscillate(void)          { return m_oscillate; }
	void        SetOscillate(bool set)
	{
		m_oscillate=set;
		if(m_oscillate)
			m_loop=false;
	}

	bool        AssignByTrackID(IFXMotion *mot,U32 id)
	{
		if(id>=mot->GetTracks().GetNumberElements())
		{
			m_track=0;
			m_motion=NULL;
			return false;
		}

		m_motion=mot;
		m_track=id;
		return true;
	}

	bool        AssignByTrackName(IFXMotion *mot,const IFXString& rName)
	{
		I32 track=mot->GetTrackIndexByName(rName);
		if(track<0)
		{
			m_track=0;
			m_motion=NULL;
			return false;
		}

		m_track=track;
		m_motion=mot;
		return true;
	}

	bool        IsValid(void) const
	{
		return (m_motion!=NULL &&
			m_track<m_motion->GetNumberTracks());
	}

	bool        Read(F32 time,IFXInstant *instant)
	{
		if(!IsValid())
		{
			instant->Reset();
			return false;
		}

		IFXASSERT(m_motion);
		IFXASSERT(m_track<m_motion->GetTracks()
			.GetNumberElements());
		m_motion->CalcInstantConst(WarpTime(time),m_track,
			instant,&m_context);
		return true;
	}

	bool        GetTimeLimits(F32 *min,F32 *max)
	{
		if(!m_motion)
			return false;

		IFXASSERT(m_track<m_motion->GetTracks()
			.GetNumberElements());
		*min=m_motion->GetTrackConst(m_track).GetHead()->Time();
		*max=m_motion->GetTrackConst(m_track).GetTail()->Time();

		*min=WarpTime(*min);
		*max=WarpTime(*max);
		return true;
	}


private:
	F32         WarpTime(F32 time) const
	{
		time *= m_timescale;
		time += m_offset;

		if(m_loop || m_oscillate)
		{
			I32 reps;
			if(time<m_start)
				reps=(I32)((time-m_start)/m_length-0.9999f);
			else
				reps=(I32)((time-m_start)/m_length);

			time -= reps*m_length;

			if(m_oscillate)
			{
				if( (reps%2) != (reps<0) )
				{
					time=(m_start+m_length)-(time-m_start);
				}
			}
		}

		return time;
	}

	F32                 m_timescale;
	F32                 m_offset;
	F32                 m_start;
	F32                 m_length;
	bool                m_loop;
	bool                m_oscillate;
	bool                m_ik;
	bool                m_reference;

	IFXMotion           *m_motion;
	U32                 m_track;
	IFXListContext      m_context;
};


#endif
