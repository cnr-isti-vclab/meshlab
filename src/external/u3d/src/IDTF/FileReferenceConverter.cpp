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
  @file FileReferenceConverter.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "FileReferenceConverter.h"
#include "FileReference.h"
#include "IFXFileReference.h"
#include "SceneUtilities.h"
#include "Tokens.h"
#include "IFXCheckX.h"

using namespace U3D_IDTF;

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

FileReferenceConverter::FileReferenceConverter( 
						SceneUtilities* pSceneUtils,
						const FileReference* pFileReference)
: m_pFileReference( pFileReference ), m_pSceneUtils( pSceneUtils )
{
	IFXCHECKX_RESULT( NULL != pSceneUtils, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pFileReference, IFX_E_INVALID_POINTER );
}

FileReferenceConverter::~FileReferenceConverter()
{
	m_pFileReference = NULL;
	m_pSceneUtils = NULL;
}

IFXRESULT FileReferenceConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXFileReference, pIFXFileReference );

	result = m_pSceneUtils->CreateFileReference( 
							m_pFileReference->GetScopeName(), 
							&pIFXFileReference );

	if( IFXSUCCESS( result ) )
	{
		IFXObjectFilters objectFilters;
		const IFXString& rCollisionPolicy = 
								m_pFileReference->GetCollisionPolicy();

		U32 i;
		for( i = 0; 
			 i < m_pFileReference->GetFilterCount() && IFXSUCCESS( result ); 
			 ++i )
		{
			const Filter& rFilter = m_pFileReference->GetFilter( i );
			const IFXString& rType = rFilter.GetType();
			IFXObjectFilter& rIFXFilter = objectFilters.CreateNewElement();
			
			if( rType == IDTF_FILTER_TYPE_TYPE )
			{
				rIFXFilter.FilterType = IFXOBJECTFILTER_TYPE;
				rIFXFilter.ObjectTypeFilterValue = rFilter.GetObjectType();
			}
			else if( rType == IDTF_FILTER_TYPE_NAME )
			{
				rIFXFilter.FilterType = IFXOBJECTFILTER_NAME;
				rIFXFilter.ObjectNameFilterValue = rFilter.GetObjectName();
			}
			else
				result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS( result ) )
		{

			pIFXFileReference->SetObjectFilters( objectFilters );

			pIFXFileReference->SetFileURLs( m_pFileReference->GetUrlList() );

			pIFXFileReference->SetScopeName( m_pFileReference->GetScopeName() );

			pIFXFileReference->SetWorldAlias( m_pFileReference->GetWorldAlias() );

			if( rCollisionPolicy == "REPLACE" )
				pIFXFileReference->SetCollisionPolicy( IFXCOLLISIONPOLICY_REPLACE );
			else if( rCollisionPolicy == "DISCARD" )
				pIFXFileReference->SetCollisionPolicy( IFXCOLLISIONPOLICY_DISCARD );
			else if( rCollisionPolicy == "PREPEND_ALL" )
				pIFXFileReference->SetCollisionPolicy( IFXCOLLISIONPOLICY_PREPENDALL );
			else if( rCollisionPolicy == "PREPENDCOLLIDED" )
				pIFXFileReference->SetCollisionPolicy( IFXCOLLISIONPOLICY_PREPENDCOLLIDED );
			else if( rCollisionPolicy == "POSTMANGLE" )
				pIFXFileReference->SetCollisionPolicy( IFXCOLLISIONPOLICY_POSTMANGLE );
			else
				result = IFX_E_UNSUPPORTED;
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
