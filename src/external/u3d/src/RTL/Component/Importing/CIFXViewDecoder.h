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
//	CIFXViewDecoder.h
//
//		Declaration of the CIFXViewDecoder.
//		The CIFXViewDecoder is used by the CIFXLoadManager to load
//		view nodes into the scene graph. CIFXViewDecoder exposes an
//		IFXDecoderX interface to the CIFXLoadManager for this purpose.
//
//*****************************************************************************
#ifndef CIFXViewDecoder_H
#define CIFXViewDecoder_H

#include "IFXDecoderX.h"
#include "CIFXNodeDecoder.h"
#include "IFXAutoRelease.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"

class CIFXViewDecoder : virtual public IFXDecoderX,
					    public CIFXNodeBaseDecoder
{
private:
	// Member functions.
    CIFXViewDecoder();
	virtual ~CIFXViewDecoder();

	void LoadViewNodeBlockX( IFXDataBlockX &rDataBlockX );
	void LoadViewNodeBlockU3DX( IFXDataBlockX &rDataBlockX );

	F64 m_units;

public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXViewDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	
	// IFXUnknown
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	
	// IFXDecoderX
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;
};

#endif
