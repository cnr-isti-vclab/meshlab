//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file	CIFXDummyModifierDecoder.h

			Declaration of the CIFXDummyModifierDecoder.
*/

#ifndef CIFXDummyModifierDecoder_H
#define CIFXDummyModifierDecoder_H

#include "IFXSceneGraph.h"
#include "IFXModifierBaseDecoder.h"
#include "IFXDummyModifier.h"
#include "IFXDecoderX.h"

class CIFXDummyModifierDecoder : virtual public IFXDecoderX, 
										 public IFXModifierBaseDecoder
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifierDecoder_Factory( 
									IFXREFIID interfaceId, 
									void** ppInterface );

	// IFXUnknown
	U32 IFXAPI  AddRef();
	U32 IFXAPI  Release();
	IFXRESULT IFXAPI  QueryInterface( 
									IFXREFIID interfaceId, 
									void** ppInterface );

	// IFXDecoderX
	void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX);
	void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer);

private:
	// Member functions.
	CIFXDummyModifierDecoder();
	virtual ~CIFXDummyModifierDecoder();
};

#endif
