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
@file FileScanner.cpp

This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************

//***************************************************************************
//  Includes
//***************************************************************************

#include "FileScanner.h"
#include "IFXException.h"
#include "IFXMatrix4x4.h"

#include "Color.h"
#include "Quat.h"
#include "Point.h"
#include "Int3.h"
#include "Int2.h"
#include "MetaDataList.h"
#include "Tokens.h"

#include <ctype.h>
#include <wchar.h>

using namespace U3D_IDTF;

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

FileScanner::FileScanner()
{
	m_currentCharacter[0] = 0;
	m_currentCharacter[1] = 0;
	m_used = TRUE;
}

FileScanner::~FileScanner()
{
}

IFXRESULT FileScanner::Initialize( const IFXCHAR* pFileName )
{
	IFXRESULT result = IFX_OK;

	result = m_file.Initialize( pFileName );

	if( IFXSUCCESS( result ) )
		m_currentCharacter[0] = m_file.ReadCharacter();

	return result;
}

IFXRESULT FileScanner::Scan( IFXString* pToken, U32 scanLine )
{
	// try to use fscanf
	IFXRESULT result = IFX_OK;

	if( NULL != pToken )
	{
		if( scanLine )
			SkipBlanks();
		else
			SkipSpaces();

		if( TRUE == IsEndOfFile() )
			result = IFX_E_EOF;
		else
		{
			U8 i = 0;
			U8 buffer[MAX_STRING_LENGTH] = {0};

			while( 0 == IsSpace( GetCurrentCharacter() ) && !IsEndOfFile() )
			{
				buffer[i++] = GetCurrentCharacter();
				NextCharacter();
			}

			result = pToken->Assign(buffer);
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanString( IFXString* pString )
{
	IFXRESULT result = IFX_OK;

	if( NULL == pString )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
	{
		SkipSpaces();
		if( '"' == GetCurrentCharacter() )
		{
			// found string, skip first quote
			NextCharacter();

			U32 i = 0;
			U8 scanBuffer[MAX_STRING_LENGTH+2];
			while( '"' != GetCurrentCharacter() && i < MAX_STRING_LENGTH )
			{
				if( '\\' == GetCurrentCharacter())
				{
					NextCharacter();
					U8 currentCharacter = GetCurrentCharacter();
					switch (currentCharacter)
					{
						case '\\':
							scanBuffer[i++] = '\\';
							break;
						case 'n':
							scanBuffer[i++] = '\n';
							break;
						case 't':
							scanBuffer[i++] = '\t';
							break;
						case 'r':
							scanBuffer[i++] = '\r';
							break;
						case '"':
							scanBuffer[i++] = '"';
							break;
						default:
							scanBuffer[i++] = currentCharacter;
					}
				}
				else
					scanBuffer[i++] = GetCurrentCharacter();
				NextCharacter();
			}

			NextCharacter(); // skip last double quote
			scanBuffer[i] = 0;

			/// @todo: Converter Unicode support
			// convert one byte string to unicode
			pString->Assign( scanBuffer );
		}
		else
		{
			result = IFX_E_STRING_NOT_FOUND;
		}
	}

	return result;
}


IFXRESULT FileScanner::ScanFloat( F32* pNumber )
{
	IFXRESULT result = IFX_OK;

	if( NULL == pNumber )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
	{
//this F32 format is preferred
#ifdef F32_EXPONENTIAL
		IFXString buffer;

		U32 fpos;
		result = m_file.GetPosition( &fpos );
		if( IFXSUCCESS( result ) )
			result = Scan( &buffer, 1 );

		if( IFXSUCCESS( result ) )
		{
			I32 scanResult = swscanf( buffer.Raw(), L"%g", pNumber );
			
			if( 0 == scanResult || EOF == scanResult )
			{
				result = IFX_E_FLOAT_NOT_FOUND;
				// new token found, not float
				// do not allow client to continue scan for new tokens
				m_used = TRUE;
				m_currentToken = buffer;
				fpos--;
				m_file.SetPosition( fpos );
				NextCharacter();
			}
		}

#else
		I32 sign = 1;
		U32 value = 0;

		SkipBlanks();

		if( '-' == GetCurrentCharacter() )
		{
			sign = -1;
			NextCharacter();
		}

		if( '-' == GetCurrentCharacter() || '+' == GetCurrentCharacter() )
			NextCharacter();

		while( isdigit( GetCurrentCharacter() ) )
		{
			value = ( value*10 ) + ( GetCurrentCharacter() - '0' );
			NextCharacter();
		}

		// there should be fraction part of float
		if( '.' == GetCurrentCharacter() )
		{
			F32 fraction = 0.0f;
			F32 divisor = 10.0f;

			if( '.' == GetCurrentCharacter() )
				NextCharacter();

			while( isdigit( GetCurrentCharacter() ) )
			{
				fraction += ( GetCurrentCharacter() - '0' ) / divisor;
				divisor *=10.0f;
				NextCharacter();
			}

			*pNumber = static_cast<float>(value);
			*pNumber += fraction;
			*pNumber *= sign;
		}
		else
		{
			result = IFX_E_FLOAT_NOT_FOUND;
		}
#endif
	}

	return result;
}

IFXRESULT FileScanner::ScanInteger( I32* pNumber )
{
	IFXRESULT result = IFX_OK;
	IFXString buffer;

	if( NULL == pNumber )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
	{
		I32 sign = 1;
		I32 value = 0;

		SkipSpaces();

		if( '-' == GetCurrentCharacter() )
		{
			sign = -1;
		}

		if( '-' == GetCurrentCharacter() || '+' == GetCurrentCharacter() )
		{
			NextCharacter();
		}

		while( isdigit( GetCurrentCharacter() ) )
		{
			value = (value*10)  + (GetCurrentCharacter() - '0');
			NextCharacter();
		}

		*pNumber = value * sign;
	}

	return result;
}

IFXRESULT FileScanner::ScanHex( I32* pNumber )
{
	IFXRESULT result = IFX_OK;
	IFXString buffer;

	if( NULL == pNumber )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
		result = Scan( &buffer );

	if( IFXSUCCESS( result ) )
	{
		buffer.ForceUppercase();
		int scanResult = swscanf( buffer.Raw(), L"%X", pNumber );
		if( 0 == scanResult || EOF == scanResult )
		{
			result = IFX_E_INT_NOT_FOUND;
		}
	}

	return result;
}

IFXRESULT FileScanner::ScanTM( IFXMatrix4x4* pMatrix )
{
	IFXRESULT result = IFX_OK;
	F32 matrix[16];

	U32 i;
	for( i = 0; i < 16 && IFXSUCCESS( result ); ++i )
	{
		result = ScanFloat( &matrix[i] );
		if( 0 == (i + 1)%4 )
		{
			// skip end of line
			SkipSpaces();
		}
	}

	if( IFXSUCCESS( result ) )
	{
		*pMatrix = matrix;
		SkipSpaces();
	}

	return result;
}

IFXRESULT FileScanner::ScanColor( Color* pColor )
{
	IFXRESULT result = IFX_OK;

	F32 red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;

	result = ScanFloat( &red );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &green );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &blue );

	if( IFXSUCCESS( result ) )
	{
		result = ScanFloat( &alpha );
		if( IFXSUCCESS( result ) )
		{
			// 4 component color
			IFXVector4 color( red, green, blue, alpha );
			pColor->SetColor( color );
		}
		else if( IFX_E_FLOAT_NOT_FOUND == result )
		{
			// 3 component color
			IFXVector4 color( red, green, blue );
			pColor->SetColor( color );
			result = IFX_OK;
		}
		SkipSpaces();
	}

	return result;
}

IFXRESULT FileScanner::ScanQuat( Quat* pQuat )
{
	IFXRESULT result = IFX_OK;

	F32 w = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;

	result = ScanFloat( &w );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &x );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &y );

	if( IFXSUCCESS( result ) )
	{
		result = ScanFloat( &z );
		if( IFXSUCCESS( result ) )
		{
			IFXVector4 quat( w, x, y, z );
			pQuat->SetQuat( quat );
			SkipSpaces();
		}
	}

	return result;
}

IFXRESULT FileScanner::ScanPoint( Point* pPoint )
{
	IFXRESULT result = IFX_OK;

	F32 x = 0.0f, y = 0.0f, z = 0.0f;

	result = ScanFloat( &x );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &y );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &z );

	if( IFXSUCCESS( result ) )
	{
		IFXVector3 point( x, y, z );
		pPoint->SetPoint( point );
		SkipSpaces();
	}

	return result;
}

IFXRESULT FileScanner::ScanVector4( IFXVector4* pVector4 )
{
	IFXRESULT result = IFX_OK;

	F32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

	result = ScanFloat( &x );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &y );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &z );

	if( IFXSUCCESS( result ) )
		result = ScanFloat( &w );

	if( IFXSUCCESS( result ) )
	{
		pVector4->Set( x, y, z, w );
		SkipSpaces();
	}

	return result;
}

IFXRESULT FileScanner::ScanInt3( Int3* pData )
{
	IFXRESULT result = IFX_OK;

	I32 x = 0, y = 0, z = 0;

	result = ScanInteger( &x );

	if( IFXSUCCESS( result ) )
		result = ScanInteger( &y );

	if( IFXSUCCESS( result ) )
		result = ScanInteger( &z );

	if( IFXSUCCESS( result ) )
	{
		pData->SetData( x, y, z );
		SkipSpaces();
	}

	return result;
}

IFXRESULT FileScanner::ScanInt2( Int2* pData )
{
	IFXRESULT result = IFX_OK;

	I32 x = 0, y = 0;

	result = ScanInteger( &x );

	if( IFXSUCCESS( result ) )
		result = ScanInteger( &y );

	if( IFXSUCCESS( result ) )
	{
		pData->SetData( x, y );
		SkipSpaces();
	}

	return result;
}

IFXRESULT FileScanner::ScanToken( const IFXCHAR* pToken )
{
	// try to use fscanf
	IFXRESULT result = IFX_OK;

	if( NULL != pToken )
	{
		if( TRUE == m_used )
		{
			// previous token was successfuly used and we can scan next
			SkipSpaces();

			if( TRUE == IsEndOfFile() )
				result = IFX_E_EOF;
			else
			{
				U8 buffer[MAX_STRING_LENGTH];
				U32 i = 0;

				if( IDTF_END_BLOCK != GetCurrentCharacter() )
				{
					while( ( 0 == IsSpace( GetCurrentCharacter() ) ) && 
						   !IsEndOfFile() && 
						   i < MAX_STRING_LENGTH
					     )
					{
						buffer[i++] = GetCurrentCharacter();
						NextCharacter();
					}
					buffer[i] = 0;
					/// @todo: Converter unicode support
					m_currentToken.Assign( buffer );
				}
				else
				{
					// block terminator found
					// do not allow client to continue scan for new tokens
					m_used = FALSE;
				}
			}
		}

		/// @todo: Converter Unicode support
		// convert one byte token to unicode
		IFXString token( pToken );

		if( m_currentToken != token )
		{
			m_used = FALSE;
			result = IFX_E_TOKEN_NOT_FOUND;
		}
		else
			m_used = TRUE;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanStringToken( const IFXCHAR* pToken, IFXString* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanString( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanIntegerToken( const IFXCHAR* pToken, I32* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanInteger( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanHexToken( const IFXCHAR* pToken, I32* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanHex( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanFloatToken( const IFXCHAR* pToken, F32* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanFloat( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanTMToken( const IFXCHAR* pToken, IFXMatrix4x4* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = FindBlockStarter();

		if( IFXSUCCESS( result ) )
			result = ScanTM( pValue );

		if( IFXSUCCESS( result ) )
			result = FindBlockTerminator();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanColorToken( const IFXCHAR* pToken, Color* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanColor( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanQuatToken( const IFXCHAR* pToken, Quat* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanQuat( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::ScanPointToken( const IFXCHAR* pToken, Point* pValue )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pToken && NULL != pValue )
	{
		result = ScanToken( pToken );

		if( IFXSUCCESS( result ) )
			result = ScanPoint( pValue );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT FileScanner::FindBlockStarter()
{
	IFXRESULT result = IFX_OK;

	SkipSpaces();

	if( TRUE == IsEndOfFile() )
		result = IFX_E_EOF;
	else
	{
		if( GetCurrentCharacter() == IDTF_BEGIN_BLOCK )
		{
			NextCharacter();
			SkipSpaces();
		}
		else
			result = IFX_E_STARTER_NOT_FOUND;
	}

	return result;
}

IFXRESULT FileScanner::FindBlockTerminator()
{
	IFXRESULT result = IFX_OK;

	SkipSpaces();

	if( TRUE == IsEndOfFile() )
		result = IFX_E_EOF;
	else
	{
		if( GetCurrentCharacter() == IDTF_END_BLOCK )
		{
			// block terminator found
			// allow client to scan for next token
			m_used = TRUE;
			NextCharacter();
		}
		else
			result = IFX_E_TERMINATOR_NOT_FOUND;
	}

	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

BOOL FileScanner::IsSpace( I8 character )
{
	return isspace( character );
}

BOOL FileScanner::IsEndOfFile()
{
	return m_file.IsEndOfFile();
}

void FileScanner::SkipSpaces()
{
	while( 0 != isspace( GetCurrentCharacter() ) && !m_file.IsEndOfFile() )
		NextCharacter();
}

void FileScanner::SkipBlanks()
{
	while( ( ' ' == GetCurrentCharacter() || '\t' == GetCurrentCharacter() )
		&& !m_file.IsEndOfFile() )
		NextCharacter();
}

U8 FileScanner::NextCharacter()
{
	return m_currentCharacter[0] = m_file.ReadCharacter();
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
