//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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
	@file	CIFXModifierDataElementIterator.h

			The header file that defines the base implementation class of the
			CIFXModifierDataElementIterator. 
*/

#ifndef __CIFXMODIFIERDATAELEMENTITERATOR_H__
#define __CIFXMODIFIERDATAELEMENTITERATOR_H__

#include "IFXModifierDataElementIter.h"
#include "IFXModifierDataPacket.h"

class CIFXModifierDataElementIterator : public IFXModifierDataElementIterator
{
public:
	CIFXModifierDataElementIterator();
	virtual ~CIFXModifierDataElementIterator();

	virtual U32 Release( void );

	//  CIFXModifierDataElementIterator
	void Initialize(U32 in_Flags,
			IFXDidEntry* in_pElements,
			U32 in_NumElements,
			IFXModifierDataPacket* in_pDp);

	//  IFXModifierDataElementIterator
	void* First();
	void* Next();
	U32	CurIndex();

private:
	IFXModifierDataPacket* m_pDataPacket;
	IFXDidEntry* m_pDids;
	U32 m_NumElements;
	U32 m_CurElement;
	U32 m_Flags;
};

#endif
