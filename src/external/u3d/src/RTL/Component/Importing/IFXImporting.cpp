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
	@file IFXImporting.cpp

	This module contains the plug-in specific functions required to
	be exported from the DL by the IFXCOM component system.  In
	addition, it also provides common cross-platform startup and
	shutdown functionality for the DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXMemory.h"
#include "IFXPlugin.h"
#include "IFXImportingCIDs.h"
#include "IFXExportingCIDs.h"
#include "IFXSchedulingCIDs.h"
#include "IFXCoreCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXOSUtilities.h"

extern IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXGroupDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLightDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLightResourceDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXMaterialDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXModelDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXAnimationModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXMotionDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXGlyphGeneratorDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXShaderLitTextureDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXTextureDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXSubdivisionModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDecoderChainX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXViewResourceDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXViewDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBlockReaderX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBTTHash_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLoadManager_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXNameMap_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXStdioReadBufferX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXInternetReadBufferX_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXPointSetDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXLineSetDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXBoneWeightsModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXCLODModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );
extern IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifierDecoder_Factory( IFXREFIID interfaceId, void** ppInterface );

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
// extern U32 g_countActiveImportingObjects;

//***************************************************************************
//  Local data
//***************************************************************************

/**
	gs_componentDescriptorList

	List of ComponentDescriptor structures for each IFXCOM component
	exposed by the plug-in module.
	Look at the IFXPlugin.h" for IFXComponentDescriptor declaration.
*/

static IFXComponentDescriptor gs_componentDescriptorList[] =
{
	{
		&CID_IFXAuthorCLODDecoder,
		{CIFXAuthorCLODDecoder_Factory},
		1
	},
	{
		&CID_IFXGroupDecoder,
		{CIFXGroupDecoder_Factory},
		1
	},
	{
		&CID_IFXLightDecoder,
		{CIFXLightDecoder_Factory},
		1
	},
	{
		&CID_IFXLightResourceDecoder,
		{CIFXLightResourceDecoder_Factory},
			1
	},
	{
		&CID_IFXViewResourceDecoder,
		{CIFXViewResourceDecoder_Factory},
		1
	},
	{
		&CID_IFXMaterialDecoder,
		{CIFXMaterialDecoder_Factory},
		1
	},
	{
		&CID_IFXModelDecoder,
		{CIFXModelDecoder_Factory},
		1
	},
	{
		&CID_IFXAnimationModifierDecoder,
		{CIFXAnimationModifierDecoder_Factory},
		1
	},
	{
		&CID_IFXMotionDecoder,
		{CIFXMotionDecoder_Factory},
		1
	},
	{
		&CID_IFXGlyphGeneratorDecoder,
		{CIFXGlyphGeneratorDecoder_Factory},
			1
	},
	{
		&CID_IFXShaderLitTextureDecoder,
		{CIFXShaderLitTextureDecoder_Factory},
		1
	},
	{
		&CID_IFXTextureDecoder,
		{CIFXTextureDecoder_Factory},
		1
	},
	{
		&CID_IFXSubdivisionModifierDecoder,
		{CIFXSubdivisionModifierDecoder_Factory},
		1
	},
	{
		&CID_IFXShadingModifierDecoder,
		{CIFXShadingModifierDecoder_Factory},
			1
	},
	{
		&CID_IFXDecoderChainX,
		{CIFXDecoderChainX_Factory},
			1
	},
	{
		&CID_IFXViewDecoder,
			{CIFXViewDecoder_Factory},
			1
	},
	{
		&CID_IFXBlockReaderX,
			{CIFXBlockReaderX_Factory},
			1
	},
	{
		&CID_IFXBTTHash,
		{CIFXBTTHash_Factory},
			1
	},
	{
		&CID_IFXLoadManager,
		{CIFXLoadManager_Factory},
			1
	},
	{
		&CID_IFXStdioReadBuffer,
		{CIFXStdioReadBufferX_Factory},
			1
	},
	{
		&CID_IFXStdioReadBufferX,
		{CIFXStdioReadBufferX_Factory},
			1
	},
	{
		&CID_IFXInternetReadBuffer,
			{CIFXInternetReadBufferX_Factory},
			1
	},
	{
		&CID_IFXInternetReadBufferX,
			{CIFXInternetReadBufferX_Factory},
			1
	},
	{
		&CID_IFXPointSetDecoder,
			{CIFXPointSetDecoder_Factory},
			1
	},
	{
		&CID_IFXLineSetDecoder,
			{CIFXLineSetDecoder_Factory},
			1
	},
	{
		&CID_IFXBoneWeightsModifierDecoder,
			{CIFXBoneWeightsModifierDecoder_Factory},
			1
	},
	{
		&CID_IFXCLODModifierDecoder,
			{CIFXCLODModifierDecoder_Factory},
			1
	},
	{
		&CID_IFXDummyModifierDecoder,
		{CIFXDummyModifierDecoder_Factory},
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
IFXRESULT IFXAPI_CALLTYPE IFXImportingStartup()
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
IFXRESULT IFXAPI_CALLTYPE IFXImportingShutdown()
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
//	if( 0 != g_countActiveImportingObjects )
		result = IFX_E_NOT_DONE;

	return result;
}
