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
	@file	IFXContourGenerator.h
			Header file for the contour generator classes.

	@note
			This class specifies an API to create a set of contours.  
			It is used by IFXGlyph3DGenerator. It is an internal API.
			See IFXGlyph3DGenerator for documentation.
*/

#ifndef __IFXCONTOURGENERATOR_INTERFACES_H__
#define __IFXCONTOURGENERATOR_INTERFACES_H__


#include "IFXUnknown.h"
#include "IFXSimpleList.h"
#include "IFXContour.h"

// {139C63D8-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXContourGenerator,
0x139c63d8, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

class IFXContourGenerator:public IFXUnknown
{
public:
	virtual IFXRESULT IFXAPI  Initialize()=0;
	virtual IFXRESULT IFXAPI  GetContourList(IFXSimpleList* pGlyphList)=0;

	virtual IFXRESULT IFXAPI  StartPath()=0;
	virtual IFXRESULT IFXAPI  LineTo(F64 fX, F64 fY)=0;
	virtual IFXRESULT IFXAPI  MoveTo(F64 fX, F64 fY)=0;
	virtual IFXRESULT IFXAPI  CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy, U32 uNumberOfSteps)=0;
	virtual IFXRESULT IFXAPI  EndPath()=0;
	virtual IFXRESULT IFXAPI  GetBoundingBox(F64 pMin[2], F64 pMax[2])=0;
};

#endif
