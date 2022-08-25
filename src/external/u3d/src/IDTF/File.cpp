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
  @file File.cpp

      This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************



//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXCheckX.h"
#include "IFXDebug.h"
#include "IFXString.h"
#include "IFXOSLoader.h"
#include "File.h"

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

File::File()
{
	IFXTRACE_GENERIC(L"File construct\n");
	m_pFile = NULL;
	m_pFileName = NULL;
}

File::~File()
{
	Close();
	m_pFileName = NULL;
	IFXTRACE_GENERIC(L"File destruct\n");
}

IFXRESULT File::Initialize( const IFXCHAR* pFileName )
{
	IFXRESULT result = IFX_OK;

	if( NULL != pFileName )
	{
		m_pFileName = pFileName;
		result = Open();
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}

BOOL File::IsEndOfFile()
{
	return feof( m_pFile );
}

U8 File::ReadCharacter()
{
	return static_cast<U8>( fgetc( m_pFile ) ); 
}

IFXRESULT File::GetPosition( U32* pFilePos )
{
	IFXRESULT result = IFX_E_ABORTED;
#if defined(STDIO_HACK) && !defined(LIBIDTF)
	if (m_pFile==stdin) {
		*pFilePos = 0;
	} else {
		*pFilePos = ftell( m_pFile );
	}
#else
	*pFilePos = ftell( m_pFile );
#endif
	if( *pFilePos != (U32)-1 ) result = IFX_OK;
	return result;
}

IFXRESULT File::SetPosition( U32 filePos )
{
	IFXRESULT result = IFX_E_ABORTED;
	if( !fseek( m_pFile, filePos, SEEK_SET ) ) result = IFX_OK;
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT File::Open()
{
	IFXRESULT result = IFX_OK;

	m_pFile = IFXOSFileOpen( m_pFileName, L"rb" );

    if( NULL == m_pFile )
	{
		IFXTRACE_GENERIC( L"Error: cannot open file %ls\n", m_pFileName );
        result = IFX_E_INVALID_FILE;
	}

    return result;
}

IFXRESULT File::Close()
{
    IFXRESULT result = IFX_OK;

    if( NULL != m_pFile )
    {
        fclose(m_pFile);
        m_pFile = NULL;
		result = IFX_E_UNDEFINED;
    }

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
