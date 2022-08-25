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
	@file	IFXNameMap.h

			Declaration of IFXNameMap interface.
			The IFXNameMap is used to access a downloaded file.
*/
#ifndef __IFXNameMap_H__
#define __IFXNameMap_H__

#include "IFXUnknown.h"
#include "IFXFileReference.h"

// {11B4728C-4D64-480b-8849-68334D3CE928}
IFXDEFINE_GUID(IID_IFXNameMap,
0x11b4728c, 0x4d64, 0x480b, 0x88, 0x49, 0x68, 0x33, 0x4d, 0x3c, 0xe9, 0x28);

class IFXNameMap : virtual public IFXUnknown
{
public:
	/** 
		Initializes NameMap component with number of supported palettes. 
	*/
	virtual IFXRESULT IFXAPI  Initialize(U32 palettesNum) = 0;

	/**
		This method generates unique scope id which is used as
		a scope identifier parameter for almost all functions in this component.
	*/
	virtual IFXRESULT IFXAPI  GenerateScopeId(U32& rScopeId) = 0;

	/**	
		This method creates and initializes new scope with specified
		collision policy.
	*/
	virtual IFXRESULT IFXAPI  NewScope(
						U32 scopeId, 
						IFXCollisionPolicy policy = IFXCOLLISIONPOLICY_POSTMANGLE) = 0;

	/**
		This method removes specified scope.
	*/
	virtual IFXRESULT IFXAPI  DeleteScope(U32 scopeId) = 0;

	/**
		This method sets prefix for scope. This prefix is used to avoid
	  names collision.
	*/
	virtual IFXRESULT IFXAPI  SetPrefix(U32 scopeId, const IFXString& rPrefix) = 0;

	/**
		This method sets collision policy for scope.
	*/
	virtual IFXRESULT IFXAPI  SetCollisionPolicy(U32 scopeId, IFXCollisionPolicy policy) = 0;

	/**
		This method returns world alias for scope.
	*/
	virtual IFXRESULT IFXAPI  GetWorldAlias(U32 scopeId, IFXString& rWorldAlias) = 0;

	/**
		This method sets world alias for scope.
	*/
	virtual IFXRESULT IFXAPI  SetWorldAlias(U32 scopeId, const IFXString& rWorldAlias) = 0;

	/**
		This method returns profile for scope.
	*/
	virtual IFXRESULT IFXAPI  GetProfile(U32 scopeId, U32& rProfile) = 0;

	/**
		This method sets profile for scope.
	*/
	virtual IFXRESULT IFXAPI  SetProfile(U32 scopeId, const U32 profile) = 0;

	/**
	This method returns units scale factor for scope.
	*/
	virtual IFXRESULT IFXAPI  GetUnits(U32 scopeId, F64& rUnits) = 0;

	/**
	This method sets units scale factor for scope.
	*/
	virtual IFXRESULT IFXAPI  SetUnits(U32 scopeId, const F64 units) = 0;

	/**
		This method creates entry in name map associated with specified
		scope and palette. Names same as entry name with different scope
		will be renamed (according to collision policy).
	*/
	virtual IFXRESULT IFXAPI  Reserve(U32 paletteId, const IFXString& rName) = 0;

	/**
		This method return TRUE if entry exists in specified scope and palette.
	*/
	virtual BOOL IFXAPI  IsExist(U32 scopeId, U32 paletteId, const IFXString& rName) = 0;

	/**
		This method creates custom name mapping within specified scope and
		palette. Param sOldName should be unique within scope, and param
		sNewName should be unique within palette.
	*/
	virtual IFXRESULT IFXAPI  AddCustom(U32 scopeId, U32 paletteId, const IFXString& rOldName, const IFXString& rNewName) = 0;

	/**
		This method provides name mapping for specified entry within scope
		and palette. If name is already exist within the same scope and palette,
		function just return. If name exist within different scope, but same
		palette, function renames it according to collision policy and creates
		name mapping, so future call with same name will result in same mapping.
	*/
	virtual IFXRESULT IFXAPI  Map(U32 scopeId, U32 paletteId, IFXString& rName) = 0;

	/**
		This method removes mapping for specified entry name.
	*/
	virtual IFXRESULT IFXAPI  Unmap(U32 scopeId, U32 paletteId, const IFXString& rName) = 0;
};

#endif
