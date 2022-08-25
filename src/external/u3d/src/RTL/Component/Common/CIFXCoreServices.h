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
	@file	CIFXCoreServices.h

    Declaration of the CIFXCoreServices class which implements
    the IFXCoreServices interface. This object is responsible for
    maintaining and handing out references to the none-singleton
    components.

	@note
	Initialize - Creates all of the other system components with error
	detection.
	GetScheduler - Returns a reference to a Scheduler Interface
	GetSceneGraph - Returns a reference to a SceneManager Interface
*/

#ifndef __CIFXCORESERVICES_H__
#define __CIFXCORESERVICES_H__

#include "IFXCoreServices.h"
#include "IFXScheduler.h"
#include "IFXSceneGraph.h"
#include "IFXDidRegistry.h"
#include "IFXNameMap.h"
#include "IFXMetaDataX.h"

class CIFXCoreServices : public IFXCoreServices,
						 public IFXMetaDataX
{
public:

	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void** ppv );

	// IFXCoreServices methods
	virtual IFXRESULT IFXAPI  Initialize( U32 uProfile, F64 units = 1.0f );
	virtual IFXRESULT IFXAPI  GetScheduler( IFXREFIID interfaceId, void** ppv );
	virtual IFXRESULT IFXAPI  GetNotificationManager(IFXREFIID interfaceId, void** ppv);
	virtual IFXRESULT IFXAPI  GetSceneGraph( IFXREFIID interfaceId, void** ppv );
	virtual IFXRESULT IFXAPI  GetWeakInterface( IFXCoreServices** ppCoreServices );
	virtual IFXRESULT IFXAPI  GetNameMap( IFXREFIID interfacId, void** ppv );
	virtual IFXRESULT IFXAPI  GetFileReferencePalette( IFXPalette** ppv );
	virtual IFXRESULT IFXAPI  GetDidRegistry( IFXREFIID interfacId, void** ppv );
	virtual IFXRESULT IFXAPI  SetBaseURL(const IFXString& sURL) { m_sBaseURL = sURL; return IFX_OK; }
	virtual IFXRESULT IFXAPI  GetBaseURL(IFXString& sURL) { sURL = m_sBaseURL; return IFX_OK; }
	virtual IFXRESULT IFXAPI  GetProfile(U32& rProfile) { rProfile = m_uProfile; return IFX_OK; }
	virtual IFXRESULT IFXAPI  GetUnits(F64& rUnits) { rUnits = m_units; return IFX_OK; }

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXCoreServices_Factory( IFXREFIID interfaceId, void** ppInterface );

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

private:
	IFXMetaDataX* m_pMetaData;

	IFXRESULT CreateComponents();

	// Private to enforce the use of the create instance method
	CIFXCoreServices();
	virtual ~CIFXCoreServices();

	// PRIVATE MEMBER VARIABLES
	IFXCoreServices*	m_pWeakCS;    ///> Ptr to subobjects IFXCoreServices iface
	BOOL				m_bInitialized;
	U32					m_uRefCount;  ///> Reference counter
	IFXString			m_sBaseURL;
	IFXPalette*			m_pFileReferencePalette;
	U32					m_uProfile;
	F64					m_units;

	// Singleton objects
	IFXScheduler*		m_pScheduler; ///> Scheduler Interface
	IFXSceneGraph*		m_pSceneGraph;  ///> SceneGraph Interface
	IFXNameMap*			m_pNameMap;
	IFXDidRegistry*		m_pDidRegistry;
};

#endif
