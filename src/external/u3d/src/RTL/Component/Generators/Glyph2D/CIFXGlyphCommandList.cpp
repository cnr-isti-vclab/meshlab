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
@file CIFXGlyphCommandList.cpp

This module defines the CIFXGlyphCommandList component
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXCOM.h"
#include "IFXCoreCIDs.h"
#include "CIFXGlyph3DGenerator.h"
#include "IFXCheckX.h"
#include "IFXException.h"
#include "IFXGlyph2DCommands.h"
#include "CIFXGlyphCommandList.h"
#include "IFXExportingCIDs.h"

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

//---------------------------------------------------------------------------
U32 CIFXGlyphCommandList::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXGlyphCommandList::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
IFXRESULT CIFXGlyphCommandList::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXGlyphCommandList )
		{
			*ppInterface = ( IFXGlyphCommandList* ) this;
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
		}
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}



// CIFXGlyphCommandList methods


IFXRESULT CIFXGlyphCommandList::AddTagBlock( IFXGlyph2DCommands::EGLYPH_TYPE comType, F64 x, F64 y )
{
	IFXRESULT result = IFX_OK;
	IFXGlyphTagBlock* pCom = NULL;
	if( !m_pData )
	{
		result = IFXCreateComponent( CID_IFXSimpleList, IID_IFXSimpleList, (void**)&m_pData );
		if( IFXSUCCESS( result ) && m_pData )
			m_pData->Initialize( 1 );
	}
	if( IFXSUCCESS( result ) && m_pData )
	{
		result = IFXCreateComponent( CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock, (void**)&pCom );
	}
	if( IFXSUCCESS( result ) )
	{
		IFXUnknown* pUnknown = NULL;
		U32 idx = 0;
		pCom->SetType( comType );
		pCom->SetData( x, y );
		pCom->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );
		IFXRELEASE( pCom );
		result = m_pData->Add( pUnknown, &idx );
		IFXRELEASE( pUnknown );
	}
	return result;
}

IFXRESULT CIFXGlyphCommandList::AddMoveToBlock( F64 x, F64 y )
{
	IFXRESULT result = IFX_OK;
	IFXGlyphMoveToBlock* pCom = NULL;
	if( !m_pData )
	{
		result = IFXCreateComponent( CID_IFXSimpleList, IID_IFXSimpleList, (void**)&m_pData );
		if( IFXSUCCESS( result ) && m_pData )
			m_pData->Initialize( 1 );
	}
	if( IFXSUCCESS( result ) && m_pData )
	{
		result = IFXCreateComponent( CID_IFXGlyphMoveToBlock, IID_IFXGlyphMoveToBlock, (void**)&pCom );
	}
	if( IFXSUCCESS( result ) )
	{
		IFXUnknown* pUnknown = NULL;
		U32 idx = 0;
		pCom->SetType( IFXGlyph2DCommands::IGG_TYPE_MOVETO );
		pCom->SetData( x, y );
		pCom->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );
		IFXRELEASE( pCom );
		result = m_pData->Add( pUnknown, &idx );
		IFXRELEASE( pUnknown );
	}
	return result;
}

IFXRESULT CIFXGlyphCommandList::AddLineToBlock( F64 x, F64 y )
{
	IFXRESULT result = IFX_OK;
	IFXGlyphLineToBlock* pCom = NULL;
	if( !m_pData )
	{
		result = IFXCreateComponent( CID_IFXSimpleList, IID_IFXSimpleList, (void**)&m_pData );
		if( IFXSUCCESS( result ) && m_pData )
			m_pData->Initialize( 1 );
	}
	if( IFXSUCCESS( result ) && m_pData )
	{
		result = IFXCreateComponent( CID_IFXGlyphLineToBlock, IID_IFXGlyphLineToBlock, (void**)&pCom );
	}
	if( IFXSUCCESS( result ) )
	{
		IFXUnknown* pUnknown = NULL;
		U32 idx = 0;
		pCom->SetType( IFXGlyph2DCommands::IGG_TYPE_LINETO );
		pCom->SetData( x, y );
		pCom->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );
		IFXRELEASE( pCom );
		result = m_pData->Add( pUnknown, &idx );
		IFXRELEASE( pUnknown );
	}
	return result;
}

IFXRESULT CIFXGlyphCommandList::AddCurveToBlock( F64 x1, F64 y1, F64 x2, F64 y2, F64 x3, F64 y3, U32 numSteps )
{
	IFXRESULT result = IFX_OK;
	IFXGlyphCurveToBlock* pCom = NULL;
	if( !m_pData )
	{
		result = IFXCreateComponent( CID_IFXSimpleList, IID_IFXSimpleList, (void**)&m_pData );
		if( IFXSUCCESS( result ) && m_pData )
			m_pData->Initialize( 1 );
	}
	if( IFXSUCCESS( result ) && m_pData )
	{
		result = IFXCreateComponent( CID_IFXGlyphCurveToBlock, IID_IFXGlyphCurveToBlock, (void**)&pCom );
	}
	if( IFXSUCCESS( result ) )
	{
		IFXUnknown* pUnknown = NULL;
		U32 idx = 0;
		pCom->SetType( IFXGlyph2DCommands::IGG_TYPE_CURVETO );
		pCom->SetData( x1, y1, x2, y2, x3, y3, numSteps );
		pCom->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );
		IFXRELEASE( pCom );
		result = m_pData->Add( pUnknown, &idx );
		IFXRELEASE( pUnknown );
	}
	return result;
}

IFXRESULT CIFXGlyphCommandList::GetList( IFXSimpleList** ppList )
{
	m_pData->AddRef();
	*ppList = m_pData;
	return IFX_OK;
}


//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************

//---------------------------------------------------------------------------
/**
This is the component constructor.  It doesn't do anything significant.

@note Significant state initialization is done by the RobustConstructor
method if it's available or an interface specific Initialize
method if one's supported.
*/

CIFXGlyphCommandList::CIFXGlyphCommandList()
{
	m_pData = NULL;
	m_refCount = 0;
}

//---------------------------------------------------------------------------
/**
This is the component destructor.  After all references to the component
are released, it performs any necessary cleanup activities.
*/
CIFXGlyphCommandList::~CIFXGlyphCommandList() {

	IFXRELEASE(m_pData);
}

//---------------------------------------------------------------------------
/**
This is the component's constructor that can return a result code.  In
it, allocation of other components or memory can be performed.  It's
called by the factory function only, immediately after the component has
been created and has been AddRef'd.

@note If nothing meaningful happens in this method, it can be removed
from this class (as well as its usage from the factory function).

@return Upon success, IFX_OK is returned.  Otherwise, one of the
following are returned:  @todo: return codes.
*/
IFXRESULT CIFXGlyphCommandList::RobustConstructor()
{
	return IFX_OK;
}

//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
This is the CIFXComponentName component factory function.  It's used by
IFXCreateComponent to create an instance of the component and hand back
a pointer to a supported interface that's requested.

@note This component can be instaniated multiple times.

@param  interfaceId Reference to the interface id that's supported by the
component that will be handed back.
@param  ppInterface Pointer to the interface pointer that will be
initialized upon successful creation of the
component.

@return Upon success, IFX_OK is returned.  Otherwise, one of the
following failures are returned:  IFX_E_COMPONENT,
IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or
IFX_E_UNSUPPORTED.
*/
IFXRESULT IFXAPI_CALLTYPE CIFXGlyphCommandList_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXComponentName component.
		CIFXGlyphCommandList *pComponent = new CIFXGlyphCommandList;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Call the component's robust constructor that can return a
			// result code to indicate success or failure.
			result = pComponent->RobustConstructor();

			// Attempt to obtain a pointer to the requested interface.
			if ( IFXSUCCESS( result ) )
				result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the RobustConstructor or QI fails,
			// this will cause the component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

//***************************************************************************
//  Local functions
//***************************************************************************
