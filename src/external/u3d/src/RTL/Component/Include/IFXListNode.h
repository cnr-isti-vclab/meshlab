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

#ifndef __IFXListNode_h__
#define __IFXListNode_h__

#include "IFXDataTypes.h"

class IFXCoreList;

/***	IFXLIST_USEALLOCATOR:
			0=none
			1=static IFXUnitAllocator
			2=static IFXChunkPool
			3=local IFXUnitAllocator */
#ifndef IFXLIST_USEALLOCATOR
#define	IFXLIST_USEALLOCATOR	1
#endif


/******************************************************************************
	m_references are for multiple contexts and one for the list itself
	other nodes don't increment m_references
******************************************************************************/

/**
	(internal)  This class shuold be entirely hidden behind IFXList<>.

	This implementation of linked list can maintain validity of contexts
	even when you remove the node it is pointing to.  This is done
	without the lists or nodes retaining information on the contexts that
	point to it.  Every node has an heir that can point to another node
	in the same list which may or may not have been removed.  The heir
	specifies where to look next for a valid node if this one is invalid.
	It will follow the chain of heirs until either it reaches a valid
	node or a NULL pointer, in which case the list is empty.
	It behaves as though it could update any contexts that were
	pointing to it, but without the overhead of maintaining pointers
	to an unlimited number of contexts.
*/
class IFXListNode
	{
	public:

#if IFXLIST_USEALLOCATOR==3
						IFXListNode(void)	{ IFXASSERT(0); }
		void			Reset(IFXCoreList *corelist)
						{	m_pCoreList = corelist;
							Initialize(); }
#else
						IFXListNode(void)	{ Reset(); };

						/** (internal) Resets to initial state.*/
		void			Reset(void)			{ Initialize(); }
#endif

						/** (internal) Sets pointer to listed object.*/
		void			SetPointer(void *set)			{ m_pointer=set; }
						/** (internal) Gets pointer to listed object.*/
		void			*GetPointer(void) const			{ return m_pointer; }
						/** (internal) Gets location of retained pointer.*/
		void			**GetPointerLocation(void)		{ return &m_pointer;}

						/** (internal) Sets pointer to node before this one.*/
		void			SetPrevious(IFXListNode *set)	{ m_previous=set; }
						/** (internal) Gets pointer to node before this one.*/
		IFXListNode		*GetPrevious(void) const		{ return m_previous;}

						/** (internal) Sets pointer to node after this one.*/
		void			SetNext(IFXListNode *set)		{ m_next=set; }
						/** (internal) Gets pointer to node after this one.*/
		IFXListNode		*GetNext(void) const			{ return m_next; }

						/** (internal) Sets pointer to heir node.*/
		void			SetHeir(IFXListNode *set)		{ m_heir=set; }
						/** (internal) Gets pointer to heir node.*/
		IFXListNode		*GetHeir(void) const			{ return m_heir; }

						/** (internal) Sets flag indicating if node is in a list.*/
		void			SetValid(bool set)				{ m_valid=set;}
						/** (internal) Gets flag indicating if node is in a list.*/
		bool			GetValid(void) const			{ return m_valid; }

						/** (internal) Marks node as invalid and determines
							an heir. This happens when the node is removed
							from its list. It may still have references. */
		void			Abandon(void)
						{
							// if someone is still pointing at this node,
							// leave a trail to a potentially valid node
							if(m_references>1)
							{
								if(m_next)
									m_heir=m_next;
								else
									m_heir=m_previous;

								if(m_heir)
									m_heir->IncReferences();
							}

							SetValid(false);
							DecReferences();
						}

						/** (internal) Increments reference count.*/
		void			IncReferences(void)				{ m_references++; }
						/** (internal) Decrements reference count.*/
		void			DecReferences(void);

	private:
		void			Initialize(void)
						{
							m_references=1;
							m_valid=true;

							m_pointer=NULL;
							m_previous=NULL;
							m_next=NULL;
							m_heir=NULL;
						}

			long			m_references;
			bool			m_valid;
			void			*m_pointer;
#if IFXLIST_USEALLOCATOR==3
			IFXCoreList		*m_pCoreList;
#endif

			IFXListNode		*m_previous;
			IFXListNode		*m_next;
			IFXListNode		*m_heir;
};

#endif /* __IFXListNode_h__ */
