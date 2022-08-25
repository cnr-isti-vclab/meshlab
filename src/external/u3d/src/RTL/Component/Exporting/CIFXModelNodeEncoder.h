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
//	CIFXModelNodeEncoder.h
//
//	DESCRIPTION:
//		Declaration of the CIFXModelNodeEncoder.
//		The CIFXModelNodeEncoder contains model node encoding functionality 
//		that is used by the write manager.
//	
//*****************************************************************************

#ifndef CIFXMODELNODEENCODER_H
#define CIFXMODELNODEENCODER_H


// no ordering requirements for include files 
#include "CIFXNodeBaseEncoder.h"
#include "IFXCoreServices.h"
#include "IFXDataBlockX.h"
#include "IFXEncoderX.h"
#include "IFXString.h"


class  CIFXModelNodeEncoder : virtual public IFXEncoderX,
							  public CIFXNodeBaseEncoder
{
public:
	
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXModelNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
	U32 IFXAPI 			AddRef ( void );
	U32 IFXAPI 			Release ( void );
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXEncoderX
	void	IFXAPI		EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units = 1.0f);
	void	IFXAPI		InitializeX( IFXCoreServices& rCoreServices );
	void	IFXAPI		SetObjectX( IFXUnknown& rObject );


private:
	// methods
	CIFXModelNodeEncoder();
	~CIFXModelNodeEncoder();

	// members
	BOOL	m_bInitialized;
	U32		m_uRefCount;
};

#endif
