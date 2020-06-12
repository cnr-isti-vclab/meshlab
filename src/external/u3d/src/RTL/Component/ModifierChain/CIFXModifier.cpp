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
	@file	CIFXModifier.cpp

			The implementation file of the base CIFXModifier component.
*/

#include "CIFXModifier.h"
#include "IFXSceneGraph.h"
#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"


CIFXModifier::CIFXModifier()
{
	m_pModChainNR			= NULL;
	m_pModChainSubNR		= NULL;
	m_pModifierDataPacket	= NULL;
	m_pInputDataPacket		= NULL;
	m_uModifierChainIndex	= (U32)-1;
}


CIFXModifier::~CIFXModifier()
{
	IFXASSERT( NULL == m_pModifierDataPacket );
	IFXASSERT( NULL == m_pInputDataPacket );

/*
	HINT:
		If the Release() method of the inherited modifier
	Looks like the example below, these asserts will go away.

U32 MyModifier::Release()
{
	if( m_uRefCount == 1 )
	{
	  	CIFXModifier::PreDestruct();
		delete this;
		return 0;
	}
	else
		return --m_uRefCount;
}

*/

	IFXRELEASE( m_pModifierDataPacket );
	IFXRELEASE( m_pInputDataPacket );
}

void CIFXModifier::PreDestruct()
{
	CIFXSubject::PreDestruct();
	IFXRELEASE( m_pModifierDataPacket );
	IFXRELEASE( m_pInputDataPacket );
	SetModifierChain( NULL, 0 );
}


IFXRESULT CIFXModifier::GetDataPacket( IFXModifierDataPacket*& rpOutDataPacket )
{
	IFXRESULT result = IFX_OK;

	if ( m_pModifierDataPacket )
	{
		rpOutDataPacket = m_pModifierDataPacket;
		rpOutDataPacket->AddRef();
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT CIFXModifier::SetModifierChain( IFXModifierChain* pInModifierChain,
										  U32 in_ChainIndex )
{
	IFXRESULT ir = IFX_OK;

	if( m_pModChainNR )
	{
		m_pModChainSubNR->Detach( (IFXObserver*)this );
		m_pModChainSubNR = NULL;
		m_pModChainNR = NULL;
	}

	SetDataPacket( NULL, NULL );

	m_pModChainNR = pInModifierChain;

	if( m_pModChainNR )
	{
		m_pModChainNR->QueryInterface(IID_IFXSubject, (void**)&m_pModChainSubNR);
		m_pModChainSubNR->Release();
		m_pModChainSubNR->Attach( (IFXObserver*)this, 0 );
	}

	m_uModifierChainIndex = in_ChainIndex;

	return ir;
}


IFXRESULT CIFXModifier::Notify( IFXModifierMessage eInMessage,
									void* pMessageContext )
{
	return IFX_OK;
}


IFXRESULT CIFXModifier::GetModifierChain( IFXModifierChain** ppOutModifierChain )
{
	if( !m_pModChainNR )
	{
		return IFX_E_NOT_INITIALIZED;
	}
	m_pModChainNR->AddRef();
	*ppOutModifierChain = m_pModChainNR;
	return IFX_OK;
}


IFXRESULT CIFXModifier::GetModifierChainIndex( U32& rOutModifierChainIndex )
{
	IFXRESULT result = IFX_OK;

	if ( m_pModChainNR && m_uModifierChainIndex != (U32)-1 )
		rOutModifierChainIndex = m_uModifierChainIndex;
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


// IFXObserver
IFXRESULT CIFXModifier::Update(IFXSubject* pInSubject, U32 uInChangeBits,IFXREFIID rIType)
{
	if( (pInSubject == m_pModChainSubNR) && !uInChangeBits )
	{
		m_pModChainSubNR->Detach( (IFXObserver*)this );
		m_pModChainSubNR = NULL;
		m_pModChainNR = NULL;
	}
	return IFX_OK;
}
