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
	@file	IFXBoundHierarchyMgr.h

			The header file that defines the IFXBoundHierarchyMgr interface.
*/

#ifndef __IFXBOUNDHIERARCHYMGR_INTERFACE_H__
#define __IFXBOUNDHIERARCHYMGR_INTERFACE_H__

#include "IFXUnknown.h"
#include "IFXBoundHierarchy.h"

// {878C1141-57D9-4c0d-AC6B-E63BF832BA0A}
IFXDEFINE_GUID(IID_IFXBoundHierarchyMgr,
0x878c1141, 0x57d9, 0x4c0d, 0xac, 0x6b, 0xe6, 0x3b, 0xf8, 0x32, 0xba, 0xa);

/** 
	This interface defines the IFXBoundHierarchyMgr.
*/
class IFXBoundHierarchyMgr : virtual public IFXUnknown

{
public:

	/**
		Returns the bounding hierarchy of the model's meshgroup.

		@param ppOutBH
				The address of the IFXBoundHierarchy pointer to contain the address
				of the bound hierarchy for the model's meshgroup.

		@return One of the following IFXRESULT codes:
				- IFX_OK
				No error.
				- IFX_E_OUT_OF_MEMORY
				Not enough memory was available to initialize.                 
	*/
	virtual IFXRESULT IFXAPI  GetBoundHierarchy( IFXBoundHierarchy** ppOutBH ) = 0;
};

#endif
