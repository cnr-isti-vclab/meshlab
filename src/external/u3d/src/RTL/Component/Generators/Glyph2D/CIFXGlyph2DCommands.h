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
//  CIFXGlyph2DCommands.h
//
//  DESCRIPTION
//    Header file for the glyph command blocks class.
//
//  NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXGLYPH2DCOMMANDS_CLASS_H__
#define __CIFXGLYPH2DCOMMANDS_CLASS_H__

#include "IFXGlyph2DCommands.h"

class CIFXGlyphTagBlock:public IFXGlyphTagBlock {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXGlyphTagBlock_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXGlyph2DCommands function.
	IFXRESULT  IFXAPI 	 GetType(EGLYPH_TYPE* uType) const ;
	IFXRESULT  IFXAPI 	 SetType(const EGLYPH_TYPE uType);
	IFXRESULT  IFXAPI 	 GetAttributes( U32* pAttr ) const ;
	IFXRESULT  IFXAPI 	 SetAttributes( const U32 attr );
	IFXRESULT  IFXAPI 	 GetData(F64* px, F64* py) const ;//only for EndGlyph
	IFXRESULT  IFXAPI 	 SetData(const F64 x, const F64 y);//

private:
	CIFXGlyphTagBlock();
	virtual ~CIFXGlyphTagBlock();

	EGLYPH_TYPE   m_eType;
	U32       m_attr;
	U32       m_uRefCount;
	F64       m_x;//only for EndGlyph
	F64       m_y;//
};

//---------------------------------------------------------------------------
class CIFXGlyphMoveToBlock:public IFXGlyphMoveToBlock {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXGlyphMoveToBlock_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXGlyph2DCommands function.
	IFXRESULT  IFXAPI 	 GetType(EGLYPH_TYPE* uType) const ;
	IFXRESULT  IFXAPI 	 SetType(const EGLYPH_TYPE uType);

	IFXRESULT  IFXAPI 	 GetData(F64* px, F64* py) const ;
	IFXRESULT  IFXAPI 	 SetData(const F64 x, const F64 y);
	IFXRESULT  IFXAPI 	 GetAttributes( U32* pAttr ) const ;
	IFXRESULT  IFXAPI 	 SetAttributes( const U32 attr );

private:
	CIFXGlyphMoveToBlock();
	virtual ~CIFXGlyphMoveToBlock();

	EGLYPH_TYPE   m_eType;
	U32       m_attr;
	F64       m_x;
	F64       m_y;
	U32       m_uRefCount;
};

//---------------------------------------------------------------------------
class CIFXGlyphLineToBlock:public IFXGlyphLineToBlock {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXGlyphLineToBlock_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXGlyph2DCommands function.
	IFXRESULT  IFXAPI 	 GetType(EGLYPH_TYPE* uType) const ;
	IFXRESULT  IFXAPI 	 SetType(const EGLYPH_TYPE uType);

	IFXRESULT  IFXAPI 	 GetData(F64* px, F64* py) const ;
	IFXRESULT  IFXAPI 	 SetData(const F64 x, const F64 y);
	IFXRESULT  IFXAPI 	 GetAttributes( U32* pAttr ) const ;
	IFXRESULT  IFXAPI 	 SetAttributes( const U32 attr );

private:
	CIFXGlyphLineToBlock();
	virtual ~CIFXGlyphLineToBlock();

	EGLYPH_TYPE   m_eType;
	U32       m_attr;
	U32       m_uRefCount;
	F64       m_x;
	F64       m_y;
};


//---------------------------------------------------------------------------
class CIFXGlyphCurveToBlock:public IFXGlyphCurveToBlock {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXCurveToBlock_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXGlyph2DCommands function.
	IFXRESULT  IFXAPI 	 GetType(EGLYPH_TYPE* uType) const ;
	IFXRESULT  IFXAPI 	 SetType(const EGLYPH_TYPE uType);

	IFXRESULT  IFXAPI 	 GetData(F64* pCx1, F64* pCy1, F64* pCx2, F64* pCy2, F64* pAx, F64* pAy, U32* uNumberOfCurveSteps) const ;
	IFXRESULT  IFXAPI 	 SetData(const F64 fCx1, const F64 fCy1, const F64 fCx2, const F64 fCy2, const F64 fAx,
					  const F64 fAy, const U32 uNumberOfCurveSteps);
	IFXRESULT  IFXAPI 	 GetAttributes( U32* pAttr ) const ;
	IFXRESULT  IFXAPI 	 SetAttributes( const U32 attr );

private:
	CIFXGlyphCurveToBlock();
	virtual ~CIFXGlyphCurveToBlock();

	EGLYPH_TYPE   m_eType;
	U32       m_attr;
	U32       m_uRefCount;
	F64       m_Cx1;
	F64       m_Cy1;
	F64       m_Cx2;
	F64       m_Cy2;
	F64       m_Ax;
	F64       m_Ay;
	U32       m_uNumberOfCurveSteps;
};

#endif // #define __CIFXGLYPH2DCOMMANDS_CLASS_H__
