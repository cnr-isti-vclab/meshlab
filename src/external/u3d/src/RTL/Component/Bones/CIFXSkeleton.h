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
	@file CIFXSkeleton.h
*/

#ifndef CIFX_BONES_GENERATOR_H
#define CIFX_BONES_GENERATOR_H

#include "IFXSkeleton.h"
#include "IFXPackWeights.h"

class CIFXSkeleton : virtual public IFXSkeleton
{
public:
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSkeleton_Factory(IFXREFIID interfaceId, void** ppInterface);
	
	// IFXUnknown
	U32 IFXAPI          AddRef (void);
	U32 IFXAPI          Release (void);

	IFXRESULT IFXAPI    QueryInterface (IFXREFIID riid, void **ppv);

	// IFXSkeleton interface...
	IFXRESULT  IFXAPI   SetBoneInfo( U32 uBoneID, IFXBoneInfo *pBoneInfo );
	IFXRESULT  IFXAPI   GetBoneInfo( U32 uBoneID, IFXBoneInfo *pBoneInfo );
	IFXRESULT  IFXAPI   GetNumBones( U32& uNumBones );
	IFXBonesManager* IFXAPI GetBonesManagerNR(void);

	// blocktype base determination methods
	IFXRESULT  IFXAPI   SetBlockTypeBase( U32 BlockType );
	IFXRESULT  IFXAPI   GetBlockTypeBase( U32* pBlockType );

protected:
	CIFXSkeleton();
	virtual ~CIFXSkeleton();

	virtual IFXRESULT IFXAPI  Construct();

	U32 m_uBonesManagerIndex;

	IFXBonesManager *m_pBonesMgr;

private:
	U32 m_uRefCount;

	U32 m_uBlockTypeBase;
};

#endif
