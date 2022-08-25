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
	@file IFXRendering.cpp

	This module contains the plug-in specific functions required to
	be exported from the DL by the IFXCOM component system.  In
	addition, it also provides common cross-platform startup and
	shutdown functionality for the DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXPlugin.h"
#include "IFXRenderingCIDs.h"
#include "IFXMemory.h"
#include "IFXOSUtilities.h"

#include "IFXRenderPCH.h"
#include "CIFXRenderDevice.h"
#include "OpenGL/CIFXOpenGLOS.h"

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


extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderServicesFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderContextFactory( IFXREFIID interfaceId, void** ppInterface );

#if defined(MAC32) || defined(LINUX) || defined( __linux__ ) || defined( __APPLE__ )
extern IFXRESULT IFXAPI_CALLTYPE CIFXOpenGLOSFactory( IFXREFIID interfaceId, void** ppInterface );
#endif

#ifdef WIN32
extern IFXRESULT IFXAPI_CALLTYPE CIFXDirectX8Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDirectX7Factory( IFXREFIID interfaceId, void** ppInterface );

extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderDX8Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderDX7Factory( IFXREFIID interfaceId, void** ppInterface );
#endif

extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderNULLFactory( IFXREFIID interfaceId, void** ppInterface );

extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderOGLFactory( IFXREFIID interfaceId, void** ppInterface );

#ifdef WIN32
extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceDX8Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceDX7Factory( IFXREFIID interfaceId, void** ppInterface );
#endif

extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceNULLFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceOGLFactory( IFXREFIID interfaceId, void** ppInterface );

#ifdef WIN32
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightDX8Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureDX8Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitDX8Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightDX7Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureDX7Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitDX7Factory( IFXREFIID interfaceId, void** ppInterface );
#endif

extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightNULLFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureNULLFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitNULLFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceLightOGLFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureOGLFactory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitOGLFactory( IFXREFIID interfaceId, void** ppInterface );

extern IFXRESULT IFXAPI_CALLTYPE CIFXSceneGraphCuller_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXOctreeCollection_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXOctreeCuller_Factory( IFXREFIID interfaceId, void** ppInterface );


//***************************************************************************
//  Global data
//***************************************************************************


/**
	Count of active objects.
	This counter is used by IFXPluginCanUnloadNow and defined in the 
	IFXCorePluginStatic.cpp.
	If this counter equals 0 it means there is no active plug-in objects
	and plug-in can be successfully unloaded.
*/
extern U32 g_countActiveObjects;
// extern U32 g_countActiveRenderingObjects;


//***************************************************************************
//  Local data
//***************************************************************************


/**
	List of ComponentDescriptor structures for each IFXCOM component
	exposed by the plug-in module.

	@note Look at IFXPlugin.h for IFXComponentDescriptor declaration.
*/
static IFXComponentDescriptor gs_componentDescriptorList[] =
{
	{
		&CID_IFXDeviceLightNULL,
		{CIFXDeviceLightNULLFactory},
		1
	},
	{
		&CID_IFXDeviceTextureNULL,
		{CIFXDeviceTextureNULLFactory},
		1
	},
	{
		&CID_IFXDeviceTexUnitNULL,
		{CIFXDeviceTexUnitNULLFactory},
		1
	},
	{
		&CID_IFXRenderNULL,
		{CIFXRenderNULLFactory},
		1
	},
	{
		&CID_IFXRenderDeviceNULL,
		{CIFXRenderDeviceNULLFactory},
		1
	},
#ifdef WIN32
	{
		&CID_IFXDeviceLightDX8,
		{CIFXDeviceLightDX8Factory},
		1
	},
	{
		&CID_IFXDeviceTextureDX8,
		{CIFXDeviceTextureDX8Factory},
		1
	},
	{
		&CID_IFXDeviceTexUnitDX8,
		{CIFXDeviceTexUnitDX8Factory},
		1
	},
	{
		&CID_IFXDeviceLightDX7,
		{CIFXDeviceLightDX7Factory},
		1
	},
	{
		&CID_IFXDeviceTextureDX7,
		{CIFXDeviceTextureDX7Factory},
		1
	},
	{
		&CID_IFXDeviceTexUnitDX7,
		{CIFXDeviceTexUnitDX7Factory},
		1
	},
	{
		&CID_IFXDirectX8,
		{CIFXDirectX8Factory},
		1
	},
	{
		&CID_IFXDirectX7,
		{CIFXDirectX7Factory},
		1
	},
	{
		&CID_IFXRenderDX8,
		{CIFXRenderDX8Factory},
		1
	},
	{
		&CID_IFXRenderDeviceDX8,
		{CIFXRenderDeviceDX8Factory},
		1
	},
	{
		&CID_IFXRenderDX7,
		{CIFXRenderDX7Factory},
		1
	},
	{
		&CID_IFXRenderDeviceDX7,
		{CIFXRenderDeviceDX7Factory},
		1
	},
#endif // WIN32
	{
		&CID_IFXOpenGL,
		{CIFXOpenGLOSFactory},
		1
	},
	{
		&CID_IFXDeviceLightOGL,
		{CIFXDeviceLightOGLFactory},
		1
	},
	{
		&CID_IFXDeviceTextureOGL,
		{CIFXDeviceTextureOGLFactory},
		1
	},
	{
		&CID_IFXDeviceTexUnitOGL,
		{CIFXDeviceTexUnitOGLFactory},
		1
	},
	{
		&CID_IFXRenderOGL,
		{CIFXRenderOGLFactory},
		1
	},
	{
		&CID_IFXRenderDeviceOGL,
		{CIFXRenderDeviceOGLFactory},
		1
	},

	{
		&CID_IFXRenderContext,
		{CIFXRenderContextFactory},
		1
	},
	{
		&CID_IFXRenderServices,
		{CIFXRenderServicesFactory},
		1
	},
	{
		&CID_IFXOctreeCollection,
		{CIFXOctreeCollection_Factory},
		1
	},
	{
		&CID_IFXSceneGraphCuller,
		{CIFXSceneGraphCuller_Factory},
		1
	},
	{
		&CID_IFXOctreeCuller,
		{CIFXOctreeCuller_Factory},
		1
	}
};


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This function should be invoked by dynamic library initialization
	function.

	@note For Windows this is the DLL_PROCESS_ATTACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXRenderingStartup()
{
	IFXRESULT result = IFX_OK;

	// Initialize persistent global data.

	// TODO: put your code for initialization here
	IFXOSInitialize();

	IFXDEBUG_STARTUP();

	return result;
}

//---------------------------------------------------------------------------
/**
	This function should be invoked by dynamic library uninitialization
	function.

	@note For Windows this is the DLL_PROCESS_DETACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXRenderingShutdown()
{
	IFXRESULT result = IFX_OK;

	// Dispose of persistent global data
	CIFXRenderDevice::Shutdown();

	CIFXOpenGLOS::Shutdown();

	// TODO: put your code for uninitialization here
	IFXDEBUG_SHUTDOWN();

	IFXOSUninitialize();

	return result;
}


//***************************************************************************
// Exported functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This function provides registration information about plug-in components
	to IFXCOM component system.

	@param  pComponentNumber	Pointer to the number of components in
								component descriptor list.

	@param  ppComponentDescriptorList
								Pointer to the component descriptor list
								array (or pointer to the first element pointer
								of component descriptor list).
	@return Upon success, it returns the value IFX_OK. If input pointers
	were not defined, it will return the value IFX_E_PARAMETER_NOT_INITIALIZED.
*/
extern "C"
IFXRESULT IFXAPI IFXPluginRegister(
								   U32* pComponentNumber,
								   IFXComponentDescriptor** ppComponentDescriptorList )
{
	IFXRESULT result = IFX_OK;

	if( 0 != pComponentNumber && 0 != ppComponentDescriptorList )
	{
		*pComponentNumber =
			sizeof(gs_componentDescriptorList)/sizeof(IFXComponentDescriptor);
		*ppComponentDescriptorList = gs_componentDescriptorList;
	}
	else
		result = IFX_E_PARAMETER_NOT_INITIALIZED;

	return result;
}

//---------------------------------------------------------------------------
/**
	This function is used to let component system know if plug-in can be
	unloaded now or not.

	@return If plug-in can be unloaded, it will return the value IFX_OK.
	Otherwise, it will return the value IFX_E_NOT_DONE.
*/
extern "C"
IFXRESULT IFXAPI IFXPluginCanUnloadNow()
{
	IFXRESULT result = IFX_OK;

	if( 0 != g_countActiveObjects )
//	if( 0 != g_countActiveRenderingObjects )
		result = IFX_E_NOT_DONE;

	return result;
}
