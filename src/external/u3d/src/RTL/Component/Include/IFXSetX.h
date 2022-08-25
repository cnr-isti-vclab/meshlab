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
/**
@file IFXSetX.h
	Declaration of IFXSet interface.
	This interface is used by the progressive geometry compression and decompression.
*/
//*****************************************************************************
#ifndef IFXSETX_H__
#define IFXSETX_H__

#include "IFXUnknown.h"

// {C9267F55-B9E7-4977-AC60-DB1B6D4FEFA0}
IFXDEFINE_GUID(IID_IFXSetX,
0xc9267f55, 0xb9e7, 0x4977, 0xac, 0x60, 0xdb, 0x1b, 0x6d, 0x4f, 0xef, 0xa0);

/**This interface is used by the progressive geometry compression and decompression.*/
class  IFXSetX : public IFXUnknown
{
public:
	virtual void IFXAPI  AddX(U32 uMember) = 0;
	virtual void IFXAPI  RemoveX(U32 uMember) = 0;
	virtual void IFXAPI  GetSizeX(U32& ruSize) = 0;
	virtual void IFXAPI  GetMemberX(U32 uIndex, U32& ruMember) = 0;
	virtual void IFXAPI  GetIndexX(U32 uMember, BOOL& rbIsMember, U32& ruIndex) = 0;
};

#endif
