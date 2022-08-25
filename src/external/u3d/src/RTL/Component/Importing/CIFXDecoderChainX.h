//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
#ifndef CIFXDECODERCHAINX__
#define CIFXDECODERCHAINX__

#include "IFXDecoderChainX.h"

class CIFXDecoderChainX : public IFXDecoderChainX
{
public:
	// IFXUnknown
	U32 IFXAPI 		  AddRef ();
	U32 IFXAPI 		  Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXDecoderChainX
	virtual void IFXAPI  AddDecoderX( IFXDecoderX& rDecoderX, U32 uIndex = END_OF_CHAIN ) ; 
	virtual void IFXAPI  InitializeX() ; 
	virtual void IFXAPI  GetDecoderCountX( U32& ruDecoderCount ) ;
	virtual void IFXAPI  GetDecoderX( U32 uIndex, IFXDecoderX*& rpOutDecoderX ) ;
	virtual void IFXAPI  RemoveDecoderX( U32 uIndex = END_OF_CHAIN ) ;

private:
	CIFXDecoderChainX();
	virtual	~CIFXDecoderChainX();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXDecoderChainX_Factory( IFXREFIID iid, void** ppv );

	void IFXAPI  ReleaseArrayDecoders();

	// member variables
	IFXDecoderX**	m_ppDecoderArray;
	U32				m_uArraySize;
	U32				m_uDecoderCount;
	U32				m_uRefCount;
	
	static const U32 m_uArrayGrowthModifier;
};

#endif // #ifndef CIFXDECODERCHAINX__
