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
//
//	CIFXUVMapperCylindrical.h
//
//	DESCRIPTION
//		Class header file for the cylindrical texture coordinate mapper classes.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __IFXUVMAPPERCYLINDRICAL_CLASS_INTERFACE_H__
#define __IFXUVMAPPERCYLINDRICAL_CLASS_INTERFACE_H__

#include "CIFXUVMapperNone.h"

const float IFX_UV_PI=3.1415926535897932384626433832795f;

class CIFXUVMapperCylindrical:public CIFXUVMapperNone 
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperCylindrical_Factory( IFXREFIID interfaceId, void** ppInterface );

protected:
	BOOL IFXAPI   NeedToMap(IFXMesh& rMesh, IFXUVMapParameters* pParams);

private:
	CIFXUVMapperCylindrical();
	~CIFXUVMapperCylindrical();
	IFXRESULT IFXAPI   Map(	IFXMesh& rMesh, 
							IFXUVMapParameters* pParams,
							IFXMatrix4x4* pModelMatrix,
							IFXMatrix4x4* pViewMatrix, 
							const IFXLightSet* pLightSet);

};

#endif
