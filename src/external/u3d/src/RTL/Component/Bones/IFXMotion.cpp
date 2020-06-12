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
	@file IFXMotion.cpp
*/

#include "IFXMotion.h"

I32 IFXMotion::GetTrackIndexByName(const IFXString& name)
{
	I32 m,len=m_tracks.GetNumberElements();

	for(m=0;m<len;m++)
	{
		if(!m_tracks[m].GetName().Compare(name))
		{
			return m;
		}
	}

	return -1;
}

IFXRESULT IFXMotion::GetTimeLimits(F32* min, F32* max)
{
	IFXASSERT(min);
	IFXASSERT(max);

	if(!min || !max)
		return IFX_E_INVALID_POINTER;

	F32 limits[2];
	bool found=false;

	*min =  1e30f;
	*max = -1e30f;

	I32 m,tracks=GetNumberTracks();
	for(m=0;m<tracks;m++)
	{
		found=true;
		limits[0]=GetTrackConst(m).GetHead()->Time();
		limits[1]=GetTrackConst(m).GetTail()->Time();

		if(*min>limits[0])
			*min=limits[0];
		if(*max<limits[1])
			*max=limits[1];
	}

	if(!found)
	{
		*min=0.0f;
		*max=0.0f;
	}

	return IFX_OK;
}
