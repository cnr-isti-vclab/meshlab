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

#ifndef IFXDECODERCHAINX_H__
#define IFXDECODERCHAINX_H__

#include "IFXDecoderX.h"

// {4E39341B-5883-4e0c-8730-C1D4B757456C}
IFXDEFINE_GUID(IID_IFXDecoderChainX,
0x4e39341b, 0x5883, 0x4e0c, 0x87, 0x30, 0xc1, 0xd4, 0xb7, 0x57, 0x45, 0x6c);

class IFXDecoderChainX : public IFXUnknown
{
public:
	enum { END_OF_CHAIN = (U32)-1 };

	virtual void IFXAPI  AddDecoderX( 
							IFXDecoderX& rDecoderX, 
							U32 index = END_OF_CHAIN ) = 0;
	virtual void IFXAPI  InitializeX() = 0;
	virtual void IFXAPI  GetDecoderCountX( U32& rDecoderCount ) = 0;
	virtual void IFXAPI  GetDecoderX( U32 index, IFXDecoderX*& rpOutDecoderX ) = 0;
	virtual void IFXAPI  RemoveDecoderX( U32 index = END_OF_CHAIN ) = 0;
};

#endif
