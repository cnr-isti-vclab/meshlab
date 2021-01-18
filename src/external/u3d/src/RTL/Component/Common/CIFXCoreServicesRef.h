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
//	CIFXCoreServicesRef.h
//
//	DESCRIPTION:
//		Declaration of the CIFXCoreServicesRef class which implements the 
//		IFXCoreServicesRef interface. Objects of this class are used to
//		prevent circular reference problems that might exist when multiple
//		references to the CoreServices object are created. 
//
//	NOTES
//	All of the interface functions in the CIFXCoreServicesRef object are
//	wrappers to the functions of the same name in the underlying 
//	CIFXCoreServices object. The purpose of these functions is to foward
//	function calls on this object to the real CoreServices object.
//
//	SetReference is the one exception. The purpose of this function is to set
//	up the pointer to the real CoreServices object so that the above forwarding
//	scheme works correctly.
//*****************************************************************************
#ifndef __CIFXCORESERVICESREF_H__
#define __CIFXCORESERVICESREF_H__

#include "IFXCoreServices.h"
#include "IFXCoreServicesRef.h"

#include "CIFXCoreServices.h"

class CIFXCoreServicesRef : 
	public IFXCoreServices,
	public IFXCoreServicesRef,
	public IFXMetaDataX
{
public:

	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void**	ppInterface );

	// IFXCoreServicesRef methods
	virtual IFXRESULT IFXAPI  SetReference( IFXCoreServices* pCoreServices );

	// IFXCoreServices methods
	virtual IFXRESULT IFXAPI  Initialize( U32 uProfile, F64 units = 1.0f );
	virtual IFXRESULT IFXAPI  GetScheduler( IFXREFIID interfaceId, void** ppv );
	virtual IFXRESULT IFXAPI  GetNotificationManager( IFXREFIID interfaceId, void** ppv );
	virtual IFXRESULT IFXAPI  GetSceneGraph( IFXREFIID interfaceId, void** ppv );
	virtual IFXRESULT IFXAPI  GetWeakInterface( IFXCoreServices** ppCoreServices );
	virtual IFXRESULT IFXAPI  GetNameMap( IFXREFIID interfacId, void** ppv );
	virtual IFXRESULT IFXAPI  GetFileReferencePalette( IFXPalette** ppv );
	virtual IFXRESULT IFXAPI  SetBaseURL(const IFXString& sURL);
	virtual IFXRESULT IFXAPI  GetBaseURL(IFXString& sURL);
	virtual IFXRESULT IFXAPI  GetProfile(U32& rProfile);
	virtual IFXRESULT IFXAPI  GetUnits(F64& rUnits);

	// IFXMetaData
	virtual void IFXAPI  GetCountX(U32& rCount)const;
	virtual IFXRESULT IFXAPI  GetIndex(const IFXString& rKey, U32& uIndex);
	virtual void IFXAPI  GetKeyX(U32 index, IFXString& rOutKey);
	virtual void IFXAPI  GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rAttribute);
	virtual void IFXAPI  SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rAttribute);
	virtual void IFXAPI  GetBinaryX(U32 uIndex, U8* pDataBuffer);
	virtual void IFXAPI  GetBinarySizeX(U32 uIndex, U32& rSize);
	virtual void IFXAPI  GetStringX(U32 uIndex, IFXString& rValue);
	virtual void IFXAPI  GetPersistenceX(U32 uIndex, BOOL& rPersistence);
	virtual void IFXAPI  SetBinaryValueX(const IFXString& rKey, U32 length, const U8* data);
	virtual void IFXAPI  SetStringValueX(const IFXString& rKey, const IFXString& rValue);
	virtual void IFXAPI  SetPersistenceX(U32 uIndex, BOOL value);
	virtual void IFXAPI  DeleteX(U32 uIndex);
	virtual void IFXAPI  DeleteAll();
	virtual void IFXAPI  AppendX(IFXMetaDataX* pSource);

	virtual void IFXAPI  GetEncodedKeyX(U32 uIndex, IFXString& rOutKey);
	virtual void IFXAPI  GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes);
	virtual IFXRESULT IFXAPI  GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex);
	virtual void IFXAPI  GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName);
	virtual void IFXAPI  GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue);
	virtual void IFXAPI  SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue);
	virtual void IFXAPI  DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex);
	virtual void IFXAPI  DeleteAllSubattributes(U32 uIndex);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXCoreServicesRef_Factory( IFXREFIID	interfaceId, void**	ppInterface );

protected:

	IFXCoreServices* m_pCoreServices;	// Ptr to IFXCoreServices interface

private:
	// Private to enforce the use of the create instance method
	CIFXCoreServicesRef();
	virtual ~CIFXCoreServicesRef();

	// PRIVATE MEMBER VARIABLES
	U32		m_uRefCount;
};

#endif // __CIFXCORESERVICES_H__
