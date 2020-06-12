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
	@file	IFXMotion.h
*/

#ifndef IFXMOTION_H
#define IFXMOTION_H

#include "IFXKeyTrackArray.h"

class IFXInstant;

/**
	This class handles the motion of an object in 3D space.
	
	IFXMotion contains a name and an IFXKeyTrackArray.
	It is a set of convenience functions.	
*/
class IFXMotion
{
public:
	IFXMotion(void)
	{
							SetName(L"");
		Reset();
	}

	virtual	~IFXMotion(void)
	{
		Reset();
	}

	/// Clears all the motion tracks.
	void Reset(void)
	{
		m_tracks.Clear();
	}

	/// Returns the name of the whole motion
	IFXString& GetName(void)	{ return m_name; }

	const IFXString& GetNameConst(void) const { return m_name; }

	/// Sets the name of the motion.
	void SetName( const IFXString& name)	{ m_name.Assign(&name); }

	/** Interpolates the data for a specific track at a
		given time.  The result is put in an instant.
		An optional context can be given to hold state
		and accelerate multiple calls with small changes
		in time. */
	void CalcInstantConst(F32 time,U32 track,
							IFXInstant *instant,
							IFXListContext *context=NULL) const
	{ 
		GetTrackConst(track).CalcInstantConst(time,instant,context); 
	}

	/** Returns the index of a given named track or -1
		if there is no such track. */
	I32 GetTrackIndexByName(const IFXString& name);

	/// Returns the number of tracks in the motion.
	U32 GetNumberTracks(void)
	{	
		return m_tracks.GetNumberElements(); 
	}

	/// Accesses a given track.
	IFXKeyTrack& GetTrack(U32 index)
	{	
		IFXASSERT(index<m_tracks.GetNumberElements());
		return m_tracks[index]; 
	}

	/// Accesses a given track (const).
	const IFXKeyTrack& GetTrackConst(U32 index) const
	{	
		IFXASSERT(index<m_tracks.GetNumberElements());
		return m_tracks[index]; 
	}

	/** Gets the minimum and maximum keyframe times,
		presumably the head and tail of the list. */
	IFXRESULT GetTimeLimits(F32 *min,F32 *max);

	/** Creates and appends a new named track.
		The index of the new track is returned. */
	U32 CreateTrack(const IFXString &name)
	{
		IFXKeyTrack &track=m_tracks.CreateNewElement();
		track.SetName(name);

		return m_tracks.GetNumberElements()-1;
	}

	/// (internal) Accesses the IFXKeyTrackArray.
	IFXKeyTrackArray& GetTracks(void)		{ return m_tracks; }

	/** Filters out all data in all tracks, eliminating
		enough keyframes so that no keyframe follows
		the previous by less than the specified delta
		time. */
	void FilterAllTracks(F32 deltatime)
	{
		U32 m,numtracks=m_tracks.GetNumberElements();
		for(m=0;m<numtracks;m++)
			m_tracks[m].Filter(deltatime);
	}

	/** Filters out all data in all tracks eliminating
		any keyframes that can be accurately interpolated
		by it adjacent keyframes.  The arguments specify
		the acceptable error. */
	void CompressAllTracks(F32 deltaposition,
							F32 deltarotation,F32 deltascale=0.01f)
	{
		U32 m,numtracks=m_tracks.GetNumberElements();
		for(m=0;m<numtracks;m++)
			m_tracks[m].Compress(deltaposition,
								deltarotation,deltascale);
	}

private:
	IFXKeyTrackArray m_tracks;
	IFXString		 m_name;
};



#endif
