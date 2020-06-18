
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

#ifndef __IFXList_h__
#define __IFXList_h__

#include "IFXListContext.h"

#if IFXLIST_USEALLOCATOR==1
class IFXUnitAllocator;
#endif


/**
	Contains type-nonspecific functionality of IFXList() in order to reduce
	template size.

	Allows access to generic list without knowing the element type
	(parser uses this).

	@warning	This class must contain all data used by IFXList;
				it must be interchangable with a IFXList through a void*.
*/
class IFXCoreList
	{
	public:
					IFXCoreList(void);
virtual				~IFXCoreList(void);

					/** Ejects all elements from the list (does not delete them).*/
		void		Clear(void);

					/** Sets the AutoDestruct flag. If TRUE, destruction of
					the list will attempt to destroy all the elements.*/
		void		SetAutoDestruct(BOOL set)		{ m_autodestruct=set; }

					/** Gets the AutoDestruct flag.*/
		BOOL		GetAutoDestruct(void)			{ return m_autodestruct; }

					/** Returns the number of elements on the list.*/
		long		GetNumberElements(void) const	{ return m_length; }

					/** Removes and delete nodes, but don't delete contents */
		void		RemoveAll(void);

					/** Moves the node at the first context before the
					node at the second context.*/
		BOOL		MoveNodeBefore(IFXListContext &from,IFXListContext &to)
					{	return CoreMoveNode(true,from,to); }
					/** Moves the node at the first context after the
					node at the second context.*/
		BOOL		MoveNodeAfter(IFXListContext &from,IFXListContext &to)
					{	return CoreMoveNode(false,from,to); }

					/** Returns TRUE if the context is at the first element.*/
		BOOL		IsAtHead(IFXListContext &context) const
					{	return !context.GetCurrent() && !context.GetAtTail(); };
					/** Returns TRUE if the context is at the last element.*/
		BOOL		IsAtTail(IFXListContext &context) const
					{	return !context.GetCurrent() && context.GetAtTail(); };

#if IFXLIST_USEALLOCATOR==3
					/// (internal) for use by IFXListNode only.*/
		void		Deallocate(void *ptr);
#else
static	void		Deallocate(void *ptr);
#endif

	protected:
		BOOL		CoreRemove(void *entry);
		BOOL		CoreRemove(void *entry, IFXListContext &context);
		void		*CoreSearchForElement(IFXListContext &context,
														void *entry) const;
		void		*CoreGetElement(long index) const;
		void		**CoreInsert(BOOL before,IFXListContext &context,
										void *entry,IFXListNode *existingNode);
		BOOL		CoreRemoveNode(IFXListNode *node);
		BOOL		CoreMoveNode(BOOL before,IFXListContext &from,
														IFXListContext &to);

		IFXListNode			*m_head;
		IFXListNode			*m_tail;
		long				m_length;
		BOOL				m_autodestruct;

#if IFXLIST_USEALLOCATOR==1
static	IFXUnitAllocator	*m_pAllocator;
#elif IFXLIST_USEALLOCATOR==2
static	IFXChunkPool		*m_pChunkPool;
#elif IFXLIST_USEALLOCATOR==3
		IFXUnitAllocator	m_allocator;
#endif

static	U32					m_listCount;

	private:
		void		*InternalToHead(IFXListContext &context) const;
		void		*InternalGetCurrent(IFXListContext &context) const;
		void		*InternalPostIncrement(IFXListContext &context) const;
		void		InternalDetachNode(IFXListNode *node);
	};



/**
	@brief Fully bidirectional Doubly-Linked List

	There is a NULL entry at both beginning and end.

	If SearchForContent() is to be used and type T has any data complexity
	the operator == should probably be defined for type T.  Some examples of
	data complexity are:
		- pointer data members
		- data members that do not contribute to 'equality'
		- data whose equality can be more efficiently determined with something
		  other than the default bytewise comparison.
		- fuzzy or soft data

	The element stored is actually T*, not T, so templating
	on a pointer is probably adding an additional level of indirection.
	Note that this is not true for IFXArray which stores actual instances.

	Example:
	@code
		IFXList<MyClass> list;
		IFXListContext context;
		MyClass *one=new MyClass();
	
		// add element
		list.Append(one);

		// read element
		MyClass *two=list.GetCurrent(context);

		// do something to all elements
		MyClass *node;
		list.ToHead(context);
		while( (node=list.PostIncrement(context)) != NULL)
			{
			node->DoSomething();
			}
	@endcode

	No contexts will lose place if elements are removed,
	even if removed with a different context.
	Reference counting prevents invalid pointers and heirs
	follow destruction chains to find valid nodes.

	By using SetAutoDestruct(true), all elements remaining
	on the list are deleted when the list is destructed.

	WARNING: if these elements were not created with new(),
	failures will probably occur.  Note that SetAutoDestruct(true)
	is precarious when elements may be in more than one list.
	In that case, it is very possible for elements to be deleted
	that are still members of other lists.

	Note that much of the functionality of this class is implemented in the 
	non-template class IFXCoreList.  This is done to reduce template
	instantiation size.

	Uses IFXUnitAllocator to manage IFXListNode's.
*/
template <class T> class IFXList: public IFXCoreList
	{
	public:
				IFXList(void)												{ }
virtual			~IFXList(void)
				{
					if(m_autodestruct)
						DeleteAll();
					else
						RemoveAll();
				}

				/** Inserts a new element before the context.
					Returns the location of the new element or NULL if the
					internal node allocation failed. */
		T		**InsertBefore(IFXListContext &context,T *entry)
				{	return (T **)CoreInsert(true,context,(void *)entry,NULL); }
				/** Inserts a new element after the context.
					Returns the location of the new element or NULL if the
					internal node allocation failed. */
		T		**InsertAfter(IFXListContext &context,T *entry)
				{	return (T **)CoreInsert(false,context,(void *)entry,NULL); }
				/** Inserts a new element at the beginning of the list.
					Returns the location of the new element or NULL if the
					internal node allocation failed. */
		T		**Prepend(T *entry)
				{
					IFXListContext context;
					context.SetCurrent(m_head);
					// \bug AJW: high: when libIFXBase gets is built with
					//	_DEBUG but used without, this pointer checking breaks 
#if IFXLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return (T **)CoreInsert(false,context,(void *)entry,NULL);
				}
				/** Inserts a new element at the end of the list.
					Returns the location of the new element or NULL if the
					internal node allocation failed. */
		T		**Append(T *entry)
				{
					IFXListContext context;
					context.SetCurrent(m_tail);
#if IFXLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return (T **)CoreInsert(false,context,(void *)entry,NULL);
				}

				/** Appends a copy of an entire list of identical type. */
		void	Append(IFXList<T> &list)
				{
					T *pT;
					IFXListContext context;
					list.ToHead(context);
					while((pT=list.PostIncrement(context))!=NULL)
						Append(pT);
				}

				/**	This is the straightforward remove implementation.  This
					Remove() will look for the entry in the list by simply
					checking each member entry in order.  This can be very slow
					in some cases.  See Remove(T *entry, IFXListContext &hint)
					for a faster alternative. */
		BOOL	Remove(T *entry)
				{	return CoreRemove((void *)entry); }

				/**	This Remove() method uses a hint IFXListContext to check
					current, next, and prev nodes (in that order) before
					reverting to a head to tail scan. In most cases this 
					significantly reduces execution time. */
		BOOL	Remove(T *entry, IFXListContext &hint)
				{	return CoreRemove((void *)entry, hint); }

				/** Removes all nodes and delete their pointers.*/
		void	DeleteAll(void)
				{
					while( m_head != NULL)
					{
//						result=Delete((T *)node->GetPointer());

						T *ptr=(T *)m_head->GetPointer();
#ifdef _DEBUG
						BOOL result =
#endif
						CoreRemoveNode(m_head);
						IFXASSERT(result);
						delete ptr;
					}
				}

				/** Removes and deletes the current node on the given context.*/
		BOOL	DeleteCurrent(IFXListContext &context)
				{
					IFXListNode *node=context.GetCurrent();

					if(node)
					{
						T *ptr=(T *)node->GetPointer();
						BOOL success=CoreRemoveNode(node);
						IFXASSERT(success);
						delete ptr;
						return success;
					}

					return false;
				};

				/// Removes and deletes the first entry that match the argument.
		BOOL	Delete(T *entry)
				{
					BOOL result=Remove(entry);
					delete entry;

					return result;
				};

				/** Rewinds the context to the beginning of the list and return
					the first element. */
		T		*ToHead(IFXListContext &context) const
				{
					context.SetCurrent(m_head);
#if IFXLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return GetCurrent(context);
				}

				/** Moves the context to the end of the list and return
					the last element. */
		T		*ToTail(IFXListContext &context) const
				{
					context.SetCurrent(m_tail);
#if IFXLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return GetCurrent(context);
				}

				/** Returns the first element matching the given pointer.
					Only pointer are compared. Matches are not made by content they point to. */
		T		*SearchForElement(IFXListContext &context,T *entry) const
				{
					return (T *)
						(CoreSearchForElement(context,(void *)entry));
				}

				/** Returns the first element with the same value.
					The elements are compared to the contents of *value.
					The value argument is passed by pointer instead of
					value or reference to prevent a requirement that the
					T be an instantiable class. */
		T		*SearchForContent(IFXListContext &context,const T *value) const
				{
					if(!value)
						return NULL;

					ToHead(context);

					T *entry2;
					while( (entry2=GetCurrent(context)) != NULL)
					{
						if(*entry2==*value)
							break;
						else
							PostIncrement(context);
					}

					if(!entry2)
						return NULL;

					return GetCurrent(context);
				}

				/** Returns the current element before incrementing to the next.*/
		T		*PostIncrement(IFXListContext &context) const
				{
/*
					IFXListNode *node=context.GetCurrent();

					if(node)
						context.SetCurrent(node->GetNext());
					else if(!IsAtEnd(context))
						ToHead(context);

					if(m_length && !context.GetCurrent())
						context.SetAtEnd(true);

					return node? (T *)(node->GetPointer()): (T *)NULL;
*/
					IFXListNode *node,*next;

					if((node=context.GetCurrent())!=NULL)
					{
						context.SetCurrent(next=node->GetNext());
						if(!next)
							context.SetAtTail(true);
						return (T *)(node->GetPointer());
					}
					else if(!IsAtTail(context))
						context.SetCurrent(m_head);

					return (T *)NULL;
				}

				/** Returns the current element before decrementing to the
					previous. */
		T		*PostDecrement(IFXListContext &context) const
				{
/*
					IFXListNode *node=context.GetCurrent();
					if(node)
						context.SetCurrent(node->GetPrevious());
					else if(IsAtEnd(context))
						ToTail(context);
					context.SetAtEnd(false);
					return node? (T *)(node->GetPointer()): (T *)NULL;
*/
					IFXListNode *node=context.GetCurrent();

					if(node)
					{
						context.SetCurrent(node->GetPrevious());
						context.SetAtTail(false);
						return (T *)(node->GetPointer());
					}
					else if(IsAtTail(context))
						context.SetCurrent(m_tail);

					context.SetAtTail(false);
					return (T *)NULL;
				}

				/** Returns the first element of the list (no context required).*/
		T		*GetHead(void) const
				{
					return m_head? (T *)(m_head->GetPointer()): (T *)NULL;
				}

				/** Returns the last element of the list (no context required).*/
		T		*GetTail(void) const
				{
					return m_tail? (T *)(m_tail->GetPointer()): (T *)NULL;
				}

				/** Returns the current element of the context.*/
		T		*GetCurrent(IFXListContext &context) const
				{
#if IFXLIST_CHECKPOINTER
					context.CheckCorePointer(this);
#endif
					IFXListNode *node=context.GetCurrent();
					return node? (T *)(node->GetPointer()): (T *)NULL;
				}

				// trivial convenience functions

				/** Increments the context and return the next element.*/
		T		*PreIncrement(IFXListContext &context) const
				{
					PostIncrement(context);
					return GetCurrent(context);
				}

				/** Decrements the context and return the previous element.*/
		T		*PreDecrement(IFXListContext &context) const
				{
					PostDecrement(context);
					return GetCurrent(context);
				}

				/** Gets an element by index.  This is not an array, so this
					can be an inefficient operation. */
		T		*GetElement(long index) const
				{	return (T *)CoreGetElement(index); }
	};

#endif
