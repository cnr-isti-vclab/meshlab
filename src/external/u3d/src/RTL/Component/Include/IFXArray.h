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
	@file IFXArray.h
*/

#ifndef IFXARRAY_H
#define IFXARRAY_H

#include <memory.h>
#include "IFXMemory.h"
#include "IFXString.h"


#define IFXARRAY_MIN      4
#define IFXARRAY_EXPANSION    2  ///< multiplicative buffer granularity

#define IFXARRAY_UNSAFEMEMORY (IFX_OSTYPE==IFX_WIN32)


/**
This is the base class of the IFXArray template, which provides the
functions that are not dependent on the specific template type.

It allows the HParser to access a generic array without knowing what kind it is.

@warning  This class must not contain data;
it must be interchangable with an IFXArray through a void*.
*/
class IFXCoreArray
{
public:
	/** Initialize the array with space for a specified number
	of preallocated contiguous entries. */
	IFXCoreArray(U32 preallocation=0);
	virtual ~IFXCoreArray(void) {};

	/** Returns the name of an indexed element, however
	applicable.  Unless overridden by a derived class,
	this always returns an empty string. */
	virtual IFXString GetElementName(U32 index) { return L""; }

	/** Returns the size of an indexed element, however
	applicable.  Unless overridden by a derived class,
	this always returns 1. */
	virtual U32 GetElementSize(U32 index) { return 1; }

	/** Deletes all elements in the array and resets the
	size of the preallocated contiguous space. */
	virtual void Clear(U32 m_preallocation=0);

	/// Returns the number of elements in the array.
	const U32& GetNumberElements(void) const { return m_elementsUsed; }

	/// if set<elementsUsed, destructs excess elements
	void ResizeToExactly(U32 set);

	/// if required<elementsUsed, does nothing (expand only)
	void ResizeToAtLeast(U32 required);

protected:
	virtual void Construct(U32 index)           =0;
	virtual void Destruct(U32 index)            =0;
	virtual void DestructAll(void)              =0;
	virtual void Preallocate(U32 preallocation) =0;
	virtual void Resize(U32 needed)             =0;

	U32     m_elementsAllocated;
	void**	m_array;
	void*	m_contiguous;
	U32     m_prealloc;
	U32     m_elementsUsed;

	IFXDeallocateFunction *m_pDeallocate;
};


/**
	@brief Dynamic Array

	@note Class T is required have a member:
	T &T::operator=(const T &operand).
	Depending on the compiler, lack of this might fail
	if the Collect() member is used.  The const is optional.

	Array will use contiguous backing of the length specified in
	constructor or Clear().  Elements beyond the length will
	use scattered new()'s.  Use Collect() to make the data
	contiguous again.

	If the elements contain pointed-to data, it is not recommended
	to use Collect().

	Mechanisms are in place that should allow the original IFXArray to
	be defined as a static in a Win32 DLL, given no preallocation on
	construction. (It's always safe in Linux.)

	Elements may be reused without reconstruction.
	To properly reset reused items in the array, the derived class should
	provide a `ResetElement(T &element)'.
*/
template<class T> class IFXArray: public IFXCoreArray
{
public:
	/** Initializes the array with space for a specified number
	of preallocated contiguous entries. */
	IFXArray(U32 preallocation=0);
	virtual       ~IFXArray(void);

	/** Function that can be provided by a derived class to
	initialize an element to state as though it were
	just constructed.  This permits reuse of array
	space without repetitive allocations. */
	virtual void ResetElement(T &element) {}

	/** Expands the array by 1 and returns a reference to
	the last element. */
	T& CreateNewElement(void);

	/// Accesses an element by index.
	T& GetElement(U32 index)
	{ 
		IFXASSERT(index<m_elementsUsed);
		return *((T**)m_array)[index]; 
	}

	void DeleteElement(U32 index);

	/// Accesses an element by index, maintaining const.
	const T& GetElementConst(U32 index) const
	{ 
		IFXASSERT(index<m_elementsUsed);
		return *((T**)m_array)[index]; 
	}

#if FALSE
	/** Alternate to GetElement(); goes directly to backing.
	Can only access elements in the contiguous region.
	@note no noticeable performance gain */
	T& Backing(U32 index)
	{
		IFXASSERT(index<m_prealloc);
		return ((T*)m_contiguous)[index];
	}

	const T& BackingConst(U32 index) const
	{
		IFXASSERT(index<m_prealloc);
		return ((T*)m_contiguous)[index];
	}
#endif

	/// Access an element by index.
	T& operator[](U32 index)  { return GetElement(index); }

	/// Access an element by index, maintaining const.
	const T& operator[](U32 index) const
	{ 
		return GetElementConst(index); 
	}

	/// Deep copy (does not automatically Collect()).
	void operator=(const IFXArray<T> &other);

	/// Copy all elements of another array to end of this one.
	void Append(const IFXArray<T> &other);

	/** Moves all elements into contiguous space.  If you
	stored pointer to elements, this can make them
	invalid. */
	T* Collect(void);

	/** Generates a string representing the contents of
	this array. */
	IFXString Out(bool verbose=false) const;

private:
	virtual void Construct(U32 index);
	virtual void Destruct(U32 index);
	virtual void DestructAll(void);
	virtual void Preallocate(U32 preallocation);
	virtual void Resize(U32 needed);
};

template<class T>
IFXINLINE IFXArray<T>::IFXArray(U32 preallocation):
IFXCoreArray(preallocation)
{
	Preallocate(preallocation);
	if(preallocation>0) 
	{
		m_elementsUsed = 0;
		Resize((preallocation>IFXARRAY_MIN)? preallocation: IFXARRAY_MIN);
	}
}

template<class T>
IFXINLINE IFXArray<T>::~IFXArray(void)
{
	/// @todo: investigate major threading issue
#if IFXARRAY_UNSAFEMEMORY
	//* temporarily reinstate deallocation function appropriate for our data
	IFXAllocateFunction     *pAllocateFunction;
	IFXDeallocateFunction   *pDeallocateFunction;
	IFXReallocateFunction   *pReallocateFunction;

	IFXGetMemoryFunctions(
		&pAllocateFunction,
		&pDeallocateFunction,
		&pReallocateFunction);

	IFXSetMemoryFunctions(
		pAllocateFunction,
		m_pDeallocate,
		pReallocateFunction);
#endif

	DestructAll();
	IFXASSERT(m_array==NULL);
	IFXASSERT(m_contiguous==NULL);

#if IFXARRAY_UNSAFEMEMORY
	//* restore current deallocation function
	IFXSetMemoryFunctions(
		pAllocateFunction,pDeallocateFunction,
		pReallocateFunction);
#endif

}

template<class T>
IFXINLINE T &IFXArray<T>::CreateNewElement(void)
{
	ResizeToAtLeast(m_elementsUsed+1);

	return GetElement(m_elementsUsed-1);
}

template<class T>
IFXINLINE T *IFXArray<T>::Collect(void)
{
	U32 m;
	U32 newsize=m_elementsUsed;
	if(newsize<m_prealloc)
		newsize=m_prealloc;

	T *newbuffer=NULL;
	if(newsize>0)
	{
		newbuffer=new T[newsize];

		// copy old ((T*)m_contiguous) data
		for(m=0;m<m_prealloc;m++)
			newbuffer[m]=((T*)m_contiguous)[m];

		// copy and destroy scattered data
		for(;m<newsize;m++)
		{
			newbuffer[m]= *(((T**)m_array)[m]);
			Destruct(m);
		}
	}

	delete[] ((T*)m_contiguous);
	m_contiguous=newbuffer;
	m_prealloc=newsize;

	// just assigns pointers into ((T*)m_contiguous)
	for(m=0;m<m_prealloc;m++)
		Construct(m);

	return ((T*)m_contiguous);
}

template<class T>
IFXINLINE void IFXArray<T>::Construct(U32 index)
{
	if(index<m_prealloc)
	{
		((T**)m_array)[index]=&((T*)m_contiguous)[index];
		ResetElement(((T*)m_contiguous)[index]);
	}
	else
		((T**)m_array)[index]=new T;
}

template<class T>
IFXINLINE void IFXArray<T>::Destruct(U32 index)
{
	if(index>=m_prealloc)
		delete ((T**)m_array)[index];

	((T**)m_array)[index]=NULL;
}

template<class T>
IFXINLINE void IFXArray<T>::DestructAll(void)
{
	U32 m;
	for(m=m_prealloc;m<m_elementsUsed;m++)
		Destruct(m);

	if(((T**)m_array))
	{
		IFXASSERT(m_pDeallocate);
		if(m_pDeallocate)
			m_pDeallocate(((T**)m_array));
	}
	m_array=NULL;

	m_elementsUsed=0;
	m_elementsAllocated=0;

	if(((T*)m_contiguous))
	{
		delete[] ((T*)m_contiguous);
		m_contiguous=NULL;
	}
	m_prealloc=0;
}

template<class T>
IFXINLINE void IFXArray<T>::Preallocate(U32 preallocation)
{
	if(((T*)m_contiguous))
	{
		delete[] ((T*)m_contiguous);
		m_contiguous=NULL;
	}

	m_prealloc=preallocation;
	if(m_prealloc>0)
		m_contiguous=new T[m_prealloc];
}

template<class T>
IFXINLINE void IFXArray<T>::Resize(U32 needed)
{
	if(m_elementsAllocated<IFXARRAY_MIN || m_elementsAllocated<needed)
	{
		m_elementsAllocated=(U32)(m_elementsAllocated*IFXARRAY_EXPANSION);

		if(m_elementsAllocated<needed)
			m_elementsAllocated=needed;

		if(m_elementsAllocated<IFXARRAY_MIN)
			m_elementsAllocated=IFXARRAY_MIN;

#if FALSE
		// always copy to new buffer
		void** temp=(void**)IFXAllocate(m_elementsAllocated*sizeof(T*));
		if(((T**)m_array))
		{
			memcpy(temp,((T**)m_array),m_elementsUsed*sizeof(T*));
			m_pDeallocate(((T**)m_array));
		}
		m_array=temp;
#else
		m_array=(void**)IFXReallocate(m_array,m_elementsAllocated*sizeof(T*));
#endif
		IFXAllocateFunction* af;
		IFXDeallocateFunction* df;
		IFXReallocateFunction* rf;
		IFXGetMemoryFunctions( &af, &df, &rf );
		m_pDeallocate = df;

		IFXASSERT(((T**)m_array));
	}
}

template<class T>
IFXINLINE void IFXArray<T>::operator=(const IFXArray<T> &other)
{
	Clear();
	Append(other);
}

template<class T>
IFXINLINE void IFXArray<T>::Append(const IFXArray<T> &other)
{
	U32 offset=GetNumberElements();
	U32 m,length=other.GetNumberElements();

	ResizeToAtLeast(offset+length);
	for(m=0;m<length;m++)
		GetElement(offset+m)=other.GetElementConst(m);
}

template<class T>
IFXINLINE void IFXArray<T>::DeleteElement(U32 index)
{
	IFXASSERT(index<m_elementsUsed);

	Destruct(index);

	U32 i;
	for (i = index; i < m_elementsUsed-1; i++)
		((T**)m_array)[i]=((T**)m_array)[i+1];

	m_elementsUsed--;
}

#endif
