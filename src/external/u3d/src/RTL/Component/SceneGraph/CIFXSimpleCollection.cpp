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
@file  CIFXSimpleCollection.cpp

	The implementation file of the base CIFXSimpleCollection class.      
*/

#include "CIFXSimpleCollection.h"

#include "IFXMemory.h"

const U32 CIFXSimpleCollection::m_allocationPad = 8;

CIFXSimpleCollection::CIFXSimpleCollection() 
{
	m_uRefCount = 0;

	U32 i;
	for (  i = IFXSpatial::TYPE_COUNT; i--; )
	{
		m_pSpatials[i] = NULL;
		m_uSpatials[i] = 0;
		m_uSpatialsAllocated[i] = 0;
	}
}


void CIFXSimpleCollection::Destruct()
{
	U32 i;
	for (  i = IFXSpatial::TYPE_COUNT; i--; )
	{
		IFXDeallocate( m_pSpatials[i] );
		m_pSpatials[i] = NULL;
		m_uSpatials[i] = 0;
		m_uSpatialsAllocated[i] = 0;
	}
}


CIFXSimpleCollection::~CIFXSimpleCollection() 
{
	Destruct();
}


IFXRESULT IFXAPI_CALLTYPE CIFXSimpleCollection_Factory( IFXREFIID interfaceId,
                                        void**    ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXSimpleCollection *pComponent = new CIFXSimpleCollection();

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


// IFXUnknown
U32 CIFXSimpleCollection::AddRef(void) 
{
	
	return ++m_uRefCount;
} 	


U32 CIFXSimpleCollection::Release(void) 
{
	
	if ( !( --m_uRefCount ) )
	{
		delete this;
		return 0;
	}

	return m_uRefCount;
} 	


IFXRESULT CIFXSimpleCollection::QueryInterface( IFXREFIID interfaceId, 
                                                void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXCollection )
			*ppInterface = ( IFXCollection* ) this;
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


// IFXCollection
IFXRESULT CIFXSimpleCollection::InitializeCollection( 
                                    IFXSpatial**      pInSpatials,
	                                U32               uInNumberOfSpatials,
	                                IFXSpatial::eType eInType )
{
	Destruct();
	return AddSpatials( pInSpatials, uInNumberOfSpatials, eInType );
}


IFXRESULT CIFXSimpleCollection::InitializeCollection( 
                                    IFXCollection* pInCollection)
{
	Destruct();

	IFXSpatial ** pSpatials=0;
	U32 uSpatialCnt =0,i=0;
	IFXRESULT iResult = IFX_OK;
	U32 uSpatialTypeCnt = IFXSpatial::TYPE_COUNT;
	
	for(i =0;i<uSpatialTypeCnt&&IFXSUCCESS(iResult);i++)
	{
		pSpatials = 0;
		uSpatialCnt = 0;
		iResult = pInCollection->GetSpatials(pSpatials,uSpatialCnt,IFXSpatial::eType(i));
		if(uSpatialCnt&&IFXSUCCESS(iResult))
		{
			
			iResult = AddSpatials( pSpatials, uSpatialCnt, IFXSpatial::eType(i));
		}
	}

	return iResult;
}


IFXRESULT CIFXSimpleCollection::AddSpatials(          
                                    IFXSpatial**      pInSpatials,
	                                U32               uInNumberOfSpatials,
	                                IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;

	if ( eInType == IFXSpatial::UNSPECIFIED )
	{
		U32 i;
		for (  i = 0; i<uInNumberOfSpatials && (IFXSUCCESS(result)); i++ )
			result = _AddSpatials( pInSpatials+i, 1, pInSpatials[i]->GetSpatialType() );
	}
	else
		result = _AddSpatials( pInSpatials, uInNumberOfSpatials, eInType );

	return result;
}


IFXRESULT CIFXSimpleCollection::_AddSpatials(          
                                    IFXSpatial**      pInSpatials,
	                                U32               uInNumberOfSpatials,
	                                IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;

	// (Re)allocate the collection as needed
	if ( uInNumberOfSpatials )
	{
		if ( m_uSpatialsAllocated[(U32)eInType] 
			 < ( m_uSpatials[(U32)eInType] + uInNumberOfSpatials ) )
		{
			if ( m_uSpatialsAllocated[(U32)eInType] )
			{ // Reallocate
				m_pSpatials[(U32)eInType] = (IFXSpatial**)
					IFXReallocate( m_pSpatials[(U32)eInType],
								   ( m_uSpatials[(U32)eInType] 
								     + uInNumberOfSpatials 
								     + m_allocationPad )
								     * sizeof(IFXSpatial*) );
				if ( m_pSpatials[(U32)eInType] )
				{
					m_uSpatialsAllocated[(U32)eInType] = m_uSpatials[(U32)eInType]
												       + uInNumberOfSpatials 
												       + m_allocationPad;
				}
				else
					result = IFX_E_OUT_OF_MEMORY;
			}
			else // Initial allocation
			{
				m_pSpatials[(U32)eInType] = (IFXSpatial**)
					IFXAllocate( (uInNumberOfSpatials + m_allocationPad)
							     * sizeof(IFXSpatial*) );
				m_uSpatialsAllocated[(U32)eInType] = uInNumberOfSpatials
											       + m_allocationPad;
			}
		}
	}

	U32 is;
	for (  is = 0; is < uInNumberOfSpatials; is++ )
	{
		// Ensure it doesn't already exist.
		BOOL bAlreadyExists = IFX_FALSE;
		U32 es;
		for (  es = 0; es < m_uSpatials[(U32)eInType]; es++ )
		{
			if ((m_pSpatials[(U32)eInType])[es] == pInSpatials[is])
			{
				bAlreadyExists = IFX_TRUE;
				break;
			}
		}

		// Add unique spatials to the collection.
		if (!bAlreadyExists) 
			(m_pSpatials[(U32)eInType])[m_uSpatials[(U32)eInType]++] = pInSpatials[is];
	}

	return result;
}


IFXRESULT CIFXSimpleCollection::RemoveSpatials(       
                                    IFXSpatial**      pInSpatials,
	                                U32               uInNumberOfSpatials,
	                                IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;

	U32 uType = (U32)eInType;
	U32 i;
	for (  i = 0; i < uInNumberOfSpatials; i++ )
	{
		if ( eInType == IFXSpatial::UNSPECIFIED )
			uType = (U32)pInSpatials[i]->GetSpatialType();

		U32 e;
		for (  e = m_uSpatials[uType]; e--; )
		{
			if ( (m_pSpatials[uType])[e] == pInSpatials[i] )
			{
				(m_pSpatials[uType])[e] = 
					(m_pSpatials[uType])[--m_uSpatials[uType]];
				break;
			}
		}
	}

	return result;
}


IFXRESULT CIFXSimpleCollection::GetSpatials(          
                                    IFXSpatial**&     rpOutSpatials,
	                                U32&              ruOutNumberOfSpatials,
	                                IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;

	if ( ((U32)eInType) < IFXSpatial::TYPE_COUNT )
	{
		rpOutSpatials = m_pSpatials[(U32)eInType];
		ruOutNumberOfSpatials = m_uSpatials[(U32)eInType];
	}
	else 
		result = IFX_E_INVALID_RANGE;

	return result;
}

