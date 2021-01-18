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
	@file	CIFXGlyphCommandList.h

			Header file for the 3D glyph generator classes.
*/

#ifndef __IFXGLYPHCOMMANDLIST_CLASS_H__
#define __IFXGLYPHCOMMANDLIST_CLASS_H__

#include "IFXGlyphCommandList.h"
#include "IFXGlyph3DGenerator.h"
#include "IFXSimpleList.h"


class CIFXGlyphCommandList : public IFXGlyphCommandList
{
public:
	// IFXUnknown
	U32 IFXAPI            AddRef ();
	U32 IFXAPI            Release ();
	IFXRESULT IFXAPI      QueryInterface (IFXREFIID riid, void **ppv);

	IFXRESULT  IFXAPI 	 AddTagBlock( IFXGlyph2DCommands::EGLYPH_TYPE comType, F64 x=0, F64 y=0 );
	IFXRESULT  IFXAPI 	 AddMoveToBlock( F64 x, F64 y );
	IFXRESULT  IFXAPI 	 AddLineToBlock( F64 x, F64 y );
	IFXRESULT  IFXAPI 	 AddCurveToBlock( F64 x1, F64 y1, F64 x2, F64 y2, F64 x3, F64 y3, U32 numSteps = CURVE_STEPS_NUM );
	IFXRESULT  IFXAPI 	 GetList( IFXSimpleList** ppList );

	//Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXGlyphCommandList_Factory(IFXREFIID iid, void** ppv);

private:
	CIFXGlyphCommandList();
	virtual ~CIFXGlyphCommandList();
	IFXRESULT RobustConstructor();

	// IFXUnknown
	U32 m_refCount;

	IFXSimpleList*        m_pData;
};

#endif
