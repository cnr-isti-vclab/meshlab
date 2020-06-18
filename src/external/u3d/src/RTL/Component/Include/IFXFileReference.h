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
	@file	IFXFileReference.h

		    IFXFileReference interface.
*/

#ifndef __IFXFILEREFERENCE_H__
#define __IFXFILEREFERENCE_H__

#include "IFXUnknown.h"
#include "IFXArray.h"

enum IFXCollisionPolicy
{	
	/** 
		Replace existing object with the new object from external file. 
		
		@note	This is not a reset because some existing items might not clash or 
				overlap at all.
	*/
	IFXCOLLISIONPOLICY_REPLACE = 0,

	/**
		Discard the new object from external file.
	*/
	IFXCOLLISIONPOLICY_DISCARD = 1,

	/**
		Prepend scope name to object name for all objects from the external file.

		@note	The new name with prepended scope name may still collide with 
				an existing object. In this situation, the new object from external file
				will replace that existing object.
	*/
	IFXCOLLISIONPOLICY_PREPENDALL = 2,

	/**
		Prepend scope name to new obkect name if there is a collision.

		@note	The new name with prepended scope name may still collide with 
				an existing object. In this situation, the new object from external file
				will replace that existing object.
	*/
	IFXCOLLISIONPOLICY_PREPENDCOLLIDED = 3,

	/**
		Append instance number to new object name if there is a collision.

		@note	Instance number will be unique.
	*/
	IFXCOLLISIONPOLICY_POSTMANGLE = 4
};

enum IFXObjectFilterType	
{	
	IFXOBJECTFILTER_NAME = 0,
	IFXOBJECTFILTER_TYPE = 1
};

// {8C03CF6E-FCDD-4a5e-8371-AB984F0DF51D}
IFXDEFINE_GUID(IID_IFXFileReference,
0x8c03cf6e, 0xfcdd, 0x4a5e, 0x83, 0x71, 0xab, 0x98, 0x4f, 0xd, 0xf5, 0x1d);

struct IFXObjectFilter 
{ 
	U32 Size; // size of whole structure (for future extensions)
	IFXObjectFilterType FilterType; // 1 - object name, 2 - object type
	IFXString ObjectNameFilterValue;
	U32 ObjectTypeFilterValue;
};

typedef IFXArray<IFXObjectFilter> IFXObjectFilters;
typedef IFXArray<IFXString> IFXFileURLs;

struct IFXFileReferenceParams 
{
	IFXString ScopeName;
	IFXFileURLs FileURLs; // should not be set for root file
	IFXObjectFilters ObjectFilters;
	IFXCollisionPolicy CollisionPolicy;
	IFXString WorldAlias;
};

class IFXFileReference : virtual public IFXUnknown
{
public:
	virtual void IFXAPI  SetScopeName(const IFXString& ScopeName) = 0;
	virtual void IFXAPI  GetScopeName(IFXString& rScopeName) = 0;
	virtual void IFXAPI  SetFileURLs(const IFXFileURLs& FileURLs) = 0;
	virtual void IFXAPI  GetFileURLs(IFXFileURLs& rFileURLs) = 0;
	virtual void IFXAPI  SetObjectFilters(const IFXObjectFilters& ObjectFilters) = 0;
	virtual void IFXAPI  GetObjectFilters(IFXObjectFilters& rObjectFilters) = 0;
	virtual void IFXAPI  SetCollisionPolicy(const IFXCollisionPolicy& CollisionPolicy) = 0;
	virtual void IFXAPI  GetCollisionPolicy(IFXCollisionPolicy& rCollisionPolicy) = 0;
	virtual void IFXAPI  SetWorldAlias(const IFXString& WorldAlias) = 0;
	virtual void IFXAPI  GetWorldAlias(IFXString& rWorldAlias) = 0;
};

#endif
