//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXLoadManager.h

			Declaration of the IFXLoadManager interface.
			The IFXLoadManager interface is used to load a file from an IFXReadBuffer
			into the scene graph.
*/

#ifndef IFXLOADMANAGER_H
#define IFXLOADMANAGER_H

#include "IFXCoreServices.h"
#include "IFXReadBuffer.h"
#include "IFXFileReference.h"

typedef enum 
{
	IFX_LOAD_STATE_UNLOADED = 0,
	IFX_LOAD_STATE_HEADER_LOADING,
	IFX_LOAD_STATE_HEADER_LOADED,
	IFX_LOAD_STATE_MEDIA_LOADING,
	IFX_LOAD_STATE_MEDIA_LOADED,
	IFX_LOAD_STATE_ERR = -1,
	IFX_LOAD_STATE_UNINITIALIZED = -2
} EIFXLoadState;

typedef enum 
{
	IFX_LOAD_ERR_NONE = 0,			// No error
	IFX_LOAD_ERR_MEMORY,			// A memory error occurred
	IFX_LOAD_ERR_NETWORK,			// A network error occurred
	IFX_LOAD_ERR_DEVICE,			// A playback device error occurred
	IFX_LOAD_ERR_FILE_NOT_FOUND,	// Specific err for file not found
	IFX_LOAD_ERR_BAD_HEADER,		// Bad file format
	IFX_LOAD_ERR_OTHER = 99			// Some other error occurred
} EIFXLoadError;

// {916782C4-D811-11d3-9519-00A0C90AB136}
IFXDEFINE_GUID(IID_IFXLoadManager,
0x916782c4, 0xd811, 0x11d3, 0x95, 0x19, 0x0, 0xa0, 0xc9, 0xa, 0xb1, 0x36);

/**
	The IFXLoadManager interface is used to load a U3D File from an IFXReadBuffer
	into the scenegraph.
*/
class IFXLoadManager : virtual public IFXUnknown
{
public:
	/**
		The Load Manager may perform any necessary initialization procedures.

		@param	pCoreServices	The Load Manager may store the core services pointer.
								The Load Manager may use the core services pointer to
								access the scene graph, the scheduler, and the core 
								services data block queue.
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *pCoreServices) = 0;

	/**
		The Load Manager will load the information from pReadBuffer into objects in 
		the scene graph obtained from the core services and create those objects as 
		needed.

		@param	pReadBuffer		The Load Manager will start loading information from 
								pReadBuffer.
		
		@param	isAtomicLoad	If bAtomicLoad is true, the Load Manager will 
								load the entire pReadBuffer is possible.
								If bAtomicLoad is false, the Load Manager will
								attempt to limit the amout of time spent in the
								Load call; the Load call may return without 
								loading the entire pReadBuffer.
								If needed, the Load Manager will register with
								the scheduler to get additional time to complete
								the loading of the pReadBuffer.

		@param	rScope			Current name scope used for names collision 
								resolution.

		@param	collisionPolicy	Collision resolution policy to use when 
									different objects has same names.

		@param	loadId			Identifier of current LoadManager. Serves for 
								names collision resolution.
	 */
	virtual IFXRESULT IFXAPI  Load(
								IFXReadBuffer* pReadBuffer, 
								const BOOL isAtomicLoad = TRUE, 
								const IFXString& rScope = L"", 
								const IFXCollisionPolicy collisionPolicy = IFXCOLLISIONPOLICY_REPLACE, 
								const U32 loadId = 0) = 0;

	/**
		The Load Manager will load the information from pReadBuffer into objects in 
		the scene graph obtained from the core services and create those objects as 
		needed.

		@param	pReadBuffer		The Load Manager will start loading information from 
								pReadBuffer.
		
		@param	bAtomicLoad		If bAtomicLoad is true, the Load Manager will 
								load the entire pReadBuffer is possible.
								If bAtomicLoad is false, the Load Manager will
								attempt to limit the amout of time spent in the
								Load call; the Load call may return without 
								loading the entire pReadBuffer.
								If needed, the Load Manager will register with
								the scheduler to get additional time to complete
								the loading of the pReadBuffer.

		@param	sScope			Current name scope used for names collision 
								resolution.

		@param	uCollisionPolicy	Collision resolution policy to use when 
									different objects has same names.

		@param	uLoadId			Identifier of current LoadManager. Serves for 
								names collision resolution.

		@param	rObjectFilters	Set of filters to apply when objects are loaded.

		@param	rWorldAlias		Default node alias in external file.

		@param	bExternalLoad	Indicates that current file is an external 
								referenced files.
	 */
	virtual IFXRESULT IFXAPI  Load(
								IFXReadBuffer* pReadBuffer, 
								const BOOL isAtomicLoad, 
								const IFXString& rScope, 
								const IFXCollisionPolicy collisionPolicy, 
								const U32 loadId, 
								const IFXObjectFilters& rObjectFilters, 
								const IFXString& rWorldAlias, 
								const BOOL isExternalLoad) = 0;

	/**
		The Load Manager will stop any load or reload operation in progress.
		If the Load Manager is registered with the scheduler, the Load Manager 
		will unregister with the scheduler.
		The Load Manager will clear internal state.
		If the Load Manager was initialized, the Load Manager will remain 
		initialized.
	*/
	virtual IFXRESULT IFXAPI  Cancel() = 0;

	/** 
		This is a reinitialization function. It unregisters tasks to use current 
		Load Manager for another loading. 
	*/
	virtual IFXRESULT IFXAPI  Reset() = 0;

	/** 
		Returns current loading state for all load managers including sub-load managers. 
	*/
	virtual IFXRESULT IFXAPI  GetAggregateLoadState(EIFXLoadState& rLoadState) = 0;

	/** 
		Returns total file size for all load managers including sub-load managers. 
	*/
	virtual IFXRESULT IFXAPI  GetAggregateFileSize(U64& rFileSize) = 0;

	/** 
		Returns loaded file size for all load managers including sub-load managers. 
	*/
	virtual IFXRESULT IFXAPI  GetAggregateFileSizeLoaded(U64& rFileSize) = 0;

	/** 
		Returns total file size for current load manager. 
	*/
	virtual IFXRESULT IFXAPI  GetFileSize(U64& rFileSize) = 0;

	/** 
		Returns loaded file size for current load manager. 
	*/
	virtual IFXRESULT IFXAPI  GetFileSizeLoaded(U64& rFileSize) = 0;

	/** 
		Returns count of sub-load managers created for external referenced files. 
	*/
	virtual IFXRESULT IFXAPI  GetSubLoadManagersCount(U32& rCount) = 0;

	/** 
		Returns specified sub-load manager. 
	*/
	virtual IFXRESULT IFXAPI  GetSubLoadManager(U32 index, IFXLoadManager*& rLoadManager) = 0;

	/** 
		Returns current blocks read timeout in msec. Used in non-atomic mode. 
	*/
	virtual IFXRESULT IFXAPI  GetReadTimeLimit(U32& rReadTimeLimit) = 0;

	/** 
		Set current blocks read timeout in msec. Used in non-atomic mode. 
	*/
	virtual IFXRESULT IFXAPI  SetReadTimeLimit(const U32 rReadTimeLimit) = 0;

	/** 
		Returns current blocks transfer timeout in msec. Used in non-atomic mode. 
	*/
	virtual IFXRESULT IFXAPI  GetTransferTimeLimit(U32& rTransferTimeLimit) = 0;

	/** 
		Set current blocks transfer timeout in msec. Used in non-atomic mode. 
	*/
	virtual IFXRESULT IFXAPI  SetTransferTimeLimit(const U32 rTransferTimeLimit) = 0;
};

#endif
