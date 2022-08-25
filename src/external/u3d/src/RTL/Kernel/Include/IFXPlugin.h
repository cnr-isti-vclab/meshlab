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
	@file	IFXPlugin.h

			This header defines the functions that need to be exported from each
			IFXCOM plug-in DLL. It also defines the type of the common factory
			function that is implemented for each supported IFXCOM component.

			Extra method to support registration of IFXCOM components need to be
			exposed by plug-in DLLs.
*/

#ifndef IFXPLUGIN_H
#define IFXPLUGIN_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXAPI.h"
#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXGUID.h"
#include "IFXDebug.h"

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
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

typedef IFXRESULT (IFXAPI_CALLTYPE *IFXFactoryFunction)( 
											IFXREFIID interfaceId,
			   								void**	  ppInterface );

typedef IFXRESULT (IFXAPI_CALLTYPE *IFXCLIFactoryFunction )( 
											IFXREFCID componentId,
											IFXREFIID interfaceId, 
											void** ppInterface );

/**
Structure that contains a full component description.  It's useful for
registering, unregistering and creating a component.
*/
struct IFXComponentDescriptor
{
	const IFXCID* pComponentId; ///< Pointer to the unique identifier.
	union 
	{
		IFXFactoryFunction pFactoryFunction;
		IFXCLIFactoryFunction pCLIFactoryFunction;
	}; ///< Pointer to the factory function.
	I32 Version;  ///< The Component Version Number (always greater than zero)
};

/**
Structure that contains a full category description.
*/
struct IFXCategoryDescriptor
{
	const IFXPID* pPlugPointId; // Pointer to the Plugin Category identifier.
	const IFXCID* pComponentId; // Pointer to the unique identifier.
};

typedef IFXRESULT ( IFXAPI_CALLTYPE *IFXPluginRegisterFunction )(
						U32* pNumberOfComponents,
						IFXComponentDescriptor** pComponentDescriptorList);

typedef IFXRESULT ( IFXAPI_CALLTYPE *IFXPluginRegisterDidsFunction )(
						U32* pNumberOfDids,
						IFXDID** ppDidsList);

typedef IFXRESULT ( IFXAPI_CALLTYPE *IFXPluginCanUnloadNowFunction )();


//***************************************************************************
//  Inline functions
//***************************************************************************

#endif
