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
	@file IFXExporting.cpp

	This module contains the plug-in specific functions required to
	be exported from the DL by the IFXCOM component system.  In
	addition, it also provides common cross-platform startup and
	shutdown functionality for the DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXExportingCIDs.h"
#include "IFXMemory.h"
#include "IFXOSUtilities.h"
#include "IFXPlugin.h"

extern IFXRESULT IFXAPI_CALLTYPE CIFXAuthorGeomCompiler_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODEncoderX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXMaterialResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXMotionResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXGlyphGeneratorEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXShaderLitTextureEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXSubdivisionModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXAnimationModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXGroupNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLightNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLightResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXModelNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXViewNodeEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXViewResourceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXFileReferenceEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBlockPriorityQueueX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBlockWriterX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXWriteManager_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXStdioWriteBufferX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXPointSetEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLineSetEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBoneWeightsModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifierEncoder_Factory( IFXREFIID interfaceId, void** ppInterface );

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
// extern U32 g_countActiveExportingObjects;

//***************************************************************************
//  Local data
//***************************************************************************

/**
	gs_componentDescriptorList

	List of ComponentDescriptor structures for each IFXCOM component
	exposed by the plug-in module.
	Look at the IFXPlugin.h for IFXComponentDescriptor declaration.
*/
static IFXComponentDescriptor gs_componentDescriptorList[] =
{
	{
		&CID_IFXAuthorCLODEncoderX,
		{CIFXAuthorCLODEncoderX_Factory},
		1
	},
	{
		&CID_IFXAnimationModifierEncoder,
		{CIFXAnimationModifierEncoder_Factory},
		1
	},
	{
		&CID_IFXMaterialResourceEncoder,
		{CIFXMaterialResourceEncoder_Factory},
		1
	},
	{
		&CID_IFXMotionResourceEncoder,
		{CIFXMotionResourceEncoder_Factory},
		1
	},
	{
		&CID_IFXGlyphGeneratorEncoder,
		{CIFXGlyphGeneratorEncoder_Factory},
		1
	},
	{
		&CID_IFXShaderLitTextureEncoder,
		{CIFXShaderLitTextureEncoder_Factory},
		1
	},
	{
		&CID_IFXSubdivisionModifierEncoder,
		{CIFXSubdivisionModifierEncoder_Factory},
		1
	},
	{
		&CID_IFXShadingModifierEncoder,
		{CIFXShadingModifierEncoder_Factory},
		1
	},
	{
		&CID_IFXGroupNodeEncoder,
		{CIFXGroupNodeEncoder_Factory},
		1
	},
	{
		&CID_IFXLightNodeEncoder,
		{CIFXLightNodeEncoder_Factory},
		1
	},
	{
		&CID_IFXLightResourceEncoder,
		{CIFXLightResourceEncoder_Factory},
		1
	},
	{
		&CID_IFXViewResourceEncoder,
		{CIFXViewResourceEncoder_Factory},
		1
	},
	{
		&CID_IFXModelNodeEncoder,
		{CIFXModelNodeEncoder_Factory},
		1
	},
	{
		&CID_IFXViewNodeEncoder,
		{CIFXViewNodeEncoder_Factory},
		1
	},
	{
		&CID_IFXFileReferenceEncoder,
		{CIFXFileReferenceEncoder_Factory},
		1
	},
	{
		&CID_IFXBlockPriorityQueueX,
		{CIFXBlockPriorityQueueX_Factory},
		1
	},
	{
		&CID_IFXBlockWriterX,
		{CIFXBlockWriterX_Factory},
		1
	},
	{
		&CID_IFXWriteManager,
		{CIFXWriteManager_Factory},
		1
	},
	{
		&CID_IFXAuthorGeomCompiler,
		{CIFXAuthorGeomCompiler_Factory},
		1
	},
	{
		&CID_IFXStdioWriteBuffer,
		{CIFXStdioWriteBufferX_Factory},
		1
	},
	{
		&CID_IFXStdioWriteBufferX,
		{CIFXStdioWriteBufferX_Factory},
		1
	},
	{
		&CID_IFXPointSetEncoder,
		{CIFXPointSetEncoder_Factory},
		1
	},
	{
		&CID_IFXLineSetEncoderX,
		{CIFXLineSetEncoder_Factory},
		1
	},
	{
		&CID_IFXBoneWeightsModifierEncoder,
		{CIFXBoneWeightsModifierEncoder_Factory},
		1
	},
	{
		&CID_IFXCLODModifierEncoder,
		{CIFXCLODModifierEncoder_Factory},
		1
	},
	{
		&CID_IFXDummyModifierEncoder,
		{CIFXDummyModifierEncoder_Factory},
		1
	}
};


//***************************************************************************
//  Global functions
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This function should be invoked by dynamic library initialization
	function.

	@note For Windows this is the DLL_PROCESS_ATTACH section of DllMain.
*/
IFXRESULT IFXAPI_CALLTYPE IFXExportingStartup()
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
IFXRESULT IFXAPI_CALLTYPE IFXExportingShutdown()
{
	IFXRESULT result = IFX_OK;

	// Dispose of persistent global data

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
	to the IFXCOM.
	Input parameters for this functions should be defined in the caller
	function.

	@param  pComponentNumber	Pointer to the number of components in
								component descriptor list.

	@param  ppComponentDescriptorList
								Pointer to the component descriptor list
								array (or pointer to the first element pointer
								of component descriptor list).
	@return Upon success, it returns the valuse IFX_OK. If input pointers
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
//	if( 0 != g_countActiveExportingObjects )
		result = IFX_E_NOT_DONE;

	return result;
}
