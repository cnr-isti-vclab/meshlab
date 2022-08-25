
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

#ifndef __IFXDeque_h__
#define __IFXDeque_h__

#include "IFXArray.h"

#define IFXDQ_RESETVACANT	FALSE


/** 
	@brief Double-Ended Queue

	Implemented as circular queue on a dynamic array.

	PopHead() and PopTail() discard the popped entry,
	so read it BEFORE you pop it.

	The queue does not do strong bounds checking.
	Do not try an index >= GetNumberQueued().

	The queue stores actual objects, if you want to queue pointers,
	then you should template on a pointer to the object's class.

	@note To properly reset reused items on the queue, the derived deque should
	provide a `ResetElement(T &element)'.

	@note Since arbitrary non-reference counted classes are accepted in the
	template, `T &Pop*()' is not supported since the object would be gone
	after the pop.

	Example:
	@code
		IFXDeque<float>	fdeque;
		fdeque.PushTail()=1.2f;
		fdeque.PushTail()=3.4f;
		float head=fdeque.ElementFromHead(0);		-> head = 1.2f
		fdeque.PopTail();
	@endcode

	@warning minimal optimization
*/

template<class T> class IFXDeque: protected IFXArray<T>
{
public:
	IFXDeque(void)
	{
		m_head=0;
		m_tail=0;
		m_elementsQueued=0;
		IFXArray<T>::ResizeToAtLeast(IFXArray<T>::m_elementsAllocated);
	}

	// see IFXArray
	virtual	void Clear(U32 preallocation=0)
	{
		IFXArray<T>::Clear(preallocation);
		m_head=0;
		m_tail=0;
		m_elementsQueued=0;
	}

			/** Pushes a new element at the beginning of the queue and return
				a reference to it. */
	T		&PushHead(void);

			/** Ejects and deletes the element at the beginning of the
				queue.  Returns TRUE if successful. */
	BOOL	PopHead(void);

			/** Pushes a new element at the end of the queue and return
				a reference to it. */
	T		&PushTail(void);

			/** Ejects and deletes the element at the end of the
				queue.  Returns TRUE if successful. */
	BOOL	PopTail(void);

			/** Returns the number of elements currently queued.
				This can be different than the GetNumberElements()
				since IFXDeque may allocate more elements in the
				underlying IFXArray than are currently queued. */
	U32		GetNumberQueued(void) const	{ return m_elementsQueued; };

			/** Gets an element indexed relative to the head.
				ElementFromHead(0) returns a reference to the
				first element in the queue. */
	T		&ElementFromHead(U32 index);

			/** Gets an element indexed relative to the head,
				retaining const. */
	const	T		&ElementFromHeadConst(U32 index) const;

			/** Gets an element indexed relative to the tail.
				ElementFromHead(0) returns a reference to the
				last element in the queue. */
	T		&ElementFromTail(U32 index);

			/** Gets an element indexed relative to the tail,
				retaining const. */
	const	T		&ElementFromTailConst(U32 index) const;
    
			/** An overridable method for resetting element*/
	virtual void ResetElement(T &element) {}

private:
	void	Expand(void);
	U32		CorrectIndex(I32 index) const;

	I32		m_head;
	I32		m_tail;
	U32		m_elementsQueued;
};

template<class T> IFXINLINE T &IFXDeque<T>::PushHead(void)
{
	Expand();

	if(m_elementsQueued>1)
		m_head++;
	if(m_head>=(I32)IFXArray<T>::m_elementsAllocated)
		m_head-=IFXArray<T>::m_elementsAllocated;

//	(*this)[m_head].Reset();
	ResetElement((*this)[m_head]);

	return (*this)[m_head];
}

template<class T> IFXINLINE BOOL IFXDeque<T>::PopHead(void)
{
	if(m_elementsQueued<1)
		return false;

	if(--m_elementsQueued)
	{
		m_head--;
		if(m_head<0)
			m_head+=IFXArray<T>::m_elementsAllocated;
	}

	return true;
}

template<class T> IFXINLINE T &IFXDeque<T>::PushTail(void)
{
	Expand();

	if(m_elementsQueued>1)
		m_tail--;
	if(m_tail<0)
		m_tail+=IFXArray<T>::m_elementsAllocated;

//	(*this)[m_tail].Reset();
	ResetElement((*this)[m_tail]);

	return (*this)[m_tail];
}

template<class T> IFXINLINE BOOL IFXDeque<T>::PopTail(void)
{
	if(m_elementsQueued<1)
		return false;

	if(--m_elementsQueued)
	{
		m_tail++;
		if(m_tail>=(I32)IFXArray<T>::m_elementsAllocated)
			m_tail-=IFXArray<T>::m_elementsAllocated;
	}

	return true;
}

template<class T> IFXINLINE T &IFXDeque<T>::ElementFromHead(U32 index)
{
	IFXASSERT(index<m_elementsQueued);
	return IFXArray<T>::GetElement(CorrectIndex((I32)(m_head-index)));
}

template<class T> IFXINLINE const T
					&IFXDeque<T>::ElementFromHeadConst(U32 index) const
{
	IFXASSERT(index<m_elementsQueued);
	return IFXArray<T>::GetElementConst(CorrectIndex((I32)(m_head-index)));
}

template<class T> IFXINLINE T &IFXDeque<T>::ElementFromTail(U32 index)
{
	IFXASSERT(index<m_elementsQueued);
	return IFXArray<T>::GetElement(CorrectIndex((I32)(index+m_tail)));
}

template<class T> IFXINLINE const T
					&IFXDeque<T>::ElementFromTailConst(U32 index) const
{
	IFXASSERT(index<m_elementsQueued);
	return IFXArray<T>::GetElementConst(CorrectIndex((I32)(index+m_tail)));
}

template<class T> IFXINLINE void IFXDeque<T>::Expand(void)
{
	m_elementsQueued++;
	if(m_elementsQueued>this->GetNumberElements())
	{
		U32 oldAllocated=IFXArray<T>::m_elementsAllocated;

		IFXArray<T>::ResizeToAtLeast(m_elementsQueued);
		IFXArray<T>::ResizeToAtLeast(IFXArray<T>::m_elementsAllocated);

		//* move high entries
		U32 m,range=oldAllocated-m_tail;
		for(m=1;m<range+1;m++)
			*((T**)IFXArray<T>::m_array)[CorrectIndex(IFXArray<T>::m_elementsAllocated-m)]=
								*((T**)IFXArray<T>::m_array)[CorrectIndex(oldAllocated-m)];

		U32 move=IFXArray<T>::m_elementsAllocated-oldAllocated;
		if(m_head>m_tail)
			m_head+=move;
		if(oldAllocated)
			m_tail+=move;

#if IFXDQ_RESETVACANT
		for(m=0;m<move;m++)
//			((T**)m_array)[CorrectIndex(m_tail-1-m)]->Reset();
			ResetElement(((T**)m_array)[CorrectIndex(m_tail-1-m)]);
#endif
	}
}

template<class T> IFXINLINE U32 IFXDeque<T>::CorrectIndex(I32 index) const
{
	if(index<0)
		index+=IFXArray<T>::m_elementsAllocated;
	if(index>=(I32)IFXArray<T>::m_elementsAllocated)
		index-=IFXArray<T>::m_elementsAllocated;
	return (U32)index;
}

#endif
