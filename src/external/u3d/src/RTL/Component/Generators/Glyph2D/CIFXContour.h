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
//	CIFXContour.h
//
//	DESCRIPTION
//		Header file for the contour class.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXCONTOUR_CLASS_H__
#define __CIFXCONTOUR_CLASS_H__

#include "IFXContour.h"

class CIFXContour:public IFXContour {
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXContour_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXContour function.
	IFXRESULT  IFXAPI 	 Initialize(U32 uSize);
	IFXRESULT  IFXAPI 	 AddNext(SIFXContourPoint* pPosition, SIFXContourPoint* pNormal, U32* pIndex);
	IFXRESULT  IFXAPI 	 Get(U32 uIndex, SIFXContourPoint* pPosition, SIFXContourPoint* pNormal);
	IFXRESULT  IFXAPI 	 GetCount(U32* pCount);
	IFXRESULT  IFXAPI 	 Delete(U32 uIndex);

	IFXRESULT  IFXAPI 	 GetPosition(U32 uIndex, SIFXContourPoint* pPosition);
	IFXRESULT  IFXAPI 	 GetNormal(U32 uIndex, SIFXContourPoint* pNormal);
	IFXRESULT  IFXAPI 	 SetPosition(U32 uIndex, SIFXContourPoint* pPosition);
	IFXRESULT  IFXAPI 	 SetNormal(U32 uIndex, SIFXContourPoint* pNormal);

private:
	CIFXContour();
	virtual ~CIFXContour();

	struct SNode {
		SIFXContourPoint vPosition;
		SIFXContourPoint vNormal;
	};

	U32		m_uRefCount;
	SNode**	m_ppList;
	U32		m_uCount;
	U32		m_uLastElement;

};

#endif // #define __CIFXCONTOUR_CLASS_H__
