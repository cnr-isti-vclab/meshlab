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
@file BlockParser.h

This header defines the ... functionality.

@note
*/


#ifndef BlockParser_H
#define BlockParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "MetaDataParser.h"

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

	/**
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class BlockParser : public MetaDataParser
	{
	public:
		BlockParser( FileScanner* pFileScanner );
		virtual ~BlockParser();

		/**
		Parse block starter
		*/
		IFXRESULT ParseStarter();

		/**
		Parse block terminator
		*/
		IFXRESULT ParseTerminator();

		/**
		Parse block terminator
		*/
		IFXRESULT BlockEnd();

		/**
		Parse block starter with name
		*/
		IFXRESULT BlockBegin( const IFXCHAR* pBlockToken );

		/**
		Parse block starter with name and string parameter
		*/
		IFXRESULT BlockBegin( const IFXCHAR* pBlockToken, IFXString* pValue );

		/**
		Parse block starter with name and integer parameter
		*/
		IFXRESULT BlockBegin( const IFXCHAR* pBlockToken, I32* pValue );

	private:
		BlockParser();

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


	//***************************************************************************
	//  Failure return codes
	//***************************************************************************

	/**
	@todo:  Insert module/interface specific return code description.
	*/

}

#endif
