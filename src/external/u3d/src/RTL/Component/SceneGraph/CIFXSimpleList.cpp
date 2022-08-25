//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	CIFXSimpleList.cpp
	
			Implementation file for the path class.
*/

#include "CIFXSimpleList.h"
#include "IFXMemory.h"



// IFXUnknown methods

//---------------------------------------------------------------------------
//	CIFXSimpleList::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXSimpleList::AddRef(void) {
	return ++m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXSimpleList::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXSimpleList::Release(void) {
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXSimpleList::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXSimpleList::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXSimpleList ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXSimpleList* ) this;
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;

}

//---------------------------------------------------------------------------
//	CIFXSimpleList_Factory (non-singleton)
//
//	This is the CIFXSimpleList component factory function.  The
//	CIFXSimpleList component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXSimpleList_Factory( IFXREFIID	interfaceId,
									void**		ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXSimpleList	*pComponent	= new CIFXSimpleList;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// IFXSimpleList methods

IFXRESULT CIFXSimpleList::Initialize(U32 uSize) {
	IFXRESULT iResult=IFX_OK;
	
	if(m_ppList)
		iResult=IFX_E_ALREADY_INITIALIZED;
	if(uSize==0)
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {
		m_ppList=(IFXUnknown**)IFXAllocate((sizeof(IFXUnknown*)*uSize));
		if(m_ppList==NULL)
			iResult=IFX_E_OUT_OF_MEMORY;
		else {
			U32 i;
			for(i=0;i<uSize;i++)
				m_ppList[i] = NULL;
			m_uLastElement=uSize-1;
		}
	}
	return iResult;
}

IFXRESULT CIFXSimpleList::Add(IFXUnknown* pObject, U32* pIndex) {
	IFXRESULT iResult=IFX_OK;
	if(pObject==NULL || pIndex==NULL)
		iResult=IFX_E_INVALID_POINTER;
	else if(m_ppList==NULL)
		iResult=Initialize(4);

	if(IFXSUCCESS(iResult)) {
		// reallocate more space?
		if(m_uCount>m_uLastElement) {
			U32 uNewSize=m_uCount+(m_uCount/2)+1;
			void* pNewList=NULL;
			pNewList=(IFXUnknown**)IFXReallocate(m_ppList, (uNewSize*sizeof(IFXUnknown*)));
			if(pNewList==NULL)
				iResult=IFX_E_OUT_OF_MEMORY;

			if(IFXSUCCESS(iResult)) {
				m_ppList=(IFXUnknown**)pNewList;

				// Set all pointers to NULL
				U32 i;
				for(i=m_uCount;i<uNewSize;i++)
					m_ppList[i] = NULL;

				m_uLastElement=uNewSize-1;
			}
		}

		// ready to add
		if(IFXSUCCESS(iResult)) {
			m_ppList[m_uCount]=pObject;
			pObject->AddRef();
			*pIndex=m_uCount;
			m_uCount++;
		}
	}

	return iResult;
}

IFXRESULT CIFXSimpleList::Delete(U32 uIndex) {
	IFXRESULT iResult=IFX_OK;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {
		IFXRELEASE( m_ppList[ uIndex ] );

		U32 uMoveIndex=uIndex+1;
		// ??? well which one is it?  m_uLastElement or m_uCount
//		while(uMoveIndex<m_uLastElement && m_ppList[uMoveIndex]!=NULL) {
		while(uMoveIndex<m_uCount && m_ppList[uMoveIndex]!=NULL) {
			m_ppList[uMoveIndex-1]=m_ppList[uMoveIndex];
			uMoveIndex++; // kick forward unless you like infinite loops!!!
		}

		m_uCount--;
	}
	return iResult;
}

IFXRESULT CIFXSimpleList::Get(U32 uIndex, IFXUnknown** ppObject) {
	IFXRESULT iResult=IFX_OK;
	if(ppObject==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {
		if(m_ppList[uIndex])
			m_ppList[uIndex]->AddRef();
		*ppObject=m_ppList[uIndex];
	}
	return iResult;
}

IFXRESULT CIFXSimpleList::Set(U32 uIndex, IFXUnknown* pObject) {
	IFXRESULT iResult=IFX_OK;
	if(pObject==NULL)
		iResult=IFX_E_INVALID_POINTER;
	else if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {
		if( !m_ppList[uIndex] ) 
			m_uCount++;
		else
			IFXRELEASE(m_ppList[uIndex]);
		m_ppList[uIndex]=pObject;
		m_ppList[uIndex]->AddRef();
	}
	return iResult;
}

IFXRESULT CIFXSimpleList::Insert(U32 uIndex, IFXUnknown* pObject) {
	IFXRESULT iResult=IFX_OK;
	U32 index;
	if(pObject==NULL)
		iResult=IFX_E_INVALID_POINTER;
	else if(m_ppList==NULL)
		return Add(pObject, &index);

	if(IFXSUCCESS(iResult)) {
		// First add new object to end of list
		iResult = Add(pObject, &index);
		if(IFXSUCCESS(iResult)) {
			// then shuffle from uIndex down
			while ( uIndex < index ) {
				m_ppList[index]=m_ppList[index-1];
				--index;
			}
			// finally, place new object at uIndex
			m_ppList[uIndex]=pObject;
		}
	}

	return iResult;
}


IFXRESULT CIFXSimpleList::GetCount(U32* pCount) {
	IFXRESULT iResult=IFX_OK;
	if(pCount==NULL)
		iResult=IFX_E_INVALID_POINTER;
	else
		*pCount=m_uCount;
	return iResult;
}

IFXRESULT CIFXSimpleList::Copy(IFXSimpleList* pDestList) {
	IFXRESULT iResult=IFX_OK;
	if(pDestList==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		U32 uIndex=0;
		U32 uDummy=0;
		while(uIndex<m_uCount && IFXSUCCESS(iResult)) {
			iResult=pDestList->Add(m_ppList[uIndex],&uDummy);
			uIndex++;
		}
	}

	return iResult;
}



// CIFXSimpleList private methods

CIFXSimpleList::CIFXSimpleList() {
	m_uRefCount=0;
	m_ppList=NULL;
	m_uCount=0;
	m_uLastElement=0;
}

CIFXSimpleList::~CIFXSimpleList() {
	if(m_ppList) {
		U32 uIndex=0;

		while(uIndex<m_uCount)
		{
			IFXRELEASE(m_ppList[uIndex]);
			++uIndex;
		}

		IFXDeallocate(m_ppList);
	}
}
