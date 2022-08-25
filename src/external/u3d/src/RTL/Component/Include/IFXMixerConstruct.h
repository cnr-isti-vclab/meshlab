//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXMixerConstruct.h

			Declaration of IFXMixerConstruct interface.
			The IFXMixerConstruct interface is used to get to animation data.
			The animation data is stored in separate tracks.
			Each track generally corresponds to the animation of one bone.
*/
#ifndef IFXMixerConstruct_H__
#define IFXMixerConstruct_H__

#include "IFXUnknown.h"
#include "IFXMarkerX.h"
#include "IFXList.h"

// {DCDB21A2-528A-11d4-BE4C-00A0C9D6AB25}
IFXDEFINE_GUID(IID_IFXMixerConstruct,
0xdcdb21a2, 0x528a, 0x11d4, 0xbe, 0x4c, 0x0, 0xa0, 0xc9, 0xd6, 0xab, 0x25);

class IFXMotionResource;

class IFXMapEntry;

class IFXMixerConstruct : virtual public IFXMarkerX
{
public:
	enum MotionType
	{
		NONE,
		SINGLETRACK,
		MULTITRACK
	};

	virtual void IFXAPI SetMotionResource(IFXMotionResource* pIFXMotionResource)       =0;
	virtual IFXMotionResource* IFXAPI GetMotionResource()                              =0;
	virtual void IFXAPI AddMapping( IFXMixerConstruct* pMixer, IFXString* pBoneName )  =0;
	virtual IFXList<IFXMapEntry>* IFXAPI GetEntryList()                                =0;

	virtual F32 IFXAPI GetDuration()                                                   =0;
	virtual MotionType IFXAPI GetType()                                                =0;
};

class IFXMapEntry
{
public:
	IFXMapEntry():m_pMixerConstruct(0) {}
	~IFXMapEntry() { IFXRELEASE(m_pMixerConstruct); }

	IFXMixerConstruct* m_pMixerConstruct;
	IFXString m_boneName;
};

#endif
