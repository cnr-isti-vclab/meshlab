//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file	IFXSmartPtr.h

			Template smart interface pointer handling class.

			@note	The IFXSmartPtr is used solely with the rendering subsystem.
			New usages are discouraged because existing IFXSmartPtr usages 
			will eventually be replaced with the less-obfuscated 
			IFXAutoRelease.h functionality and the explicit interface 
			reference management technique used by the rest of the engine.
*/

#ifndef IFXSmartPtr_H
#define IFXSmartPtr_H


#include "IFXAPI.h"
#include "IFXDataTypes.h"
#include "IFXGUID.h"
#include "IFXResult.h"


extern "C" IFXRESULT IFXAPI IFXCreateComponent(
	IFXREFCID	componentId,
	IFXREFIID	interfaceId,
	void**		ppInterface );

template <class T> class IFXSmartPtr;

/**
*  Abstract base class for IFXSmartPtr.  The implementation is
*  split between the two classes to avoid as much code generation
*  as possible with new template specializations of IFXSmartPtr<T>.
*/
class IFXCoreSmartPtr
{
public:
	IFXCoreSmartPtr() {}  /// Default Constructor
	virtual ~IFXCoreSmartPtr() {} /// Default Destructor

	/**
	*  Determines if the smart pointer is valid.
	*
	*  @return BOOL value.
	*  @retval TRUE  The smart pointer is pointing to a valid object
	*  @retval FALSE The smart pointer does not reference a valid object
	*/
	BOOL IFXAPI IsValid() const
	{
		return (m_pPtr != 0);
	}

	/**
	*  This will attempt to query interface the underlying
	*  object for an interface of type S, and fill the @a rPtr
	*  argument with that new interface pointer.  If the method
	*  succeeds, the total reference count of the underlying object
	*  will be incremented (since @a rPtr now owns a reference).
	*
	*  @param  @rPtr IFXSmartPtr of a different interface 'S' than
	*          this instance.  If the interface type S can by
	*          queried by the underlying object, @a rPtr will
	*          be filled with the pointer to the queried interface.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The interface type of @a rPtr is not compitable
	*                with this object.
	*/
	template <class S>
		IFXRESULT IFXAPI QI(IFXSmartPtr<S> &rPtr, IFXREFIID iid) const
	{
		void* vPtr = 0;

		IFXRESULT rc = ReCastTo(iid, vPtr);
		if(IFXSUCCESS(rc))
		{
			rPtr = (S*)vPtr;

			// Both the ReCastTo and the '=' operation above increments
			// the reference count.  This will fix it.
			rPtr.DecRef();
		}

		return rc;
	}

protected:

	/**
	*  Recast (QueryInterface) the underlying object to an interface with IID
	*  @a guid, and place the results in @a rvPtr.  If this succeeds, the reference
	*  count of the object will be incremented.
	*
	*  @param  guid  Input IFXGUID containing the IID of the interface to convert
	*          to.
	*  @param  rvPtr Reference to a void* that will be filled in with a pointer
	*          to the interface type denoted by @a guid.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The interface type @a guid is not compatible
	*                with this object.
	*/
	virtual IFXRESULT IFXAPI ReCastTo(const IFXGUID& guid, void*& rvPtr) const=0;

	void* m_pPtr;
};

/**
*  The IFXSmartPtr object contains a pointer to an IFXUnknown derived object.  The
*  smart pointer will automatically adjust the reference count of the underlying
*  object when necessary (object goes out of scope, assigned to a new object, etc.).
*/
template <class T> class IFXSmartPtr : public IFXCoreSmartPtr
{
public:
	/// Constructor
	IFXSmartPtr()
	{
		m_pPtr = 0;
	}

	/// Assignment constructor
	IFXSmartPtr(T* pPtr)
	{
		if(pPtr)
		{
			pPtr->AddRef();
		}

		m_pPtr = pPtr;
	}

	/// Copy constructor
	IFXSmartPtr(const IFXSmartPtr<T> &rPtr)
	{
		if(rPtr.m_pPtr)
		{
			((T*)rPtr.m_pPtr)->AddRef();
		}

		m_pPtr = rPtr.m_pPtr;
	}

	/// Destructor - release the underlying object.
	~IFXSmartPtr()
	{
		if(IsValid())
		{
			((T*)m_pPtr)->Release();
		}
	}

	/**
	*  This allows the user to call methods of the underlying object
	*  directly.
	*
	*  @return A pointer to the underlying object.
	*/
	T* IFXAPI operator->() const
	{
		return (T*)m_pPtr;
	}

	/**
	*  Retrieve a direct pointer to the underlying object.  This is done
	*  without adding to the reference count of the object.
	*
	*  @return A pointer to the underlying object.
	*/
	T* IFXAPI GetPointerNR() const
	{
		return (T*)m_pPtr;
	}

	/**
	*  Retrieve a direct pointer to the underlying object.  This will
	*  add a reference count to the object.
	*
	*  @return A pointer to the underlying object.
	*/
	T* IFXAPI GetPointerAR() const
	{
		if(IsValid())
		{
			((T*)m_pPtr)->AddRef();
		}

		return (T*)m_pPtr;
	}

	/**
	*  Queries to see if the underlying object is the same as @a pPtr.
	*
	*  @param  pPtr  Input pointer to the same object type as the one
	*          held in this smart pointer.
	*
	*  @param  A BOOL value.
	*  @retval TRUE  The underlying object is the same as @a pPtr.
	*  @retval FALSE The underlying object is not the same instance as @a pPtr.
	*/
	BOOL IFXAPI PointsTo(const T* pPtr) const
	{
		return m_pPtr == pPtr;
	}

	/**
	*  Assign a new object to this smart ptr.  This will release the old object and
	*  addref the new object.
	*
	*  @param  pPtr  Pointer to a new object to manage.
	*
	*  @return A reference to this smart pointer.
	*/
	IFXSmartPtr<T>& IFXAPI operator=(T* pPtr)
	{
		if(pPtr)
		{
			pPtr->AddRef();
		}

		if(IsValid())
		{
			((T*)m_pPtr)->Release();
		}

		m_pPtr = pPtr;

		return *this;
	}

	/**
	*  Assign a new object to this smart ptr.  This will release the old object and
	*  addref the new object.
	*
	*  @param  pPtr  Smart pointer to a new object to manage.
	*
	*  @return A reference to this smart pointer.
	*/
	IFXSmartPtr<T>& IFXAPI operator=(const IFXSmartPtr<T>& rPtr)
	{
		return operator=((T*)rPtr.m_pPtr);
	}

	/**
	*  Implementation of the '==' operator.
	*
	*  @param  rPtr  Another smart pointer to compare with.
	*
	*  @return A BOOL value.
	*  @retval TRUE  This object and @a rPtr reference the same thing.
	*  @retval FALSE This object does not hold the same thing as @a rPtr.
	*/
	BOOL IFXAPI operator==(const IFXSmartPtr<T>& rPtr) const
	{
		return operator==((T*)rPtr.m_pPtr);
	}

	/**
	*  Implementation of the '==' operator.
	*
	*  @param  pPtr  Another object pointer to compare with.
	*
	*  @return A BOOL value.
	*  @retval TRUE  This object holds a reference to @a pPtr.
	*  @retval FALSE This object does not hold a reference to @a pPtr.
	*/
	BOOL IFXAPI operator==(const T* pPtr) const
	{
		return m_pPtr == (const void*)pPtr;
	}

	/**
	*  Create a new object of the type with class id @a impl, and hold a reference
	*  to the created object using the interface type specified with the
	*  specialized type of this template class.
	*
	*  @param  impl  Input IFXREFCID containing the class id of the class type
	*          we are to create.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified with @a impl is invalid.
	*/
	IFXRESULT IFXAPI Create(IFXREFCID impl, IFXREFIID iid)
	{
		T* pPtr = 0;

		IFXRESULT rc = IFXCreateComponent(impl, iid, (void**)&pPtr);

		if(IFXSUCCESS(rc))
		{
			if(IsValid())
			{
				((T*)m_pPtr)->Release();
			}

			m_pPtr = pPtr;
		}

		return rc;
	}

	/**
	*  Increment the reference count of the underlying object if it is valid.
	*  @return The new reference count of the object.
	*/
	U32 IFXAPI IncRef()
	{
		if(IsValid())
		{
			return ((T*)m_pPtr)->AddRef();
		}

		return 0;
	}

	/**
	*  Decrement the reference count of the underlying object if it is valid.
	*  If this causes the object to be deleted, set the underlying object
	*  pointer to 0.
	*  @return The new reference count of the underlying object.
	*/
	U32 IFXAPI DecRef()
	{
		if(IsValid())
		{
			U32 rc = ((T*)m_pPtr)->Release();
			if(rc == 0)
			{
				m_pPtr = 0;
			}

			return rc;
		}

		return 0;
	}

	/**
	*  Recast (QueryInterface) the underlying object to an interface with IID
	*  @a guid, and place the results in @a rvPtr.  If this succeeds, the reference
	*  count of the object will be incremented.
	*
	*  @param  guid  Input IFXGUID containing the IID of the interface to convert
	*          to.
	*  @param  rvPtr Reference to a void* that will be filled in with a pointer
	*          to the interface type denoted by @a guid.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The interface type @a guid is not compatible
	*                with this object.
	*/
	virtual IFXRESULT IFXAPI ReCastTo(const IFXGUID& guid, void*& rvPtr) const
	{
		if(!IsValid())
		{
			return IFX_E_INVALID_POINTER;
		}

		return ((T*)m_pPtr)->QueryInterface(guid, &rvPtr);
	}

	/**
	*  This will release the underlying object contained by the smart pointer
	*  and return a pointer the to the smart pointer's pointer.  (Got that :)
	*  This can be used with like:
	*
	*  void GetObject(Obj ** ppObj);
	*
	*  IFXSmartPtr<Obj> spObj;
	*  GetObj(spObj.GetPPtr());
	*  spObj.IncRef();
	*
	*  @return A pointer to the pointer contained within this object.
	*/
	T** GetPPtr()
	{
		if(IsValid())
		{
			((T*)m_pPtr)->Release();
			m_pPtr = NULL;
		}
		return (T**)&m_pPtr;
	}
};


#endif
