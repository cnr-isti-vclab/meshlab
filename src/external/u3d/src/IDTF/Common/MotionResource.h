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
	@file	MotionResource.h

			This header defines the motion resource container.
*/

#ifndef MotionResource_H
#define MotionResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "Quat.h"

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

/**
	Data structure used to keep key frame info.
*/
struct KeyFrame
{
	F32 m_time;
	Point m_displacement;
	Quat m_rotation;
	Point m_scale;
};

/**
	Data structure used to keep motion track info.
*/
class MotionTrack
{
public:
	IFXString m_name;

	MotionTrack() {};
	~MotionTrack() {};

	void AddKeyFrame( const KeyFrame& rKeyFrame );
	const KeyFrame& GetKeyFrame( U32 index ) const ;
	U32 GetKeyFrameCount() const;

private:
	IFXArray< KeyFrame > m_keyFrames;
};

/**
	This is the implementation of a class that is used to handle model 
	resources.	
*/
class MotionResource : public Resource
{
public:
	MotionResource() {};
	~MotionResource() {};

	void AddMotionTrack( const MotionTrack& rMotionTrack );
	const MotionTrack& GetMotionTrack( U32 index ) const ;
	U32 GetMotionTrackCount() const;

private:
	IFXArray< MotionTrack > m_motionTracks;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void MotionTrack::AddKeyFrame( const KeyFrame& rKeyFrame )
{
	KeyFrame& keyFrame = m_keyFrames.CreateNewElement();
	keyFrame = rKeyFrame;
}

IFXFORCEINLINE const KeyFrame& MotionTrack::GetKeyFrame( U32 index ) const
{
	return m_keyFrames.GetElementConst( index );
}

IFXFORCEINLINE U32 MotionTrack::GetKeyFrameCount() const
{
	return m_keyFrames.GetNumberElements();
}

IFXFORCEINLINE void MotionResource::AddMotionTrack( const MotionTrack& rMotionTrack )
{
	MotionTrack& track = m_motionTracks.CreateNewElement();
	track = rMotionTrack;
}

IFXFORCEINLINE const MotionTrack& MotionResource::GetMotionTrack( U32 index ) const 
{
	return m_motionTracks.GetElementConst( index );
}

IFXFORCEINLINE U32 MotionResource::GetMotionTrackCount() const
{
	return m_motionTracks.GetNumberElements();
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}


#endif
