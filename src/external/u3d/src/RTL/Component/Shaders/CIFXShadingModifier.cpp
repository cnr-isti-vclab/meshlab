//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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
@file CIFXShadingModifier.cpp

This module defines the CIFXShadingModifier component.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "CIFXShadingModifier.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXExportingCIDs.h"
#include "IFXAutoRelease.h"
#include "IFXMesh.h"

//***************************************************************************
//  Defines
//***************************************************************************

//***************************************************************************
//  Constants
//***************************************************************************

const IFXGUID* CIFXShadingModifier::m_scpOutputDIDs[] =
{
	&DID_IFXRenderableGroup
};


const IFXGUID* CIFXShadingModifier::m_scpInputDIDs[] =
{
	&DID_IFXRenderableGroup
};

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
U32 CIFXShadingModifier::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
U32 CIFXShadingModifier::Release()
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
IFXRESULT CIFXShadingModifier::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXShadingModifier )
		{
			*ppInterface = ( IFXShadingModifier* ) this;
		}
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = (IFXMarker*)this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = (IFXMarkerX*)this;
		else if ( interfaceId == IID_IFXModifier )
			*ppInterface = (IFXModifier*)this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = (IFXSubject*)this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = ( IFXMetaDataX* ) this;
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


//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void CIFXShadingModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXShadingModifierEncoder, rpEncoderX);
}


IFXRESULT CIFXShadingModifier::GetOutputs( IFXGUID**& rpOutOutputs,
										  U32&       rOutNumberOfOutputs,
										  U32*&    rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = 1;
	rpOutOutputDepAttrs = NULL;
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	return IFX_OK;
}


IFXRESULT CIFXShadingModifier::GetDependencies( IFXGUID*   pInOutputDID,
											   IFXGUID**& rppOutInputDependencies,
											   U32&       rOutNumberInputDependencies,
											   IFXGUID**& rppOutOutputDependencies,
											   U32&       rOutNumberOfOutputDependencies,
											   U32*&    rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;


	if ( pInOutputDID == &DID_IFXRenderableGroup )
	{
		rOutNumberInputDependencies = sizeof(m_scpInputDIDs) / sizeof(IFXGUID*);
		rppOutInputDependencies = (IFXDID**)m_scpInputDIDs;
		rOutNumberOfOutputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXShadingModifier::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXShadingModifier::GenerateOutput(U32 inOutputDataElementIndex, void*& rpOutData, BOOL& rNeedRelease)
{
	IFXRESULT result = IFX_OK;
	if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
	{
		IFXDECLARELOCAL(IFXMeshGroup, pInputMeshGroup);
		IFXDECLARELOCAL(IFXMeshGroup, pOutputMeshGroup);

		result = m_pInputDataPacket->GetDataElement( m_uMeshGroupDataElementIndex,
			IID_IFXMeshGroup,
			(void**)&pInputMeshGroup );
		if (IFXSUCCESS(result)) 
		{
			U32 Size = pInputMeshGroup->GetNumElements();
			result = IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&pOutputMeshGroup);
			if (IFXSUCCESS(result)) 
			{
				U32 i;
				result = pOutputMeshGroup->Allocate( Size );
				if (IFXSUCCESS(result))
				{
					for( i = 0; i < Size; i++ )
					{
						IFXDECLARELOCAL(IFXMesh, pMesh);
						pInputMeshGroup->GetMesh( i, pMesh );
						pOutputMeshGroup->SetMesh( i, pMesh );
						IFXDECLARELOCAL(IFXShaderList, pShaderList);
						EIFXRenderable type = pMesh->GetRenderableType();
						if( ( type == IFXRENDERABLE_ELEMENT_TYPE_MESH && m_attributes & MESH ) ||
							( type == IFXRENDERABLE_ELEMENT_TYPE_LINESET && m_attributes & LINE ) ||
							( type == IFXRENDERABLE_ELEMENT_TYPE_POINTSET && m_attributes & POINT ) ||
							( type == IFXRENDERABLE_ELEMENT_TYPE_GLYPH && m_attributes & GLYPH ) )
							GetElementShaderList(i, &pShaderList );
						if( !pShaderList )
							pInputMeshGroup->GetElementShaderList( i, &pShaderList );
						if( pShaderList )
							pOutputMeshGroup->SetElementShaderList(i, pShaderList );
						IFXRELEASE( pMesh );
					}
				}

				pOutputMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
				rNeedRelease = TRUE;
			}
		}
	}
	return result;
}


IFXRESULT CIFXShadingModifier::SetDataPacket( IFXModifierDataPacket* pInInputDataPacket,
											 IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInDataPacket && pInInputDataPacket)
	{
		pInDataPacket->AddRef();
		m_pModifierDataPacket = pInDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	if( IFXSUCCESS(result) )
		result = pInInputDataPacket->GetDataElementIndex( DID_IFXRenderableGroup,
		m_uMeshGroupDataElementIndex );
	return result;
}


IFXRESULT CIFXShadingModifier::Notify( IFXModifierMessage eInMessage,
									  void* pMessageContext )
{
	return IFX_OK;
}


// IFXShadingModifier
IFXRESULT CIFXShadingModifier::GetEnable( BOOL *pbEnable )
{
	IFXRESULT rc = IFX_OK;

	if (!pbEnable)
		rc = IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(rc))
	{
		*pbEnable = m_bEnabled;
	}

	return rc;
}


IFXRESULT CIFXShadingModifier::SetEnable( BOOL bEnable )
{
	IFXRESULT rc = IFX_OK;
	m_bEnabled = bEnable;
	return rc;
}

// the main functionality
IFXRESULT CIFXShadingModifier::GetElementShaderList( U32 uInElement, IFXShaderList** out_ppShaderList )
{
	IFXRESULT result = IFX_OK;
	IFXASSERT(out_ppShaderList);
	if(uInElement >= m_uNumElements)
	{
		result = IFX_E_INVALID_RANGE;
	}

	if( IFXSUCCESS( result ) )
	{
        *out_ppShaderList = m_ppShaders[uInElement];
		if( m_ppShaders[uInElement] )
			m_ppShaders[uInElement]->AddRef();
	}
	return result;
}

IFXRESULT CIFXShadingModifier::SetElementShaderList( U32 uInElement, IFXShaderList* in_pShaderList, BOOL isValid )
{
	IFXRESULT result = IFX_OK;

	if(uInElement >= m_uNumElements)
	{
		IFXShaderList** ppShaders = m_ppShaders;
		U32 elemOld = m_uNumElements;
		m_uNumElements = uInElement+1;
		m_ppShaders = NULL;
		result = AllocateShaders( ppShaders, TRUE, elemOld );
		if( IFXSUCCESS( result ) )
		{
			IFXShaderList** ppShadersBackup = m_ppShaders;
			m_ppShaders = ppShaders;
			if( m_ppShaders )
				Deallocate(elemOld);
			m_uNumElements = uInElement+1;
			m_ppShaders = ppShadersBackup;
		}
	}

	if( in_pShaderList == NULL && !isValid )
	{
		IFXShaderList** ppShaders = m_ppShaders;
		U32 elemOld = m_uNumElements;
		m_uNumElements = uInElement+1;
		m_ppShaders = NULL;
		result = AllocateShaders( ppShaders, TRUE, m_uNumElements );
		if( IFXSUCCESS( result ) )
		{
			IFXShaderList** ppShadersBackup = m_ppShaders;
			m_ppShaders = ppShaders;
			if( m_ppShaders )
				Deallocate(elemOld);
			m_uNumElements = uInElement+1;
			m_ppShaders = ppShadersBackup;
		}
	}
	else if( isValid )
	{
		IFXRELEASE(m_ppShaders[uInElement]);
		m_ppShaders[uInElement] = in_pShaderList;
		in_pShaderList->AddRef();
	}
	else //!isValid && in_pShaderList
	{
		U32 i = 0;
		for( ; i <= uInElement; i++ )
		{
			IFXRELEASE(m_ppShaders[i]);
			m_ppShaders[i] = in_pShaderList;
			in_pShaderList->AddRef();
		}
	}

	return result;
}

IFXRESULT CIFXShadingModifier::GetShaderDataArray(IFXShaderList*** out_pppShaders)
{

	IFXASSERT(out_pppShaders);
	*out_pppShaders = m_ppShaders;

	return IFX_OK;
}

IFXRESULT CIFXShadingModifier::GetAttributes(U32* pShadingAttributes)
{
	*pShadingAttributes = m_attributes;
	return IFX_OK;
}

IFXRESULT CIFXShadingModifier::SetAttributes(U32 shadingAttributes)
{
	m_attributes = shadingAttributes;
	return IFX_OK;
}

IFXRESULT CIFXShadingModifier::GetSize(U32* size)
{
	*size = m_uNumElements;
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
CIFXShadingModifier::CIFXShadingModifier() :  m_refCount( 0 )
{
	m_bEnabled = IFX_TRUE;
	m_pSceneGraph = NULL;
	m_uMeshGroupDataElementIndex = (U32)-1;
	m_uNumElements = 0;
	m_ppShaders = NULL;
	m_attributes = MESH | LINE | POINT | GLYPH;
}

//---------------------------------------------------------------------------
/**
This is the component destructor.  After all references to the component
are released, it performs any necessary cleanup activities.
*/
CIFXShadingModifier::~CIFXShadingModifier()
{
	Deallocate(m_uNumElements);
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
IFXRESULT CIFXShadingModifier::RobustConstructor()
{
	return IFX_OK;
}



IFXRESULT CIFXShadingModifier::AllocateShaders(IFXShaderList** in_ppData, BOOL in_bCopy, U32 numCopy)
{
	IFXRESULT rVal = IFX_OK;
	U32 i;

	if(!m_ppShaders)
		m_ppShaders = new IFXShaderList*[m_uNumElements];

	for (i = 0; i < m_uNumElements; ++i)
		m_ppShaders[i] = NULL;

	if(in_ppData)
	{
		for(i = 0; i < numCopy; ++i)
		{
			IFXRELEASE(m_ppShaders[i]);
			if(in_bCopy && in_ppData[i])
			{
				IFXCreateComponent(CID_IFXShaderList, IID_IFXShaderList, (void**)&(m_ppShaders[i]));
				m_ppShaders[i]->Copy(in_ppData[i]);
			}
			else
			{
				m_ppShaders[i] = in_ppData[i];
				if( m_ppShaders[i] )
					m_ppShaders[i]->AddRef();
			}
		}
	}
	return rVal;
}

void CIFXShadingModifier::Deallocate( U32 num )
{
	U32 i;
	for( i = 0; i < num; ++i)
	{
		IFXRELEASE(m_ppShaders[i]);
	}
	IFXDELETE_ARRAY(m_ppShaders);
	m_uNumElements = 0;

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
IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifier_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		// Create the CIFXComponentName component.
		CIFXShadingModifier *pComponent = new CIFXShadingModifier;

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
