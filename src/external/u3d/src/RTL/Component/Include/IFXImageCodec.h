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
	@file	IFXImageCodec.h
		
			Interface header file for IFXImageCodec.
*/

#ifndef __IFXIMAGECODEC_INTERFACES_H__
#define __IFXIMAGECODEC_INTERFACES_H__

#include "IFXUnknown.h"

#include "IFXDataBlockQueueX.h"
#include "IFXTextureObject.h"
#include "IFXCoreCIDs.h"
#include "IFXCoreServices.h"

// {50A6E475-80A8-4075-AC30-EA5FB027F4C0}
IFXDEFINE_GUID(IID_IFXImageCodec,
0x50a6e475, 0x80a8, 0x4075, 0xac, 0x30, 0xea, 0x5f, 0xb0, 0x27, 0xf4, 0xc0);

/** Interface for image compression/decompression (codec) services. */
class IFXImageCodec : virtual public IFXUnknown 
{
public:
	/**
		This method stores the CoreServices pointer and performs other
		initialization procedures. This methods should be called after
		the object gets created.

		@param  void* pCallbacks
				The pointer to callback object.
		@param	IFXCoreServices* pCoreServices
				The pointer to CoreServices object.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.
		@retval	IFX_E_ALREADY_INITIALIZED	Object has already initialized.
	*/
	virtual IFXRESULT IFXAPI  Initialize(void* pCallbacks, IFXCoreServices* pCoreServices)=0;


	/**
		This method compresses raw image data into datablocks and stores in
		DataBlockQueue. This methods should be called after the object gets
		created and initialized.

		@param	IFXDataBlockQueue* pDestinationQueue
				The pointer to DataBlockQueue.
		@param	STextureSourceInfo* pImageInfo
				The pointer to image info.
		@param	void* pImage
				The pointer to raw image data.
		@param	U32 uPriority
				Priority for this texture image.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.
		@retval	IFX_TEXTURE_SUBSYSTEM_OBJECT_UNINIT	Object has not initialized.
		@retval	IFX_TEXTURE_CODEC_PLUGIN_NOT_FOUND	Codec plugin is not in the system.
	*/
	virtual IFXRESULT IFXAPI  CompressImageToBlockQueue(
							IFXDataBlockQueueX* pDestinationQueueX, 
							STextureSourceInfo* pImageInfo, 
							void* pImage, 
							U32 uPriority)=0;


	/**
		This method decompresses datablocks in DataBlockQueue to raw image data.
		This methods should be called after the object gets created and initialized.

		@param	IFXDataBlockQueue* pSourceQueue
				The pointer to DataBlockQueue.
		@param	STextureSourceInfo* pImageInfo
				The pointer to image info.
		@param	void** ppImage
				The pointer to raw image buffer.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.
		@retval	IFX_TEXTURE_SUBSYSTEM_OBJECT_UNINIT	Object has not initialized.
		@retval	IFX_TEXTURE_CODEC_PLUGIN_NOT_FOUND	Codec plugin is not in the system.
		@retval IFX_TEXTURE_CREATE_STREAM_FAILURE	Fails to create 3D image stream.
	*/
	virtual IFXRESULT IFXAPI  DecompressBlockQueueToImage(
							IFXDataBlockQueueX* pSourceQueueX, 
							STextureSourceInfo* pImageInfo, 
							void** ppImage)=0;
};

#endif
