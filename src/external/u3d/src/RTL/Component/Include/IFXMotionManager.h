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
	@file IFXMotionManager.h
*/

#ifndef IFXMOTIONMANAGER_H
#define IFXMOTIONMANAGER_H

#include "IFXString.h"
#include "IFXMotion.h"

class IFXMotionMixer;
class IFXBonesManager;

/**
	This class works with the IFXMotionMixer and works with motions in an array 
	indexed by motionid.

	Each IFXMotionMixer draws from one IFXMotionManager.

	There can probably be just one master motion manager,
	but it might be useful to group independent motion sets into
	different managers.

	@note The motions are stored in an array indexed by the motionid.
	Try to keep your requested id's small so that the array is not
	terribly sparse.

*/
class IFXMotionManager
{
public:
	virtual ~IFXMotionManager() {}
	/// Resets manager as though just instantiated.
	virtual IFXRESULT IFXAPI 	Reset(void)	=0;

	/// Finds motionid for a named motion.
	virtual	IFXRESULT IFXAPI 	FindMotion(const IFXString& rName,U32* pMotionId) =0;

	/** Hooks in motion data for a given named motion or
	composite of motions */
	virtual	IFXRESULT IFXAPI 	PopulateMotionMixer(
									const IFXString& rMixerName,
									IFXMotionMixer *pMotionMixer)
								{	return IFX_E_UNSUPPORTED; }

	//	internal:
	/** Internal access only:
	For a given index, gets the actual IFXMotion structure
	and name. You must be able to declare an IFXMotion
	in order to extract the data. Arguments motion and
	sourcename are output only.
	They can be NULL (do not get). If the motion was
	loaded, sourcename should be the filename. */
	virtual	IFXRESULT IFXAPI 	GetMotion(
									I32 motionid,
									IFXMotion** ppMotion,
									IFXString* pSourceName)	=0;
};


#endif
