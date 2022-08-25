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


#ifndef __IFXVoidStar_h__
#define __IFXVoidStar_h__

#include "IFXMemory.h"

/**************************************************************************//**
	internal
*//***************************************************************************/
class IFXBaseVoidHolder
	{
	public:
		long	GetClassID(void) const			{ return m_classid; };
		long	GetPointerClassID(void) const	{ return m_pointerclassid; };
		void	*GetRawPointer(void)			{ return m_rawptr; };

virtual	IFXBaseVoidHolder *Clone(void *newptr=NULL)=0;
virtual	IFXBaseVoidHolder *SubClone(U32 offset=0)=0;
	virtual ~IFXBaseVoidHolder() {}

		void	SetPointerClassID(long set)		{ m_pointerclassid=set; };

	protected:
		void	SetClassID(long set)			{ m_classid=set; };
		void	SetRawPointer(void *set)		{ m_rawptr=set; };

	private:
		long	m_classid;
		long	m_pointerclassid;
		void	*m_rawptr;
	};


/**************************************************************************//**
	internal
*//***************************************************************************/
template<class T> class IFXVoidHolder: public IFXBaseVoidHolder
	{
	public:
				IFXVoidHolder(T *initval)
					{
					SetClassID(GetStaticID());
					SetRawPointer((void *)initval);
					m_tptr=initval;
					};

		T		*GetValue(void)		{ return m_tptr; };
static	long	GetStaticID(void)	{ return (long)&m_staticvalue; };

virtual	IFXBaseVoidHolder *Clone(void *newptr=NULL)
					{
					IFXBaseVoidHolder *clone;
					if(newptr)
						clone=new IFXVoidHolder<T>((T *)newptr);
					else
						clone=new IFXVoidHolder<T>(m_tptr);

					clone->SetPointerClassID(GetPointerClassID());
					return clone;
					};

virtual	IFXBaseVoidHolder *SubClone(U32 offset=0)
					{
					U8 *offsem_tptr=(U8 *)m_tptr;
					offsem_tptr+=offset;
					IFXBaseVoidHolder *clone=
										new IFXVoidHolder<T>((T *)offsem_tptr);

					clone->SetPointerClassID(GetPointerClassID());
					return clone;
					};

	private:
static	char	m_staticvalue;
		T		*m_tptr;
	};


/**************************************************************************//**
	IFXVoidStar is a typesafe version of (void *)

	NOTE: 
	-	Requires compiler support of templated members. 
	-	This is not a memory management or reference counting object. 
	-	In other projects this class is	called an IFXVariant.  
	-	A typedef is provided.

Usage:
@verbatim
	IFXVoidStar voidstar=new int(7);	 [7 is just an arbitrary test value.]

	char *pchar=NULL;
	IFXRESULT ok1=voidstar.CopyPointerTo(&pchar);	[fails, pchar unchanged.]

	int *pint=NULL;
	IFXRESULT ok2=voidstar.CopyPointerTo(&pint);	[points pint at the 7]

	float *pfloat=NULL;
	IFXRESULT ok3=voidstar.CopyPointerTo(&pfloat);	[fails, pfloat unchanged]

Ignoring result codes, you can also use direct assignment using implied cast:

	char *pchar=voidstar;							[pointer set to NULL]
	int *pint=voidstar;								[valid pointer returned]
	float *pfloat=voidstar;							[ pointer set to NULL]

To bypass type checking:

	-	other *pother=NULL;
	IFXRESULT ok3=voidstar.CopyPointerTo((void **)&pother);

	-	(objects typecast to/from a real (void *) are not type checked)

Remember:

	(IFXVoidStar) is like (void *), so (IFXVoidStar *) is like (void **).
	If you are thinking (void *), don't use (IFXVoidStar *).

	IFXVoidStar is very picky.  Like a general (void *), you are not allowed
	to pass between base and derived classes.
	Also, separate types that may be equivalent, such as long and int,
	are not interchangeable.

	Any pointer set to NULL is treated as a special NULL type.
	In the following example, vs1 and vs2 are different types and
	vs2 and vs3 are the same type.
		IFXVoidStar vs1=new int(7);
		IFXVoidStar vs2=((int *)NULL);
		IFXVoidStar vs3=((float *)NULL);
@endverbatim
*//***************************************************************************/
/**IFXVoidStar is a typesafe version of (void *).*/
class IFXVoidStar
	{
	public:
				/** General constructor.  The optional argument allows for
					assignment to NULL.  A non-zero argument may assert. */
				IFXVoidStar(long ignored=0)
					{
					IFXASSERT(ignored==0);
					m_holder=NULL;
					};

				/// Copy constructor.
				IFXVoidStar(const IFXVoidStar &vs)
					{
					m_holder=NULL;
					operator=(vs);
					};

				/** Construct on arbitrary pointer and retain the type
					of that pointer. */
				template<class T>
				IFXVoidStar(T *m_tptr)
					{
					m_holder=NULL;
					operator=(m_tptr);
					}

virtual			~IFXVoidStar(void)
					{
					if(m_holder)
						delete m_holder;
					};

				/** Operator specifically intended for assignment to NULL.
					A non-zero integer argument may assert. */
		void	operator=(long ignored)
					{
					IFXASSERT(ignored==0);
					if(m_holder)
						delete m_holder;
					m_holder=NULL;
					}

				/// Deep copy.
		void	operator=(const IFXVoidStar &vs)
					{
					IFXASSERT( (!m_holder && !vs.m_holder) ||
													m_holder!=vs.m_holder );
					if(m_holder)
						delete m_holder;
					if(vs.m_holder)
						m_holder=vs.m_holder->Clone();
					else
						m_holder=NULL;
					}

				/** Sets the state using an arbitrary pointer and
					retains the type of that pointer. */
				template<class T>
		T		*operator=(T *tptr)
					{
					if(m_holder)
						delete m_holder;
					if(!tptr)
						return tptr;

					m_holder=new IFXVoidHolder<T>(tptr);
					m_holder->SetPointerClassID(
										IFXVoidHolder<T *>::GetStaticID());
					return tptr;
					}

				/// Accesses the stored pointer, bypassing all type checking.
		void	*GetRawPointer(void)
					{
					if(!m_holder)
						return NULL;

					return m_holder->GetRawPointer();
					}

				//* Transparent return, but without result code.
				template<class T>
				/// Conversion Operator: see Stroustrup #2,pg232 and #3,pg275.
				operator T*(void)
					{
					T *pointer=NULL;
					CopyPointerTo(&pointer);
					return pointer;
					}

				/** Copy the contained pointer into the argument if
					they are exactly compatible types.  No conversions
					are made.

					Returns IFX_TRUE for an exact match,
					IFX_OK for a successful conversion,
					IFX_E_NOT_INITIALIZED if this variant is empty,
					IFX_E_INVALID_POINTER for a NULL pointer argument,
					or IFX_E_CANNOT_FIND for a type mismatch. */
				template<class T>
		IFXRESULT CopyPointerTo(T **tptr) const
					{
					long voidT=IFXVoidHolder<void>::GetStaticID();
					long staticT=IFXVoidHolder<T>::GetStaticID();
					if(!m_holder)
						{
						return IFX_E_NOT_INITIALIZED;
						}
					if(staticT==voidT || m_holder->GetClassID()==voidT)
						{
						// If either is (void *), ignore check.
						*tptr=(T *)((IFXVoidHolder<T> *)m_holder)->GetValue();
						return IFX_OK;
						}
					else if(m_holder->GetClassID() == staticT)
						{
						*tptr=((IFXVoidHolder<T> *)m_holder)->GetValue();
						return IFX_OK;
						}
					else
						{
						return IFX_E_INVALID_POINTER;
						}
					}

				/** Returns true if the given object is a pointer to the
					type of this object. */
		bool	IsPointerToTypeOf(const IFXVoidStar &vs)
					{
					if(!m_holder && !vs.m_holder)
						return true;
					if(!m_holder || !vs.m_holder)
						return false;
					long m_classid=m_holder->GetClassID();
					long m_pointerclassid=vs.m_holder->GetPointerClassID();
					return (m_classid && m_classid==m_pointerclassid);
					}

				/// Creates new m_holder of same type and specified size.
		void	*Malloc(U32 size)
					{
					if(!m_holder)
						return NULL;

					IFXBaseVoidHolder *old_holder=m_holder;

					void *buffer=IFXAllocate(size);
					m_holder=old_holder->Clone(buffer);

					delete old_holder;

					return buffer;
					}

					/** Returns object of same type, but offset in memory
						(element in array). */
		IFXVoidStar	GetOffsetElement(U32 offset)
					{
					IFXVoidStar indexed;
					indexed.m_holder=m_holder->SubClone(offset);
					return indexed;
					}

	private:
		IFXBaseVoidHolder	*m_holder;
	};



// Stroustrup C++ 2nd Ed r.9.4 pg548
// "Static members of a global class have external linkage."
// however,
// Stroustrup C++ 2nd Ed r.3.3 pg484
// "There must be exactly one definition for each function, object,
// class and enumerator used in a program."
// This compiles under g++ and VC6, so does this link into one definition
// even after it is included in every file?
// The same seems to apply to enumerators found in headers.
template<class T> char IFXVoidHolder<T>::m_staticvalue=0;


typedef IFXVoidStar IFXVariant;


#endif
