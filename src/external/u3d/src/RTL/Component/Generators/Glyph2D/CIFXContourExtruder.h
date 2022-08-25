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
//	IFXContourExtruder.h
//
//	DESCRIPTION
//		Header file for the Contour extruder classes.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXCONTOUREXTRUDER_CLASS_H__
#define __CIFXCONTOUREXTRUDER_CLASS_H__

#include "IFXContourExtruder.h"


class CIFXContourExtruder:public IFXContourExtruder {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXContourExtruder_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXTextGenerator methods
	IFXRESULT IFXAPI   Extrude(SIFXExtruderProperties* pExtruderProperties, IFXSimpleList* pGlyphList, IFXMeshGroup** ppMeshGroup);
	IFXRESULT IFXAPI   Extrude(SIFXExtruderProperties* pExtruderProperties, IFXContour* pGlyph, IFXMesh** ppMesh);

private:
	CIFXContourExtruder();
	virtual ~CIFXContourExtruder();
	IFXRESULT DetermineVertexCount(IFXContour* pContour, U32* pVertexCount, U32* pFaceCount);

	U32		m_uRefCount;
};

#endif // #define __CIFXCONTOUREXTRUDER_CLASS_H__
