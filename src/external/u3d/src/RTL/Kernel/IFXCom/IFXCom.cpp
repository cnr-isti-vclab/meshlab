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
@file IFXCOM.cpp

This module implements the core IFXCOM functionality that's
exported for the client and plug-in DLs to use.  In addition, it
also provides common cross-platform startup and shutdown
functionality for the DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************


#include "CIFXComponentManager.h"
#include "IFXDebug.h"
#include "IFXOSUtilities.h"
#include "CIFXInterleavedData.h"
#include "CIFXTextureObject.h"
#include "CIFXModel.h"

//***************************************************************************
//  Global data
//***************************************************************************


static CIFXComponentManager* gs_pComponentManager = 0;
extern U32 g_countActiveObjects;
// extern const size_t maxObjects;
// extern size_t numObjects;
// extern void* objects[];


//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/*
Creates IFXComponentManager, which manages all actions with components.
*/
extern "C"
IFXRESULT IFXAPI IFXCOMInitialize()
{
	IFXRESULT result = IFX_OK;

	if( 0 == gs_pComponentManager )
	{
		gs_pComponentManager = new CIFXComponentManager;
		if( 0 != gs_pComponentManager )
		{
			gs_pComponentManager->AddRef();
			result = gs_pComponentManager->Initialize();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
	{
		// this is not the first call, so AddRef and return false
		gs_pComponentManager->AddRef();
	}

	return result;
}

//---------------------------------------------------------------------------
/*
Releases IFXComponentManager, which release all active plug-in components,
unloads all plug-ins, frees any other resources
*/
extern "C"
IFXRESULT IFXAPI IFXCOMUninitialize()
{
	IFXRESULT result = IFX_OK;

	// release IFXComponentManager
	if( 0 != gs_pComponentManager )
	{
		result = gs_pComponentManager->UnloadAllPlugins();
		//IFXASSERTBOX( result == IFX_OK,
		//  "IFXCOMUninitialize - some plug-in cannot be unloaded" );

		if( 0 == gs_pComponentManager->Release() )
		{
			gs_pComponentManager = 0;
		}
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
/*
Uses IFXComponentManager to create component.
*/
extern "C"
IFXRESULT IFXAPI IFXCreateComponent( IFXREFCID  rComponentId,
									IFXREFIID  rInterfaceId,
									void**   ppInterface )
{
	IFXRESULT result = IFX_OK;

	if( 0 != gs_pComponentManager && 0 != ppInterface )
	{
		result = gs_pComponentManager->CreateComponent( rComponentId,
			rInterfaceId,
			ppInterface);
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
/*
Uses IFXComponentManager to register component.
*/
extern "C"
IFXRESULT IFXAPI IFXRegisterComponent(
									  IFXComponentDescriptor* pComponentDescriptor )
{
	IFXRESULT result = IFX_OK;

	if( 0 != gs_pComponentManager )
	{
		if( 0 != pComponentDescriptor )
			result = gs_pComponentManager->RegisterComponent( pComponentDescriptor );
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

/*
Uses IFXComponentManager to register DIDs.
*/
extern "C"
IFXRESULT IFXAPI IFXGetPluginsDids( IFXArray<IFXDID*> *&pDidsList )
{
	IFXRESULT result = IFX_OK;

	if( 0 != gs_pComponentManager )
	{
		pDidsList = gs_pComponentManager->GetPluginsDids();
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
/**
This function should be invoked by dynamic library initialization
function.

@note For Windows this is the DLL_PROCESS_ATTACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXCoreStartup()
{
	IFXRESULT result = IFX_OK;

	IFXOSInitialize();

	IFXDEBUG_STARTUP();

	// Initialize persistent global data.

	// TODO: put your code for initialization here

	return result;
}

//---------------------------------------------------------------------------
/**
This function should be invoked by dynamic library uninitialization
function.

@note For Windows this is the DLL_PROCESS_DETACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXCoreShutdown()
{
	IFXRESULT result = IFX_OK;

	if( 0 != g_countActiveObjects )
		result = IFX_E_NOT_DONE;

//	for(size_t i=0; i<numObjects; i++)
//		if (objects[i])
//			fprintf(stderr,"%zu %p\n",i, objects[i]);

	// Dispose of persistent global data

	// TODO: put your code for uninitialization here
	CIFXInterleavedData::Shutdown();
	CIFXTextureObject::Shutdown();
	CIFXModel::Shutdown();

	IFXDEBUG_SHUTDOWN();

	IFXOSUninitialize();

	return result;
}
