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
	@file	IFXModifierDataElement.h

			The header file that defines the IFXModifierDataElement. 
*/

#ifndef __IFXMODIFIERDATAELEMENTITER_H__
#define __IFXMODIFIERDATAELEMENTITER_H__

#include "IFXDataTypes.h"

class IFXModifierDataElementIterator
{
public:
	virtual ~IFXModifierDataElementIterator() {}
	virtual U32 Release( void ) = 0;

	//  IFXModifierDataElementIterator
	virtual void* First() = 0;
	virtual void* Next() = 0;
	virtual U32   CurIndex() = 0;
};

#endif
