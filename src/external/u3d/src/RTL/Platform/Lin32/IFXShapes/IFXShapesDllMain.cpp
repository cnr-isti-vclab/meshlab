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
@file IFXShapesDllMain.cpp

This module defines for IFXShapes the main Windows DL entry
point.  It should be placed in the Platform\IFXShapes\Win32
folder.
*/


//***************************************************************************
//  Includes
//***************************************************************************


#ifndef STRICT
#define STRICT  1
#endif

#include "IFXAPI.h"
#include "IFXResult.h"
#include "IFXDebug.h"

//***************************************************************************
//  Global function prototypes (not exposed to the client)
//***************************************************************************


extern IFXRESULT IFXAPI_CALLTYPE IFXShapesStartup();
extern IFXRESULT IFXAPI_CALLTYPE IFXShapesShutdown();


//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the main DLL entry point.  It performs DLL startup and shutdown
	activities.

	@return Upon success, TRUE is returned.  Otherwise, FALSE is returned.
	The Win32 OS only acts upon the return value if the reason is
	DLL_PROCESS_ATTACH.
*/
void __attribute((constructor)) PluginStartUp( void )
{
    IFXRESULT result = IFXShapesStartup();
	
	if( IFXFAILURE( result ) )
		IFXTRACE_GENERIC(L"IFXCore failed to start up successfully");
}

void __attribute((destructor)) PluginShutDown( void )
{
    IFXRESULT result = IFXShapesShutdown();
	
	if( IFXFAILURE( result ) )
		IFXTRACE_GENERIC(L"IFXCore failed to shut down successfully");
}
