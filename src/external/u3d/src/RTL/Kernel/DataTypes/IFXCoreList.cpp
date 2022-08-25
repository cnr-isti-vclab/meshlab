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

#include "IFXList.h"
#include "IFXUnitAllocator.h"

#if IFXLIST_USEALLOCATOR==1
IFXUnitAllocator  *IFXCoreList::m_pAllocator=NULL;
#elif IFXLIST_USEALLOCATOR==2
IFXChunkPool    *IFXCoreList::m_pChunkPool=NULL;
#endif

U32         IFXCoreList::m_listCount=0;


IFXCoreList::IFXCoreList(void)
{
	m_head=NULL;
	m_tail=NULL;
	m_length=0;
	m_autodestruct=false;

	if(!m_listCount++)
	{
#if IFXLIST_USEALLOCATOR==1
		m_pAllocator=new IFXUnitAllocator;
		// unitsize, initial, growth
		m_pAllocator->Initialize(sizeof(IFXListNode),100,25);

#elif IFXLIST_USEALLOCATOR==2
		IFXASSERT(!m_pChunkPool);
		m_pChunkPool=new IFXChunkPool(1024);
#endif
	}

#if IFXLIST_USEALLOCATOR==3
	/// @todo: tune this IFXList m_allocator settings
	m_allocator.Initialize(sizeof(IFXListNode),2,10);
#endif

#if IFXLIST_USEALLOCATOR==2
	IFXASSERT(m_pChunkPool);
#endif
}


IFXCoreList::~IFXCoreList(void)
{
	if(!--m_listCount)
	{
#if IFXLIST_USEALLOCATOR==1
		delete m_pAllocator;
		m_pAllocator=NULL;
#elif IFXLIST_USEALLOCATOR==2
		IFXASSERT(m_pChunkPool);
		delete m_pChunkPool;
		m_pChunkPool=NULL;
#endif
	}
}


void IFXCoreList::Deallocate(void *ptr)
{
#if IFXLIST_USEALLOCATOR==1
	IFXASSERT(m_pAllocator);
	if(m_pAllocator)
		m_pAllocator->Deallocate((U8 *)ptr);
#elif IFXLIST_USEALLOCATOR==2
	m_pChunkPool->Deallocate(ptr,sizeof(IFXListNode));
#elif IFXLIST_USEALLOCATOR==3
	m_allocator.Deallocate((U8 *)ptr);
#else
	delete ptr;
#endif
}


void IFXCoreList::Clear(void)
{
	IFXListNode *node;
	while( (node=m_head) != NULL)
		CoreRemoveNode(node);
}


void IFXCoreList::RemoveAll(void)
{
	while( m_head != NULL)
	{
#ifdef _DEBUG
		BOOL result=
#endif
		CoreRemoveNode(m_head);
		IFXASSERT(result);
	}
}


void *IFXCoreList::InternalToHead(IFXListContext &context) const
{
#if IFXLIST_CHECKPOINTER
	context.SetCorePointer(this);
#endif
	context.SetCurrent(m_head);
	return InternalGetCurrent(context);
}


void *IFXCoreList::InternalPostIncrement(IFXListContext &context) const
{
#if IFXLIST_CHECKPOINTER
	context.CheckCorePointer(this);
#endif
	IFXListNode *node=context.GetCurrent();
	if(node)
		context.SetCurrent(node->GetNext());
	else if(!(context.GetAtTail()))
		InternalToHead(context);
	if(m_length && !context.GetCurrent())
		context.SetAtTail(true);
	return node? (node->GetPointer()): NULL;
}


BOOL IFXCoreList::CoreRemove(void *entry, IFXListContext &hint)
{
#if IFXLIST_CHECKPOINTER
	hint.CheckCorePointer(this);
#endif

	/*** Due to the nature of normal list usage (PostIncrementing and Post
	Decrementing), Remove can now check current, next, and prev nodes before
	churning through the whole list. */

	IFXListNode *pNode;
	if((pNode=hint.GetCurrent()))
	{
		if(pNode->GetPointer()==entry)
			return CoreRemoveNode(pNode);
		else if(pNode->GetNext() && pNode->GetNext()->GetPointer()==entry)
			return CoreRemoveNode(pNode->GetNext());
		else if(pNode->GetPrevious() &&
			pNode->GetPrevious()->GetPointer()==entry)
			return CoreRemoveNode(pNode->GetPrevious());
	}

	return CoreRemove(entry);
	/*
	IFXListContext context;
	void *entry2;

	InternalToHead(context);
	while( (entry2=InternalGetCurrent(context)) != NULL)
	{
	if(entry2==entry)
	break;
	else
	InternalPostIncrement(context);
	}

	if(!entry2)
	return false;

	return RemoveNode(context.GetCurrent());
	*/
}


BOOL IFXCoreList::CoreRemove(void *entry)
{
	IFXListContext context;
	void *entry2;

	InternalToHead(context);
	while( (entry2=InternalGetCurrent(context)) != NULL)
	{
		if(entry2==entry)
			break;
		else
			InternalPostIncrement(context);
	}

	if(!entry2)
		return false;

	return CoreRemoveNode(context.GetCurrent());
}


void *IFXCoreList::CoreSearchForElement(IFXListContext &context,
										void *entry) const
{
	InternalToHead(context);

	void *entry2;
	while( (entry2=InternalGetCurrent(context)) != NULL)
	{
		if(entry2==entry)
			break;
		else
			InternalPostIncrement(context);
	}

	if(!entry2)
		return NULL;

	IFXListNode *node=context.GetCurrent();
	return node? (node->GetPointer()): NULL;
}


void *IFXCoreList::CoreGetElement(long index) const
{
	//* retains no state, so this can be slow on a long list

	// FUTURE internal context just for GetElement()
	// would optimise for incremental use

	IFXListContext context;
	InternalToHead(context);

	long m;
	for(m=0;m<index;m++)
		InternalPostIncrement(context);

	IFXListNode *node=context.GetCurrent();
	return node? (node->GetPointer()): NULL;
}


void **IFXCoreList::CoreInsert(BOOL before,IFXListContext &context,
							   void *entry,IFXListNode *existingNode)
{
#if IFXLIST_CHECKPOINTER
	context.CheckCorePointer(this);
#endif

	IFXListNode *node=existingNode;
	if(!node)
	{
#if IFXLIST_USEALLOCATOR==1
		IFXASSERT(m_pAllocator);
		node=(IFXListNode *)m_pAllocator->Allocate();
		IFXASSERT(node);
		node->Reset();
#elif IFXLIST_USEALLOCATOR==2
		node=(IFXListNode*)m_pChunkPool->Allocate(
			sizeof(IFXListNode));
		if(!node)
			return NULL;
		node->Reset();
#elif IFXLIST_USEALLOCATOR==3
		node=(IFXListNode *)m_allocator.Allocate();
		IFXASSERT(node);
		node->Reset(this);
#else
		node=new IFXListNode;
		if(!node)
			return NULL;
#endif

		IFXASSERT(entry);
		node->SetPointer(entry);
	}

	IFXListNode *current=context.GetCurrent();
	IFXListNode *left,*right;


	if(!m_length)
	{
		left=NULL;
		right=NULL;
		m_head=node;
		m_tail=node;
	}
	else if(current)
	{
		if(before)
		{
			left=current->GetPrevious();
			right=current;
		}
		else
		{
			left=current;
			right=current->GetNext();
		}
	}
	else if(context.GetAtTail())
	{
		// m_tail is NULL
		left=m_tail;
		right=NULL;
	}
	else
	{
		// m_head is NULL
		left=NULL;
		right=m_head;
	}

	node->SetPrevious(left);
	node->SetNext(right);

	if(left)
		left->SetNext(node);
	else
		m_head=node;

	if(right)
		right->SetPrevious(node);
	else
		m_tail=node;

	m_length++;

	return (void **)(node->GetPointerLocation());
}


void *IFXCoreList::InternalGetCurrent(IFXListContext &context) const
{
#if IFXLIST_CHECKPOINTER
	context.CheckCorePointer(this);
#endif

	IFXListNode *node=context.GetCurrent();
	return node? (node->GetPointer()): NULL;
}


BOOL IFXCoreList::CoreRemoveNode(IFXListNode *node)
{
	if(!node)
		return false;

	InternalDetachNode(node);

	//* let node delete itself later
	node->Abandon();
	return true;
}


void IFXCoreList::InternalDetachNode(IFXListNode *node)
{
	IFXListNode *left,*right;
	left=node->GetPrevious();
	right=node->GetNext();

	if(left)
		left->SetNext(right);
	else
		m_head=right;

	if(right)
		right->SetPrevious(left);
	else
		m_tail=left;

	m_length--;
}


BOOL IFXCoreList::CoreMoveNode(BOOL before,IFXListContext &from,
							   IFXListContext &to)
{
	IFXListNode *node=from.GetCurrent();
	InternalDetachNode(node);
	return (CoreInsert(before,to,NULL,node)!=NULL);
}
