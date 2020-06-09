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
	@file	CIFXFileReferenceEncoder.h
			
			Declaration of CIFXFileReferenceEncoder class implementation
*/

#ifndef CIFXFileReferenceEncoder_H__
#define CIFXFileReferenceEncoder_H__

#include "IFXCoreServices.h"
#include "IFXEncoderX.h"
#include "IFXFileReference.h"
#include "IFXAutoRelease.h"

class CIFXFileReferenceEncoder : virtual public IFXEncoderX
{
public:
	// IFXUnknown
	U32 IFXAPI  AddRef ( void );
	U32 IFXAPI  Release ( void );
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	// IFXEncoderX
	/// Provide the encoder with a pointer to the object which is to be encoded.
	void IFXAPI	SetObjectX(IFXUnknown &rObject);
	/// Initialize and get a reference to the core services
	void IFXAPI	InitializeX(IFXCoreServices &rCoreServices);
	/// Encode data into data blocks and place these blocks in a queue
	void IFXAPI	EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units = 1.0f);

	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXFileReferenceEncoder_Factory(IFXREFIID interfaceId, void** ppInterface);

private:

	CIFXFileReferenceEncoder();
	virtual ~CIFXFileReferenceEncoder();

	U32 m_uRefCount;	// Reference count for IFXUnknown

	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXFileReference,m_pFileReference);
};

#endif
