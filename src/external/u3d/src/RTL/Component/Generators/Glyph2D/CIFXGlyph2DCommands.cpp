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
//
//  CIFXGlyph2DCommands.cpp
//
//  DESCRIPTION
//    Implementation file for the block generator classes.
//
//  NOTES
//      None.
//
//***************************************************************************
#include "CIFXGlyph2DCommands.h"


//****************************************************************
//  IID_IFXGlyphTagBlock
//****************************************************************

// IFXGlyph2DCommands methods

CIFXGlyphTagBlock::CIFXGlyphTagBlock() {
	m_uRefCount = 0;
	m_eType = IGG_TYPE_STARTGLYPHSTRING;
	m_x = 0;
	m_y = 0;
}
CIFXGlyphTagBlock::~CIFXGlyphTagBlock() {
}

//---------------------------------------------------------------------------
//  CIFXGlyphTagBlock::GetType
//
//  This method returns the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphTagBlock::GetType(EGLYPH_TYPE* pType) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pType)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pType = m_eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphTagBlock::SetType
//
//  This method sets the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphTagBlock::SetType(const EGLYPH_TYPE eType)
{
	IFXRESULT iResult = IFX_OK;

	m_eType = eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphTagBlock::GetAttributes
//
//  This method returns the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphTagBlock::GetAttributes(U32* pAttr) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pAttr)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pAttr = m_attr;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphTagBlock::SetAttributes
//
//  This method sets the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphTagBlock::SetAttributes(const U32 attr)
{
	IFXRESULT iResult = IFX_OK;

	m_attr = attr;

	return iResult;
}


// IFXUnknown methods

//---------------------------------------------------------------------------
//  CIFXGlyphTagBlock::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXGlyphTagBlock::AddRef(void) {
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXGlyphTagBlock::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXGlyphTagBlock::Release(void) {
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
//  CIFXGlyphTagBlock::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphTagBlock::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXGlyphTagBlock )
			*ppInterface = ( IFXGlyphTagBlock* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXGlyph2DCommands )
			*ppInterface = ( IFXGlyph2DCommands* ) this;
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
//  CIFXGlyphTagBlock_Factory (non-singleton)
//
//  This is the CIFXGlyphTagBlock component factory function.  The
//  CIFXGlyphTagBlock component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXGlyphTagBlock_Factory( IFXREFIID  interfaceId,
									void**    ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXGlyphTagBlock *pComponent = new CIFXGlyphTagBlock;

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

IFXRESULT CIFXGlyphTagBlock::GetData(F64* px, F64* py) const
{

	IFXRESULT iResult = IFX_OK;

	if(NULL == px || NULL == py )
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		*px = m_x;
		*py = m_y;
	}

	return iResult;
}

IFXRESULT CIFXGlyphTagBlock::SetData(const F64 x, const F64 y)
{
	IFXRESULT iResult = IFX_OK;

	m_x = x;
	m_y = y;

	return iResult;
}


//****************************************************************
//  IID_IFXGlyphMoveToBlock
//****************************************************************

// IFXGlyph2DCommands methods

CIFXGlyphMoveToBlock::CIFXGlyphMoveToBlock() {
	m_uRefCount = 0;
	m_eType = IGG_TYPE_MOVETO;
	m_x = 0;
	m_y = 0;
}
CIFXGlyphMoveToBlock::~CIFXGlyphMoveToBlock() {
}
//---------------------------------------------------------------------------
//  CIFXGlyphMoveToBlock::GetType
//
//  This method returns the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphMoveToBlock::GetType(EGLYPH_TYPE* pType) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pType)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pType = m_eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphMoveToBlock::GetType
//
//  This method sets the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphMoveToBlock::SetType(const EGLYPH_TYPE eType)
{
	IFXRESULT iResult = IFX_OK;

	m_eType = eType;

	return iResult;
}

IFXRESULT CIFXGlyphMoveToBlock::GetData(F64* px, F64* py) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == px || NULL == py)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		*px = m_x;
		*py = m_y;
	}

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphMoveToBlock::GetAttributes
//
//  This method returns the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphMoveToBlock::GetAttributes(U32* pAttr) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pAttr)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pAttr = m_attr;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphMoveToBlock::SetAttributes
//
//  This method sets the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphMoveToBlock::SetAttributes(const U32 attr)
{
	IFXRESULT iResult = IFX_OK;

	m_attr = attr;

	return iResult;
}

IFXRESULT CIFXGlyphMoveToBlock::SetData(const F64 x, const F64 y)
{
	IFXRESULT iResult = IFX_OK;

	m_x = x;
	m_y = y;
	return iResult;
}




// IFXUnknown methods

//---------------------------------------------------------------------------
//  CIFXGlyphMoveToBlock::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXGlyphMoveToBlock::AddRef(void) {
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXGlyphMoveToBlock::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXGlyphMoveToBlock::Release(void) {
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
//  CIFXGlyphMoveToBlock::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphMoveToBlock::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXGlyphMoveToBlock )
			*ppInterface = ( IFXGlyphMoveToBlock* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXGlyph2DCommands )
			*ppInterface = ( IFXGlyph2DCommands* ) this;

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
//  CIFXGlyphMoveToBlock_Factory (non-singleton)
//
//  This is the CIFXGlyphMoveToBlock component factory function.  The
//  CIFXGlyphMoveToBlock component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXGlyphMoveToBlock_Factory( IFXREFIID interfaceId,
									   void**    ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXGlyphMoveToBlock  *pComponent = new CIFXGlyphMoveToBlock;

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
//****************************************************************
//  IID_IFXGlyphLineToBlock
//****************************************************************

// IFXGlyph2DCommands methods

CIFXGlyphLineToBlock::CIFXGlyphLineToBlock() {
	m_uRefCount = 0;
	m_eType = IGG_TYPE_LINETO;
	m_x = 0;
	m_y = 0;
}
CIFXGlyphLineToBlock::~CIFXGlyphLineToBlock() {
}
//---------------------------------------------------------------------------
//  CIFXGlyphLineToBlock::GetType
//
//  This method returns the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphLineToBlock::GetType(EGLYPH_TYPE* pType) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pType)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pType = m_eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphLineToBlock::GetType
//
//  This method sets the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphLineToBlock::SetType(const EGLYPH_TYPE eType)
{
	IFXRESULT iResult = IFX_OK;

	m_eType = eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphLineToBlock::GetAttributes
//
//  This method returns the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphLineToBlock::GetAttributes(U32* pAttr) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pAttr)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pAttr = m_attr;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphLineToBlock::SetAttributes
//
//  This method sets the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphLineToBlock::SetAttributes(const U32 attr)
{
	IFXRESULT iResult = IFX_OK;

	m_attr = attr;

	return iResult;
}

IFXRESULT CIFXGlyphLineToBlock::GetData(F64* px, F64* py) const
{

	IFXRESULT iResult = IFX_OK;

	if(NULL == px || NULL == py )
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		*px = m_x;
		*py = m_y;
	}

	return iResult;
}

IFXRESULT CIFXGlyphLineToBlock::SetData(const F64 x, const F64 y)
{
	IFXRESULT iResult = IFX_OK;

	m_x = x;
	m_y = y;

	return iResult;
}


// IFXUnknown methods

//---------------------------------------------------------------------------
//  CIFXGlyphLineToBlock::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXGlyphLineToBlock::AddRef(void) {
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXGlyphLineToBlock::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXGlyphLineToBlock::Release(void) {
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
//  CIFXGlyphLineToBlock::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphLineToBlock::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXGlyphLineToBlock )
			*ppInterface = ( IFXGlyphLineToBlock* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXGlyph2DCommands )
			*ppInterface = ( IFXGlyph2DCommands* ) this;
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
//  CIFXGlyphLineToBlock_Factory (non-singleton)
//
//  This is the CIFXGlyphLineToBlock component factory function.  The
//  CIFXGlyphLineToBlock component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXGlyphLineToBlock_Factory( IFXREFIID interfaceId,
									   void**    ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXGlyphLineToBlock  *pComponent = new CIFXGlyphLineToBlock;

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
//****************************************************************
//  IID_IFXGlyphCurveToBlock
//****************************************************************

// IFXGlyph2DCommands methods

CIFXGlyphCurveToBlock::CIFXGlyphCurveToBlock() {
	m_uRefCount = 0;
	m_eType = IGG_TYPE_CURVETO;
	m_Cx1 = 0;
	m_Cy1 = 0;
	m_Cx2 = 0;
	m_Cy2 = 0;
	m_Ax = 0;
	m_Ay = 0;
	m_uNumberOfCurveSteps = 0;
}
CIFXGlyphCurveToBlock::~CIFXGlyphCurveToBlock() {
}
//---------------------------------------------------------------------------
//  CIFXGlyphCurveToBlock::GetType
//
//  This method returns the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphCurveToBlock::GetType(EGLYPH_TYPE* pType) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pType)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pType = m_eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphCurveToBlock::GetType
//
//  This method sets the type of tag
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphCurveToBlock::SetType(const EGLYPH_TYPE eType)
{
	IFXRESULT iResult = IFX_OK;

	m_eType = eType;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphCurveToBlock::GetAttributes
//
//  This method returns the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphCurveToBlock::GetAttributes(U32* pAttr) const
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pAttr)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
		*pAttr = m_attr;

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXGlyphCurveToBlock::SetAttributes
//
//  This method sets the attributes of command
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphCurveToBlock::SetAttributes(const U32 attr)
{
	IFXRESULT iResult = IFX_OK;

	m_attr = attr;

	return iResult;
}


IFXRESULT CIFXGlyphCurveToBlock::GetData(F64* pCx1, F64* pCy1, F64* pCx2, F64* pCy2, F64* pAx, F64* pAy, U32* pNumberOfCurveSteps) const
{

	IFXRESULT iResult = IFX_OK;

	if(NULL == pCx1 || NULL == pCy1 || NULL == pCx2
		|| NULL == pAx || NULL == pAy || NULL == pNumberOfCurveSteps)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		*pCx1 = m_Cx1;
		*pCy1 = m_Cy1;
		*pCx2 = m_Cx2;
		*pCy2 = m_Cy2;
		*pAx = m_Ax;
		*pAy = m_Ay;
		*pNumberOfCurveSteps = m_uNumberOfCurveSteps;
	}

	return iResult;
}

IFXRESULT CIFXGlyphCurveToBlock::SetData(const F64 fCx1, const F64 fCy1, const F64 fCx2, const F64 fCy2, const F64 fAx,
										 const F64 fAy, const U32 uNumberOfCurveSteps)
{
	IFXRESULT iResult = IFX_OK;

	m_Cx1 = fCx1;
	m_Cy1 = fCy1;
	m_Cx2 = fCx2;
	m_Cy2 = fCy2;
	m_Ax = fAx;
	m_Ay = fAy;
	m_uNumberOfCurveSteps = uNumberOfCurveSteps;
	return iResult;
}




// IFXUnknown methods

//---------------------------------------------------------------------------
//  CIFXGlyphCurveToBlock::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXGlyphCurveToBlock::AddRef(void) {
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXGlyphCurveToBlock::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXGlyphCurveToBlock::Release(void) {
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
//  CIFXGlyphCurveToBlock::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphCurveToBlock::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXGlyphCurveToBlock )
			*ppInterface = ( IFXGlyphCurveToBlock* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXGlyph2DCommands )
			*ppInterface = ( IFXGlyph2DCommands* ) this;

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
//  CIFXGlyphCurveToBlock_Factory (non-singleton)
//
//  This is the CIFXGlyphCurveToBlock component factory function.  The
//  CIFXGlyphCurveToBlock component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXCurveToBlock_Factory( IFXREFIID interfaceId,
								   void**    ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXGlyphCurveToBlock *pComponent = new CIFXGlyphCurveToBlock;

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
