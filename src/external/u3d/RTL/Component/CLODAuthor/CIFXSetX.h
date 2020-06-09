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
@file CIFXSetX.h
	Declaration of CIFXSetX class.  
	This class is used by the progressive geometry compression and decompression.
*/
//*****************************************************************************
#ifndef CIFXSETX_H__
#define CIFXSETX_H__

#include "IFXSetX.h"

class  CIFXSetX : public IFXSetX 
{
public:
	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void**	ppInterface );

	// IFXSetX
	virtual void IFXAPI  AddX(U32 uMember);
	virtual void IFXAPI  RemoveX(U32 uMember);
	virtual void IFXAPI  GetSizeX(U32& ruSize);
	virtual void IFXAPI  GetMemberX(U32 uIndex, U32& ruMember);
	virtual void IFXAPI  GetIndexX(U32 uMember, BOOL& rbIsMember, U32& ruIndex);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSetX_Factory( IFXREFIID	interfaceId, 
											void**		ppInterface );
private:
	CIFXSetX();
	virtual ~CIFXSetX();

	U32 m_uRefCount;
	U32* m_puMemberArray;
	U32 m_uArraySize;
	U32 m_uMemberCount;

	static const U32 m_uArrayGrowthSize;
};

#endif //#ifndef CIFXSETX_H__
