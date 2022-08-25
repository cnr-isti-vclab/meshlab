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
@file  IFXModifierChainState.cpp

The implementation file of the CIFXModifierChain component.
*/

#include "IFXModifierChainState.h"
#include "IFXModifierChain.h"
#include "IFXModifierChainInternal.h"
#include "IFXModifierDataPacketInternal.h"
#include "IFXModifier.h"
#include "IFXCoreCIDs.h"
#include <memory.h>


IFXDataPacketState::IFXDataPacketState()
{
	m_NumDataElements = 0;
	m_Enabled = FALSE;
	m_LockedDataElement = INVALID_DATAELEMENT_INDEX;
	m_pDids = NULL;
	m_pDataElements = NULL;
	m_pDataPacket = NULL;
	m_pModifier = NULL;
}

IFXDataPacketState::~IFXDataPacketState()
{
	IFXDELETE_ARRAY(m_pDataElements);
	IFXRELEASE(m_pDataPacket);
	IFXRELEASE(m_pModifier);
}

IFXDataElementState::IFXDataElementState()
{
	State = IFXDATAELEMENTSTATE_INVALID;
	AspectBit = 0;
	Pad = 0;
	pValue = NULL;
	bNeedRelease = FALSE;
	ChangeCount = 0;
	Generator = INVALID_DATAPACKET_INDEX;
	m_uInvCount = 0;
	m_uInvAllocated = 0;
	m_pInvSeq = 0;
}

IFXDataElementState::~IFXDataElementState()
{
	if (bNeedRelease)
		((IFXUnknown*)pValue)->Release();
	IFXDELETE_ARRAY(m_pInvSeq);
}


IFXRESULT IFXDataElementState::AddInv(U32 in_ModIdx, U32 in_ElIdx)
{
	// Check For Duplicate
	if( m_pInvSeq )
	{
		U32 i;
		for( i = 0; i < m_uInvCount; ++i)
		{
			if(m_pInvSeq[i].uEIndex == in_ElIdx && m_pInvSeq[i].uMIndex == in_ModIdx)
			{
				return IFX_OK;
			}
		}
	}

	// Grow if Needed
	if( m_uInvCount == m_uInvAllocated )
	{
		IFXDidInvElement* pTmpInvs = new IFXDidInvElement[m_uInvAllocated + IFXDIDINVSEQGROWSIZE];
		if(!pTmpInvs)
		{
			return IFX_E_OUT_OF_MEMORY;
		}
		if(m_pInvSeq)
		{
			memcpy(pTmpInvs, m_pInvSeq, sizeof(IFXDidInvElement) * (m_uInvAllocated));
			IFXDELETE_ARRAY(m_pInvSeq);
		}
		m_pInvSeq = pTmpInvs;
		m_uInvAllocated += IFXDIDINVSEQGROWSIZE;
	}

	m_pInvSeq[m_uInvCount].uEIndex = in_ElIdx;
	m_pInvSeq[m_uInvCount].uMIndex = in_ModIdx;
	++m_uInvCount;

	return IFX_OK;
}



IFXIntraDependencies::IFXIntraDependencies()
{
	Size = 0;
	AllocatedSize = 0;
	pDepElementsList = NULL;
}

IFXIntraDependencies::~IFXIntraDependencies()
{
	IFXDELETE_ARRAY( pDepElementsList );
}

IFXRESULT IFXIntraDependencies::AddDependentElement(U32 in_DepEl, U32 in_Attr)
{
	// Check For Duplicate
	U32 i;
	for( i = 0; i < Size; ++i)
	{
		if(pDepElementsList[i].uEIndex == in_DepEl)
		{
			pDepElementsList[i].uDepAttr |= in_Attr;
			return IFX_OK;
		}
	}

	// Grow if Needed
	if( AllocatedSize == Size )
	{
		sElementDependency* pTmpDepElementsList = new sElementDependency[AllocatedSize + IFXDIDDEPSEQGROWSIZE];
		if(!pTmpDepElementsList)
		{
			return IFX_E_OUT_OF_MEMORY;
		}
		if(pDepElementsList)
		{
			memcpy(pTmpDepElementsList, pDepElementsList,
				sizeof(sElementDependency) * (Size));
			IFXDELETE_ARRAY(pDepElementsList);
		}
		pDepElementsList = pTmpDepElementsList;
		AllocatedSize += IFXDIDDEPSEQGROWSIZE;
	}

	pDepElementsList[Size].uEIndex = in_DepEl;
	pDepElementsList[Size].uDepAttr = in_Attr;
	++Size;

	return IFX_OK;
}

IFXRESULT IFXIntraDependencies::CopyFrom(IFXIntraDependencies* in_pSrc)
{
	Size = in_pSrc->Size;
	AllocatedSize = in_pSrc->AllocatedSize;
	if(AllocatedSize)
	{
		IFXASSERT( NULL == pDepElementsList );
		pDepElementsList = new sElementDependency[AllocatedSize];
		if(!pDepElementsList)
		{
			return IFX_E_OUT_OF_MEMORY;
		}
		if(in_pSrc->pDepElementsList)
		{
			memcpy(pDepElementsList, in_pSrc->pDepElementsList,
				sizeof(sElementDependency) * (Size));
		}
	}
	return IFX_OK;
}

void IFXIntraDependencies::CopyTo(IFXIntraDependencies* in_pSrc)
{
	Size = in_pSrc->Size;
	AllocatedSize = in_pSrc->AllocatedSize;
	IFXASSERT( !pDepElementsList || ( in_pSrc->pDepElementsList != pDepElementsList ) );
	IFXDELETE_ARRAY( pDepElementsList );
	pDepElementsList = in_pSrc->pDepElementsList;

	in_pSrc->Size = 0;
	in_pSrc->AllocatedSize = 0;
	in_pSrc->pDepElementsList = NULL;
}



IFXModifierChainState::IFXModifierChainState()
{
	m_bNeedTime = FALSE;
	m_NumModifiers = 0;
	m_NumDataElements = 0;
	m_NumAllocatedDataElements = 0;
	m_pDids = NULL;
	m_pDepSeq = NULL;

	m_pDidRegistry = NULL;

	m_pDataPacketState = NULL;

	m_pBaseDataPacket = NULL;
	m_pTime = NULL;

	m_pPreviousModifierChain= NULL;
	m_pModChain = NULL;

	m_pTransform = NULL;
}


IFXModifierChainState::~IFXModifierChainState()
{
	Destruct();
}

IFXRESULT IFXModifierChainState::Destruct()
{
	IFXRELEASE(m_pBaseDataPacket);
	m_pDidRegistry = NULL;

	IFXDELETE_ARRAY(m_pDids);
	IFXDELETE_ARRAY(m_pDepSeq);
	IFXDELETE_ARRAY(m_pDataPacketState); // this cleans up the references and allocated data on each state.

	m_NumModifiers = 0;
	m_NumAllocatedDataElements = 0;
	m_bNeedTime = FALSE;
	m_NumDataElements = 0;

	IFXRELEASE(m_pPreviousModifierChain);
	m_pModChain = NULL;

	IFXDELETE(m_pTransform);

	return IFX_OK;
}


IFXRESULT IFXModifierChainState::Initialize(IFXModifierChainInternal* in_pModChain,
											IFXModifierChainInternal* in_pBaseChain,
											IFXModifierDataPacketInternal *in_pOverrideDP,
											U32 in_Size,
											IFXDidRegistry* in_pDidRegistry)
{
	IFXRESULT result = IFX_OK;

	m_pModChain = in_pModChain;
	m_pPreviousModifierChain = in_pBaseChain;
	IFXADDREF(m_pPreviousModifierChain);

	if( m_pPreviousModifierChain && !in_pOverrideDP )
	{
		IFXASSERT( !m_pBaseDataPacket );
		IFXModifierDataPacket* pDp = NULL;
		m_pPreviousModifierChain->GetDataPacket(pDp);
		pDp->QueryInterface(IID_IFXModifierDataPacketInternal, (void**)&m_pBaseDataPacket);
		IFXRELEASE(pDp);
	}
	else if( in_pOverrideDP )
	{
		m_pBaseDataPacket = in_pOverrideDP;
		m_pBaseDataPacket->AddRef();
	}

	m_pDidRegistry = in_pDidRegistry;

	if( IFXSUCCESS(result) )
	{
		m_NumModifiers = in_Size+1;

		// Allocate the DataPacket States
		IFXASSERT(!m_pDataPacketState);
		m_pDataPacketState = new IFXDataPacketState[m_NumModifiers];
		if(!m_pDataPacketState)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	// Do Partial initialization of the states.
	if( IFXSUCCESS(result) )
	{

		U32 i;
		for( i = 0; i < m_NumModifiers; ++i)
		{
			IFXModifierDataPacketInternal* pDataPacket = NULL;
			result = IFXCreateComponent( CID_IFXModifierDataPacket,
				IID_IFXModifierDataPacketInternal,
				(void**) &(pDataPacket) );

			if(IFXSUCCESS(result))
			{
				// set up the proxy data packet
				result = pDataPacket->SetModifierChain( in_pModChain,
					i - 1,
					m_pDataPacketState + i);
			}

			if(IFXSUCCESS(result))
			{
				m_pDataPacketState[i].m_pDataPacket = pDataPacket;
			}
			else
			{
				IFXRELEASE(pDataPacket);
			}
		}
	}

	if(IFXFAILURE(result))
	{
		Destruct();
	}

	return result;
}

IFXRESULT IFXModifierChainState::SetModifier(U32 in_Idx, IFXModifier* in_pMod,
											 BOOL in_bEnabled)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(in_Idx < m_NumModifiers);

	if(IFXSUCCESS(result) && in_pMod)
	{
		m_pDataPacketState[in_Idx].m_pModifier = in_pMod;
		m_pDataPacketState[in_Idx].m_Enabled  = in_bEnabled;
		if(in_pMod)
			in_pMod->AddRef();
	}

	return result;
}

IFXRESULT IFXModifierChainState::GetModifier(U32  in_Idx,
											 IFXModifier** out_ppMod)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(in_Idx < m_NumModifiers);

	if(IFXSUCCESS(result))
	{
		*out_ppMod = m_pDataPacketState[in_Idx].m_pModifier;
		(*out_ppMod)->AddRef();
	}

	return result;
}

IFXRESULT IFXModifierChainState::GetModifierDataPacket(U32  in_Idx,
													   IFXModifierDataPacket** out_ppModDP)
{
	IFXRESULT result = IFX_OK;

	if(in_Idx > (m_NumModifiers - 1))
	{
		result = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(result))
	{
		result = m_pDataPacketState[in_Idx].m_pDataPacket->QueryInterface(
			IID_IFXModifierDataPacket,
			(void**)out_ppModDP);
	}

	return result;

}

IFXRESULT IFXModifierChainState::SetActive()
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pDataPacketState);
	IFXModifier* pMod = NULL;

	if(m_NumModifiers > 1)
	{
		pMod = m_pDataPacketState[1].m_pModifier;
		if(pMod)
		{
			if(m_pDataPacketState[1].m_Enabled )
			{

				pMod->SetModifierChain( m_pModChain, 0 );
				result = pMod->SetDataPacket( GetBaseDataPacketNR(),
					m_pDataPacketState[1].m_pDataPacket);
			}
			else
			{
				pMod->SetModifierChain( NULL, (U32)-1 );
				pMod->SetDataPacket( NULL, NULL );
			}
		}
	}

	U32 stage;
	for(  stage=2; stage<m_NumModifiers && IFXSUCCESS(result); stage++ )
	{
		pMod = m_pDataPacketState[stage].m_pModifier;
		if( pMod )
		{
			if(m_pDataPacketState[stage].m_Enabled )
			{
				pMod->SetModifierChain( m_pModChain, stage-1 );
				result = pMod->SetDataPacket( m_pDataPacketState[stage-1].m_pDataPacket, // lod setdp failing
					m_pDataPacketState[stage].m_pDataPacket);
			}
			else
			{
				pMod->SetModifierChain( NULL, (U32)-1 );
				pMod->SetDataPacket( NULL, NULL );
			}
		}
	}
	return result;
}


IFXRESULT IFXModifierChainState::NotifyActive()
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pDataPacketState);
	U32 stage;
	for (  stage=1; stage<m_NumModifiers && IFXSUCCESS(result)
		; stage++ )
	{
		if(m_pDataPacketState[stage].m_Enabled )
		{
			m_pDataPacketState[stage].m_pModifier->Notify(IFXModifier::NEW_MODCHAIN_STATE, NULL);
		}
	}

	return result;
}


IFXRESULT IFXModifierChainState::Build(BOOL in_bReqValidation)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pDataPacketState);

	// 1. Set up the proxy data packet
	if(IFXSUCCESS(result))
	{
		result = BuildProxyDataPacket();
	}
	IFXASSERT(IFXSUCCESS(result));

	// 2. Iterate the Modifiers and attempt to build them
	if(IFXSUCCESS(result))
	{
		U32 i;
		for( i = 1; i < m_NumModifiers; ++i)
		{
			result = BuildModifierDataPacket(i, in_bReqValidation);
			IFXASSERT(IFXSUCCESS(result));
		}
	}

	// 3. Add the invalidation sequence to make the last
	// data packet trigger forwarding invalidations to the
	// appended chains.
	if(IFXSUCCESS(result))
	{
		result = AddAppendedChainInvSeq();
	}

	return result;
}


IFXRESULT IFXModifierChainState::BuildProxyDataPacket()
{
	IFXRESULT result = IFX_OK;
	// "external copy"

	// if we have an m_pBaseDataPacket (prepended mod chain) -- Add all the elements from that
	// else just add time;
	if( m_pBaseDataPacket )
	{
		U32 NumDids = 0;
		IFXDataPacketState* pState = NULL;
		IFXDidEntry* pDids = NULL;
		IFXIntraDependencies* pDepSeq = NULL;

		// Copy All of the Dids Forward
		result = m_pBaseDataPacket->GetDataPacketState( &pState, &pDepSeq );

		if(IFXSUCCESS(result))
		{
			NumDids = pState->m_NumDataElements;
			pDids = pState->m_pDids;
			// cause dids array & outputs to be set to specific size...
			if(!GrowDids(NumDids))
			{
				result = IFX_E_OUT_OF_MEMORY;
			}
		}

		// Iteration & set important state
		if(IFXSUCCESS(result))
		{
			memcpy(m_pDids, pDids, sizeof(IFXDidEntry) * NumDids);
			m_NumDataElements = NumDids;

			m_pDataPacketState[0].m_NumDataElements = NumDids;
			m_pDataPacketState[0].m_Enabled  = TRUE;

			// Copy Consumed State
			IFXDataElementState* pDEState = new IFXDataElementState[NumDids];
			IFXASSERT( NULL == m_pDataPacketState[0].m_pDataElements );
			m_pDataPacketState[0].m_pDataElements = pDEState;
			IFXDataElementState* pSrcDEState = pState->m_pDataElements;
			U32 i;
			for( i = 0; i < NumDids; ++i)
			{
				pDEState[i].State = IFXDATAELEMENTSTATE_INVALID;
				if (pDEState[i].bNeedRelease && pDEState[i].pValue)
					((IFXUnknown*)pDEState[i].pValue)->Release();
				pDEState[i].bNeedRelease = pSrcDEState[i].bNeedRelease;
				pDEState[i].pValue = pSrcDEState[i].pValue;
				if (pDEState[i].bNeedRelease)
					((IFXUnknown*)pDEState[i].pValue)->AddRef();
				pDEState[i].ChangeCount = pSrcDEState[i].ChangeCount;
				pDEState[i].Generator = PROXY_DATAPACKET_INDEX;

				// Copy the DepSeq
				m_pDepSeq[i].CopyFrom(pDepSeq+i);
			}
		}
	}
	else
	{
		// Hardcode 1st elements:  0 = simtime, 1 = xform
		IFXASSERT( 0 == TIME_ELEMENT_INDEX );
		IFXASSERT( 1 == TRANSFORM_ELEMENT_INDEX );

		if(INVALID_DATAELEMENT_INDEX == AppendDid(DID_IFXSimulationTime, 0))
		{
			result = IFX_E_OUT_OF_MEMORY;
		}

		if(INVALID_DATAELEMENT_INDEX == AppendDid(DID_IFXTransform, 0))
		{
			result = IFX_E_OUT_OF_MEMORY;
		}

		IFXDataElementState* pDEState = new IFXDataElementState[2];
		IFXASSERT( NULL == m_pDataPacketState[0].m_pDataElements );
		m_pDataPacketState[0].m_pDataElements = pDEState;
		pDEState[0].State = IFXDATAELEMENTSTATE_INVALID;
		pDEState[0].pValue = NULL;
		pDEState[0].bNeedRelease = FALSE;
		pDEState[0].ChangeCount = 0;
		pDEState[0].Generator = 0;

		IFXDELETE( m_pTransform );
		m_pTransform = new IFXArray<IFXMatrix4x4>;
		IFXASSERT( NULL != m_pTransform );
		m_pTransform->CreateNewElement();
		m_pTransform->GetElement(0).MakeIdentity();

		pDEState[1].State = IFXDATAELEMENTSTATE_VALID;
		pDEState[1].pValue = m_pTransform;
		pDEState[1].bNeedRelease = FALSE;
		pDEState[1].ChangeCount = 0;
		pDEState[1].Generator = 0;
	}

	return IFX_OK;
}

// Add Invalidation Sequence to the Appended Chain
IFXRESULT IFXModifierChainState::AddAppendedChainInvSeq()
{
	IFXRESULT result = IFX_OK;
	IFXDataElementState* pDEState = NULL;
	U32 NumDE = 0;
	if( IFXSUCCESS(result) )
	{
		pDEState = m_pDataPacketState[m_NumModifiers-1].m_pDataElements;
		NumDE = m_pDataPacketState[m_NumModifiers-1].m_NumDataElements;
	}

	if( IFXSUCCESS(result) )
	{ // tell the generators of every data element that exists at the end of
		// the Data packet that they need to invalidate appended chains.
		U32 i;
		for(  i = 0; i < NumDE; i++ )
		{
			if( pDEState->State != IFXDATAELEMENTSTATE_CONSUMED )
			{
				U32 GenIdx = (pDEState->Generator == PROXY_DATAPACKET_INDEX) ? 0 : pDEState->Generator;
				IFXASSERT(GenIdx < m_NumModifiers);
				m_pDataPacketState[GenIdx].m_pDataElements[i].AddInv( APPENDED_DATAPACKET_INDEX, i );
			}
			pDEState++;
		}
	}

	return result;
}


IFXRESULT IFXModifierChainState::BuildModifierDataPacket(U32 in_ModIdx,
														 BOOL in_bReqValidation)
{
	IFXRESULT result = IFX_OK;

	IFXGUID** pOutputs               = NULL;
	U32*    upOutputUnchangedAttrs = NULL;
	U32       uOutputCount           = 0;

	IFXGUID** pInputs                = NULL;
	U32       uInputCount            = 0;

	IFXGUID** pOutputDependencies    = NULL;
	U32*    upOutputDependencyAttrs = NULL;
	U32       uOutputDependencyCount = 0;

	U32 o = 0;
	//U32 uOutputIdx = (U32)INVALID_DATAPACKET_INDEX;

	IFXDataPacketState* pDPState = &(m_pDataPacketState[in_ModIdx]);

	IFXModifier* pMod = m_pDataPacketState[in_ModIdx].m_pModifier;

	// Temp data for the duration of this function
	U32* pOutputIndexes = NULL;

	if( !pMod )
	{ // if the modifier is NULL then we just populate the datapacket and escape.
		// this may be the case during loads.
		pDPState->m_NumDataElements = m_pDataPacketState[in_ModIdx-1].m_NumDataElements;
		result = BMDPPopulateDataElements(in_ModIdx);
		return result;
	}

	// 1. Get the output list from the Modifier
	if( IFXSUCCESS(result) )
	{
		result = pMod->GetOutputs( pOutputs, uOutputCount,
			upOutputUnchangedAttrs );
	}

	// 2. verify all of the inputs are satisfied
	if( IFXSUCCESS(result) )
	{
		result = BMDPVerifyInputs( in_ModIdx, pMod, pOutputs, uOutputCount );
		if( IFXFAILURE(result) )
		{
			// if Req Validation -- pass on failure
			// else not req Validation Mark this as disabled
			// and move along.
			// the logic is as follows: in validation is not required,
			// no problem, if validation is required and a modifier
			// that was previously enabled becomes disabled, then
			// it is and error. However if a previously disabled
			// modifier becomes enabled then all is good.
			if( !in_bReqValidation || pDPState->m_Enabled  == FALSE )
			{
				pDPState->m_NumDataElements = m_pDataPacketState[in_ModIdx-1].m_NumDataElements;
				result = BMDPPopulateDataElements(in_ModIdx);
				// make sure we're set to disabled
				pDPState->m_Enabled  = FALSE;
				return result;
			}
			return IFX_E_MODIFIERCHAIN_VALIDATION_FAILED;
		}
		else
		{
			pDPState->m_Enabled = TRUE;
		}
	}

	if( IFXSUCCESS(result) )
	{
		pOutputIndexes = new U32[uOutputCount];
		if( pOutputIndexes )
		{
			memset( pOutputIndexes, 0, sizeof(U32) * uOutputCount );
		}
		else
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	// 3. Initialize the DataPacket
	// - Get all of the current Items in to the new Data Packet
	// - Add the Modifier's Output DataElements to the new DataPacket
	//    pOutputs and uOutputCount retreived on previous call
	// - Has Side effect of building the output index array
	if( IFXSUCCESS(result) )
	{
		pDPState->m_NumDataElements = m_pDataPacketState[in_ModIdx-1].m_NumDataElements;
		result = BMDPAddOutputs( in_ModIdx, pOutputs, uOutputCount, pOutputIndexes );
	}
	/// @todo:  for all intra dependent outputs,
	/// call XYXY() = BMDPCollapseDependencies Purge/Cleanse will never

	// 4. Allocate and Copy the DataElementStates
	if( IFXSUCCESS(result) )
	{
		result = BMDPPopulateDataElements( in_ModIdx );
	}

	// 4.5 Configure all of the Outputs
	if( IFXSUCCESS(result) )
	{
		result = BMDPConfigureOutputs( in_ModIdx, uOutputCount,
			pOutputIndexes );
	}


	// 5. Iterate over the  Outputs to build up the inv sequence and the
	// dep seq.

	// 5.1  consume any consumed data elements
	o = uOutputCount;
	while( o-- && IFXSUCCESS(result) )
	{
		if ((*(pOutputs[o]) == DTS_IFXRenderable) || (*(pOutputs[o]) == DTS_IFXBound))
		{
			/// @todo: what we probably should do is iterate all out puts and do
			/// the call below for each renderable output.
		}
		else
		{
			// 5.1.1 now check to see if the generation of this data element
			// causes invalidation of a previously generated dependent data
			// element.
			if( IFXSUCCESS(result) )
			{
				result = BMDPConsumeElements( in_ModIdx, pOutputIndexes[o],
					upOutputUnchangedAttrs ? upOutputUnchangedAttrs[o] : 0 );
			}
		}
	}

	// 5.2 Build up all the validation/invalidation seqs
	o = uOutputCount;
	while( o-- && IFXSUCCESS(result) )
	{

		// 5.2.1 Get the  input dependencies and the output dependencies
		if( IFXSUCCESS(result) )
		{
			result = pMod->GetDependencies( pOutputs[o],
				pInputs,
				uInputCount,
				pOutputDependencies,
				uOutputDependencyCount,
				upOutputDependencyAttrs );
		}

		// 5.2.3 for each input dep of output o, add the input dependencies
		// to the list
		if( IFXSUCCESS(result) )
		{
			result = BMDPScheduleInvalidations( in_ModIdx,
				pOutputIndexes[o], pOutputs[o], pInputs, uInputCount );
		}

		// 5.2.4 foreach output dependency dep of output o,
		//    add the input dependencies to the list
		if( IFXSUCCESS(result) )
		{
			result = BMDPSetOutputDeps( in_ModIdx,
				pOutputIndexes[o],
				pOutputs[o],
				pOutputDependencies,
				uOutputDependencyCount,
				upOutputDependencyAttrs);
		}
	}

	// 6. for each element not generated by this modifier set it as a dependendent on
	// the last modifier that generated it.
	if( IFXSUCCESS(result) )
	{
		result = BMDPScheduleDefaultInvalidations(in_ModIdx);
	}

	IFXDELETE_ARRAY(pOutputIndexes);
	return result;
}


IFXRESULT IFXModifierChainState::BMDPVerifyInputs(U32 in_ModIdx,
												  IFXModifier* pMod,
												  IFXDID** ppOutputs,
												  U32 NumOutputs)
{
	IFXRESULT result = IFX_OK;
	IFXGUID** pInputs                = NULL;
	U32       uInputCount            = 0;

	IFXGUID** pOutputDependencies    = NULL;
	U32*    upOutputDependencyAttrs = NULL;
	U32       uOutputDependencyCount = 0;

	U32 i;
	for(  i = 0; i < NumOutputs && IFXSUCCESS(result); ++i)
	{
		result = pMod->GetDependencies( ppOutputs[i],
			pInputs,
			uInputCount,
			pOutputDependencies,
			uOutputDependencyCount,
			upOutputDependencyAttrs );

		if( IFXSUCCESS(result) )
		{
			U32 j;
			for(  j = 0; j < uInputCount; ++j )
			{
				U32 uDidIndex = GetDidIndex(*(pInputs[j]), in_ModIdx-1);

				if( INVALID_DATAPACKET_INDEX == uDidIndex )
				{
					result = IFX_E_DATAPACKET_ELEMENT_NOT_FOUND;
					break;
				}
				// need to check if this is consumed
				else if( m_pDataPacketState[in_ModIdx-1].m_pDataElements[uDidIndex].State == IFXDATAELEMENTSTATE_CONSUMED )
				{
					/// @todo: clean up these err codes - change to IFX_E_DATAPACKET...
					result = IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED;
					break;
				}
			}
		}
	}

	return result;
}

IFXRESULT IFXModifierChainState::BMDPAddOutputs( U32 in_ModIdx,
												IFXDID** in_ppOutputs,
												U32 in_uNumOutputs,
												U32* pOutputIndices )
{
	IFXRESULT result = IFX_OK;
	U32 o = in_uNumOutputs;

	while( o-- && IFXSUCCESS(result) )
	{
		if ( (DTS_IFXRenderable == *(in_ppOutputs[o])) || (DTS_IFXBound == *(in_ppOutputs[o])) )
		{
			pOutputIndices[o] = INVALID_DATAELEMENT_INDEX;
		}
		else
		{
			pOutputIndices[o] = GetDidIndex( *(in_ppOutputs[o]), in_ModIdx );
			if( pOutputIndices[o] == INVALID_DATAELEMENT_INDEX )
			{
				pOutputIndices[o] = AppendDid(*(in_ppOutputs[o]), in_ModIdx);
				if( INVALID_DATAELEMENT_INDEX == pOutputIndices[o] )
				{
					result = IFX_E_OUT_OF_MEMORY;
				}
			}
		}
	}

	return result;
}

// Create a New Array of Data Element States for the DataPacket State of this modifier
IFXRESULT IFXModifierChainState::BMDPPopulateDataElements(U32 in_ModIdx)
{
	IFXDataPacketState*  pDPState = &(m_pDataPacketState[in_ModIdx]);

	IFXDataElementState* pDE = new IFXDataElementState[pDPState->m_NumDataElements];
	if( !pDE )
	{
		return IFX_E_OUT_OF_MEMORY;
	}

	IFXDELETE_ARRAY( pDPState->m_pDataElements );
	pDPState->m_pDataElements = pDE;

	U32 NumSrcDE = m_pDataPacketState[in_ModIdx-1].m_NumDataElements;
	IFXDataElementState* pSrcDE = m_pDataPacketState[in_ModIdx-1].m_pDataElements;

	U32 i;
	for(  i = 0; i < NumSrcDE; i++ )
	{
		pDE[i].Generator = pSrcDE[i].Generator;
		pDE[i].ChangeCount = pSrcDE[i].ChangeCount;
		pDE[i].State = pSrcDE[i].State;
		if (pDE[i].bNeedRelease && pDE[i].pValue)
			((IFXUnknown*)pDE[i].pValue)->Release();
		pDE[i].bNeedRelease = pSrcDE[i].bNeedRelease;
		pDE[i].pValue = pSrcDE[i].pValue;
		if (pDE[i].bNeedRelease)
			((IFXUnknown*)pDE[i].pValue)->AddRef();
	}

	return IFX_OK;
}


IFXRESULT IFXModifierChainState::BMDPConfigureOutputs( U32 in_ModIdx,
													  U32 in_uNumOutputs,
													  U32* pOutputIndices)
{
	IFXRESULT result = IFX_OK;
	U32 o = in_uNumOutputs;

	IFXDataElementState* pDEStates = m_pDataPacketState[in_ModIdx].m_pDataElements;

	while( o-- && IFXSUCCESS(result) )
	{
		// if not all renderables
		if( pOutputIndices[o] != INVALID_DATAELEMENT_INDEX)
			// this is a performance Hack See in BMDPAddOutputs
			// where only DID_RENDERABLE are set to this value
		{
			// Iterate over all of the outputs one more time and up date
			// the generating Modifier and validation state - additionally
			// mark any dataelements that are possibly consumed.
			pDEStates[pOutputIndices[o]].State = IFXDATAELEMENTSTATE_INVALID;
			pDEStates[pOutputIndices[o]].Generator = in_ModIdx;
		}
		else
		{ // if all renderables

			U32 NumElements = m_pDataPacketState[in_ModIdx-1].m_NumDataElements;
			IFXDataElementState* pSrcDEStates =
				m_pDataPacketState[in_ModIdx-1].m_pDataElements;

			// iter all of the date elements and make the renderables outputs of this
			// modifier chain
			U32 i;
			for( i = 0; i < NumElements; ++i)
			{
				if(((m_pDids[i].Flags & IFX_DID_RENDERABLE) || ((m_pDids[i].Flags & IFX_DID_BOUND)))
					&& pSrcDEStates[i].State != IFXDATAELEMENTSTATE_CONSUMED)
				{
					pDEStates[i].State = IFXDATAELEMENTSTATE_INVALID;
					pSrcDEStates[i].AddInv(in_ModIdx, i);
					pDEStates[i].Generator = in_ModIdx;
				}
			}
		}
	}

	return result;
}

IFXRESULT IFXModifierChainState::BMDPConsumeElements(U32 in_ModIdx,
													 U32 in_OutputIdx,
													 U32 in_UnChangedAttrs)
{
	IFXRESULT result = IFX_OK;

	//  walk thru dependencies that are defined for this element.
	if(IFXSUCCESS(result))
	{
		U32 idx = 0;
		IFXDataElementState* pDEState =
			m_pDataPacketState[in_ModIdx].m_pDataElements;

		// transverse intra dependencies for this output (parallel array)
		IFXIntraDependencies* pOutElDeps = &(m_pDepSeq[in_OutputIdx]);
		// this is the list of "arrows" that contain element index & attributes
		sElementDependency* pElDepsList = pOutElDeps->pDepElementsList;

		U32 i;
		for(  i = 0; i < pOutElDeps->Size; i++ )
		{
			idx = pElDepsList[i].uEIndex;

			// test if any of the dependent sub attributes are being changed
			if( (pElDepsList[i].uDepAttr & in_UnChangedAttrs) != pElDepsList[i].uDepAttr )
			{
				// the Generator is the last modifier to generate this element
				// essentially an optmization to short circuit intermediate
				// modifiers
				// UPD: in case of partial loading of a file - when node with its chain is
				// decoded prior its resource and node has some modifiers that request some
				// data which are contained in resource MC - output of such modifier can be
				// messed up and it can wrongly consume some elements of data packet. This causes failure
				// Of following modifiers. Example: Shading Modifier in Node MC and CLOD Modifier
				// following it. To prevent such behavior the second condition check
				// was added: pDEState[idx].Generator != PROXY_DATAPACKET_INDEX
				if( pDEState[idx].Generator != in_ModIdx && pDEState[idx].Generator != PROXY_DATAPACKET_INDEX )
				{
					// we need to remove all dependencies of a previously generated output
					// of a currently generated output in TB written function XYXY(), and then this case should
					// always

					// this dependency has now been violated remove it
					pDEState[idx].State = IFXDATAELEMENTSTATE_CONSUMED;
				}

				// if not last elements,
				// remove this entry from the list.
				if( i != (pOutElDeps->Size - 1) )
				{
					// copy last entry over current entry.
					pOutElDeps->pDepElementsList[i]
					= pElDepsList[pOutElDeps->Size-1];
					// set i to repeat this iteration
					i--;
				}
				pOutElDeps->Size--; // decrement the number
				// of dependent elements
			}
		}
	}

	return result;
}

IFXRESULT IFXModifierChainState::BMDPScheduleInvalidations( U32 in_ModIdx,
														   U32 uOutputIdx,
														   IFXDID* pOutputDid,
														   IFXDID** in_ppInputs,
														   U32 uInputCount )
{
	IFXRESULT result = IFX_OK;

	U32 e = uInputCount, uTmpIndex;
	IFXDataPacketState* pPrevState = &(m_pDataPacketState[in_ModIdx-1]);
	IFXDataElementState* pPrevDE = pPrevState->m_pDataElements;


	while ( e-- && IFXSUCCESS(result) )
	{
		if ( *(in_ppInputs[e]) == DTS_IFXRenderable )
		{ // When a DataTypeSpecifier(DTS) is an input, all dataElements of that type are implied inputs.
			IFXASSERT(!(*pOutputDid == *(in_ppInputs[e])));

			U32 cnt = pPrevState->m_NumDataElements;

			while( cnt-- )
			{
				if ( m_pDids[cnt].Flags & IFX_DID_RENDERABLE
					&& pPrevDE[cnt].State != IFXDATAELEMENTSTATE_CONSUMED )
				{
					// Add the Invalidation link to the input data element.
					IFXASSERT(pPrevDE[cnt].Generator != INVALID_DATAPACKET_INDEX);
					m_pDataPacketState[pPrevDE[cnt].Generator].m_pDataElements[cnt]
					.AddInv(in_ModIdx, uOutputIdx);
				}
			}
		}
		if ( *(in_ppInputs[e]) == DTS_IFXBound )
		{ // When a DataTypeSpecifier(DTS) is an input, all dataElements of that type are implied inputs.
			IFXASSERT(!(*pOutputDid == *(in_ppInputs[e])));

			U32 cnt = pPrevState->m_NumDataElements;

			while( cnt-- )
			{
				if( m_pDids[cnt].Flags & IFX_DID_BOUND
					&& pPrevDE[cnt].State != IFXDATAELEMENTSTATE_CONSUMED )
				{
					// Add the Invalidation link to the input data element.
					IFXASSERT(pPrevDE[cnt].Generator != INVALID_DATAPACKET_INDEX);
					m_pDataPacketState[pPrevDE[cnt].Generator].m_pDataElements[cnt]
					.AddInv(in_ModIdx, uOutputIdx);
				}
			}
		}
		else
		{
			uTmpIndex = GetDidIndex( *(in_ppInputs[e]), in_ModIdx-1 );

			IFXDataElementState* pDEState = &(pPrevDE[uTmpIndex]);
			IFXASSERT(pDEState->Generator != INVALID_DATAPACKET_INDEX);

			// Add the Invalidation link to the input data element.
			U32 GenIdx = pDEState->Generator == PROXY_DATAPACKET_INDEX ? 0 : pDEState->Generator;

			IFXDataElementState* pGenDE =
				&(m_pDataPacketState[GenIdx].m_pDataElements[uTmpIndex]);
			pGenDE->AddInv(in_ModIdx, uOutputIdx);

			if( *(in_ppInputs[e]) == DID_IFXSimulationTime )
			{
				m_bNeedTime = TRUE;
			}
		}
	}

	return result;
}


IFXRESULT IFXModifierChainState::BMDPSetOutputDeps(
	U32 in_ModIdx,
	U32 uOutputIdx,
	IFXDID* pOutputDid,
	IFXDID** ppOutputDependencies,
	U32 uNumOutputDeps,
	U32* upOutputDependencyAttrs)
{
	IFXRESULT result = IFX_OK;
	U32 uTmpIndex = 0;
	U32 e = uNumOutputDeps;

	while( e-- && IFXSUCCESS(result) )
	{
		// Output cannot be dependent on it's self
		IFXASSERT(!(*pOutputDid == *(ppOutputDependencies[e])));

		if( *(ppOutputDependencies[e]) == DTS_IFXRenderable)
		{ // When a DataTypeSpecifier(DTS) is provided, all dataElements of that type are implied output dependencies.
			IFXASSERT(!(*pOutputDid == *(ppOutputDependencies[e])));
			U32 cnt = m_pDataPacketState[in_ModIdx].m_NumDataElements;

			IFXDataElementState* pDEState = m_pDataPacketState[in_ModIdx].m_pDataElements;

			while(cnt--)
			{
				if( m_pDids[cnt].Flags & IFX_DID_RENDERABLE
					&& uOutputIdx != cnt)
				{
					// say that if the Output dependency is invalidated, or changed
					// the dependent output should have the same happen
					m_pDepSeq[cnt].AddDependentElement(uOutputIdx,
						upOutputDependencyAttrs?upOutputDependencyAttrs[e]:0xFFFFFFFF);

					// also add a default invalidation
					pDEState[cnt].AddInv(in_ModIdx, uOutputIdx);
				}
			}
		}
		if( *(ppOutputDependencies[e]) == DTS_IFXBound)
		{ // When a DataTypeSpecifier(DTS) is provided, all dataElements of that type are implied output dependencies.
			IFXASSERT(!(*pOutputDid == *(ppOutputDependencies[e])));
			U32 cnt = m_pDataPacketState[in_ModIdx].m_NumDataElements;

			IFXDataElementState* pDEState = m_pDataPacketState[in_ModIdx].m_pDataElements;

			while(cnt--)
			{
				if( m_pDids[cnt].Flags & IFX_DID_BOUND
					&& uOutputIdx != cnt)
				{
					// say that if the Output dependency is invalidated, or changed
					// the dependent output should have the same happen
					m_pDepSeq[cnt].AddDependentElement(uOutputIdx,
						upOutputDependencyAttrs?upOutputDependencyAttrs[e]:0xFFFFFFFF);

					// also add a default invalidation
					pDEState[cnt].AddInv(in_ModIdx, uOutputIdx);
				}
			}
		}
		else
		{
			uTmpIndex = GetDidIndex(*(ppOutputDependencies[e]), in_ModIdx);
			IFXASSERT(uTmpIndex  != INVALID_DATAELEMENT_INDEX);
			IFXDataElementState* pDEState =
				&(m_pDataPacketState[in_ModIdx].m_pDataElements[uTmpIndex]);

			if (IFXSUCCESS(result))
			{ // "reversing the order of the link:  element x invalidates y, if y depends on x...
				// say that if the Output dependency is invalidated, or changed
				// the dependent output should have the same happen
				m_pDepSeq[uTmpIndex].AddDependentElement(uOutputIdx,
					upOutputDependencyAttrs?upOutputDependencyAttrs[e]:0xFFFFFFFF);

				// also add a default invalidation
				pDEState->AddInv(in_ModIdx, uOutputIdx);
			}
		}
	}

	return result;
}


IFXRESULT IFXModifierChainState::BMDPScheduleDefaultInvalidations(U32 in_ModIdx)
{
	IFXRESULT result = IFX_OK;

	U32 DataElementCount = m_pDataPacketState[in_ModIdx].m_NumDataElements;
	IFXDataElementState* pDEState = m_pDataPacketState[in_ModIdx].m_pDataElements;

	U32 i;
	for(  i = 0; i < DataElementCount; i++ )
	{
		// add this item as an invalidation dep for the last generator of it
		U32 GenIdx = pDEState[i].Generator == PROXY_DATAPACKET_INDEX ? 0 : pDEState[i].Generator;

		IFXASSERT(GenIdx != INVALID_DATAPACKET_INDEX);
		if( GenIdx != in_ModIdx )
		{
			m_pDataPacketState[GenIdx].m_pDataElements[i].AddInv( in_ModIdx, i );
		}
	}

	return result;
}

U32 IFXModifierChainState::GetDidIndex(const IFXDID& in_Did, U32 in_ModIdx)
{
	U32 NumDids = m_pDataPacketState[in_ModIdx].m_NumDataElements;

	U32 i;
	for(  i = 0; i < NumDids; i++ )
	{
		if( m_pDids[i].Did == in_Did )
		{
			return i;
		}
	}

	return INVALID_DATAELEMENT_INDEX;
}

U32 IFXModifierChainState::AppendDid(const IFXDID& in_Did , U32 in_ModIdx)
{
	if(m_NumDataElements == m_NumAllocatedDataElements)
	{
		if(!GrowDids(m_NumAllocatedDataElements+16))
		{
			return INVALID_DATAELEMENT_INDEX;
		}
	}

	m_pDids[m_NumDataElements].Did = in_Did;
	m_pDids[m_NumDataElements].Flags = m_pDidRegistry->GetDidFlags(in_Did);
	m_NumDataElements++;
	m_pDataPacketState[in_ModIdx].m_NumDataElements++;

	return m_NumDataElements-1;
}

BOOL IFXModifierChainState::GrowDids(U32 in_Size)
{
	IFXDidEntry* pNewDids = new IFXDidEntry[in_Size];

	if(!pNewDids)
	{
		return FALSE;
	}

	IFXIntraDependencies* pDepSeq = new IFXIntraDependencies[in_Size];
	if(!pDepSeq)
	{
		IFXDELETE_ARRAY(pNewDids);
		return FALSE;
	}

	if(m_pDids)
	{
		memcpy(pNewDids, m_pDids, sizeof(IFXDidEntry) * m_NumDataElements);
		delete[] m_pDids;
	}

	if(m_pDepSeq)
	{
		U32 i;
		for( i = 0; i < m_NumDataElements; ++i)
		{
			//m_pDepSeq[i].CopyTo(pDepSeq+i);
			pDepSeq[i].CopyTo(&m_pDepSeq[i]);
		}
		delete[] m_pDepSeq;
	}

	m_pDids = pNewDids;
	m_pDepSeq = pDepSeq;
	m_NumAllocatedDataElements = in_Size;

	U32 i;
	for( i = 0; i < m_NumModifiers; ++i)
	{
		m_pDataPacketState[i].m_pDids = m_pDids;
	}

	return TRUE;
}

void IFXModifierChainState::AttachToPrevChain()
{
	if(m_pPreviousModifierChain)
	{
		m_pPreviousModifierChain->AddAppendedModifierChain(m_pModChain);
	}
}

void IFXModifierChainState::DetachFromPrevChain()
{
	if(m_pPreviousModifierChain)
	{
		m_pPreviousModifierChain->RemoveAppendedModifierChain(m_pModChain);
	}
}
