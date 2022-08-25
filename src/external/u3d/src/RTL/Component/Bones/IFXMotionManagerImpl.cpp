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
	@file IFXMotionManagerImpl.cpp
*/

#include "IFXMotionManagerImpl.h"

#define IFXMOTIONMGR_DEBUG          FALSE
#define IFXMOTIONMGR_CHECKBOUNDS    _DEBUG
#define IFXMOTIONMGR_CHECKNULL      TRUE

IFXMotionManagerImpl::IFXMotionManagerImpl(void)
{
	Reset();
}

IFXMotionManagerImpl::~IFXMotionManagerImpl(void)
{
}

IFXRESULT IFXMotionManagerImpl::Reset(void)
{
	return IFX_OK;
}

IFXRESULT IFXMotionManagerImpl::FindMotion(const IFXString& rName,U32 *motionid)
{
	U32 m,motions=m_motionarray.GetNumberElements();
	for(m=0;m<motions;m++)
		if(!rName.Compare(m_motionarray[m].m_motion.GetNameConst()))
		{
			*motionid=m;
			return IFX_OK;
		}

	return IFX_E_CANNOT_FIND;
}

/******************************************************************************
	motion and/or filename can be NULL (do not get)
******************************************************************************/
IFXRESULT IFXMotionManagerImpl::GetMotion(I32 motionid,IFXMotion **motion,
														IFXString *sourcename)
{
	IFXASSERT(motionid>=0);
	IFXASSERT((U32)motionid<m_motionarray.GetNumberElements());

	#if IFXMOTIONMGR_CHECKBOUNDS
		if(motionid<0 || (U32)motionid>=m_motionarray.GetNumberElements())
			return IFX_E_INVALID_RANGE;
	#endif

	if(motion)
		*motion=&m_motionarray[motionid].m_motion;
	if(sourcename)
		*sourcename=m_motionarray[motionid].m_filename;
	return IFX_OK;
}
