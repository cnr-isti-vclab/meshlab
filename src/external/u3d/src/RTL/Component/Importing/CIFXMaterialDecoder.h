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
//
//	CIFXMaterialDecoder.h
//
//		Declaration of the CIFXMaterialDecoder.
//		The CIFXMaterialDecoder is used by the CIFXLoadManager to load
//		materials into the scene graph. CIFXMaterialDecoder exposes an
//		IFXDecoderX interface to the CIFXLoadManager for this purpose.
//
//*****************************************************************************
#ifndef CIFXMATERIALDECODER_H
#define CIFXMATERIALDECODER_H

#include "IFXDecoderX.h"
#include "IFXDataBlockQueueX.h"
#include "IFXMaterialResource.h"
#include "IFXAutoRelease.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"

class CIFXMaterialDecoder : virtual public IFXDecoderX
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXMaterialDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXDecoderX
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;

private:
	// Member functions.
    CIFXMaterialDecoder();
	virtual ~CIFXMaterialDecoder();

	void ProcessMaterialBlockX(IFXDataBlockX &rDataBlockX);

	// Member data.
	U32	m_uRefCount;
	U32 m_uLoadId;
	BOOL m_bExternal;
	IFXDECLAREMEMBER(IFXMaterialResource,m_pMaterialResource);
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXDataBlockQueueX,m_pDataBlockQueueX);
};

#endif
