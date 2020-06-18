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
//	CIFXUVMapperReflection.h
//
//	DESCRIPTION
//		Class header file for the Reflection texture coordinate mapper classes.
//
//***************************************************************************
#ifndef __IFXUVMAPPERReflection_CLASS_INTERFACE_H__
#define __IFXUVMAPPERReflection_CLASS_INTERFACE_H__

#include "CIFXUVMapperNone.h"

class CIFXUVMapperReflection : public CIFXUVMapperNone 
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperReflection_Factory( IFXREFIID interfaceId, void** ppInterface );

private:
	CIFXUVMapperReflection();
	~CIFXUVMapperReflection();
};


#endif
