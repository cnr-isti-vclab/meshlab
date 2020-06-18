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
//	IFXCoreServicesRef.h
//
//	DESCRIPTION:
//		Declaration of IFXCoreServicesRef interface. This interface is used to
//		prevent circular reference problems that might exist when multiple
//		references to the CoreServices object are created.
//
//	NOTES
//*****************************************************************************
#ifndef __IFXCORESERVICESREF_H__
#define __IFXCORESERVICESREF_H__

#include "IFXUnknown.h"

// {B47CC271-AD04-11d3-98A9-00A0C9902D25}
IFXDEFINE_GUID(IID_IFXCoreServicesRef,
0xb47cc271, 0xad04, 0x11d3, 0x98, 0xa9, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

/**This interface is used to prevent circular reference problems that might exist when multiple references to the CoreServices object are created.*/
class IFXCoreServicesRef : public IFXUnknown
{
public:

	// IFXCoreServicesRef
	virtual IFXRESULT IFXAPI  SetReference(IFXCoreServices* pCoreServices) = 0;

};

#endif
