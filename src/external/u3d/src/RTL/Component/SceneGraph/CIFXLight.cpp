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
	@file	CIFXLight.cpp
			
			Implementation of IFXLight interface.
*/

#include "IFXSceneGraphPCH.h"
#include "CIFXLight.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXExportingCIDs.h"

CIFXLight::CIFXLight()
{
	m_lightResourceID = 0;
}


CIFXLight::~CIFXLight()
{
}


IFXRESULT IFXAPI_CALLTYPE CIFXLight_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXLight *pLight = new CIFXLight;

		if ( pLight )
		{
			// Perform a temporary AddRef for our usage of the component.
			pLight->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pLight->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pLight->Release();
		}
		else 
			result = IFX_E_OUT_OF_MEMORY;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXLight::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXLight::Release()
{
	if (m_uRefCount == 1)
	{
		PreDestruct();
		delete this ;
		return 0 ;
	}
	else return (--m_uRefCount);
}


IFXRESULT CIFXLight::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXLight )
			*ppInterface = ( IFXLight* ) this;
		else if ( interfaceId == IID_IFXCollection )
			*ppInterface = ( IFXCollection* ) this;
		else if ( interfaceId == IID_IFXSpatial )
			*ppInterface = ( IFXSpatial* ) this;
		else if ( interfaceId == IID_IFXNode )
			*ppInterface = ( IFXNode* ) this;
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXModifier)
			*ppInterface = ( IFXModifier* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXObserver )
			*ppInterface = ( IFXObserver* ) this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = ( IFXMetaDataX* ) this;
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


// Node
void CIFXLight::Counter(EIFXNodeCounterType type, U32* pOutCount)
{
	if ( pOutCount )
	{
		U32 childCount = m_pChildren.GetNumberElements();
		while (childCount) 
		{
			m_pChildren[--childCount]->Counter(type, pOutCount);
		}
		if ((type == IFX_NODE_COUNTER_NODES) ||
			(type == IFX_NODE_COUNTER_LIGHTS))
			(*pOutCount)++;
	}
	else
		IFXTRACE_GENERIC(
			L"CIFXLight::Counter() called with an invalid pointer! Line:%d\n", 
			__LINE__);
}


// IFXMarkerX
void CIFXLight::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXLightNodeEncoder, rpEncoderX);
}

IFXRESULT CIFXLight::GetSpatialBound(IFXVector4& rOutSphere, U32 WorldInstance)
{
	IFXMatrix4x4* pResultantTransform = NULL;
	IFXArray<IFXMatrix4x4> *pAResultantTransform;

	IFXModifierChain* pModifierChain = NULL;
	IFXRESULT result = GetModifierChain( &pModifierChain );

	IFXModifierDataPacket* pDataPacket = NULL;
	if( IFXSUCCESS(result) )
	{
		result = pModifierChain->GetDataPacket( pDataPacket );
		if( IFXSUCCESS(result) )
		{
			result = pDataPacket->
				GetDataElement( m_uMyDataPacketTransformIndex,
				(void**)&pAResultantTransform );

			pResultantTransform = &pAResultantTransform->GetElement(WorldInstance);
		}
	}

	IFXRELEASE( pDataPacket );
	IFXRELEASE( pModifierChain );

	if( IFXSUCCESS(result) )
	{
		IFXLightResource *pLR = GetLightResource();
		if( NULL == pLR )
			result = IFX_E_UNDEFINED;

		if( IFXSUCCESS( result ) )
			rOutSphere.Set( *(pResultantTransform->RawConst()+12),
			*(pResultantTransform->RawConst()+13),
			*(pResultantTransform->RawConst()+14),
			pLR->GetRenderLight().GetRange() );

		IFXRELEASE( pLR );
	}

	return result;
}


IFXINLINE IFXSpatial::eType CIFXLight::GetSpatialType()
{
	IFXSpatial::eType type = IFXSpatial::UNSPECIFIED;

	IFXLightResource *pLR = this->GetLightResource();

	if( pLR )
	{
		IFXRenderLight rL = pLR->GetRenderLight();

		switch ( rL.GetType() )
		{
		case IFX_AMBIENT:
		case IFX_DIRECTIONAL:
			type = IFXSpatial::INFINITE_LIGHT;
			break;
		case IFX_POINT:
		case IFX_SPOT:
			type = IFXSpatial::ATTENUATED_LIGHT;
			break;
		default:
			IFXASSERT(0);
		}
	}
	else
	{
		type = IFXSpatial::ATTENUATED_LIGHT;
	}

	IFXRELEASE( pLR );

	return type;
}


// IFXModifier
const IFXGUID* CIFXLight::m_scpOutputDIDs[] =
{
	&DID_IFXTransform,
};


IFXRESULT CIFXLight::GetOutputs( IFXGUID**& rpOutOutputs,
								U32&       rOutNumberOfOutputs,
								U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;

	return IFX_OK;
}


IFXRESULT CIFXLight::GetDependencies( IFXGUID*   pInOutputDID,
									 IFXGUID**& rppOutInputDependencies,
									 U32&       rOutNumberInputDependencies,
									 IFXGUID**& rppOutOutputDependcies,
									 U32&       rOutNumberOfOutputDependcies,
									 U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if (pInOutputDID == &DID_IFXTransform)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependcies = NULL;
		rOutNumberOfOutputDependcies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXLight::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXLight::GenerateOutput( U32    inOutputDataElementIndex,
									void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	result = CIFXNode::GenerateOutput( inOutputDataElementIndex, rpOutData, rNeedRelease );

	return result;
}


IFXRESULT CIFXLight::SetDataPacket(
								   IFXModifierDataPacket* pInInputDataPacket,
								   IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = CIFXNode::SetDataPacket(pInInputDataPacket, pInDataPacket);

	return result;
}

IFXRESULT CIFXLight::Notify( IFXModifierMessage eInMessage,
									void* pMessageContext )
{
	return IFX_OK;
}


// IFXLight
void CIFXLight::Disable(IFXRenderContext* pRenderContext, U32 uInLightID)
{
	pRenderContext->Disable(IFX_LIGHT0 + uInLightID);
}


void CIFXLight::Enable(IFXRenderContext* pRenderContext, U32* puInLightID, U32 lightInstance)
{
	IFXMatrix4x4* pResultantTransform = NULL;
	IFXArray<IFXMatrix4x4>* pAResultantTransform = NULL;
	IFXLightResource* pLR = GetLightResource();

	if( pLR )
	{
		U8 attributes = pLR->GetAttributes();

		GetWorldMatrices(&pAResultantTransform);

		IFXRenderLight rL = pLR->GetRenderLight();

		pResultantTransform = &pAResultantTransform->GetElement(lightInstance);

		if (pResultantTransform)
			rL.SetWorldMatrix(*pResultantTransform);

		switch ( rL.GetType() )
		{
		case IFX_AMBIENT:
			{
				IFXVector4 globalAmbient;
				pRenderContext->GetGlobalAmbient(globalAmbient);
				globalAmbient.Add(rL.GetDiffuse());
				pRenderContext->SetGlobalAmbient(globalAmbient);
			}
			break;

		case IFX_SPOT:
			rL.SetSpotDecay(attributes & IFXLightResource::SPOTDECAY);
			// fallthough...
		case IFX_DIRECTIONAL:
		case IFX_POINT:
			if (attributes & IFXLightResource::SPECULAR )
				rL.SetSpecular(rL.GetDiffuse());
			else
				rL.SetSpecular(IFXVector4(0,0,0,1));

			pRenderContext->SetLight(IFX_LIGHT0 + *puInLightID, rL);
			pRenderContext->Enable(IFX_LIGHT0 + *puInLightID);
			break;

		default:
			IFXASSERT(0);
		}
		if (puInLightID)
			(*puInLightID)++;
	}

	IFXRELEASE( pLR );
}

IFXLightResource* CIFXLight::GetLightResource( void )
{
	IFXRESULT result = IFX_E_UNDEFINED;

	// clear it out so that it's NULL if the resource doesn't exist:
	IFXLightResource *pLR = NULL;

	IFXPalette* pLightResourcePalette = NULL ;

	if( m_pSceneGraph )
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightResourcePalette);
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXUnknown* pUnknown = NULL ;

	if ((IFXSUCCESS(result)) && (NULL != pLightResourcePalette))
		result = pLightResourcePalette->GetResourcePtr( m_lightResourceID, &pUnknown);

	if ((IFXSUCCESS(result)) && (NULL != pUnknown))
		result = pUnknown->QueryInterface( IID_IFXLightResource, (void**)&pLR );

	IFXRELEASE( pUnknown );
	IFXRELEASE( pLightResourcePalette );

	if( IFXFAILURE( result ) )
		pLR = NULL;

	return pLR;
}


IFXRESULT CIFXLight::GetLightResourceID(U32* pValue)
{
	IFXRESULT result = IFX_E_UNDEFINED;

	if ( pValue )
	{
		*pValue = m_lightResourceID;
		result = IFX_OK;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXLight::SetLightResourceID(U32 value)
{
	m_lightResourceID = value;

	return IFX_OK;
}
