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
	@file	CIFXUVGenerator.h

			Class header file for the interface generator classes.
*/
#ifndef __IFXUVGENERATOR_CLASS_INTERFACE_H__
#define __IFXUVGENERATOR_CLASS_INTERFACE_H__

#include "IFXUVGenerator.h"
class IFXUVMapper;

class CIFXUVGenerator : public IFXUVGenerator 
{
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void **ppInterface);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXUVGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUVGenerator methods
	IFXRESULT IFXAPI   Generate(	IFXMesh& pMesh, 
									IFXUVMapParameters* pMapParams, 
									IFXMatrix4x4* pModelMatrix,
									IFXMatrix4x4* pViewMatrix, 
									const IFXLightSet* pLightSet);

	// variables
	U32 m_uRefCount;

private:
	CIFXUVGenerator();
	virtual ~CIFXUVGenerator();

	IFXenum m_LastWrapMode;
	IFXUVMapper* m_pMapper;
};

#endif
