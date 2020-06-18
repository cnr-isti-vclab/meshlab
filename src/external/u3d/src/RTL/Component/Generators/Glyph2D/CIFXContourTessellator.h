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
	@file	CIFXContourTesselator.h

			Header file for the TrueType font classes.
*/

#ifndef __CIFXCONTOURTESSELATOR_CLASS_H__
#define __CIFXCONTOURTESSELATOR_CLASS_H__


#include "IFXContourTessellator.h"

#include <math.h>
#include "IFXVector2.h"
#include "IFXVector3.h"
#include "CIFXQuadEdge.h"

class CIFXContourTessellator : public IFXContourTessellator 
{
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXContourTessellator_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXContourTessellator methods
	IFXRESULT IFXAPI   Tessellate(SIFXTessellatorProperties* pTessellatorProperties, IFXSimpleList* pGlyphList, IFXMeshGroup** ppMeshGroup);
	IFXRESULT IFXAPI   Tessellate(SIFXTessellatorProperties* pTessellatorProperties, IFXContour* pGlyph, IFXMesh** ppMesh);
	IFXRESULT Tessellate
	(
	 IFXSimpleList* pGlyphList,
	 SIFXTessellatorProperties* pTessellatorPropertiesFront, 
	 IFXMeshGroup** ppMeshGroupFront,
	 SIFXTessellatorProperties* pTessellatorPropertiesBack, 
	 IFXMeshGroup** ppMeshGroupBack
	);

private:
	CIFXContourTessellator();
	virtual ~CIFXContourTessellator();

	U32	m_uRefCount;
};

#endif
