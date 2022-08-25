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
// CIFXBlockPriorityQueueX.cpp
//
// DESCRIPTION:
//     Declaration of CIFXBlockPriorityQueue class implementation.
// CIFXBlockPriorityQueue is used to hold a list of pointers to IFXDataBlock.
//
//*****************************************************************************

#include "CIFXBlockPriorityQueueX.h"
#include "IFXBitStreamX.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXBlockTypes.h"

//---------------------------------------------------------------------
// PUBLIC STRUCT IFXDataBlockNodeX
//---------------------------------------------------------------------

IFXDataBlockNodeX::IFXDataBlockNodeX()
{
	m_isPriorityNode = FALSE;
	m_priority       = 0xFFFFFFF; // "obvious" marker
	m_pDataBlock     = NULL;
	m_pNext          = NULL;
	m_pLastPriority  = NULL;
}

IFXDataBlockNodeX::~IFXDataBlockNodeX()
{
	// check for incorrect usage (ie potential memory leaks)
	IFXASSERT(m_pDataBlock    == NULL);
	IFXASSERT(m_pNext         == NULL);
	IFXASSERT(m_pLastPriority == NULL);
}

//---------------------------------------------------------------------
// PUBLIC constructor
//---------------------------------------------------------------------
CIFXBlockPriorityQueueX::CIFXBlockPriorityQueueX()
{
	m_uRefCount    = 0;
	m_pQueue     = NULL;
	m_accessing    = FALSE;
}

//---------------------------------------------------------------------
// PUBLIC destructor
//---------------------------------------------------------------------
CIFXBlockPriorityQueueX::~CIFXBlockPriorityQueueX()
{
	ClearX();
}

//---------------------------------------------------------------------
// PUBLIC IFXDataBlockQueue::Clear
// Clear out the data block list, releasing the contents
// No tail recursion here...
//---------------------------------------------------------------------
void CIFXBlockPriorityQueueX::ClearX()
{
	/// @todo: Need to examine for potential memory leaks which might occur if
	//     an exception takes place.

	if (m_pQueue)
	{
		IFXDataBlockNodeX *pCurrentNode = m_pQueue;
		IFXDataBlockNodeX *pSoonToDie = NULL;

		while (pCurrentNode)
		{
			pSoonToDie = pCurrentNode;
			pCurrentNode = pCurrentNode->m_pNext;

			if (pSoonToDie->m_isPriorityNode)
			{
				pSoonToDie->m_pLast = NULL;
				pSoonToDie->m_pNextPriority = NULL;
			}

			else
			{
				if (pSoonToDie->m_pDataBlock)
					pSoonToDie->m_pDataBlock->Release();
				pSoonToDie->m_pDataBlock = NULL;
				pSoonToDie->m_pLastPriority = NULL;
			}

			pSoonToDie->m_pNext = NULL;
			delete pSoonToDie;
		}

		m_pQueue = NULL;
	}

	m_accessing = FALSE; // reset
}

//---------------------------------------------------------------------
// PUBLIC IFXDataBlockQueue::GetNextBlock
// Get the next data block from the list (removing it from the list)
//---------------------------------------------------------------------
void CIFXBlockPriorityQueueX::GetNextBlockX(IFXDataBlockX*& rpDataBlock, BOOL& rbDone)
{
	if (m_pQueue)
	{
		// pop the next entry off the Queue
		IFXDataBlockNodeX *pNode = m_pQueue;
		m_pQueue = m_pQueue->m_pNext;
		pNode->m_pNext = NULL;

		m_accessing = TRUE;

		if (pNode->m_isPriorityNode)
		{
			IFXBitStreamX* pBitStream = NULL;
			IFXDECLARELOCAL(IFXDataBlockX,pDataBlock);

			// got a priority node
			// create a BitStream to create a DataBlock
			IFXCHECKX( IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStream) );

			// Write the new priority
			pBitStream->WriteU32X(pNode->m_priority);

			// Get the block
			pBitStream->GetDataBlockX(pDataBlock);

			// Set the data block type
			pDataBlock->SetBlockTypeX(BlockType_FilePriorityUpdateU3D);

			// Release the bitstream
			IFXRELEASE(pBitStream);

			if (pDataBlock)
			{
				// set it as return value
				rpDataBlock = pDataBlock;
				rpDataBlock->AddRef(); // yeah I know this is redundant, but...
				pDataBlock->Release();
				pDataBlock = NULL;
			}

			pNode->m_pLast = NULL;
			pNode->m_pNextPriority = NULL;
			delete pNode;
		}
		else // regular (non-priority) node
		{
			rpDataBlock = pNode->m_pDataBlock;
			rpDataBlock->AddRef();
			IFXRELEASE( pNode->m_pDataBlock );

			pNode->m_pLastPriority = NULL;
			delete pNode;
		}

		if (m_pQueue == NULL)
		{
			rbDone = TRUE;
		}
		else
		{
			rbDone = FALSE;
		}
	}
	else // queue is empty
	{
		rbDone = TRUE;
	}
}

// Peek at the next block in the list without removing it from the list
void CIFXBlockPriorityQueueX::PeekNextBlockX(IFXDataBlockX*& rpDataBlockX)
{
	IFXCHECKX(IFX_E_UNSUPPORTED);
}

//---------------------------------------------------------------------
// PUBLIC IFXDataBlockQueue::AppendBlock
// Add a data block to the end of the list
// Lots of Icky Pointers
//---------------------------------------------------------------------
void CIFXBlockPriorityQueueX::AppendBlockX(IFXDataBlockX& rDataBlock)
{
	//  if (m_accessing)
	//    IFXCHECKX(IFX_E_UNDEFINED);

	// get the data block priority
	U32 priority = rDataBlock.GetPriorityX();

	// locate correct priority node in the queue
	IFXDataBlockNodeX* pPriorityNode = m_pQueue;

	if (pPriorityNode)
	{
		// queue is not empty, find the correct priority node
		// first node in queue should ALWAYS be a priority node...
		IFXASSERT(pPriorityNode->m_isPriorityNode);

		// ok, find...
		while (pPriorityNode)
		{
			if (pPriorityNode->m_priority > priority)
			{
				// insert at head of non-empty list
				IFXDataBlockNodeX * pTemp = new IFXDataBlockNodeX;
				if (pTemp)
				{
					pTemp->m_isPriorityNode = TRUE;
					pTemp->m_priority = priority;
					pTemp->m_pLast = pTemp; // yes this is a circular reference

					// link into queue
					pTemp->m_pNextPriority = pPriorityNode;
					pTemp->m_pNext = pPriorityNode;
					m_pQueue = pTemp;
					pPriorityNode = pTemp;
					break; // found it
				}
				else
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
			}

			else if (pPriorityNode->m_priority == priority)
			{
				// found it
				break;
			}

			else if ((pPriorityNode->m_pNextPriority == NULL) ||
				(pPriorityNode->m_pNextPriority->m_priority > priority))
			{
				// insert here
				IFXDataBlockNodeX* pTemp = new IFXDataBlockNodeX;
				if (pTemp)
				{
					pTemp->m_isPriorityNode = TRUE;
					pTemp->m_priority = priority;
					pTemp->m_pLast = pTemp; // yes this is a circular reference

					// link into queue
					pTemp->m_pNextPriority = pPriorityNode->m_pNextPriority;
					pTemp->m_pNext = pPriorityNode->m_pNextPriority;
					pPriorityNode->m_pNextPriority = pTemp;
					pPriorityNode->m_pLast->m_pNext = pTemp;
					pPriorityNode = pTemp;
					break; // found it
				}
				else
					IFXCHECKX(IFX_E_OUT_OF_MEMORY);
			}
			else
			{
				// jump to the next Priority Node
				pPriorityNode = pPriorityNode->m_pNextPriority;
			}

		} // while
	}
	else // queue is empty; create a new priority node
	{
		pPriorityNode = new IFXDataBlockNodeX;
		if (pPriorityNode)
		{
			pPriorityNode->m_isPriorityNode = TRUE;
			pPriorityNode->m_priority = priority;
			pPriorityNode->m_pLast = pPriorityNode; // yes this is a circular reference
			pPriorityNode->m_pNextPriority = NULL;
			pPriorityNode->m_pNext = NULL;

			// link into queue
			m_pQueue = pPriorityNode;
		}
		else
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);

	}

	IFXDataBlockNodeX* pDataNode = NULL;
	// Pkay, if we got this far successfully, we now have a valid
	// Priority Node of the correct priority level. But just in case...
	IFXASSERT(pPriorityNode);
	IFXASSERT(pPriorityNode->m_priority == priority);

	// now we need to create a new Regular Node for our Data Block
	pDataNode = new IFXDataBlockNodeX;

	if (pDataNode)
	{
		pDataNode->m_isPriorityNode = FALSE;
		pDataNode->m_priority = priority;
		pDataNode->m_pDataBlock = &rDataBlock;
		pDataNode->m_pDataBlock->AddRef();
	}
	else
		IFXCHECKX(IFX_E_OUT_OF_MEMORY);


	// now we have a pPriorityNode and a pDataNode
	// link everything together...
	pDataNode->m_pNext = pPriorityNode->m_pLast->m_pNext;
	pPriorityNode->m_pLast->m_pNext = pDataNode;
	pDataNode->m_pLastPriority = pPriorityNode;
	pPriorityNode->m_pLast = pDataNode;
}

//---------------------------------------------------------------------
// PUBLIC IFXDataBlockQueue::Copy
// This method is not supported by this object.
//---------------------------------------------------------------------
void CIFXBlockPriorityQueueX::CopyX(IFXDataBlockQueueX*& rpDataBlockQueue)
{
	IFXCHECKX(IFX_E_UNSUPPORTED);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXBlockPriorityQueueX::AddRef()
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXBlockPriorityQueueX::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::QueryInterface
//---------------------------------------------------------------------
IFXRESULT CIFXBlockPriorityQueueX::QueryInterface( IFXREFIID  interfaceId,
												  void**   ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( IID_IFXDataBlockQueueX == interfaceId )
		{
			*ppInterface = (IFXDataBlockQueueX*) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	}

	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN( rc );
}

//---------------------------------------------------------------------
// Factory function.
//---------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXBlockPriorityQueueX_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXBlockPriorityQueue component.
		CIFXBlockPriorityQueueX *pComponent = new CIFXBlockPriorityQueueX;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}
