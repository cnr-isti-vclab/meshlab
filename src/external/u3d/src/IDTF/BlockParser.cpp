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
  @file BlockParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Includes
//***************************************************************************

#include "BlockParser.h"
#include "FileScanner.h"
#include "IFXCheckX.h"

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

BlockParser::BlockParser( FileScanner* pFileScanner ) :
	MetaDataParser( pFileScanner ), m_pScanner( pFileScanner )
{
}

BlockParser::~BlockParser()
{
	m_pScanner = NULL;
}


IFXRESULT BlockParser::ParseStarter()
{
	return m_pScanner->FindBlockStarter();
}

IFXRESULT BlockParser::ParseTerminator()
{
	return m_pScanner->FindBlockTerminator();
}

IFXRESULT BlockParser::BlockEnd()
{
	return m_pScanner->FindBlockTerminator();
}

IFXRESULT BlockParser::BlockBegin( const IFXCHAR* pBlockToken )
{
	IFXRESULT result = IFX_OK;

	result = m_pScanner->ScanToken( pBlockToken );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->FindBlockStarter();

	return result;
}

IFXRESULT BlockParser::BlockBegin( const IFXCHAR* pBlockToken, IFXString* pValue )
{
	IFXRESULT result = IFX_OK;

	result = m_pScanner->ScanStringToken( pBlockToken, pValue );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->FindBlockStarter();

	return result;
}

IFXRESULT BlockParser::BlockBegin( const IFXCHAR* pBlockToken, I32* pValue )
{
	IFXRESULT result = IFX_OK;

	result = m_pScanner->ScanIntegerToken( pBlockToken, pValue );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->FindBlockStarter();


	return result;
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
