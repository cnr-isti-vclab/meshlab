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
	@file	CIFXNameMap.h
*/

#ifndef __CIFXNameMap_H__
#define __CIFXNameMap_H__

#include "IFXNameMap.h"
#include "IFXArray.h"
#include "IFXHash.h"
#include "IFXString.h"

// container for remapping entry in the name map
struct IFXNameMapEntry 
{
	U32 uScopeId;
	IFXString sOldName;
	IFXString sNewName;
};

// container for entry in the scope table
struct IFXScopeEntry 
{
	IFXCollisionPolicy uCollisionPolicy;
	IFXString sPrefix;
	IFXString sWorldAlias;
	U32 uProfile;
	F64 units;
};

// utility class for hash table
class IFXStringHasher 
{
public:
	U32 operator() (const IFXString& arg)
	{
		U32 hash = 0, i, len = arg.ByteCount();
		for (i = 0; i < len; i++)
			hash += arg.GetByte(i) * 13;
		return hash;
	}
};

// several type definitions to simplify member variables decralation
typedef IFXHash<const IFXString, U32, IFXStringHasher> IFXNameHash;
typedef IFXHash<const IFXString, IFXNameMapEntry, IFXStringHasher> IFXNameMapHash;
typedef IFXHash<U32, IFXScopeEntry, IFXU32Hasher> IFXScopeHash;

// CIFXNameMap
class CIFXNameMap : public IFXNameMap
{
public:
	// IFXUnknown
	U32 IFXAPI  AddRef ( void );
	U32 IFXAPI  Release ( void );
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXNameMap
	IFXRESULT  IFXAPI 	 Initialize(U32 uPalettesNum);
	IFXRESULT  IFXAPI 	 GenerateScopeId(U32& uScopeId);
	IFXRESULT  IFXAPI 	 NewScope(U32 uScopeId, IFXCollisionPolicy uPolicy = IFXCOLLISIONPOLICY_POSTMANGLE);
	IFXRESULT  IFXAPI 	 DeleteScope(U32 uScopeId);
	IFXRESULT  IFXAPI 	 SetPrefix(U32 uScopeId, const IFXString& sPrefix);
	IFXRESULT  IFXAPI 	 SetCollisionPolicy(U32 uScopeId, IFXCollisionPolicy uPolicy);
	IFXRESULT  IFXAPI 	 GetWorldAlias(U32 uScopeId, IFXString& rsWorldAlias);
	IFXRESULT  IFXAPI 	 SetWorldAlias(U32 uScopeId, const IFXString& sWorldAlias);
	IFXRESULT  IFXAPI 	 GetProfile(U32 uScopeId, U32& rProfile);
	IFXRESULT  IFXAPI 	 SetProfile(U32 uScopeId, const U32 uProfile);
	IFXRESULT  IFXAPI 	 GetUnits(U32 uScopeId, F64& rUnits);
	IFXRESULT  IFXAPI 	 SetUnits(U32 uScopeId, const F64 units);
	IFXRESULT  IFXAPI 	 Reserve(U32 uPaletteId, const IFXString& sName);
	BOOL IFXAPI	IsExist(U32 uScopeId, U32 uPaletteId, const IFXString& sName);
	IFXRESULT  IFXAPI 	 AddCustom(U32 uScopeId, U32 uPaletteId, const IFXString& sOldName, const IFXString& sNewName);
	IFXRESULT  IFXAPI 	 Map(U32 uScopeId, U32 uPaletteId, IFXString& rsName);
	IFXRESULT  IFXAPI 	 Unmap(U32 uScopeId, U32 uPaletteId, const IFXString& sName);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXNameMap_Factory(IFXREFIID interfaceId, void** ppInterface);

protected:
	CIFXNameMap();
	virtual ~CIFXNameMap();

	IFXString Scope(U32 uScopeId, const IFXString& sName);

	U32 m_uRefCount;
	U32 m_uLastLoadId;

	BOOL m_bInitialized;
	IFXScopeHash m_aScope;
	IFXArray<IFXNameHash> m_aNameHash;
	IFXArray<IFXNameMapHash> m_aNameMapHash;
};

#endif
