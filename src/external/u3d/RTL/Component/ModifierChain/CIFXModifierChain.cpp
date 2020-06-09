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
	@file	CIFXModifierChain.cpp

			The implementation file of the CIFXModifierChain component. 
*/

#include "CIFXModifierChain.h"
#include "IFXModifierChainState.h"
#include "CIFXModifierDataPacket.h"


static U32 s_changeCount = 1;

// Scratch memory used to accelerate invalidate Data Element
// since determining the exact size that a invalidation statestack
// would have to be is a non trivial calculation we
// grow the invstate stack when needed which should stabilize
// quickly.. first frame after a change in modifier chain states.

struct InvRecord
{
	U32 ModIdx;
	IFXDataElementState* pDEState;
	U32 InvIdx;
};

static InvRecord* s_pInvState = NULL;
static I32 s_invDepth = -1;
static U32 s_invRefCnt = 0;
static I32 s_invSize = 0;

const U32 INV_GROW_SIZE = 16;

static IFXRESULT GrowInvState()
{
	IFXRESULT result = IFX_OK;

	InvRecord* pNewInvState = new InvRecord[s_invSize + INV_GROW_SIZE];

	if( NULL != pNewInvState )
	{
		if(s_pInvState)
		{
			memcpy(pNewInvState, s_pInvState, sizeof(InvRecord) * s_invSize);
			IFXDELETE_ARRAY(s_pInvState);
		}
		s_pInvState = pNewInvState;
		s_invSize += INV_GROW_SIZE;
	}
	else
	{
		result = IFX_E_OUT_OF_MEMORY;
	}

	return result;
}


//-----------------------------------------------------------------------------
// -- CIFXModifierChain
//-----------------------------------------------------------------------------

CIFXModifierChain::CIFXModifierChain()
{
	m_refCount       = 0;

	m_pModChainState    = NULL;

	m_pCachedState      = NULL;
	m_pOldState       = NULL;
	m_bNeedTime       = FALSE;
	m_bInApplyState     = FALSE;

	m_Time          = 0;
	m_pClockNR        = NULL;
	m_pClockSubjectNR   = NULL;

	m_pDidRegistry = NULL;

	++s_invRefCnt;
}


CIFXModifierChain::~CIFXModifierChain()
{
	// last mod chain out cleans up the invalidation stack
	--s_invRefCnt;
	if(s_invRefCnt == 0)
	{
		IFXASSERT(s_invDepth == -1);
		IFXDELETE_ARRAY(s_pInvState);
		s_invSize = 0;
	}
}


void CIFXModifierChain::Destruct()
{
	CIFXSubject::PreDestruct();

	IFXASSERT(m_appendedChains.Size() == 0);

	if( m_pModChainState )
	{
		IFXDataPacketState* pDPState = m_pModChainState->m_pDataPacketState;
		U32 i;
		for( i = 0; i < m_pModChainState->m_NumModifiers; ++i)
		{
			if(pDPState[i].m_pModifier)
			{
				pDPState[i].m_pModifier->SetDataPacket(NULL, NULL);
				pDPState[i].m_pModifier->SetModifierChain(NULL, 0);
			}
			pDPState[i].m_pDataPacket->SetModifierChain(NULL, 0, NULL);
		}
		m_pModChainState->DetachFromPrevChain();
	}

	IFXDELETE( m_pModChainState );
	IFXDELETE( m_pCachedState );
	IFXDELETE( m_pOldState );
	IFXRELEASE( m_pDidRegistry );
}


IFXRESULT IFXAPI_CALLTYPE CIFXModifierChain_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if( ppv )
	{
		// Create the CIFXClassName component.
		CIFXModifierChain *pModifierChain = new CIFXModifierChain;

		if( pModifierChain )
		{
			// Perform a temporary AddRef for our usage of the component.
			pModifierChain->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pModifierChain->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pModifierChain->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown interface support...
U32 CIFXModifierChain::AddRef(void)
{
	return ++m_refCount;
}


U32 CIFXModifierChain::Release(void)
{
	if( m_refCount == 1 )
	{
		Destruct();
		SetClock( NULL );

		delete this;
		return 0;
	}
	else
		return --m_refCount;
}


IFXRESULT CIFXModifierChain::QueryInterface( IFXREFIID interfaceId,
											void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if( ppInterface )
	{
		if( interfaceId == IID_IFXModifierChainInternal )
			*ppInterface = ( IFXModifierChainInternal* ) this;
		else if( interfaceId == IID_IFXModifierChain )
			*ppInterface = ( IFXModifierChain* ) this;
		else if( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if( interfaceId == IID_IFXObserver )
			*ppInterface = ( IFXObserver* ) this;
		else if( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS( result ) )
			AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}


// IFXModifierChain interface...
IFXRESULT CIFXModifierChain::Initialize()
{
	IFXRESULT result = IFX_OK;

	Destruct();

	m_pModChainState = new IFXModifierChainState;
	if( !m_pModChainState )
	{
		result = IFX_E_OUT_OF_MEMORY;
	}

	if(IFXSUCCESS(result))
	{
		result = IFXCreateComponent(CID_IFXDidRegistry, IID_IFXDidRegistry,
			(void**) &m_pDidRegistry);
	}


	if(IFXSUCCESS(result))
	{
		result = m_pModChainState->Initialize(this, NULL, NULL, 0, m_pDidRegistry);
	}


	if(IFXSUCCESS(result))
	{
		result = m_pModChainState->Build( TRUE );
	}

	if(IFXFAILURE(result))
	{
		IFXDELETE(m_pModChainState);
		IFXRELEASE(m_pDidRegistry);
	}

	return result;
}


IFXRESULT CIFXModifierChain::PrependModifierChain( IFXModifierChain* pInModifierChain )
{
	IFXRESULT result = IFX_OK;
	IFXModifierChainState* pNewModChainState = NULL;
	IFXModifierChainInternal* pModChainInt = NULL;

	if( NULL == m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS( result) && (NULL != pInModifierChain) )
	{
		result = pInModifierChain->QueryInterface(
										IID_IFXModifierChainInternal, 
										(void**)&pModChainInt);
	}

	// if the prev mod chain has changed.
	if( (IFXSUCCESS(result)) && (m_pModChainState->GetPrevChainNR() != pModChainInt) )
	{
		// Attempt to Reconfigure the mod chain with this new base chain
		result = BuildNewModifierState(
							pModChainInt, NULL,
							INVALID_DATAPACKET_INDEX, NULL,
							&pNewModChainState, FALSE, FALSE);

		// Apply the changes
		if( IFXSUCCESS(result) )
		{
			result = ApplyNewModifierState(pNewModChainState);
			IFXDELETE(m_pOldState);
		}
		else
		{
			IFXDELETE(pNewModChainState);
			// Back out cached states on appended Mods
			U32 AppendedDPiter = 0;
			IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
			while( ppIntMC )
			{
				result = (*ppIntMC)->ClearOldState();
				ppIntMC = m_appendedChains.Next(AppendedDPiter);
			}
		}
	}

	IFXRELEASE(pModChainInt);

	return result;
}


IFXRESULT CIFXModifierChain::AddModifier( 
								IFXModifier& rInModifier,
								U32 index, 
								BOOL isReqValidation)
{
	IFXRESULT result = IFX_OK;
	IFXModifierChainState* pNewState = NULL;

	IFXASSERT(&rInModifier); // references can be NULL!

	if( !m_pModChainState )
	{
		result = Initialize();
	}

	// Step 1 validate inputs.
	if( IFXSUCCESS(result) )
	{
		if( index == END_OF_MODCHAIN)
		{
			index = m_pModChainState->NumModifiers();
		}

		if( isReqValidation && (index > m_pModChainState->NumModifiers() ) )
		{
			result = IFX_E_INVALID_RANGE;
		}
	}

	// Step 2 Attempt to build a modifier state with the new Modifier
	if( IFXSUCCESS(result) )
	{
		++index; // adjust the index to the correct location
		result = BuildNewModifierState( 
						m_pModChainState->GetPrevChainNR(), NULL,
						index, &rInModifier, &pNewState, FALSE, isReqValidation );
	}

	// Step 3 this all worked; apply the new state
	if( IFXSUCCESS(result) )
	{
		result = ApplyNewModifierState( pNewState );
		IFXDELETE( m_pOldState );
	}
	else // all of this did not work clear the state
	{
		IFXDELETE( pNewState ); // delete the new state if we did not apply it.

		// Back out cached states on appended Mods
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC )
		{
			result = (*ppIntMC)->ClearOldState();
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}

	return result;
} 


IFXRESULT CIFXModifierChain::SetModifier( IFXModifier& rInModifier,
										 U32 index, BOOL isReqValidation )
{
	IFXRESULT result = IFX_OK;
	IFXModifierChainState* pNewState = NULL;

	IFXASSERT( &rInModifier ); // references can be NULL!

	if( !m_pModChainState )
	{
		result = Initialize();
	}

	// Step 1 validate inputs.

	if(IFXSUCCESS(result))
	{
		if( isReqValidation && (index > m_pModChainState->NumModifiers() ) )
		{
			result = IFX_E_INVALID_RANGE;
		}
	}

	// Step 2 Attempt to build a modifier state with the new Modifier
	if( IFXSUCCESS(result))
	{
		++index; // adjust the index to the correct location
		result = BuildNewModifierState(m_pModChainState->GetPrevChainNR(), NULL,
			index, &rInModifier, &pNewState, TRUE, isReqValidation);
	}

	// Step 3 this all worked apply the new state
	if(IFXSUCCESS(result))
	{
		result = ApplyNewModifierState(pNewState);
		IFXDELETE(m_pOldState);
	}
	else // all of this did not work clear the state
	{
		IFXDELETE(pNewState); // delete the new state if we did not apply it.

		// Back out cached states on appended Mods
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC )
		{
			result = (*ppIntMC)->ClearOldState();
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}

	return result;
}


IFXRESULT CIFXModifierChain::RemoveModifier(U32 index)
{
	IFXRESULT result = IFX_OK;
	IFXModifierChainState* pNewState = NULL;

	// 1. Validate the inputs.
	if( !m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS(result) && !m_pModChainState->NumModifiers() )
	{
		result = IFX_E_MODIFIERCHAIN_EMPTY;
	}

	if( IFXSUCCESS(result) )
	{
		if(index == END_OF_MODCHAIN)
		{
			index = m_pModChainState->NumModifiers()-1;
		}
	}

	if( IFXSUCCESS(result) )
	{
		if(index > m_pModChainState->NumModifiers())
		{
			result = IFX_E_INVALID_RANGE;
		}
	}

	// 2. Tell the modifier we are trying to remove to delete temp data
	if( IFXSUCCESS(result) )
	{
		IFXModifier* pMod = NULL;

		if( IFXSUCCESS(m_pModChainState->GetModifier(index+1, &pMod)) )
			pMod->Notify(IFXModifier::DEALLOCATE_INTERMEDIATE_DATA, NULL);

		IFXRELEASE(pMod);
	}

	// 3. Check that subsequent modifiers are ok with the removal
	// to do this we build up a new modifier chain state, this may
	// tell us that the new mod chain state does not meet the input
	// requirements any more.
	if( IFXSUCCESS(result) )
	{
		++index; // adjust the index to the correct location
		result = BuildNewModifierState( m_pModChainState->GetPrevChainNR(), NULL,
			index, NULL, &pNewState, FALSE, TRUE);
	}

	// 4. Apply the new modifier chain state.
	if( IFXSUCCESS(result) )
	{
		result = ApplyNewModifierState( pNewState );
		IFXDELETE( m_pOldState );
		pNewState = NULL;
	}
	else
	{ // all of this did not work back out changes.
		IFXDELETE(pNewState);

		// Back out cached states on appended Mods
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);

		while( ppIntMC )
		{
			result = (*ppIntMC)->ClearOldState();
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}

	return result;
}


IFXRESULT CIFXModifierChain::GetDataPacket(IFXModifierDataPacket*& rpOutDataPacket )
{
	IFXRESULT result = IFX_OK;

	if( !m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS(result) )
	{
		// Note: Intentionally passing reference count out -- as it should be
		rpOutDataPacket = m_pModChainState->GetOutputDataPacketNR();
		rpOutDataPacket->AddRef();
	}

	return result;
}


IFXRESULT CIFXModifierChain::GetModifierCount( U32& rOutModifierCount )
{
	IFXRESULT result = IFX_OK;

	if( !m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS(result) )
	{
		rOutModifierCount = m_pModChainState->NumModifiers();
	}

	return result;
}


IFXRESULT CIFXModifierChain::GetModifier( 
									U32           index,
									IFXModifier*& rpOutModifier )
{
	IFXRESULT result = IFX_OK;

	if( NULL == m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS(result) ) 
	{
		if( index < m_pModChainState->NumModifiers() )
		{
			++index; // Adjust index to correct location
			result = m_pModChainState->GetModifier(index, &rpOutModifier);
		}
		else
		{
			result = IFX_E_INVALID_RANGE;
		}
	}

	return result;
}


IFXRESULT CIFXModifierChain::AddAppendedModifierChain( 
									IFXModifierChainInternal* pInModChain)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(pInModChain);
	if( NULL != pInModChain )
		m_appendedChains.Insert(pInModChain);
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXModifierChain::RemoveAppendedModifierChain( 
									IFXModifierChainInternal* pInModChain)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(pInModChain);
	if( NULL != pInModChain )
		m_appendedChains.Remove(pInModChain);
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXModifierChain::RebuildDataPackets( BOOL isReqValidation )
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(m_pModChainState->GetPrevChainNR());

	if( !m_pCachedState )
	{
		result = BuildNewModifierState(
						m_pModChainState->GetPrevChainNR(), NULL,
						INVALID_DATAPACKET_INDEX, NULL,
						&m_pCachedState, FALSE, isReqValidation);
	}

	// Apply new State
	if( IFXSUCCESS(result) )
	{
		result = ApplyNewModifierState( m_pCachedState );
		// Old state will be cleaned up by call to Clear/Restore old state by
		// the prepended mod chain
		m_pCachedState = NULL;
	}
	else
	{
		IFXDELETE( m_pCachedState );
		// Back out cached states on appended Mods will be forced by calling
		// mod chains -- ie the predecessor
	}

	return result;
}


IFXRESULT CIFXModifierChain::RestoreOldState()
{
	IFXRESULT result = IFX_OK;

	IFXDELETE(m_pCachedState);
	if( m_pOldState )
	{
		IFXDELETE(m_pModChainState);
		m_pModChainState = m_pOldState;
		m_pOldState = NULL;

		// 1. Set the old state active again.
		result = m_pModChainState->SetActive();

		if(IFXSUCCESS(result))
			result = m_pModChainState->NotifyActive();

		// 2. Tell appended chains to do the same.
		if(IFXSUCCESS(result))
		{
			U32 AppendedDPiter = 0;
			IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);

			while(ppIntMC)
			{
				result = (*ppIntMC)->RestoreOldState();

				if(IFXRESULT(result))
					ppIntMC = m_appendedChains.Next(AppendedDPiter);
				else
					break;
			}
		}
	}

	return result;
}


IFXRESULT CIFXModifierChain::ClearOldState()
{
	IFXRESULT result = IFX_OK;

	IFXDELETE(m_pOldState);
	IFXDELETE(m_pCachedState);

	U32 AppendedDPiter = 0;
	IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);

	while(ppIntMC)
	{
		result = (*ppIntMC)->ClearOldState();

		if(IFXRESULT(result))
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		else
			break;
	}

	return result;
}


IFXRESULT CIFXModifierChain::BuildCachedState( 
								IFXModifierDataPacketInternal* pInDP, BOOL isReqValidation )
{
	IFXRESULT result = IFX_OK;
	IFXModifierChainState* pNewState = NULL;

	IFXDELETE(m_pCachedState);

	// 1. attempt to build a new state.
	result = BuildNewModifierState( 
					m_pModChainState->GetPrevChainNR(), 
					pInDP,/// @todo: override for the base datapacket
					INVALID_DATAPACKET_INDEX, NULL, &pNewState,
					FALSE, isReqValidation);

	// 2. check to see if all appended chains are going to be ok
	// with the new state.
	if(IFXSUCCESS(result))
	{
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC)
		{
			result = (*ppIntMC)->BuildCachedState( 
									pNewState->GetOutputDataPacketNR(), 
									isReqValidation );

			if(IFXSUCCESS(result))
				ppIntMC = m_appendedChains.Next( AppendedDPiter );
			else
				break;
		}
	}

	if(IFXSUCCESS(result))
	{
		// hold the cached state in case we need it
		m_pCachedState = pNewState;
	}
	else
	{
		IFXDELETE( pNewState );
	}

	return result;
}


//-------------------------------------------------------------------------
// ModChain Utility Functions.
//-------------------------------------------------------------------------
/** 
Basically Allocate a new state, populate it with modifiers and attempt to
Build the state.
*/
IFXRESULT CIFXModifierChain::BuildNewModifierState(
				IFXModifierChainInternal* pInBaseChain,
				IFXModifierDataPacketInternal* pInOverrideDP,
				U32 modIdx,
				IFXModifier* pInMod, // NULL if Remove
				IFXModifierChainState** ppOutModChainState,
				BOOL bReplace,
				BOOL isReqValidation)

{
	IFXRESULT result = IFX_OK;
	IFXModifierChainState* pNewState = NULL;
	U32 NewSize = 0;
	U32 OldSize = 0;

	if( NULL == ppOutModChainState )
		result = IFX_E_INVALID_POINTER;

	// 1. create the new state object
	if(IFXSUCCESS(result))
	{
		pNewState = new IFXModifierChainState;
		if(!pNewState)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	// 2. Calculate the new size of the chain
	if(IFXSUCCESS(result))
	{
		OldSize = m_pModChainState->NumModifiers()+1;
		if(modIdx != INVALID_DATAPACKET_INDEX)
		{
			// Set
			if(bReplace)
			{
				if(modIdx < OldSize)
				{
					NewSize = OldSize;
				}
				else
				{
					NewSize = modIdx+1;
				}

			}
			else // insert/Remove
			{
				// insert in middle or end of chain
				if(modIdx < OldSize)
				{
					if(m_pModChainState->GetModifierNR(modIdx) != NULL)
					{
						NewSize = OldSize + (pInMod?1:-1);
					}
					// else the modifier is Null we replace it.
					else
					{
						bReplace = TRUE;
					}
				}
				else
				{
					if(NULL != pInMod) // add new
						NewSize = modIdx+1;
					else // remove last
						NewSize = modIdx-1;
				}
			}
		}
		else // doing something like changing the base chain
		{
			NewSize = OldSize;
		}
	}

	// 3. Initialize the newState
	if(IFXSUCCESS(result))
	{
		result = pNewState->Initialize(
								this, pInBaseChain, 
								pInOverrideDP, NewSize-1, m_pDidRegistry);
	}

	// 4. Set the modifiers in to the new state and build the data packets.
	if(IFXSUCCESS(result))
	{
		U32 OldStateIdx = 1;
		U32 NewStateIdx = 1;
		while( IFXSUCCESS(result) && 
			   (NewStateIdx < NewSize) && 
			   (OldStateIdx < OldSize) )
		{
			if(OldStateIdx == modIdx)
			{
				if(pInMod) // add
				{
					result = pNewState->SetModifier(NewStateIdx, pInMod, TRUE);
	
					if(IFXSUCCESS(result))
					{
						if(bReplace)
						{
							++OldStateIdx;
						}
						++NewStateIdx;
					}
				}
				else // remove
				{
					++OldStateIdx;
				}
			}
			else
			{
				IFXModifier* pMod = m_pModChainState->GetModifierNR(OldStateIdx);
				result = 
					pNewState->SetModifier( 
									NewStateIdx, pMod,
									m_pModChainState->GetModifierEnabled(OldStateIdx) );
				if(IFXSUCCESS(result))
				{
					++NewStateIdx;
					++OldStateIdx;
				}
			}
		}

		// pInMod == NULL means removing modifier
		if( pInMod != NULL && modIdx != INVALID_DATAPACKET_INDEX && modIdx >= NewStateIdx && IFXSUCCESS(result) )
		{ 
			// if we are growing the chain then set the new mod.
			pNewState->SetModifier( modIdx, pInMod, TRUE );
		}
	}

	// 5. Tell the new state to Build it's self
	if(IFXSUCCESS(result))
	{
		result = pNewState->Build( isReqValidation );
	}

	// iterate across appended chains
	//for appended:
	//BuildCachedState( pInBaseChain, pNewState, isReqValidation )
	// 3. check that appended chains are ok with the removal
	// test if appended chains are ok with the delete.
	if( IFXSUCCESS(result) && m_appendedChains.Size() )
	{
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin( AppendedDPiter );
		while( ppIntMC )
		{
			result = (*ppIntMC)->BuildCachedState( 
									pNewState->GetOutputDataPacketNR(), 
									isReqValidation );
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}

	// 6. Set the return value or clean up in error condition
	if( IFXSUCCESS(result) )
	{
		*ppOutModChainState = pNewState;
	}
	else
	{
		IFXDELETE( pNewState );
	}

	return result;
}


IFXRESULT CIFXModifierChain::ApplyNewModifierState(
									IFXModifierChainState* pInNewState)
{
	IFXRESULT result = IFX_OK;
	m_bInApplyState = TRUE;
	BOOL bNewPrevChain = TRUE;

	if( NULL == pInNewState )
		result = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(result))
	{
		bNewPrevChain = 
			pInNewState->GetPrevChainNR() != m_pModChainState->GetPrevChainNR();

		if( bNewPrevChain )
		{
			pInNewState->AttachToPrevChain();
		}
	}

	if(IFXSUCCESS(result))
	{
		result = pInNewState->SetActive();

		if(IFXSUCCESS(result))
		{
			IFXDELETE(m_pOldState);
			m_pOldState = m_pModChainState;
			m_pModChainState = pInNewState;
		}
		else
		{
			if(bNewPrevChain)
			{
				pInNewState->DetachFromPrevChain();
			}

			IFXDELETE(pInNewState);
			m_pModChainState->SetActive();

			return result; // note early exit
		}
	}

	if(IFXSUCCESS(result) && m_appendedChains.Size() )
	{
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC && IFXSUCCESS(result))
		{
			/// @todo: isReqValidation
			result = (*ppIntMC)->RebuildDataPackets(TRUE);
			if((*ppIntMC)->NeedTime())
			{
				m_pModChainState->SetNeedTime();
			}

			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}

	if( IFXFAILURE(result) )
	{ // attempt to reinstate the old state
		// assume appended chains

		if(bNewPrevChain)
		{
			pInNewState->DetachFromPrevChain();
		}

		IFXDELETE(pInNewState);
		result = m_pOldState->SetActive();
		m_pModChainState = m_pOldState;
		m_pOldState = NULL;

		// reapply all of the old state on all of the appended chains.
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC)
		{
			result = (*ppIntMC)->RestoreOldState();
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}
	else // all succeeded
	{ // clean up the old state

		if(bNewPrevChain)
		{
			m_pOldState->DetachFromPrevChain();
		}

		IFXDELETE(m_pOldState);
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC)
		{
			(*ppIntMC)->ClearOldState();
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}
	}

	m_bInApplyState = FALSE;

	// irregardles set up the proper behavior to watch the clock
	RecheckNeedTime();

	// irregardless  Tell the modifiers of the change in state.
	m_pModChainState->NotifyActive();
	if( IFXFAILURE(result) )
	{
		U32 AppendedDPiter = 0;
		IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
		while(ppIntMC)
		{
			result = (*ppIntMC)->NotifyActive();
			ppIntMC = m_appendedChains.Next(AppendedDPiter);
		}

	}

	IFXASSERT(IFXSUCCESS(result));

	return result;
}


//  Use this clock for this modifier
IFXRESULT CIFXModifierChain::SetClock( IFXSubject* pInClockSubject )
{
	if( pInClockSubject != m_pClockSubjectNR )
	{
		if( m_pClockSubjectNR )
		{
			m_pClockSubjectNR->Detach( (IFXObserver*)this);
			m_pClockSubjectNR = NULL;
			m_pClockNR = NULL;
		}

		// subject attach...
		if( pInClockSubject )
		{
			m_pClockSubjectNR = pInClockSubject;
			m_pClockSubjectNR->Attach( (IFXObserver*)this, 0);
		}

	}

	return IFX_OK;
}


// IFXObserver
IFXRESULT CIFXModifierChain::Update( 
								IFXSubject* pInSubject, 
								U32 changeBits,
								IFXREFIID rIType)
{
	// result is commented out since it is ignored in the original code
	// and unused value warning results, but may be it was an error
	// IFXRESULT result = IFX_OK;

	if( (IFXUnknown*)pInSubject == (IFXUnknown*)m_pClockSubjectNR )
	{
		if( !changeBits )
		{
			SetClock( NULL );
		}
		else if( changeBits & IFX_CLOCK_NOTIFY_TIME_CHANGED )
		{   //  this where time notifications will come in ->

			// IFXASSERT(!m_pPreviousModifierChain); 
			// only resource chains should watch the clock

			IFXASSERT(m_pModChainState && m_bNeedTime && m_pClockNR);
			m_Time = m_pClockNR->GetSimulationTime();
			IFXDataElementState* pDEState =
				&(m_pModChainState->m_pDataPacketState->
					m_pDataElements[TIME_ELEMENT_INDEX]);
			pDEState->pValue = (void*)(uintptr_t)m_Time;
			pDEState->bNeedRelease = FALSE;
			pDEState->State = IFXDATAELEMENTSTATE_VALID;
			pDEState->ChangeCount = s_changeCount++;

			// this is the tricky part we need to follow the invalidation seq
			// with out invalidating the proxy
			/* result = */ Invalidate(TIME_ELEMENT_INDEX, PROXY_DATAPACKET_INDEX);
		}
	}

	return IFX_OK;
}


IFXRESULT CIFXModifierChain::ProcessDependencies( U32 dataElementIndex,
												  U32 modifierIndex )
{
	IFXRESULT result = IFX_OK;
	IFXDataPacketState* pDPState = NULL;
	IFXDataElementState* pDEState = NULL;

	if( !m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS(result) && modifierIndex >= m_pModChainState->NumModifiers()
		&& modifierIndex != PROXY_DATAPACKET_INDEX)
	{
		result = IFX_E_INVALID_RANGE;
	}

	if( IFXSUCCESS(result) )
	{
		IFXDataPacketState* pDataPacketState = m_pModChainState->m_pDataPacketState;

		if(modifierIndex != PROXY_DATAPACKET_INDEX)
		{
			++modifierIndex; // Adjust index to correct location
			// Get a reference to the ModifierDataElement
			pDPState = &(pDataPacketState[modifierIndex]);
			IFXASSERT(dataElementIndex < pDPState->m_NumDataElements);
			pDEState = &(pDPState->m_pDataElements[dataElementIndex]);

			if( pDEState->State != IFXDATAELEMENTSTATE_CONSUMED )
			{
				IFXDataPacketState* pGenDPState = NULL;
				IFXDataElementState* pGenDEState = NULL;

				// This means we are revalidating valid data elements
				IFXASSERT(pDEState->State != IFXDATAELEMENTSTATE_VALID);
				if( pDEState->Generator == PROXY_DATAPACKET_INDEX)
				{
					IFXASSERT(m_pModChainState->GetPrevChainNR());
					result = m_pModChainState->GetPrevChainNR()
						->GetDEState(dataElementIndex,&pGenDEState);
				}
				else if( pDEState->Generator == 0 )
				{

					if(dataElementIndex == TIME_ELEMENT_INDEX)
					{
						IFXASSERT(m_bNeedTime);
						m_Time = m_pClockNR->GetSimulationTime();
						pGenDEState =
							&(m_pModChainState->m_pDataPacketState->
							m_pDataElements[TIME_ELEMENT_INDEX]);
						pGenDEState->pValue = (void*)(uintptr_t)m_Time;
						pGenDEState->bNeedRelease = FALSE;
						pGenDEState->State = IFXDATAELEMENTSTATE_VALID;
						pGenDEState->ChangeCount = s_changeCount++;
					}
					else if(dataElementIndex == TRANSFORM_ELEMENT_INDEX)
					{
						IFXASSERT(m_pModChainState->m_pTransform);

						pGenDEState =
							&(m_pModChainState->m_pDataPacketState->
							m_pDataElements[TIME_ELEMENT_INDEX]);
						pGenDEState->pValue = &(m_pModChainState->m_pTransform);
						pGenDEState->bNeedRelease = FALSE;
						pGenDEState->State = IFXDATAELEMENTSTATE_VALID;
						pGenDEState->ChangeCount = s_changeCount++;
					}
					else
					{
						IFXASSERT(0);
						result = IFX_E_DATAPACKET_INVALID_INDEX;
					}
				}
				else
				{
					void* pGenData = NULL;
					BOOL bNeedRelease = FALSE;
					pGenDPState = &(pDataPacketState[pDEState->Generator]);
					pGenDEState = &(pGenDPState->m_pDataElements[dataElementIndex]);

					pGenDPState->m_LockedDataElement = dataElementIndex;
					result = pGenDPState->m_pModifier->GenerateOutput( 
															dataElementIndex, 
															pGenData, 
															bNeedRelease );
					pGenDPState->m_LockedDataElement = INVALID_DATAELEMENT_INDEX;

					if(result == IFX_W_MODIFIER_OUTPUT_NOT_CHANGED)
					{
						pGenDEState->State = IFXDATAELEMENTSTATE_VALID;
						IFXASSERT( !dataElementIndex || (pGenData && 
							       (pGenDEState->pValue == pGenData) ) );
					}
					else if(IFXSUCCESS(result))
					{
						IFXASSERT( pGenData );
						pGenDEState->State = IFXDATAELEMENTSTATE_VALID;
						if (pGenDEState->bNeedRelease && pGenDEState->pValue)
							((IFXUnknown*)pGenDEState->pValue)->Release();
						pGenDEState->pValue = pGenData;
						pGenDEState->bNeedRelease = bNeedRelease;
						pGenDEState->ChangeCount = s_changeCount++;
					}
					else
					{
						pGenDEState->State = IFXDATAELEMENTSTATE_VALIDATIONFAILED;
						if (pGenDEState->bNeedRelease && pGenDEState->pValue)
							((IFXUnknown*)pGenDEState->pValue)->Release();
						pGenDEState->pValue = NULL;
						pGenDEState->bNeedRelease = FALSE;
					}
				}

				// Bring this item upto date
				if(IFXSUCCESS(result) && result != IFX_W_MODIFIER_OUTPUT_NOT_CHANGED)
				{
					IFXUnknown* pUnk = NULL;
					if (pDEState->bNeedRelease && pDEState->pValue)
						pUnk = (IFXUnknown*)pDEState->pValue;
					pDEState->bNeedRelease = pGenDEState->bNeedRelease;
					pDEState->pValue = pGenDEState->pValue;
					if (pDEState->bNeedRelease)
						((IFXUnknown*)pDEState->pValue)->AddRef();
					pDEState->ChangeCount = pGenDEState->ChangeCount;
					if (pUnk)
						pUnk->Release();
				}

				if(IFXSUCCESS(result) )
				{
					pDEState->State = IFXDATAELEMENTSTATE_VALID;
				}
				else
				{
					if (pDEState->bNeedRelease && pDEState->pValue)
						((IFXUnknown*)pDEState->pValue)->Release();
					pDEState->State = IFXDATAELEMENTSTATE_VALIDATIONFAILED;
					pDEState->pValue = NULL;
					pDEState->bNeedRelease = FALSE;
				}
			}
			else
			{
				result = IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED;
			}
		}
		else
		{
			if( !m_pModChainState->m_pBaseDataPacket )
				// no base data packet -- had best be time from the proxy.
			{
				if(dataElementIndex == TIME_ELEMENT_INDEX)
				{
					m_Time = m_pClockNR->GetSimulationTime();
					IFXDataElementState* pDEState =
						&(m_pModChainState->m_pDataPacketState->
						m_pDataElements[TIME_ELEMENT_INDEX]);
					pDEState->pValue = (void*)(uintptr_t)m_Time;
					pDEState->State = IFXDATAELEMENTSTATE_VALID;

				}
				else
				{
					result = IFX_E_DATAPACKET_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				// Currently think that this should not happen.
				IFXASSERT(0);
				result = IFX_E_UNDEFINED;
			}
		}
	}

	return result;
}


IFXRESULT CIFXModifierChain::Invalidate( U32 dataElementIndex,
										 U32 modifierIndex )
{
	IFXRESULT result = IFX_OK;

	if( !m_pModChainState )
	{
		result = Initialize();
	}

	if( IFXSUCCESS(result) )
	{
		if( modifierIndex == PROXY_DATAPACKET_INDEX )
		{
			modifierIndex = 0;
		}
		else if( modifierIndex < m_pModChainState->NumModifiers() )
		{
			// adjust the index to the correct location to offset the proxy
			++modifierIndex;
		}
		else
		{
			result = IFX_E_INVALID_RANGE;
		}
	}

	if( IFXSUCCESS(result) )
	{
		IFXDataPacketState* pDataPacketState = m_pModChainState->m_pDataPacketState;

		// only do the invalidation if the element is valid
		// Assume that the previous invalidation did the propagation
		// work. Or if the modifier is the proxy then always invalidate
		I32 StartDepth = s_invDepth;

		if( s_invDepth == s_invSize-1 )
		{
			result = GrowInvState();
		}

		++s_invDepth;

		if( s_invDepth >= s_invSize )
			result = IFX_E_INVALID_RANGE;

		if( IFXSUCCESS( result ) )
		{
			// Set the state for the Initial invalidation
			s_pInvState[s_invDepth].ModIdx = modifierIndex;
			s_pInvState[s_invDepth].pDEState =
				&(pDataPacketState[modifierIndex].
				m_pDataElements[dataElementIndex]);
			s_pInvState[s_invDepth].InvIdx = 0;
		}

		// we never actually invalidate the proxy data packet
		// all of the proxy data packet entries except for time
		// should always be valid
		if( IFXSUCCESS( result ) && modifierIndex != 0 )
		{ // invalidate this element
			s_pInvState[s_invDepth].pDEState->State = IFXDATAELEMENTSTATE_INVALID;

			if( s_pInvState[s_invDepth].pDEState->AspectBit )
			{
				pDataPacketState[modifierIndex].m_pDataPacket->DoPostChanges( 
					s_pInvState[s_invDepth].pDEState->AspectBit );
			}
		}

		// only iterate if we have inv seq
		if( IFXSUCCESS( result ) && !s_pInvState[s_invDepth].pDEState->m_pInvSeq )
		{
			s_invDepth = StartDepth;
		}

		// Iterate -- follow all of the invalidation sequences
		while( IFXSUCCESS( result ) && s_invDepth > StartDepth )
		{
			InvRecord* pCurIterState = s_pInvState + s_invDepth;

			// Get the current Inv Seq
			IFXModifierDataPacketInternal* pDP = 
				pDataPacketState[pCurIterState->ModIdx].m_pDataPacket;
			IFXDidInvElement* pInvEl = 
				&(pCurIterState->pDEState->m_pInvSeq[pCurIterState->InvIdx]);
			pCurIterState->InvIdx++;

			// pop this iter state if we are processing the last entry
			if( pCurIterState->InvIdx == pCurIterState->pDEState->m_uInvCount )
			{
				--s_invDepth;
			}

			// Get the Invalidation Target and Do The Invalidation
			if( pInvEl->uMIndex != APPENDED_DATAPACKET_INDEX )
			{
				IFXDataPacketState* pTrgDPState = 
					&(pDataPacketState[pInvEl->uMIndex]);
				IFXDataElementState* pTrgDEState = 
					&(pTrgDPState->m_pDataElements[pInvEl->uEIndex]);

				if( pTrgDEState->State == IFXDATAELEMENTSTATE_VALID )
				{ // only invalidate if valid
					pTrgDEState->State = IFXDATAELEMENTSTATE_INVALID;
					if( pTrgDEState->m_pInvSeq )
					{ // if this element has invalidations
						// set the state to follow them on the next iter
						s_invDepth++;
						if( s_invDepth == s_invSize-1 )
						{
							result = GrowInvState();
						}

						s_pInvState[s_invDepth].ModIdx = pInvEl->uMIndex;
						s_pInvState[s_invDepth].pDEState =
							&(pDataPacketState[pInvEl->uMIndex].
							m_pDataElements[pInvEl->uEIndex]);
						s_pInvState[s_invDepth].InvIdx = 0;
					}
				}

				if( pTrgDEState->AspectBit )
				{
					pDP->DoPostChanges( pTrgDEState->AspectBit );
				}
			}
			else // the last data packet for this modifier
			{
				IFXDataPacketState* pTrgDPState = 
					&(pDataPacketState[pCurIterState->ModIdx]);
				IFXDataElementState* pTrgDEState = 
					&(pTrgDPState->m_pDataElements[pInvEl->uEIndex]);

				if( pTrgDEState->State == IFXDATAELEMENTSTATE_VALID )
				{ // only invalidate if valid
					pTrgDEState->State = IFXDATAELEMENTSTATE_INVALID;
				}

				if( pTrgDEState->AspectBit )
				{
					// DoPostChanges for last modifier in the chain which 
					// affects DE with pTrgDEState->AspectBit index
					// It mightn't be the last modifier in the modifier chain
					pDP->DoPostChanges( pTrgDEState->AspectBit );

					U32 chainIndex;
					result = pDP->GetChainIndex( &chainIndex );

					/** 
						@todo	Investigate behavior of MC::Invalidate method in case if
								last modifier doesn't have current DE in it's output 
								(e.g. Transform in Shading Modifier).
					*/
					if( IFXSUCCESS( result ) && chainIndex != INVALID_DATAPACKET_INDEX )
					{
						/* 
						Compare the MC index of pDP and the index of last modifier in 
						the chain. If they are different then update observers of last
						DP using current aspect bit.
						*/
						if( chainIndex != m_pModChainState->NumModifiers() - 1 )
						{
							IFXDataPacketState* pDPState = NULL;
							IFXDataElementState* pDEState = NULL;
							U32 cnt;

							// Iterate thru all DPs from current modifier's to the
							// last DP in the chain and set all states of given data
							// element to invalid.
							for( cnt = pCurIterState->ModIdx + 1; 
								 cnt < m_pModChainState->NumModifiers() + 1; 
								 cnt++ )
							{
								pDPState = &(pDataPacketState[ cnt ]);
								pDEState = 
									&(pDPState->m_pDataElements[pInvEl->uEIndex]);

								if( pDEState->State == IFXDATAELEMENTSTATE_VALID )
								{
									pDEState->State = IFXDATAELEMENTSTATE_INVALID;
								}
							}

							IFXModifierDataPacketInternal* pLastDP = 
								m_pModChainState->GetOutputDataPacketNR();
							if( pLastDP != NULL )
								pLastDP->DoPostChanges( pTrgDEState->AspectBit );
						}
					}
				}

				// if there is a chain appended to the end
				// force the invalidations forward on to it
				U32 AppChainIter = 0;
				IFXModifierChainInternal** pAppendedChain = 
					m_appendedChains.Begin(AppChainIter);
				while( pAppendedChain )
				{
					result = (*pAppendedChain)->Invalidate( 
													pInvEl->uEIndex, 
													PROXY_DATAPACKET_INDEX );
					pAppendedChain = m_appendedChains.Next( AppChainIter );
				}
			}
			pDP = NULL;
		}
	}

	return result;
}


// Validation should have been done by CIFXModifierChain::Invalidate
IFXRESULT CIFXModifierChain::GetDEState(
								U32 dataElementIndex, 
								IFXDataElementState** ppDEState)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(ppDEState );

	if( NULL != ppDEState )
	{
		if( !m_pModChainState )
		{
			result = Initialize();
		}

		if(IFXSUCCESS(result))
		{
			U32 genIdx = m_pModChainState->NumModifiers();
			IFXDataPacketState* pDPState = &(m_pModChainState->m_pDataPacketState[genIdx]);
			IFXDataElementState* pDEState= &(pDPState->m_pDataElements[dataElementIndex]);

			if(pDEState->State != IFXDATAELEMENTSTATE_VALID)
			{
				result = ProcessDependencies(dataElementIndex, genIdx-1);
			}

			if(IFXSUCCESS(result))
			{
				*ppDEState = pDEState;
			}
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXModifierChain::GetIntraDeps(IFXIntraDependencies** ppIntraDeps)
{
	IFXRESULT result = IFX_OK;

	if( NULL != ppIntraDeps )
	{
		if( !m_pModChainState )
		{
			result = Initialize();
		}

		if(IFXSUCCESS(result))
		{
			*ppIntraDeps = m_pModChainState->GetIntraDeps();
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


void CIFXModifierChain::RecheckNeedTime()
{
	if (!(m_pModChainState && m_pClockSubjectNR))
		return;

	// don't do this calculation while this modchain is in the middle of
	// applying a new state -- prevents extra work.
	if(m_bInApplyState)
	{
		return;
	}

	if(m_pModChainState->GetNeedTime())
	{
		m_bNeedTime = TRUE;
	}

	U32 AppendedDPiter = 0;
	IFXModifierChainInternal** ppIntMC = m_appendedChains.Begin(AppendedDPiter);
	while(ppIntMC && !m_bNeedTime)
	{
		m_bNeedTime = (*ppIntMC)->NeedTime();
		ppIntMC = m_appendedChains.Next(AppendedDPiter);
	}

	if(!m_pModChainState->GetPrevChainNR())
	{
		// if we have a state and are a resource chain then we need to
		// observe the clock
		if(m_bNeedTime)
		{
			m_pClockSubjectNR->Detach((IFXObserver*)this);
			m_pClockSubjectNR->Attach( 
								(IFXObserver*)this, 
								IFX_CLOCK_NOTIFY_TIME_CHANGED);
			m_pClockSubjectNR->QueryInterface(IID_IFXClock, (void**)&m_pClockNR);
			m_pClockNR->Release();
		}
		else if(m_pClockNR)
		{
			m_pClockSubjectNR->Detach((IFXObserver*)this);
			m_pClockSubjectNR->Attach((IFXObserver*)this, 0);
			m_pClockNR = NULL;
		}

	}
	else
	{
		m_pClockSubjectNR->Detach((IFXObserver*)this);
		m_pClockSubjectNR->Attach((IFXObserver*)this, 0);

		IFXASSERT(m_pModChainState->GetPrevChainNR());
		m_pModChainState->GetPrevChainNR()->RecheckNeedTime();
	}
}


BOOL CIFXModifierChain::NeedTime()
{
	return m_bNeedTime;
}


IFXRESULT  CIFXModifierChain::NotifyActive()
{
	return m_pModChainState->NotifyActive();
}
