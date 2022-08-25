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
	@file	FileScanner.h

	This header defines the FileScanner class functionality.
*/


#ifndef FileScanner_H
#define FileScanner_H


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "File.h"
#include "IFXString.h"

class IFXMatrix4x4;
class IFXVector4;

namespace U3D_IDTF
{
//***************************************************************************
//  Constants
//***************************************************************************

const U32 MAX_STRING_LENGTH = 128*256;

const U32 IDTF_INVALID = 0;
const U32 IDTF_BEGIN_BLOCK = '{';
const U32 IDTF_END_BLOCK = '}';
const U32 IDTF_BEGIN_TYPE = '[';
const U32 IDTF_END_TYPE = ']';
const U32 IDTF_UNKNOWN = 305;


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

class Quat;
class Color;
class Point;
class Int3;
class Int2;

/**
	This is the implementation of a class that is used to scan file for
	objects such as tokens, strings, colors, etc.
*/
class FileScanner
{
public:
	FileScanner();
	virtual ~FileScanner();

	IFXRESULT Initialize( const IFXCHAR* pFileName );

	IFXRESULT ScanStringToken( const IFXCHAR* token, IFXString* value );
	IFXRESULT ScanIntegerToken( const IFXCHAR* token, I32* value );
	IFXRESULT ScanHexToken( const IFXCHAR* token, I32* value );
	IFXRESULT ScanFloatToken( const IFXCHAR* token, F32* value );
	IFXRESULT ScanTMToken( const IFXCHAR* token, IFXMatrix4x4* value );
	IFXRESULT ScanColorToken( const IFXCHAR* token, Color* value );
	IFXRESULT ScanQuatToken( const IFXCHAR* token, Quat* value );
	IFXRESULT ScanPointToken( const IFXCHAR* token, Point* value );

	/**
		Scan current file for tokens.

		@param	pToken		A pointer to the output string which contains
							found token.
		@param	scanLine	If non-zero then scanning will be stopped at
							the end of line.

		@return Upon success, IFX_OK is returned.  Otherwise, one of the
		following failures are returned:  @todo: return codes.
	*/
	IFXRESULT Scan( IFXString* pToken, U32 scanLine = 0 );

	IFXRESULT ScanToken( const IFXCHAR* pToken );
	IFXRESULT ScanString( IFXString* pString );
	IFXRESULT ScanFloat( F32* pNumber );
	IFXRESULT ScanInteger( I32* pNumber );
	IFXRESULT ScanHex( I32* pNumber );
	IFXRESULT ScanTM( IFXMatrix4x4* pMatrix );
	IFXRESULT ScanColor( Color* pColor );
	IFXRESULT ScanQuat( Quat* pQuat );
	IFXRESULT ScanPoint( Point* pPoint );
	IFXRESULT ScanInt3( Int3* pInt3 );
	IFXRESULT ScanInt2( Int2* pInt2 );
	IFXRESULT ScanVector4( IFXVector4* pVector4 );

	IFXRESULT FindBlockStarter();
	IFXRESULT FindBlockTerminator();

	BOOL IsEndOfFile();

protected:
	/**
	Skip spaces.
	*/
	void SkipSpaces();
	void SkipBlanks();
	U8 NextCharacter();
	U8 GetCurrentCharacter() const;
	BOOL IsSpace( I8 character );

private:
	IFXString m_currentToken;
	BOOL m_used;
	U8 m_currentCharacter[2];
	File m_file;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE U8 FileScanner::GetCurrentCharacter() const
{
	return m_currentCharacter[0];
}

}

#endif
