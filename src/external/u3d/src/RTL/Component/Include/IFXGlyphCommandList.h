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
//
//  IFXGlyphCommandList.h
//
//  DESCRIPTION
//    Inteface file for the glyph command list class.
//
//  NOTES
//      None.
//
//***************************************************************************
/**
This class provides an interface to data structure for storing glyph commands.
*/
#ifndef __IFXGLYPHCOMMANDLIST_INTERFACES_H__
#define __IFXGLYPHCOMMANDLIST_INTERFACES_H__

#include "IFXSimpleList.h"

// {84E06D18-053E-4215-B40B-5A99D57EFBDD}
IFXDEFINE_GUID(IID_IFXGlyphCommandList,
			   0x84e06d18, 0x53e, 0x4215, 0xb4, 0xb, 0x5a, 0x99, 0xd5, 0x7e, 0xfb, 0xdd);

#define CURVE_STEPS_NUM 5

class IFXGlyphCommandList : public IFXUnknown
{
public:
	virtual IFXRESULT IFXAPI  AddTagBlock( IFXGlyph2DCommands::EGLYPH_TYPE comType, F64 x=0, F64 y=0 ) =0;
	virtual IFXRESULT IFXAPI  AddMoveToBlock( F64 x, F64 y ) =0;
	virtual IFXRESULT IFXAPI  AddLineToBlock( F64 x, F64 y ) =0;
	virtual IFXRESULT IFXAPI  AddCurveToBlock( F64 x1, F64 y1, F64 x2, F64 y2, F64 x3, F64 y3, U32 numSteps=CURVE_STEPS_NUM ) =0;
	virtual IFXRESULT IFXAPI  GetList( IFXSimpleList** ppList ) =0;
};

#endif // #define __IFXGLYPHCOMMANDLIST_INTERFACES_H__
