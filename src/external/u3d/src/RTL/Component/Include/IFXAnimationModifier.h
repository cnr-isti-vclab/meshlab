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
	@file	IFXAnimationModifier.h

			The header file that defines the IFXAnimationModifier interface.      
*/

#ifndef __IFXAnimationModifier_h__
#define __IFXAnimationModifier_h__

#include "IFXUnknown.h"
#include "IFXModifier.h"
#include "IFXBones.h"

// {61DC3058-1358-49ea-B6C8-F6B5F4B0464B}
IFXDEFINE_GUID(IID_IFXAnimationModifier,
0x61dc3058, 0x1358, 0x49ea, 0xb6, 0xc8, 0xf6, 0xb5, 0xf4, 0xb0, 0x46, 0x4b);

/// Interface for an IFXAnimationModifier
class IFXAnimationModifier : virtual public IFXModifier,
							 virtual public IFXMixerQueue
{
public:
	/** Inits instance as keyframe player.  
	Can be initialized only once. Must be initialized right after creation.*/
	virtual void IFXAPI SetAsKeyframe()	= 0;

	/** Inits instance as bones player. Default value.
	Can be initialized only once. Must be initialized right after creation. */
	virtual void IFXAPI SetAsBones()		= 0;

	/** Returns TRUE if instance is keyframe player. */
	virtual BOOL IFXAPI IsKeyframe()	= 0;

    /** Returns a pointer to the associated BonesManager without
      affecting its reference count (for immediate use only). */
	virtual IFXBonesManager* IFXAPI GetBonesManagerNR(void) = 0;
};

#endif
