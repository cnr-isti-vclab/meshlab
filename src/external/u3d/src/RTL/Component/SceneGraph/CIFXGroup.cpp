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
/*
@file  CIFXGroup.cpp                                                         */

#include "IFXSceneGraphPCH.h"
#include "CIFXGroup.h"
#include "IFXModifierDataPacket.h"

#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXException.h"
#include "IFXDids.h"


#include "IFXExportingCIDs.h"

CIFXGroup::CIFXGroup()
{
}


CIFXGroup::~CIFXGroup()
{
}


IFXRESULT IFXAPI_CALLTYPE CIFXGroup_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXGroup *pGroup = new CIFXGroup;

		if ( pGroup )
		{
			// Perform a temporary AddRef for our usage of the component.
			pGroup->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pGroup->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pGroup->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXGroup::AddRef()
{
	
	return ++m_uRefCount;
}


U32 CIFXGroup::Release()
{
	
	if (m_uRefCount == 1)
	{
	  	PreDestruct();
		delete this ;
		return 0 ;
	}
	else
		return (--m_uRefCount);
}


IFXRESULT CIFXGroup::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXCollection )
			*ppInterface = ( IFXCollection* ) this;
		else if ( interfaceId == IID_IFXNode )
			*ppInterface = ( IFXNode* ) this;
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
void CIFXGroup::Counter(EIFXNodeCounterType type, U32* pOutCount)
{
	if ( pOutCount )
	{
		U32 childCount = m_pChildren.GetNumberElements();
		while (childCount) 
		{
			m_pChildren[--childCount]->Counter(type, pOutCount);
		}
		if ((type == IFX_NODE_COUNTER_NODES) ||
			(type == IFX_NODE_COUNTER_GROUPS))
			(*pOutCount)++;
	}
	else
		IFXTRACE_GENERIC(
			L"CIFXGroup::Counter() called with an invalid pointer! Line:%d\n", 
			__LINE__);
}


// IFXModifier
const IFXGUID* CIFXGroup::m_scpOutputDIDs[] = 
{ 
	&DID_IFXTransform
};


IFXRESULT CIFXGroup::GetOutputs( IFXGUID**& rpOutOutputs, 
	                             U32&       rOutNumberOfOutputs,
	                             U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;
	
	return IFX_OK;
}


IFXRESULT CIFXGroup::GetDependencies( IFXGUID*   pInOutputDID,
	                              IFXGUID**& rppOutInputDependencies,
	                              U32&       rOutNumberInputDependencies,
	                              IFXGUID**& rppOutOutputDependencies,
	                              U32&       rOutNumberOfOutputDependencies,
	                              U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if (pInOutputDID == &DID_IFXTransform)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else 
	{
		IFXASSERTBOX( 1, "CIFXGroup::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXGroup::GenerateOutput( U32    inOutputDataElementIndex, 
                                     void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	result = CIFXNode::GenerateOutput( inOutputDataElementIndex, rpOutData, rNeedRelease );

	return result;
}

	
IFXRESULT CIFXGroup::SetDataPacket(
                      IFXModifierDataPacket* pInInputDataPacket,
                      IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = CIFXNode::SetDataPacket(pInInputDataPacket, pInDataPacket);
	
	return result;
}

IFXRESULT CIFXGroup::Notify( IFXModifierMessage eInMessage,
									void* pMessageContext )
{
	return IFX_OK;
}

// IFXMarkerX interface...
void CIFXGroup::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXGroupNodeEncoder, rpEncoderX);
}

