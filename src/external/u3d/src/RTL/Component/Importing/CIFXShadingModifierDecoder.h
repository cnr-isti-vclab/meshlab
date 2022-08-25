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
	@file	CIFXShadingModifierDecoder.h

			Declaration of the CIFXShadingModifierDecoder.
*/

#ifndef CIFXShadingModifierDecoder_H
#define CIFXShadingModifierDecoder_H

#include "IFXDecoderX.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"
#include "IFXModifierBaseDecoder.h"

class CIFXShadingModifierDecoder : virtual public IFXDecoderX,
										   public IFXModifierBaseDecoder
{
public:
	// IFXUnknown
	virtual U32 IFXAPI  AddRef();
	virtual U32 IFXAPI  Release();
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXDecoderX
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;

private:
	// Member functions.
    CIFXShadingModifierDecoder();
	virtual ~CIFXShadingModifierDecoder();

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	void ProcessShadingModifierBlockX( IFXDataBlockX &rDataBlockX );
};

#endif
