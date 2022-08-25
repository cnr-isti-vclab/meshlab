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
	@file	IFXGlyph2DCommands.h

    Inteface file for the glyph command classes.

	@note
	This set of classes provides a storage class for the contour api.  It is used to regenerate the glyph
	models if an appearance attribute is changed.   It is also used to clone 3D text, and to
	export 3D text. It is an internal API.  See IFXModifier3DGlphGen.h for documentation.
*/

#ifndef __IFXGlyph2DCommands_INTERFACES_H__
#define __IFXGlyph2DCommands_INTERFACES_H__

#include "IFXUnknown.h"

// {CBC4ECA6-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyph2DCommands,
			   0xcbc4eca6, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

// {CBC4ECA7-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyphTagBlock,
			   0xcbc4eca7, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

// {CBC4ECA8-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyphMoveToBlock,
			   0xcbc4eca8, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

// {CBC4ECA9-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyphLineToBlock,
			   0xcbc4eca9, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

// {CBC4ECAA-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyphCurveToBlock,
			   0xcbc4ecaa, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);


class IFXGlyph2DCommands : public IFXUnknown {
public:
	enum EGLYPH_TYPE
	{
		IGG_TYPE_STARTGLYPHSTRING,
		IGG_TYPE_STARTGLYPH,
		IGG_TYPE_STARTPATH,
		IGG_TYPE_MOVETO,
		IGG_TYPE_LINETO,
		IGG_TYPE_CURVETO,
		IGG_TYPE_ENDPATH,
		IGG_TYPE_ENDGLYPH,
		IGG_TYPE_ENDGLYPHSTRING
	};

	virtual IFXRESULT IFXAPI  GetType(EGLYPH_TYPE* uType) const =0;
	virtual IFXRESULT IFXAPI  SetType(const EGLYPH_TYPE uType)=0;
	virtual IFXRESULT IFXAPI  GetAttributes( U32* pAttr ) const =0;
	virtual IFXRESULT IFXAPI  SetAttributes( const U32 attr )=0;
};

class IFXGlyphTagBlock : public IFXGlyph2DCommands {
public:
	virtual IFXRESULT IFXAPI  GetData(F64* px, F64* py) const =0;//only for EndGlyph
	virtual IFXRESULT IFXAPI  SetData(const F64 x, const F64 y)=0;//only for EndGlyph
};

class IFXGlyphMoveToBlock : public IFXGlyph2DCommands {
public:
	virtual IFXRESULT IFXAPI  GetData(F64* px, F64* py) const =0;
	virtual IFXRESULT IFXAPI  SetData(const F64 x, const F64 y)=0;
};

class IFXGlyphLineToBlock : public IFXGlyph2DCommands {
public:
	virtual IFXRESULT IFXAPI  GetData(F64* px, F64* py) const =0;
	virtual IFXRESULT IFXAPI  SetData(const F64 x, const F64 y)=0;
};

class IFXGlyphCurveToBlock : public IFXGlyph2DCommands {
public:
	virtual IFXRESULT IFXAPI  GetData(F64* pCx1, F64* pCy1, F64* pCx2, F64* pCy2, F64* pAx, F64* pAy, U32* uNumberOfCurveSteps) const =0;
	virtual IFXRESULT IFXAPI  SetData(const F64 fCx1, const F64 fCy1, const F64 fCx2, const F64 fCy2, const F64 fAx,
							  const F64 fAy, const U32 uNumberOfCurveSteps)=0;
};


#endif
