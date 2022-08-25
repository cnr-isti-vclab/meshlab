//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
@file IFXEncoderX.h
	Declaration of IFXEncoder interface.
	This interface is used by the Write Manager to get data blocks from an encoder.
*/
//*****************************************************************************
#ifndef IFXENCODERX_H__
#define IFXENCODERX_H__

#include "IFXString.h"
#include "IFXDataBlockQueueX.h"
#include "IFXCoreServices.h"

// {53B8214D-E3FB-4207-B6E4-1D3FF96D3300}
IFXDEFINE_GUID(IID_IFXEncoderX,
0x53b8214d, 0xe3fb, 0x4207, 0xb6, 0xe4, 0x1d, 0x3f, 0xf9, 0x6d, 0x33, 0x0);

/// The IFXEncoderX interface is implemented by various encoders used by the CIFXWriteManager.
///	Each instance of an encoder is used to create data blocks for one instance of a scene graph object.
/// Each type of encoder is used to create data blocks for a particular type of object.
/// The data blocks contain encoded state information for the object.
class  IFXEncoderX : virtual public IFXUnknown
{
public:
	/// Provides the encoder with a pointer to the object which is to be encoded.
	/**
	@note SetObject will normally be called by the associated object during GetEncoder.
	SetObject is not normally called by the Write Manager.
	Although not the usual case, it is possible for the object to expose
	the encoder interface in which case the encoder and the object are
	the same object.
	*/
	/** @return One of the following IFXRESULT codes: \n\n
	-		IFX_OK \n
			No error \n\n
	-		IFX_E_NOT_SUPPORTED \n
			SetObject is not supported if the object and the encoder are the same object.  \n\n
	*/
	virtual void IFXAPI  SetObjectX(IFXUnknown &rObject) = 0;

	/// Initializes and get a reference to the core services
	virtual void IFXAPI  InitializeX(IFXCoreServices &rCoreServices) = 0;

	/// Encodes data into data blocks and place these blocks in a queue
	virtual void IFXAPI  EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units = 1.0f ) = 0;
};

#endif
