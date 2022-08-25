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
  @file ConverterResult.h

      This header defines the ... functionality.

  @note
*/


#ifndef ConverterResult_H
#define ConverterResult_H


//***************************************************************************
//  Includes
//***************************************************************************

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


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Failure return codes
//***************************************************************************

#define IFX_E_WRONG_FORMAT			MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0000 )
#define IFX_E_WRONG_VERSION_NUMBER  MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0001 )

#define IFX_E_TOKEN_NOT_FOUND		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0002 )
#define IFX_E_STRING_NOT_FOUND		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0003 )
#define IFX_E_INT_NOT_FOUND			MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0004 )
#define IFX_E_FLOAT_NOT_FOUND		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0005 )
#define IFX_E_EOF					MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0006 )
#define IFX_E_STARTER_NOT_FOUND		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0007 )
#define IFX_E_TERMINATOR_NOT_FOUND	MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0008 )

#define IFX_E_SCENE_DATA_NOT_FOUND	MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0009 )

#define IFX_E_UNDEFINED_RESOURCE	MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0010 )
#define IFX_E_UNKNOWN_LIGHT_TYPE	MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0011 )
#define IFX_E_UNKNOWN_VIEW_TYPE		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0012 )
#define IFX_E_UNKNOWN_MODEL_TYPE	MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0013 )
#define IFX_E_UNKNOWN_NODE_TYPE		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0014 )	
#define IFX_E_UNKNOWN_MODIFIER_TYPE		MAKE_IFXRESULT_FAIL( IFXRESULT_CONVERTERS, 0x0015 )	

}

#endif
