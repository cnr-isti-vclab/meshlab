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
	@file	IFXContour.h
	
			Header file for the contour class.
*/

#ifndef __IFXCONTOUR_INTERFACES_H__
#define __IFXCONTOUR_INTERFACES_H__


#include "IFXUnknown.h"

// {139C63D6-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXContour,
0x139c63d6, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

struct SIFXContourPoint 
{
	F64 x;
	F64 y;
	F64 z;
};


/**
This class provides a means to store a set of contours.  It is used by IFXGlyph3DGenerator.
It is an internal API.  See IFXGlyph3DGenerator for documentation.
*/

class IFXContour:public IFXUnknown 
{
public:
	virtual IFXRESULT IFXAPI  Initialize(U32 uSize)=0;
	virtual IFXRESULT IFXAPI  AddNext(SIFXContourPoint* pPosition, SIFXContourPoint* pNormal, U32* pIndex)=0;
	virtual IFXRESULT IFXAPI  Get(U32 uIndex, SIFXContourPoint* pPosition, SIFXContourPoint* pNormal)=0;
	virtual IFXRESULT IFXAPI  GetCount(U32* pCount)=0;
	virtual IFXRESULT IFXAPI  Delete(U32 uIndex)=0;

	virtual IFXRESULT IFXAPI  GetPosition(U32 uIndex, SIFXContourPoint* pPosition)=0;
	virtual IFXRESULT IFXAPI  GetNormal(U32 uIndex, SIFXContourPoint* pNormal)=0;
	virtual IFXRESULT IFXAPI  SetPosition(U32 uIndex, SIFXContourPoint* pPosition)=0;
	virtual IFXRESULT IFXAPI  SetNormal(U32 uIndex, SIFXContourPoint* pNormal)=0;
};

#endif
