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
//	CIFXContourGenerator.h
//
//	DESCRIPTION
//		Header file for the contour generator classes.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXCONTOURGENERATOR_CLASS_H__
#define __CIFXCONTOURGENERATOR_CLASS_H__

#include "IFXContourGenerator.h"


class CIFXContourGenerator:public IFXContourGenerator {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXContourGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFX3DTextCallback methods
	IFXRESULT  IFXAPI 	 StartPath();
	IFXRESULT  IFXAPI 	 LineTo(F64 fX, F64 fY);
	IFXRESULT  IFXAPI 	 MoveTo(F64 fX, F64 fY);
	IFXRESULT  IFXAPI 	 CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy, U32 uNumberOfSteps);
	IFXRESULT  IFXAPI 	 EndPath();
	IFXRESULT  IFXAPI 	 GetBoundingBox(F64 pMin[2], F64 pMax[2]);

	// IFXContourGenerator methods
	IFXRESULT  IFXAPI 	 Initialize();	
	IFXRESULT  IFXAPI 	 GetContourList(IFXSimpleList* pGlyphList);

private:
	CIFXContourGenerator();
	virtual ~CIFXContourGenerator();
	IFXRESULT ContourCleanup();

	U32		m_uRefCount;
	IFXSimpleList* m_pGlyphList;
	IFXContour* m_pCurrentPath;
	SIFXContourPoint m_vLastPoint;

	void AddBoundingBox(F64 fX, F64 fY);
	F64 m_pBoundingBoxMin[2];
	F64 m_pBoundingBoxMax[2];

};

#endif // #define __CIFXCONTOURGENERATOR_CLASS_H__
