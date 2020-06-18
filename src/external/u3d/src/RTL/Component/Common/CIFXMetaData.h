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
	@file	CIFXMetaData.h

			This module defines the CIFXMetaData component.
*/


#ifndef CIFXMetaData_H
#define CIFXMetaData_H


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXMetaDataX.h"
#include "IFXArray.h"

//***************************************************************************
//	Defines
//***************************************************************************

#define IFX_E_NO_METADATA					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0014 )
#define IFX_E_OUT_OF_METADATA				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0015 )
#define IFX_E_NO_BINARY_METADATA			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0016 )
#define IFX_E_NO_STRING_METADATA			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0017 )


//***************************************************************************
//	Classes, structures and types
//***************************************************************************
struct IFXMetaDataSubattribute
{
	IFXString Name;
	IFXString Value;
	BOOL NoValue;
	IFXMetaDataSubattribute() : NoValue(FALSE) {}
};

typedef IFXArray<IFXMetaDataSubattribute> IFXSubattributes;

struct IFXMetaDataContainer 
{
	IFXString				Key;
	IFXMetaDataAttribute	Attribute;
	BOOL					Persistence;
	U32						Size;
	void*					pBuffer;
	IFXSubattributes		Subattributes;
	IFXMetaDataContainer*	pPrev;
	IFXMetaDataContainer*	pNext;
	IFXMetaDataContainer() : Attribute(IFXMetaDataAttribute(0)), Persistence(FALSE), Size(0), pBuffer(NULL), pPrev(NULL), pNext(NULL) {}
};

class CIFXMetaData : public IFXMetaDataX
{
public:

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXMetaData_Factory( IFXREFIID interfaceId, void** ppInterface );

	// Member functions:  IFXUnknown.
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// Member functions:  IFXMetaData.
	void IFXAPI 	 GetCountX(U32& rCount) const;
	IFXRESULT IFXAPI GetIndex(const IFXString& rKey, U32& rIndex);
	void IFXAPI 	 GetKeyX(U32 uIndex, IFXString& rOutKey);
	void IFXAPI 	 GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rAttribute);
	void IFXAPI 	 SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rAttribute);
	void IFXAPI 	 GetBinaryX(U32 uIndex, U8* pDataBuffer);
	void IFXAPI 	 GetBinarySizeX(U32 uIndex, U32& rSize);
	void IFXAPI 	 GetStringX(U32 uIndex, IFXString& rValue); 
	void IFXAPI 	 GetPersistenceX(U32 uIndex, BOOL& rPersistence);
	void IFXAPI 	 SetBinaryValueX(const IFXString& rKey, U32 size, const U8* pData);
	void IFXAPI 	 SetStringValueX(const IFXString& rKey, const IFXString& rValue);
	void IFXAPI 	 SetPersistenceX(U32 uIndex, BOOL value);
	void IFXAPI 	 DeleteX(U32 uIndex); 
	void IFXAPI 	 DeleteAll(); 
	void IFXAPI 	 AppendX(IFXMetaDataX* pSource);

	void IFXAPI 	 GetEncodedKeyX(U32 uIndex, IFXString& rOutKey);
	void IFXAPI 	 GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes);
	IFXRESULT IFXAPI GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex);
	void IFXAPI 	 GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName);
	void IFXAPI 	 GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue);
	void IFXAPI 	 SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue);
	void IFXAPI 	 DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex);
	void IFXAPI 	 DeleteAllSubattributes(U32 uIndex);

private:
	// Member functions.
	CIFXMetaData();
	virtual	~CIFXMetaData();

	IFXMetaDataContainer* FindTheKey(const IFXString& rValue, U32* pIndex);
	BOOL IsKeyExists(const IFXString& rValue);
	IFXMetaDataContainer* GetMetaData(U32 uIndex);
	void UnpackKey(IFXString& rKey, IFXSubattributes& rSubattributes);
	BOOL IsSubattributeExists(IFXSubattributes& rSubattributes, const IFXString& rName);
	IFXMetaDataSubattribute* GetSubattribute(IFXSubattributes& rSubattributes, const IFXString& rName);

	// Member data.
	U32	m_refCount;	///< Number of outstanding references to the component.
	U32 m_uMDCount;

	IFXMetaDataContainer* m_pMetaData;
	IFXMetaDataContainer* m_pEmptyMetaData;
};


#endif
