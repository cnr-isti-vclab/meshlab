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
	@file	IFXFastHeap.h

			This header defines the IFXFastHeap class and its functionality.
*/

#ifndef __IFXFASTHEAP_H__
#define __IFXFASTHEAP_H__

#include "IFXDataTypes.h"
#include "IFXDebug.h"

//#define FAST_HEAP_VALIDATE

/**
	This is the main interface and definition for IFXFastHeap.
*/
template<class K, class T>
class IFXFastHeap
{
public:
	IFXFastHeap(U32);
	~IFXFastHeap();

	/**
		This method inserts an object into the heap.

		@param	A const reference to the sorting key.
		@param	A const reference to the data.

		@return A UPTR representing the heap context.
	*/
	UPTR Insert(const K&, const T&);

	/**
		This method pops an item off the heap and returns it.

		@return A template class type representing the data from the heap.
	*/
	T Pop();

	/**
		This method removes an item from the heap.

		@param A UPTR representing the heap context.

		@return void
	*/
	void Remove(UPTR);

	/**
		This method inserts an object into the heap.

		@param	A UPTR representing the heap context.
		@param	A const reference to the sorting key.

		@return A U32 representing the heap context.
	*/
	void Reinsert(UPTR, const K&);

	/**
		This method returns the size of the heap.

		@return A U32 representing the heap size.
	*/
	U32 Size();

	/**
		This method frees all heap contexts.

		@return void.
	*/
	void Clear();
	
	/**
		This method assures that each entry is in the correct
		place within the heap.

		@return void.
	*/
#ifdef FAST_HEAP_VALIDATE
	void Validate();
#endif

private:
	struct Node
	{
		Node() : pContext(0)
		{ };

		void Set(const K& in_Key, const T& in_Data, UPTR* in_Ctx)
		{
			Key = in_Key;
			Data = in_Data;
			pContext = in_Ctx;
		};

		UPTR* pContext;
		K Key;
		T Data;
	};
	void HeapInsert(U32);
	void Heapify(U32);
	void SwapNodes(Node*, Node*);

	UPTR* NewContext(UPTR);
	void FreeContext(UPTR* in_Ctx);

	Node* m_pHeap;
	UPTR* m_pHeapContext;
	U32 m_NextFreeContextId;
	UPTR* m_pNextFreeContext;
	U32 m_Size;
	U32 m_MaxSize;
};

// -----------------------------------------------------------------------
//	Inline implementation
// -----------------------------------------------------------------------
#define Parent(x) (x==0?(U32)-1:((x-1)/2))
#define Left(x) (((x)*2)+1)
#define Right(x) (((x)*2)+2)
#define Index(x)  (x - m_pHeap)

template<class K, class T>
inline IFXFastHeap<K, T>::IFXFastHeap(U32 in_Size)
{
	m_MaxSize = in_Size;
	m_pHeap = new Node[in_Size];
	m_pHeapContext = new UPTR[in_Size];
	m_NextFreeContextId = 0;
	m_pNextFreeContext = NULL;
	m_Size = 0;
}

template<class K, class T>
inline IFXFastHeap<K, T>::~IFXFastHeap()
{
	IFXDELETE_ARRAY(m_pHeap);
	IFXDELETE_ARRAY(m_pHeapContext);
}

template<class K, class T>
inline UPTR IFXFastHeap<K, T>::Insert(const K& in_Key, const T& in_Data)
{
	m_pHeap[m_Size].Set(in_Key, in_Data, NewContext(m_Size));
	UPTR Ctx = (UPTR) m_pHeap[m_Size].pContext;
	HeapInsert(m_Size);
	++m_Size;
	return Ctx;
}

template<class K, class T>
inline T IFXFastHeap<K, T>::Pop()
{
	IFXASSERT(m_Size);
	T temp = m_pHeap[0].Data;
	--m_Size;	
	FreeContext(m_pHeap[0].pContext);
	if(m_Size)
	{
		m_pHeap[0] = m_pHeap[m_Size];
		m_pHeap[m_Size].pContext = 0;
		*(m_pHeap[0].pContext) = 0;
		Heapify(0);	
#ifdef FAST_HEAP_VALIDATE
		Validate();
#endif
	}
	
	return temp;
}

template<class K, class T>
void IFXFastHeap<K, T>::Remove(UPTR in_Context)
{
	IFXASSERT(in_Context >= ((UPTR)m_pHeapContext) 
			&& in_Context < (UPTR)(m_pHeapContext + m_MaxSize) );
	U32 NodeIdx = *((U32*)in_Context);
	--m_Size;
	FreeContext(m_pHeap[NodeIdx].pContext);
	if(m_Size && NodeIdx != m_Size)
	{
		m_pHeap[NodeIdx] = m_pHeap[m_Size];
		m_pHeap[m_Size].pContext = 0;
		*(m_pHeap[NodeIdx].pContext) = NodeIdx;
		
		U32 parIdx = Parent(NodeIdx);
		if( parIdx != (U32)-1 && m_pHeap[NodeIdx].Key < m_pHeap[parIdx].Key) // less then parent
		{
			HeapInsert(NodeIdx);
		}
		else // assume  possibly greater then children
		{
			Heapify(NodeIdx);
		}
	}
}

template<class K, class T>
void IFXFastHeap<K, T>::Reinsert(UPTR in_Context, const K& in_NewKey)
{
	IFXASSERT(in_Context >= ((UPTR)m_pHeapContext) 
			&& in_Context < (UPTR)(m_pHeapContext + m_MaxSize) );
	U32 NodeIdx = *(U32*)in_Context;
	m_pHeap[NodeIdx].Key = in_NewKey;
	U32 parIdx = Parent(NodeIdx);
	if( parIdx != (U32)-1 && m_pHeap[NodeIdx].Key < m_pHeap[parIdx].Key) // less then parent
	{
		HeapInsert(NodeIdx);
	}
	else // assume  possibly greater then children
	{
		Heapify(NodeIdx);
	}
}


template<class K, class T>
inline U32 IFXFastHeap<K, T>::Size()
{
	return m_Size;
}

template<class K, class T>
inline void IFXFastHeap<K, T>::Clear()
{
	U32 i;
	for(i = 0; i < m_Size; ++i)
	{
		FreeContext(m_pHeap[i].pContext);
		m_pHeap[i].pContext = NULL;
	}
	m_Size = 0;
}

// bubble node up
template<class K, class T>
inline void IFXFastHeap<K, T>::HeapInsert(U32 in_Idx)
{
	U32 parentIdx = Parent(in_Idx);
	while(parentIdx != (U32)-1)
	{
		if(m_pHeap[in_Idx].Key < m_pHeap[parentIdx].Key)
		{
			SwapNodes(m_pHeap+parentIdx, m_pHeap+in_Idx);
		}
		else
		{
			return;
		}
		in_Idx = parentIdx;
		parentIdx = Parent(in_Idx);
	}
}

// Bubble Node Down
template<class K, class T>
inline void IFXFastHeap<K, T>::Heapify(U32 in_Idx)
{
  U32 left, right;
  BOOL hasLeft, hasRight;

  while(1)
  {
	  left = Left( in_Idx );
	  right = Right( in_Idx );
	  hasLeft = left < m_Size;
	  hasRight = right < m_Size;
	  if( !hasLeft	 && !hasRight )
	  { // in_Idx is a node
		   return;
	  }   

	  if(hasLeft && hasRight)
	  {
			if(m_pHeap[left].Key < m_pHeap[right].Key && m_pHeap[left].Key < m_pHeap[in_Idx].Key)
			{
				SwapNodes(m_pHeap+in_Idx, m_pHeap+left);
				in_Idx = left;
				continue;
			}
			if(m_pHeap[right].Key < m_pHeap[in_Idx].Key)
			{
				SwapNodes(m_pHeap+in_Idx, m_pHeap+right);
				in_Idx = right;
				continue;
			}
			return;
	  }
	  
	  if( hasLeft && m_pHeap[left].Key < m_pHeap[in_Idx].Key )
	  {
		  SwapNodes(m_pHeap+in_Idx, m_pHeap+left);
		  in_Idx = left;
	  }	
	  else if( hasRight && m_pHeap[right].Key < m_pHeap[in_Idx].Key )
	  {
		  SwapNodes(m_pHeap+in_Idx, m_pHeap+right);
		  in_Idx = right;
	  }
	  else
	  {   // heap property is satisfied
		  // left child is greater than parent and right child is greater than
		  // parent if it exists
			return;
	  }
	  // Start again on the child that changed 
  }
}

template<class K, class T>
void IFXFastHeap<K, T>::SwapNodes(Node* in_l, Node* in_r)
{
	T tData(in_l->Data);
	K tKey(in_l->Key);
	UPTR* pCtx = in_l->pContext;

	in_l->Data = in_r->Data;
	in_l->Key = in_r->Key;
	in_l->pContext = in_r->pContext;
	*(in_l->pContext) = Index(in_l);

	in_r->Data= tData;
	in_r->Key = tKey;
	in_r->pContext = pCtx;
	*(in_r->pContext) = Index(in_r);
}

template<class K, class T>
UPTR* IFXFastHeap<K, T>::NewContext(UPTR in_Idx)
{
	if(m_pNextFreeContext)
	{
		UPTR* pContext = NULL;
		pContext = m_pNextFreeContext;
		m_pNextFreeContext = (UPTR*)*pContext;
		*pContext = in_Idx;
		return pContext;
	}
	IFXASSERT(m_NextFreeContextId < m_MaxSize);
	m_pHeapContext[m_NextFreeContextId] = in_Idx;
	return m_pHeapContext + (m_NextFreeContextId++);
}


template<class K, class T>
void IFXFastHeap<K, T>::FreeContext(UPTR* in_Ctx)
{
	*in_Ctx = (UPTR)m_pNextFreeContext;
	m_pNextFreeContext = in_Ctx;
}

#ifdef FAST_HEAP_VALIDATE
template<class K, class T>
void IFXFastHeap<K, T>::Validate()
{
	K lowValue = m_pHeap[0].Key;
	for(U32 i = 1; i < m_Size; ++i)
	{
		IFXASSERT(m_pHeap[i].Key >= lowValue && m_pHeap[i].Key >= m_pHeap[Parent(i)].Key);
	}
}
#endif

#undef Parent
#undef Left
#undef Right
#undef Index


#endif
