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
//
// CIFXDataBlockQueueX.h
//
// DESCRIPTION:
//     Declaration of CIFXDataBlockQueue class which implements 
// the IFXDataBlockQueue interface.  IFXDataBlockQueue is used 
// to hold a list of pointers to IFXDataBlock.
//
//*****************************************************************************
#ifndef CIFXDATABLOCKQUEUEX_H__
#define CIFXDATABLOCKQUEUEX_H__

#include "IFXDataBlockQueueX.h"
#include "IFXAutoRelease.h"

class CIFXDataBlockQueueX : public IFXDataBlockQueueX
{
public:
	// IFXDataBlockQueueX

	/// Clear out the data block list, releasing the contents
	virtual void IFXAPI  ClearX();

	/// Get the next data block from the list (removing it from the list)
	virtual void IFXAPI  GetNextBlockX(IFXDataBlockX*& rpDataBlockX, BOOL& rbDone);

	/// Peek at the next block in the list without removing it from the list
	virtual void IFXAPI  PeekNextBlockX(IFXDataBlockX*& rpDataBlockX);

	/// Add a data block to the end of the list
	virtual void IFXAPI  AppendBlockX(IFXDataBlockX& rDataBlockX);

	/// Get a copy of the data block list
	virtual void IFXAPI  CopyX(IFXDataBlockQueueX*& rpDataBlockQueueX);

	// IFXUnknown 
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void** ppInterface);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXDataBlockQueueX_Factory(IFXREFIID interfaceId, 
											void** ppInterface );
private:
	// Private to enforce the use of the create instance method
	CIFXDataBlockQueueX();
	virtual ~CIFXDataBlockQueueX();

	// PRIVATE MEMBER VARIABLES
	U32	m_uRefCount;	// Reference counter

	// The data block list is stored as an array of pointers to IFXDataBlock
	// The size of the array is m_uDataBlockListSize
	// The number of elements actually used in the array is m_uDataBlockCount.
	// m_uDataBlockCount cannot be larger than m_uDataBlockListSize.
	// m_uDataBlockCurrent is the index of the next element to read.
	// There are no more elements to read when 
	//		m_uDataBlockCurrent == m_uDataBlockCount
	//		or m_ppDataBlockList == NULL.
	IFXDataBlockX** m_ppDataBlockList;

	U32 m_uDataBlockListSize;
	U32 m_uDataBlockCount;
	U32 m_uDataBlockCurrent;

	static const U32 m_uInitialListSize;
	static const U32 m_uIncrementListSize;
};

#endif
