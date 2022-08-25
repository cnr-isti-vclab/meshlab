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
  @file IFXCoreDllMain.cpp

    This module defines for IFXCore the main MacOSX DL entry point.  It
    should be placed in the Platform/Mac32/IFXCore folder.
*/


//***************************************************************************
//  Includes
//***************************************************************************


#ifndef STRICT
#define STRICT  1
#endif

#include "IFXResult.h"
#include "IFXAPI.h"
#include "IFXDebug.h"

//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Global function prototypes (not exposed to the client)
//***************************************************************************


extern IFXRESULT IFXAPI_CALLTYPE IFXSchedulingStartup();
extern IFXRESULT IFXAPI_CALLTYPE IFXSchedulingShutdown();


//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
  This is the main DLL entry point.  It performs DLL startup and shutdown
  activities.

*/
void __attribute((constructor)) PluginStartUp( void )
{
    IFXRESULT result = IFXSchedulingStartup();

  if( IFXFAILURE( result ) )
    IFXTRACE_GENERIC(L"IFXScheduling failed to start up successfully\n");
}

void __attribute((destructor)) PluginShutDown( void )
{
    IFXRESULT result = IFXSchedulingShutdown();

  if( IFXFAILURE( result ) )
    IFXTRACE_GENERIC(L"IFXScheduling failed to shut down successfully\n");
}
