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
@file  CIFXModifierDataPacket.cpp

	The implementation file of the CIFXModifierDataPacket component. 
*/

#include "CIFXModifierDataPacket.h"
#include "CIFXModifier.h"
#include "IFXModifierChain.h"
#include "IFXModifierChainInternal.h"
#include "CIFXModifierDataElementIter.h"

#include "IFXDids.h"


CIFXModifierDataPacket::CIFXModifierDataPacket()
{
	m_uRefCount                 = 0;
	m_uObservedElementCount		= 0;
	m_pState					= NULL;
	m_pModChain					= NULL;
	m_pModChainInternal			= NULL;
}


CIFXModifierDataPacket::~CIFXModifierDataPacket()
{
	IFXASSERT( NULL == m_pModChain );
	IFXASSERT( (U32)-1 == m_ModChainIndex );
}


void CIFXModifierDataPacket::Destruct()
{
	CIFXSubject::PreDestruct();
	
	m_ModChainIndex = (U32)-1;
	m_pModChain = NULL;
	m_pModChainInternal = NULL;

	m_uObservedElementCount = 0;
}


IFXRESULT IFXAPI_CALLTYPE CIFXModifierDataPacket_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if( ppv )
	{
		// Create the CIFXClassName component.
		CIFXModifierDataPacket *pModifierDataPacket =  new CIFXModifierDataPacket;

		if( pModifierDataPacket )
		{
			// Perform a temporary AddRef for our usage of the component.
			pModifierDataPacket->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pModifierDataPacket->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pModifierDataPacket->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


U32 g_uWatchDP = 0x1e;

// IFXUnknown
U32 CIFXModifierDataPacket::AddRef(void) 
{
	
	return ++m_uRefCount;
} 	


U32 CIFXModifierDataPacket::Release(void) 
{
	
	if( 1 == m_uRefCount )
	{
		Destruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
} 	


IFXRESULT CIFXModifierDataPacket::QueryInterface( IFXREFIID interfaceId, 
                                                  void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if( ppInterface )
	{
		if( interfaceId == IID_IFXModifierDataPacketInternal )
			*ppInterface = ( IFXModifierDataPacketInternal* ) this;
		else if( interfaceId == IID_IFXModifierDataPacket )
			*ppInterface = ( IFXModifierDataPacket* ) this;
		else if( interfaceId == IID_IFXDataPacket )
			*ppInterface = ( IFXDataPacket* ) this;
		else if( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this; 
		else if( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS( result ) )
		{
			AddRef();
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataElementAspectBit(
                                                 IFXREFGUID rInDataElementGUID,
                                                 U32&       rOutBitField )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pState);

	IFXDidEntry* pDid = m_pState->m_pDids;
	U32 Count = m_pState->m_NumDataElements;
	IFXDataElementState* pDEState = m_pState->m_pDataElements;

	result = IFX_E_DATAPACKET_ELEMENT_NOT_FOUND;
	U32 index;
	for(  index=0; index<Count; index++ )
	{
		if( pDid[index].Did == rInDataElementGUID )
		{
			if( pDEState[index].AspectBit == 0 )
			{
				if( m_uObservedElementCount < 32 )
				{
					rOutBitField = 1<<m_uObservedElementCount;
					pDEState[index].AspectBit = rOutBitField;
					m_uObservedElementCount++;
					result = IFX_OK;						
				}
				else
				{
					result = IFX_E_DATAPACKET_NO_INTEREST_BIT_PROVIDED;
				}
				return result;
			}
			else
			{
				rOutBitField = pDEState[index].AspectBit;
				result = IFX_OK;						
				return result;
			}
		}
	}

	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataElementIndex( IFXREFGUID rInDataElementGUID,
													   U32&       rOutIndex )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pState);

	IFXDidEntry* pDid = m_pState->m_pDids;
	U32 Count = m_pState->m_NumDataElements;
	IFXDataElementState* pDEState = m_pState->m_pDataElements;
	BOOL found = FALSE;

	U32 index;
	for(  index=0; index<Count&& IFXSUCCESS(result); index++ )
	{
		if( pDid[index].Did == rInDataElementGUID )
		{
			if( pDEState[index].State != IFXDATAELEMENTSTATE_CONSUMED)
			{
				found = TRUE;
				rOutIndex = index;
				return IFX_OK;
			}
			else 
			{
				result = IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED;
			}
		}
	}

	if( !found ) 
	{
		result = IFX_E_DATAPACKET_ELEMENT_NOT_FOUND;
	}

	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataPacketSize( 
                                               U32& rOutNumberOfDataElements )
{
	IFXASSERT(m_pState);
	rOutNumberOfDataElements = m_pState->m_NumDataElements;
	return IFX_OK;
}


IFXRESULT CIFXModifierDataPacket::GetDataElementGUID( 
                                              U32       inDataElementIndex, 
                                              IFXGUID*& rpOutDataElementGUID )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pState);

	if( inDataElementIndex < m_pState->m_NumDataElements )
	{
		rpOutDataElementGUID =  &(m_pState->m_pDids[inDataElementIndex].Did);
	}
	else 
	{
		result = IFX_E_DATAPACKET_INVALID_INDEX;
	}
	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataElement( U32    inDataElementIndex, 
                                                  void** ppOutDataElement )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pState);

	if( !ppOutDataElement )
	{
		result = IFX_E_INVALID_POINTER;
	}
	else
		*ppOutDataElement = NULL;

	if( IFXSUCCESS(result) && m_pState->m_LockedDataElement == inDataElementIndex )
	{
		result = IFX_E_DATAPACKET_LOCKED;
	}

	if( IFXSUCCESS(result) && inDataElementIndex < m_pState->m_NumDataElements )
	{
		IFXDataElementState* pDEState = &(m_pState->m_pDataElements[inDataElementIndex]);
		// if not valid attempt to bring it up to date. 
		if( pDEState->State == IFXDATAELEMENTSTATE_INVALID || 
			pDEState->State == IFXDATAELEMENTSTATE_VALIDATIONFAILED )
		{
			result = m_pModChainInternal->ProcessDependencies(inDataElementIndex, m_ModChainIndex);
		}
	
		if(IFXSUCCESS(result))
		{
			if( pDEState->State == IFXDATAELEMENTSTATE_CONSUMED )
			{
				result = IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED;
			}
			else if( pDEState->State == IFXDATAELEMENTSTATE_VALIDATIONFAILED )
			{
				result = IFX_E_MODIFIERCHAIN_VALIDATION_FAILED;
			}
			else 
			{
				IFXASSERT(pDEState->State == IFXDATAELEMENTSTATE_VALID);
				*ppOutDataElement = pDEState->pValue;
				if (pDEState->bNeedRelease)
					((IFXUnknown*)pDEState->pValue)->AddRef();
			}
		}
	}
	else
	{
		result = IFX_E_DATAPACKET_INVALID_INDEX;
	}
	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataElement(
                                    U32            inDataElementIndex, 
                                    IFXREFIID      desiredInterface,
                                    void**         ppOutDataElement )
{
	IFXUnknown* pUnk = NULL;
	IFXRESULT result = GetDataElement( inDataElementIndex, (void**)&pUnk );
	if( IFXSUCCESS(result) )
	{
		if( pUnk && m_pState->m_pDids[inDataElementIndex].Flags & IFX_DID_UNKNOWN )
		{
			result = pUnk->QueryInterface( desiredInterface, ppOutDataElement );
		}
		else
		{
			result = IFX_E_INVALID_POINTER;
		}
	}
	IFXRELEASE(pUnk);
	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataElementChangeCount( U32  inDataElementIndex, 
															 U32& outChangeCount )
{
	IFXRESULT result = IFX_OK;
	IFXDataElementState* pDEState = NULL;

	IFXASSERT(m_pState);

	if (IFXSUCCESS(result) &&  inDataElementIndex >= m_pState->m_NumDataElements )
	{
		result = IFX_E_DATAPACKET_INVALID_INDEX;
	}

	if(IFXSUCCESS(result) && m_pState->m_LockedDataElement == inDataElementIndex)
	{
		result = IFX_E_DATAPACKET_LOCKED;
	}
	else
	{
		pDEState = &(m_pState->m_pDataElements[inDataElementIndex]);
	}

	if(IFXSUCCESS(result) && pDEState->State == IFXDATAELEMENTSTATE_CONSUMED)
	{
		result = IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED;
	}
	
		
	if(IFXSUCCESS(result) && pDEState->State == IFXDATAELEMENTSTATE_INVALID)
	{ // Ensure validity;
		result = m_pModChainInternal->ProcessDependencies(inDataElementIndex, m_ModChainIndex);
	}

	if(IFXSUCCESS(result))
	{
		outChangeCount = pDEState->ChangeCount;
	}
		
	return result;
}


IFXRESULT CIFXModifierDataPacket::InvalidateDataElement( U32 inInvalidDataElementIndex )
{
	IFXRESULT result = IFX_OK;
	//IFXDataElementState* pDEState = NULL;

	IFXASSERT(m_pState && inInvalidDataElementIndex < m_pState->m_NumDataElements);

	if( IFXSUCCESS(result)  
		&& m_pState->m_pDataElements[inInvalidDataElementIndex].State == IFXDATAELEMENTSTATE_VALID)
	{
		result = m_pModChainInternal->Invalidate(  inInvalidDataElementIndex, 
													m_ModChainIndex );
	}
			
	return result;
}


IFXRESULT CIFXModifierDataPacket::DataElementIsValid( U32   inDataElementIndex,
                                                      BOOL& rbOutIsValid )
{
	IFXDataElementState* pDEState = NULL;
	IFXASSERT(m_pState);

	if( inDataElementIndex < m_pState->m_NumDataElements)
	{
		return IFX_E_DATAPACKET_INVALID_INDEX;
	}
	else
	{
		pDEState = &(m_pState->m_pDataElements[inDataElementIndex]);
	}

	if( pDEState->State == IFXDATAELEMENTSTATE_CONSUMED)
	{
		return IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED;
	}
	
	rbOutIsValid = pDEState->State == IFXDATAELEMENTSTATE_VALID;
	return IFX_OK;
}


IFXRESULT CIFXModifierDataPacket::SetModifierChain(IFXModifierChain* pInModChain,
												   U32 index, 
												   IFXDataPacketState* pInState)
{
	IFXRESULT result = IFX_OK;
	
	m_pModChain = pInModChain;
	if( m_pModChain )
	{
		m_pModChain->QueryInterface(IID_IFXModifierChainInternal, (void**)&m_pModChainInternal);
		m_pModChainInternal->Release();
	}
	m_ModChainIndex = index;
	m_pState = pInState;
	
	return result;
}

IFXRESULT CIFXModifierDataPacket::GetModifierChain(IFXModifierChain** ppOutModChain)
{
	IFXASSERT(m_pModChain);

	if( !ppOutModChain )
	{
		return IFX_E_INVALID_POINTER;
	}

	m_pModChain->AddRef();
	*ppOutModChain = m_pModChain;

	return IFX_OK;	
}

IFXRESULT CIFXModifierDataPacket::GetChainIndex( U32* pOutChainIndex )
{
	if( !m_pModChain )
	{
		return IFX_E_NOT_INITIALIZED;
	}
	if( !pOutChainIndex )
	{
		return IFX_E_INVALID_POINTER;
	}
	*pOutChainIndex = m_ModChainIndex;

	return IFX_OK;
}



// Iterator methods
IFXRESULT CIFXModifierDataPacket::GetIterator( U32 inFlags, 
									IFXModifierDataElementIterator** pOutpItor )
{
	IFXRESULT result = IFX_OK;
	CIFXModifierDataElementIterator* pIterator = NULL;
	if( !pOutpItor )
	{
		result = IFX_E_INVALID_POINTER;
	}

	if( IFXSUCCESS(result) )
	{
		pIterator = new CIFXModifierDataElementIterator;
		if(!pIterator)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( IFXSUCCESS(result) )
	{
		pIterator->Initialize(inFlags, m_pState->m_pDids, 
						m_pState->m_NumDataElements, this);
	}

	if( IFXSUCCESS(result) )
	{
		*pOutpItor = pIterator;
	}

	return result;
}


IFXRESULT CIFXModifierDataPacket::GetDataPacketState(
					IFXDataPacketState** pOutpState,
					IFXIntraDependencies** pOutpIntraDeps)
{
	*pOutpState = m_pState;
	return m_pModChainInternal->GetIntraDeps(pOutpIntraDeps);
}
