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
  @file MetaDataParser.h

      This header defines the meta data parser functionality.

  @note
*/


#ifndef MetaDataParser_H
#define MetaDataParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "FileScanner.h"

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

class MetaDataList;
class MetaData;

/**
This is the implementation of a class that is used to parse meta data.
*/
class MetaDataParser
{
public:
	MetaDataParser( FileScanner* pFileScanner );
	virtual ~MetaDataParser();

	/**
		Parse meta data
	*/
	IFXRESULT ParseMetaData( MetaDataList* pMetaDataList );

private:
	MetaDataParser();
	IFXRESULT ParseMetaDataItem( MetaData* pMetaData );

	FileScanner* m_pScanner;
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
