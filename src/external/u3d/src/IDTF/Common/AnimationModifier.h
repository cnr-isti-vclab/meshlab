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
@file AnimationModifier.h

This header defines the ... functionality.

@note
*/


#ifndef AnimationModifier_H
#define AnimationModifier_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "Modifier.h"

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

	struct MotionInfo
	{
		IFXString m_name;
		BOOL m_loop;
		BOOL m_sync;
		F32 m_timeOffset;
		F32 m_timeScale;
	};

	/**
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class AnimationModifier : public Modifier
	{
	public:
		AnimationModifier() {};
		virtual ~AnimationModifier() {};

		/**
		*/
		void SetTimeScale( const F32& rTimeScale );
		const F32& GetTimeScale() const;

		/**
		*/
		void SetBlendTime( const F32& rBlendTime );
		const F32& GetBlendTime() const;

		void AddMotionInfo( const MotionInfo& rMotionInfo );
		const MotionInfo& GetMotionInfo( U32 index ) const;
		U32   GetMotionInfoCount() const;

		BOOL m_playing;
		BOOL m_rootLock;
		BOOL m_autoBlend;
		BOOL m_singleTrack;
	private:
		F32 m_timeScale;
		F32 m_blendTime;
		IFXArray< MotionInfo > m_motionInfoList;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void AnimationModifier::SetTimeScale(
		const F32& rTimeScale )
	{
		m_timeScale = rTimeScale;
	}

	IFXFORCEINLINE const F32& AnimationModifier::GetTimeScale() const
	{
		return m_timeScale;
	}

	IFXFORCEINLINE void AnimationModifier::SetBlendTime( const F32& rBlendTime )
	{
		m_blendTime = rBlendTime;
	}

	IFXFORCEINLINE const F32& AnimationModifier::GetBlendTime() const
	{
		return m_blendTime;
	}


	IFXFORCEINLINE void AnimationModifier::AddMotionInfo(
		const MotionInfo& rMotionInfo )
	{
		MotionInfo& motionInfo = m_motionInfoList.CreateNewElement();
		motionInfo = rMotionInfo;
	}

	IFXFORCEINLINE const MotionInfo& AnimationModifier::GetMotionInfo( U32 index ) const
	{
		return m_motionInfoList.GetElementConst( index );
	}

	IFXFORCEINLINE U32 AnimationModifier::GetMotionInfoCount() const
	{
		return m_motionInfoList.GetNumberElements();
	}


	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************

	//***************************************************************************
	//  Failure return codes
	//***************************************************************************

	/**
	@todo:  Insert module/interface specific return code description.
	*/
	//#define IFX_E_????  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )

}

#endif
