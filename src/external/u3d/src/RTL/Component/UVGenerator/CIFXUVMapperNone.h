//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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
	@file	CIFXUVMapperNone.h

			Class header file for the "none" texture coordinate mapper classes.
			This class only applies the texturematrix to texture coordinates.
*/
#ifndef __IFXUVMAPPERNone_CLASS_INTERFACE_H__
#define __IFXUVMAPPERNone_CLASS_INTERFACE_H__

#include "IFXUVMapper.h"
#include "IFXMesh.h"
#include "IFXCoreCIDs.h"

class IFXLight;

class CIFXUVMapperNone : public IFXUVMapper 
{
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void **ppInterface);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperNone_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUVMapper functions
	IFXRESULT IFXAPI   Apply(	IFXMesh& pMesh, 
						IFXUVMapParameters* pMapParams, 
						IFXMatrix4x4* pModelMatrix,
						IFXMatrix4x4* pViewMatrix, 
						const IFXLightSet* pLightSet );

protected:
	CIFXUVMapperNone(BOOL bNeedTexCoords = FALSE);
	virtual ~CIFXUVMapperNone();

	virtual BOOL	 IFXAPI 	NeedToMap(	IFXMesh& rMesh, 
									IFXUVMapParameters* pParams );

	virtual IFXRESULT IFXAPI 	Map(		IFXMesh& rMesh, 
									IFXUVMapParameters* pParams, 
									IFXMatrix4x4* pModelMatrix,
									IFXMatrix4x4* pViewMatrix, 
									const IFXLightSet* pLightSet );

	IFXLight* GetClosestLight(const IFXLightSet* pLightSet);

	U32 m_uRefCount;

	BOOL m_bNeedTexCoords;
};

#endif
