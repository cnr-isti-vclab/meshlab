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

/**
	@file	IFXAutoRelease.h

			Definition of IFXAutoRelease template. IFXAutoRelease is used to 
			automatically release a reference to an IFXCom interface when a 
			pointer variable goes out of scope.

	@note	IFXAutoRelease is similar in purpose to IFXAutoPtr. IFXAutoPtr 
			calls delete [] but IFXAutoRelease calls Release ().
*/

#ifndef IFXAUTORELEASE_H
#define IFXAUTORELEASE_H

#include "IFXDebug.h"

/**
	IFXAutoRelease is used to automatically
	release a reference to an IFXCom interface when a pointer variable goes
	out of scope.

	@note All normal reference counting rules should still be followed.

	The IFXAutoRelease object should be declared immediately
	after the IFXCom pointer is declared.

	Usage Example:

	@verbatim
	{
		IFXBitStreamX* pBitStreamX ;
		IFXAutoRelease<IFXBitStreamX> arpBitStreamX(pBitStreamX);
		...
	}
	@endverbatim

	pBitStreamX will be set to NULL in the constructor for the IFXAutoRelease.
	pBitStreamX will be released in the destructor for the IFXAutoRelease.

	Macros are defined to make use of the IFXAutoRelease mechanism easier.

	The macro IFXDECLARELOCAL(type, name) is used to declare
	a local pointer variable and an autorelease wrapper object.

	@verbatim
	{
		IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
		...
	}
	@endverbatim

	The macro IFXDECLAREMEMBER(type, name) is used to declare
	a pointer variable as a member of a class in the class definition.

	@verbatim
	class CIFXXXX
	{
		...
	private:
		U32 m_refCount;
		IFXDECLAREMEMBER(IFXBitStreamX,m_pBitStreamX);
		IFXDECLAREMEMBER(IFXDataBlockX,m_pDataBlockX);
		...
	}
	@endverbatim

	The macro IFXDEFINEMEMBER(name) is used in the constructor
	for the object with the auto release member pointer variable.

	@verbatim
	CIFXXXX::CIFXXXX() :
		IFXDEFINEMEMBER(m_pBitStreamX),
		IFXDEFINEMEMBER(m_pDataBlockX)
	{
		m_refCount = 0;
	}
	@endverbatim
*/
template<class T>
class IFXAutoRelease
{
public:
  IFXAutoRelease(T*& rpPtr);
  ~IFXAutoRelease();

private:
  T** m_ppPtr; ///< Address of the IFXCom interface pointer being auto released.
};

/// Constructor holds address of pointer being auto released
template<class T>
inline IFXAutoRelease<T>::IFXAutoRelease(T*& rpPtr)
{
  m_ppPtr = &rpPtr;
  IFXASSERT(m_ppPtr);
  *m_ppPtr = NULL;
}

/// Destructor releases pointer being auto ref'ed
template< class T>
inline IFXAutoRelease< T>::~IFXAutoRelease()
{
  IFXASSERT(m_ppPtr);
  T* pPtr = *m_ppPtr;
  if(pPtr) {
    pPtr->Release();
    *m_ppPtr = NULL;
  }
}

#define IFXDECLARELOCAL(type,name) \
type* name; \
IFXAutoRelease<type> ar##name(name)

#define IFXDECLAREMEMBER(type,name) \
type* name; \
IFXAutoRelease<type> ar##name

#define IFXDEFINEMEMBER(name) \
ar##name(name)

#endif
