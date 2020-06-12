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
	@file	File.h

			This header defines the file handling functionality.
*/


#ifndef File_H
#define File_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************

const U32 MAXIMUM_FILENAME_LENGTH = 512;

//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

/**
	This is the implementation of a class that is used to:
	- load file
	- creates and initializes scanner
*/
class File
{
public:
	File();
	virtual ~File();

	IFXRESULT Initialize( const IFXCHAR* pFileName );
	IFXRESULT Open();
	IFXRESULT Close();
	BOOL IsEndOfFile();
	U8 ReadCharacter();
	IFXRESULT GetPosition( U32* pFilePos );
	IFXRESULT SetPosition( U32 filePos );

private:
	const IFXCHAR* m_pFileName;
	FILE* m_pFile;
};

//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}


#endif
