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
	@file	IFXTextureModifierDllMain.cpp

			This module defines for IFXTextureModifier the main Windows 
			DL entry point.  It should be placed in the 
			Platform\IFXTextureModifier\Win32 folder.
*/


//***************************************************************************
//	Includes
//***************************************************************************


#include <windows.h>

#include "IFXAPI.h"
#include "IFXResult.h"


//***************************************************************************
//  Global function prototypes (not exposed to the client)
//***************************************************************************


extern IFXRESULT IFXAPI_CALLTYPE IFXTextureModifierPluginStartup();
extern IFXRESULT IFXAPI_CALLTYPE IFXTextureModifierPluginShutdown();


//***************************************************************************
//	Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This is the main DLL entry point.  It performs DLL startup and shutdown
	activities.

	@return	Upon success, TRUE is returned.  Otherwise, FALSE is returned.  
			The Win32 OS only acts upon the return value if the reason is 
			DLL_PROCESS_ATTACH.
*/
extern "C"
BOOL WINAPI DllMain(	HINSTANCE	hInstance,
                        DWORD		reason,
                        LPVOID		pReserved )
{
	IFXRESULT	result	= IFX_OK;

    if ( reason == DLL_PROCESS_ATTACH )
    {
		DisableThreadLibraryCalls( hInstance );

		result = IFXTextureModifierPluginStartup();
    }
    else if ( reason == DLL_PROCESS_DETACH )
    {
		result = IFXTextureModifierPluginShutdown();
    }

	return ( IFXSUCCESS( result ) ? TRUE : FALSE );
}
