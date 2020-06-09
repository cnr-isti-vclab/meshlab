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
	@brief Automatic pointer management class

	This manages a pointer to memory allocated on the heap.
	After a pointer is assigned to an auto point, it assumes
	responsibility for deleting it. 

	This differs from a smart pointer, in that when another
	auto pointer is given control over the object,
	the previous owner does not retain any control.
*/

#ifndef __IFXAUTOPTR_H__
#define __IFXAUTOPTR_H__

template<class T>
class IFXAutoPtr
{
public:
	IFXAutoPtr();
	IFXAutoPtr(const IFXAutoPtr& );
	~IFXAutoPtr();

	T* operator *();
//	T** operator &(); // incompatible with STL
	operator T*();

	BOOL IsValid();
	BOOL IsNull();
	BOOL operator!();
	BOOL operator==(T* pInPtr);
	BOOL operator!=(T* pInPtr);

	T* Detach();
	IFXAutoPtr& operator=(T* in_pInPtr);
	IFXAutoPtr& operator=(const IFXAutoPtr& in_pInPtr);

private:
	T* m_pPtr; ///< Ptr for object that we are wrapping.
};


//---------------------------------------------------------
// IFXAutoPtr Implementation
//---------------------------------------------------------
/**	Default Constructor. */
template< class T>
inline IFXAutoPtr< T>::IFXAutoPtr()
{
	m_pPtr = NULL;
}

/**	Copy Constructor. */
template< class T>
inline IFXAutoPtr< T >::IFXAutoPtr(const IFXAutoPtr& in_Ptr)
{
	m_pPtr = in_Ptr.m_pPtr;
	T** t = (T**)&(in_Ptr.m_pPtr);
	*t = NULL;
}


/**	Default Destructor.*/
template< class T>
inline IFXAutoPtr< T>::~IFXAutoPtr()
{
	IFXDELETE_ARRAY(m_pPtr);
}

/**	
**/
template< class T>
inline T* IFXAutoPtr< T>::operator*()
{
	return m_pPtr;
}

/**	
**/
/*template< class T>
inline T** IFXAutoPtr< T>::operator&()
{
	IFXDELETE_ARRAY(m_pPtr);
	return &m_pPtr;
}
*/

/**	Dereferences to auto pointer.  In reality, this exposes the
	owned pointer as the return value.*/
template< class T>
inline IFXAutoPtr< T>::operator T*()
{
	return m_pPtr;
}

/**	Tests whether the owned pointer is not NULL and therefore valid.*/
template< class T>
inline BOOL IFXAutoPtr< T>::IsValid()
{
	return m_pPtr != NULL;
}

/**	Tests whether the owned pointer is NULL.*/
template< class T>
inline BOOL IFXAutoPtr< T>::IsNull()
{
	return m_pPtr == NULL;
}


/**	Tests whether the owned pointer is NULL.*/
template< class T>
inline BOOL IFXAutoPtr< T>::operator !()
{
	return !m_pPtr;
}


/**	Removes ownership of the pointer.  This auto-pointer will no longer
	affect the pointed-to object.

*/
template< class T>
inline T* IFXAutoPtr< T>::Detach()
{
	T* tPtr = m_pPtr;
	m_pPtr = NULL;
	return tPtr;
}

/**	Assigns ownership of the pointer.*/
template< class T>
inline IFXAutoPtr< T>& IFXAutoPtr< T>::operator=(T* pInPtr)
{
	if(pInPtr != m_pPtr) // check for self assignment
	{
		IFXDELETE_ARRAY(m_pPtr);
		m_pPtr = pInPtr;
	}
	return *this;
}


/**	 Autopointer assignment operator -- transfers ownership of the pointer.
	The autopointer in the given argument does not retain any control
	over the pointed-to object.
*/
template< class T>
inline IFXAutoPtr< T>& IFXAutoPtr<T>::operator=(const IFXAutoPtr<T>& pInPtr)
{
	if(&pInPtr != this) // check for self assignment
	{
		IFXDELETE_ARRAY(m_pPtr);
		m_pPtr = pInPtr.m_pPtr;
		T** t = &const_cast<T*>(pInPtr.m_pPtr);
		*t = NULL;
	}
	return *this;
}

#endif
