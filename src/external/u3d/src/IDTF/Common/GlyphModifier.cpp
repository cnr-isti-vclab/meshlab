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
  @file GlyphModifier.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "GlyphModifier.h"
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

GlyphModifier& GlyphModifier::operator= (const GlyphModifier& rModifier)
{
	if( this != &rModifier )
	{
		IFXRESULT result = IFX_OK;

		U32 i;
		for( i = 0; 
			 i < rModifier.GetCommandCount() && IFXSUCCESS( result ); 
			 ++i )
		{
			const GlyphCommand* pCommand = rModifier.GetCommand( i );
			result = AddCommand( pCommand );
			IFXCHECKX( IFX_OK == result );
		}

		m_billboard = rModifier.m_billboard;
		m_tm = rModifier.m_tm;

		SetType( rModifier.GetType() );
		SetName( rModifier.GetName() );
		SetChainType( rModifier.GetChainType() );
	}

	return *this;
}

void GlyphModifier::SetBillboard( const IFXString& rValue )
{
	m_billboard = rValue;
}

void GlyphModifier::SetSingleShader( const IFXString& rValue )
{
	m_singleShader = rValue;
}

const IFXString& GlyphModifier::GetBillboard() const
{
	return m_billboard;
}

const IFXString& GlyphModifier::GetSingleShader() const
{
	return m_singleShader;
}

void GlyphModifier::SetTM( const IFXMatrix4x4& rMatrix )
{
	m_tm = rMatrix;
}

const IFXMatrix4x4& GlyphModifier::GetTM() const
{
	return m_tm;
}

IFXRESULT GlyphModifier::AddCommand( const GlyphCommand* pCommand )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pCommand )
	{
		result = m_commandList.AddCommand( pCommand );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

const GlyphCommand* GlyphModifier::GetCommand( U32 index ) const
{
	return m_commandList.GetCommand( index );
}

U32 GlyphModifier::GetCommandCount() const
{
	return m_commandList.GetCommandCount();
}


//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
