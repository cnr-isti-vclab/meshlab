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
	@file	CIFXContour.cpp

			Implementation file for the path segment class.
*/

#include "CIFXContour.h"
#include "IFXDebug.h"
#include <memory.h>



// IFXUnknown methods

//---------------------------------------------------------------------------
//	CIFXContour::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXContour::AddRef(void) {
	return ++m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXContour::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXContour::Release(void) {
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
//	CIFXContour::QueryInterface
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
IFXRESULT CIFXContour::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXContour ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXContour* ) this;
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
//	CIFXContour_Factory (non-singleton)
//
//	This is the CIFXContour component factory function.  The
//	CIFXContour component is NOT a singleton.  This function creates the
//  path segment object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXContour_Factory( IFXREFIID	interfaceId,
									void**		ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXContour	*pComponent	= new CIFXContour;

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

// IFXTextGenerator methods

IFXRESULT CIFXContour::Initialize(U32 uSize) {
	IFXRESULT iResult=IFX_OK;
	
	if(m_ppList)
		iResult=IFX_E_ALREADY_INITIALIZED;
	if(uSize==0)
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {

		m_ppList = new SNode *[uSize];

		memset((void *)m_ppList,0,uSize*sizeof(SNode *));

		if(m_ppList==NULL)
			iResult=IFX_E_OUT_OF_MEMORY;
		else {
			m_uLastElement=uSize-1;
		}
	}
	return iResult;
}

IFXRESULT CIFXContour::AddNext(SIFXContourPoint* pPosition, SIFXContourPoint* pNormal, U32* pIndex){
	IFXRESULT iResult=IFX_OK;
	if(pPosition==NULL || pNormal==NULL || pIndex==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;


	SNode **ppNewList=NULL;

	if(IFXSUCCESS(iResult)) {
		// reallocate more space?
		if(m_uCount>m_uLastElement) {
			U32 uNewSize=m_uCount+(m_uCount/2)+1;
			ppNewList = new SNode *[uNewSize];

			if( ppNewList == NULL)
			{
				iResult = IFX_E_OUT_OF_MEMORY;
			}

			if(IFXSUCCESS(iResult)) {

				memset((void*)ppNewList,0,uNewSize*sizeof(SNode*));
				for(U32 i=0; i< m_uCount; i++)
				{
					ppNewList[i]=m_ppList[i];
				}
				delete[] m_ppList;
				m_ppList = ppNewList;

			}

			if(IFXSUCCESS(iResult)) {
				m_ppList=(SNode**)ppNewList;
				m_uLastElement=uNewSize-1;
			}
		}

		// ready to add
		if(IFXSUCCESS(iResult)) {
			SNode* pNode=new SNode;
			pNode->vPosition=*pPosition;
			pNode->vNormal=*pNormal;
			m_ppList[m_uCount]=pNode;
			*pIndex=m_uCount;
			m_uCount++;
		}
	}

	return iResult;
}

IFXRESULT CIFXContour::Get(U32 uIndex, SIFXContourPoint* pPosition, SIFXContourPoint* pNormal){
	IFXRESULT iResult=IFX_OK;
	if(pPosition==NULL || pNormal==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {
		*pPosition=m_ppList[uIndex]->vPosition;
		*pNormal=m_ppList[uIndex]->vNormal;
	}
	return iResult;
}

IFXRESULT CIFXContour::GetCount(U32* pCount){
	IFXRESULT iResult=IFX_OK;
	if(pCount==NULL)
		iResult=IFX_E_INVALID_POINTER;
	else
		*pCount=m_uCount;
	return iResult;
}

IFXRESULT CIFXContour::Delete(U32 uIndex){

	// this function is not tested. 
	IFXASSERT(0);
	IFXRESULT iResult=IFX_E_UNSUPPORTED;
/*
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) // or equal???
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) {
		delete m_ppList[uIndex];
		U32 uMoveIndex=uIndex+1;
		while(uMoveIndex<=m_uCount) {
			m_ppList[uMoveIndex-1]=m_ppList[uMoveIndex];
		}
		m_ppList[uMoveIndex] = NULL;
		m_uCount--;

	}
*/
	return iResult;
}

IFXRESULT CIFXContour::GetPosition(U32 uIndex, SIFXContourPoint* pPosition){
	IFXRESULT iResult=IFX_OK;
	if(pPosition==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) 
		*pPosition=m_ppList[uIndex]->vPosition;

	return iResult;
}

IFXRESULT CIFXContour::GetNormal(U32 uIndex, SIFXContourPoint* pNormal){
	IFXRESULT iResult=IFX_OK;

	if(pNormal==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) 
		*pNormal=m_ppList[uIndex]->vNormal;
	
	return iResult;
}

IFXRESULT CIFXContour::SetPosition(U32 uIndex, SIFXContourPoint* pPosition){
	IFXRESULT iResult=IFX_OK;
	if(pPosition==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) 
		m_ppList[uIndex]->vPosition=*pPosition;

	return iResult;
}

IFXRESULT CIFXContour::SetNormal(U32 uIndex, SIFXContourPoint* pNormal){
	IFXRESULT iResult=IFX_OK;

	if(pNormal==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_ppList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(uIndex>m_uLastElement) 
		iResult=IFX_E_INVALID_RANGE;

	if(IFXSUCCESS(iResult)) 
		m_ppList[uIndex]->vNormal=*pNormal;
	
	return iResult;
}



// CIFXContour private methods

CIFXContour::CIFXContour() {
	m_uRefCount=0;
	m_ppList=NULL;
	m_uCount=0;
	m_uLastElement=0;
}

CIFXContour::~CIFXContour() {
	if(m_ppList) {
		U32 uIndex=0;
		while(uIndex<=m_uLastElement) {
			if(m_ppList[uIndex]!=NULL) {
				delete m_ppList[uIndex];
				m_ppList[uIndex]=NULL;
			}

			// umm, it would be nice to kick the counter unless you like infinite loops
			++uIndex;
		}
		delete [] m_ppList;

		m_ppList=NULL;
	}
	m_uCount=0;
	m_uLastElement=0;
}
