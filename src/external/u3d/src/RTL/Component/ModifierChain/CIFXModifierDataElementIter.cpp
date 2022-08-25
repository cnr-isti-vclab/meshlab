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
	@file	IFXDataElement.cpp

			The implementation file for the IFXModifierDataElement.
*/

#include "CIFXModifierDataElementIter.h"
#include "IFXDebug.h"

CIFXModifierDataElementIterator::CIFXModifierDataElementIterator()
{
	m_pDataPacket = NULL;
	m_pDids = NULL;	
	m_Flags = 0;
	m_NumElements = 0;
	m_CurElement = 0;
}


CIFXModifierDataElementIterator::~CIFXModifierDataElementIterator()
{
	IFXRELEASE( m_pDataPacket );
}


// IFXRefCountObject ??
U32 CIFXModifierDataElementIterator::Release()
{
	delete this;
	return 0;
}

// IFXModifierDataElementIterator
void CIFXModifierDataElementIterator::Initialize(	U32 in_Flags, 
													IFXDidEntry* in_pDids,
													U32 in_NumElements,
													IFXModifierDataPacket* in_pDp)
{
	IFXASSERT( !m_pDataPacket );
	m_pDataPacket = in_pDp;
	m_pDataPacket->AddRef();
	m_pDids = in_pDids;
	m_NumElements = in_NumElements;	
	m_CurElement = (U32)-1;
	m_Flags = in_Flags;

}

void* CIFXModifierDataElementIterator::First()
{
	m_CurElement = (U32)-1; // set current to previous to the first
	return Next();
}

void* CIFXModifierDataElementIterator::Next()
{
	IFXASSERT(m_pDataPacket);

	++m_CurElement;
	while(m_CurElement < m_NumElements)
	{
		if( m_pDids[m_CurElement].Flags & m_Flags )
		{
			void* pTmp = NULL;
			if(IFXSUCCESS(m_pDataPacket->GetDataElement(m_CurElement, (void**)&pTmp)))
			{
				return pTmp;
			}
		}
		++m_CurElement;
	}

	return NULL;
}


U32 CIFXModifierDataElementIterator::CurIndex()
{
	return m_CurElement;
}
