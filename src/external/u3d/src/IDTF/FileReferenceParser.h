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
  @file FileReferenceParser.h

      This header defines the file reference parser functionality.

  @note
*/


#ifndef FileReferenceParser_H
#define FileReferenceParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "BlockParser.h"

namespace U3D_IDTF
{
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

class FileScanner;
class FileReference;
class Filter;

/**
This is the implementation of a class that is used to parse file reference.
*/
class FileReferenceParser : public BlockParser
{
public:
	FileReferenceParser( 
				FileScanner* pFileScanner, 
				FileReference* pFileReference );
	virtual ~FileReferenceParser();

	/**
		Parse file reference
	*/
	IFXRESULT Parse();

private:
	FileReferenceParser();

	IFXRESULT ParseFilter( Filter& rFilter );
	IFXRESULT ParseFilterList();

	FileScanner* m_pScanner;
	FileReference* m_pFileReference;
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
