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
  @file ConverterHelpers.h

      This header defines the ... functionality.

  @note
*/


#ifndef Converter_H
#define Converter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterOptions.h"
#include "IFXResult.h"

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


//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************

/**
Sets all input options to known defaults.

@param ConverterOptions* pConverterOptions  The reset user options

@return	void
*/
extern "C"
void SetDefaultOptionsX( 
				ConverterOptions* pConverterOptions,
				FileOptions* pFileOptions );

/**
Parses all user options

@param	int argc		The number of command-line arguments
@param	char *argv[]	The command-line itself
@param	ConverterOptions* pConverterOptions	
						The user options structure to populate

@return	IFXRESULT	The status after parsing all user options
*/
extern "C"
IFXRESULT ReadAndSetUserOptionsX( int argc, wchar_t* argv[],
								  ConverterOptions* pConverterOptions,
								  FileOptions* pFileOptions );
									 
//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
