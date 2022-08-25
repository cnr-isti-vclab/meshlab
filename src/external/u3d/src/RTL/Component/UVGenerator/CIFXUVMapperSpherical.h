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
	@file	CIFXUVMapperSpherical.h

			Class header file for the spherical texture coordinate mapper classes.
*/
#ifndef __IFXUVMAPPERSPHERICAL_CLASS_INTERFACE_H__
#define __IFXUVMAPPERSPHERICAL_CLASS_INTERFACE_H__

#include "CIFXUVMapperNone.h"

const float IFX_UV_PI=3.1415926535897932384626433832795f;

class CIFXUVMapperSpherical : public CIFXUVMapperNone 
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperSpherical_Factory( IFXREFIID interfaceId, void** ppInterface );

protected:
	BOOL IFXAPI   NeedToMap(IFXMesh& rMesh, IFXUVMapParameters* pParams);

private:
	CIFXUVMapperSpherical();
	virtual ~CIFXUVMapperSpherical();

	IFXRESULT IFXAPI   Map(	IFXMesh& rMesh, 
					IFXUVMapParameters* pParams,
					IFXMatrix4x4* pModelMatrix,
					IFXMatrix4x4* pViewMatrix, 
					const IFXLightSet* pLightSet);

};

#endif
