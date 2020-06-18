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
//	CIFXUVMapperPlanar.h
//
//	DESCRIPTION
//		Class header file for the planar texture coordinate mapper classes.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __IFXUVMAPPERPLANAR_CLASS_INTERFACE_H__
#define __IFXUVMAPPERPLANAR_CLASS_INTERFACE_H__

#include "CIFXUVMapperNone.h"

class CIFXUVMapperPlanar:public CIFXUVMapperNone 
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperPlanar_Factory( IFXREFIID interfaceId, void** ppInterface );

protected:
	BOOL IFXAPI   NeedToMap(IFXMesh& rMesh, IFXUVMapParameters* pParams);

private:
	CIFXUVMapperPlanar();
	virtual ~CIFXUVMapperPlanar();
	IFXRESULT IFXAPI   Map(	IFXMesh& rMesh, 
							IFXUVMapParameters* pParams,
							IFXMatrix4x4* pModelMatrix,
							IFXMatrix4x4* pViewMatrix, 
							const IFXLightSet* pLightSet);

};



#endif
