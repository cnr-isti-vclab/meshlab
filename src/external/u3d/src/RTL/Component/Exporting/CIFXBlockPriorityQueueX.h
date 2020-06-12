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
//
// CIFXBlockPriorityQueueX.h
//
// DESCRIPTION:
//     Declaration of CIFXBlockPriorityQueueX class which implements 
// the IFXBlockPriorityQueueX and IFXBlockPriorityQueue interfaces.  
// IFXBlockPriorityQueue is used to hold a list of pointers to IFXDataBlock.
//
//*****************************************************************************
#ifndef __CIFXBLOCKPRIORITYQUEUEX_H__
#define __CIFXBLOCKPRIORITYQUEUEX_H__

#include "IFXDataBlockQueueX.h"
#include "IFXAutoRelease.h"

/**
	used internally for our linked-list

	There are two types of nodes: data nodes and priority nodes.

	Priority nodes indicate a change in priority level.
	m_pPriority links forward to the next priority node, if any.
	m_pDataBlock is used to 

	Data nodes contain regular IFXDataBlocks, and the m_pPriority
	member points BACK to the governing priority node.
*/
struct IFXDataBlockNodeX 
{
	// constructor makes sure everything gets properly initialized to NULL
	IFXDataBlockNodeX();

	// destructor minimizes memory leaks
	~IFXDataBlockNodeX();

	BOOL m_isPriorityNode; ///< TRUE = priority node, FALSE = regular node
	U32 m_priority; ///< new priority
	
	union 
	{
		IFXDataBlockX*		m_pDataBlock;		///< regular node
		IFXDataBlockNodeX*	m_pLast;			///< priority node
	};
	
	union 
	{
		IFXDataBlockNodeX*	m_pLastPriority;	///< regular node
		IFXDataBlockNodeX*	m_pNextPriority;	///< priority node
	};

	IFXDataBlockNodeX* m_pNext;
};


class CIFXBlockPriorityQueueX : public IFXDataBlockQueueX
{
public:
	
	// IFXDataBlockQueueX
	/// Clear out the data block list, releasing the contents
	virtual void IFXAPI  ClearX();

	/// Get the next data block from the list (removing it from the list)
	virtual void IFXAPI  GetNextBlockX(IFXDataBlockX*& rpDataBlock, BOOL& rbDone);

	/// Peek at the next block in the list without removing it from the list
	virtual void IFXAPI  PeekNextBlockX(IFXDataBlockX*& rpDataBlockX);

	/// Add a data block to the end of the list
	virtual void IFXAPI  AppendBlockX(IFXDataBlockX& rDataBlock);

	/// Get a copy of the data block list
	virtual void IFXAPI  CopyX(IFXDataBlockQueueX*& rpDataBlockQueue);

	// IFXUnknown 
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void** ppInterface);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXBlockPriorityQueueX_Factory( IFXREFIID interfaceId, 
													  void** ppInterface );
private:
	// Private to enforce the use of the create instance method
	CIFXBlockPriorityQueueX();
	virtual ~CIFXBlockPriorityQueueX();

	// PRIVATE MEMBER VARIABLES
	U32 m_uRefCount;	// Reference counter

	/** 
		the "queue" is a series of sequential queues which are built in place,
		then accessed in the manner of a stack. See code for explanation... :-)
	*/
	IFXDataBlockNodeX* m_pQueue;

	/**
		ONCE YOU CALL "GetNextBlock" YOU MAY NOT CALL "AppendBlock" AGAIN
		This flag enforces this behavior
	*/
	BOOL m_accessing;
};

#endif
