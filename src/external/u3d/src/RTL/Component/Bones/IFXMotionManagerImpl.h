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
	@file IFXMotionManagerImpl.h
*/

#ifndef IFXMOTIONMANAGERIMPL_H
#define IFXMOTIONMANAGERIMPL_H

#include "IFXMotionManager.h"

class IFXMotionManagerImpl : public IFXMotionManager
{
public:
		IFXMotionManagerImpl(void);
virtual ~IFXMotionManagerImpl(void);

virtual IFXRESULT IFXAPI    Reset(void);

virtual IFXRESULT IFXAPI    FindMotion(const IFXString& rName,U32 *motionid);
virtual IFXRESULT IFXAPI    GetMotion(
									  I32 motionid,
									  IFXMotion **motion,
									  IFXString *sourcename);
private:
	struct IFXMotionEntry
	{
		IFXMotion m_motion;
		IFXString m_filename;
	};

	IFXArray<IFXMotionEntry>    m_motionarray;
};

#endif
