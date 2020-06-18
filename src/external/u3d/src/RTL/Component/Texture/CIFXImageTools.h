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
	@file	CIFXImageTools.h
*/

#ifndef __CIFXIMAGETOOLS_H__
#define __CIFXIMAGETOOLS_H__

#include "IFXImageCodec.h"
#include "IFXInet.h"
#include <stdio.h>

// error handling requires setjmp/longjump unless
// we want the application to exit on decode errors...

#define XMD_H
/*#include <setjmp.h>*/

// supporting our own write/read functions for PNG compression/decompression
// as soon as simple FILE* acess is not avalible at IFX
#define PNG_NO_STDIO 

extern "C"
{
#include "jpeglib.h"
#include "png.h"
}

typedef unsigned char byte;

enum EIFXImageChannel
{
  IFX_IMAGE_CHANNELS_BASE,
  IFX_IMAGE_CHANNELS_ALPHA,
  IFX_IMAGE_CHANNELS_DECLARATION
};

class CIFXImageTools : virtual public IFXImageCodec, 
					   virtual public IFXReadingCallback
{
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXImageTools_Factory( 
						IFXREFIID interfaceId, void** ppInterface );

	// IFXImageTools methods
	IFXRESULT  IFXAPI 	 Initialize(void* pCallbacks, IFXCoreServices* pCoreServices);
	IFXRESULT  IFXAPI 	 CompressImageToBlockQueue(
						IFXDataBlockQueueX* pDestinationQueue, 
						STextureSourceInfo* pImageInfo, 
						void* pImage, U32 uPriority);
	IFXRESULT  IFXAPI 	 DecompressBlockQueueToImage(
						IFXDataBlockQueueX* pSourceQueue, 
						STextureSourceInfo* pImageInfo, 
						void** ppImage);

	// IFXReadingCallback methods
	IFXRESULT  IFXAPI 	 GetURLCount(U32& rUrlCount);
	IFXRESULT  IFXAPI 	 GetURL(U32 nUrl, IFXString& rUrl);
	IFXRESULT  IFXAPI 	 AcceptSuccess(U32 nUrl, void* pImage, U32 uImageSize);
	IFXRESULT  IFXAPI 	 AcceptFailure(IFXRESULT uErrorCode);

private:
  
	CIFXImageTools();
	virtual ~CIFXImageTools();

	// private member methods
	IFXRESULT BuildImageFromBlocks(
						IFXDataBlockQueueX* pSourceQueue, 
						STextureSourceInfo* pImageInfo, 
						U32& continuationIndex, 
						void** ppImage );

	IFXRESULT BuildBlocksFromImageData(
						void* pCompressedImage, 
						STextureSourceInfo* pImageInfo, 
						U32 continuationIndex,
						IFXDataBlockQueueX* pDestinationQueue, U32 uPriority);

	IFXRESULT SplitColorChannels(
						void* pSourceImage, 
						STextureSourceInfo* pImageInfo, 
						void** ppColorChannels );

	IFXRESULT MergeColorChannels(
						void* pChannelsSrc1, 
						STextureSourceInfo* pChannelInfoSrc1, 
						U8 chanBit1, U32 contInd1,
						void* pChannelsSrc2, 
						STextureSourceInfo* pChannelInfoSrc2, 
						U8 chanBit2, U32 contInd2,
						U8 chanNumber, void** ppImage, STextureSourceInfo* pImageInfo);

	// for imageType (from GetImageType)
	// Texture Image Type identifies the colour channels present in the texture image
	enum {
		IFXIMAGE_TYPE_ALPHA				= 0x01,
		IFXIMAGE_TYPE_RGB				= 0x0E,
		IFXIMAGE_TYPE_RGBA				= 0x0F,
		IFXIMAGE_TYPE_LUMINANCE			= 0x10,
		IFXIMAGE_TYPE_LUMINANCE_ALPHA	= 0x11
	};

	// for IFXContinuationImageFormat.m_uImageChannels
	/// Texture Image Channels indicates which colour channels of the texture image 
	///are composed using this continuation image.  
	enum {
		IFXIMAGECHANNEL_ALPHA		= 0x01,
		IFXIMAGECHANNEL_BLUE		= 0x02,
		IFXIMAGECHANNEL_GREEN		= 0x04,
		IFXIMAGECHANNEL_RED			= 0x08,
		IFXIMAGECHANNEL_LUMINANCE	= 0x10
	};

	class IFXContinuationImageFormat 
	{
	public:
		IFXContinuationImageFormat();
		~IFXContinuationImageFormat();

		U8		m_uCompressionType; // 0x01 JPEG24, 0x02 PNG; 0x03 JPEG8
		U8		m_uImageChannels;	// x01 alpha, x02 blue, x04 green, x08 red, 
									// x10 luminance (red, blue, green channels )
		U8		m_bpp;
		U16		m_uAttributes;
		U32		m_uImageDataByteCount;
		U32		m_uExtImageURLCount;
		IFXString**	m_pImageURLNames;	
	};

	enum { 
		IFXIMAGEREADING_NOTSTARTED, 
		IFXIMAGEREADING_PROCESS, 
		IFXIMAGEREADING_SUCCESS, 
		IFXIMAGEREADING_FAILED, 
		IFXIMAGEREADING_FINISHED 
	};

	IFXRESULT EvaluateContinuationFormat(STextureSourceInfo* pImageInfo);

	void CleanContinuationFormat();

	U32 CalculateBitsPerPixel(U8 channel);

	IFXRESULT CompressImage(void* pSourceImage, STextureSourceInfo* pImageInfo, 
							U32& continuationIndex, void** ppCompressedData);

	IFXRESULT DecompressImage(void* pCompressedData, STextureSourceInfo* pImageInfo, 
							U32& continuationIndex, void** ppDecompressedImage);

	void SetCompressionProperties(STextureSourceInfo* pImageInfo);

	void* SwapImageToRGB(STextureSourceInfo* pImageInfo, void* pSrcImage);

	BOOL  IsWholeImageFromExternalFile(const STextureSourceInfo* pImageInfo); 

	// Jpeg routines
	IFXRESULT CompressImageJPEG(void* pSourceImage, STextureSourceInfo* pImageInfo, 
								U32& continuationIndex, void** ppCompressedData);
	IFXRESULT DecompressImageJPEG(void* pCompressedData, STextureSourceInfo* pImageInfo, 
								U32& continuationIndex, void** ppDecompressedImage);

	// PNG routines
	IFXRESULT CompressImagePng(void* pSourceImage, STextureSourceInfo* pImageInfo, 
							U32& continuationIndex, void** ppCompressedData);
	IFXRESULT DecompressImagePng(void* pCompressedData, STextureSourceInfo* pImageInfo, 
								U32& continuationIndex, void** ppDecompressedImage);

	IFXRESULT MakeDeclarationBlock(
					IFXDataBlockQueueX *pDataBlockQueue, 
					STextureSourceInfo *pImageInfo);

	IFXRESULT ProcessDeclarationBlock(
					IFXDataBlockQueueX *pDataBlockQueue, 
					STextureSourceInfo *pImageInfo);

	IFXRESULT DecompressImageDeclaration(
					STextureSourceInfo* pImageInfo, 
					void** ppDecompressedImage);

	IFXRESULT CompressJPEGScanLines(
					U8* pSourceImageBuffer, 
					jpeg_compress_struct* pcinfo, 
					STextureSourceInfo* pImageInfo,
					U32& continuationIndex);

	IFXRESULT DecompressJPEGScanLines(
					U8* pImageBase, 
					jpeg_decompress_struct* pcinfo, 
					STextureSourceInfo* pImageInfo,
					U32& continuationIndex);

	// memory source structure definition for the IJG libraries
	struct ifx_jpeg_memory_source_mgr
	{
		struct jpeg_source_mgr base;
		CIFXImageTools *pSource;
	};

	struct ifx_jpeg_memory_destination_mgr
	{
		struct jpeg_destination_mgr base;
		CIFXImageTools* pDestination;
	};

	struct ifx_jpeg_error_mgr
	{
		struct jpeg_error_mgr base;
		CIFXImageTools *pSource;
	};

	// error handling jump buffer to allow
	// unwinding of the stack...
	jmp_buf m_envErrorRecovery;

	// static functions for IJG source decompression module...
	static struct jpeg_source_mgr* create_ifx_jpeg_memory_source_mgr( 
										struct ifx_jpeg_memory_source_mgr *pmgr, 
										CIFXImageTools *pSource);
	static void ijg_init_source( j_decompress_ptr cinfo);
	static boolean ijg_fill_input_buffer( j_decompress_ptr cinfo);
	static void ijg_skip_input_data( j_decompress_ptr cinfo, long num_bytes);
	static boolean ijg_resync_to_restart( j_decompress_ptr cinfo, int desired);
	static void ijg_term_source( j_decompress_ptr cinfo);

	// static methods for IJG destination compression module...
	static struct jpeg_destination_mgr* create_jpeg_memory_destination_mgr( 
											struct ifx_jpeg_memory_destination_mgr *pmgr,
											CIFXImageTools *pSource);
	static void init_destination( j_compress_ptr cinfo );
	static boolean empty_output_buffer( j_compress_ptr cinfo );
	static void term_destination( j_compress_ptr cinfo );

	// static functions for IJG error handling module...
	static struct jpeg_error_mgr* create_ifx_jpeg_error_mgr( 
											struct ifx_jpeg_error_mgr *errmgr, 
											CIFXImageTools *pSource);
	static void ijg_error_exit( j_common_ptr cinfo);
	static void ijg_emit_message( j_common_ptr cinfo, int msg_level);

	U32 m_uRefCount;
	IFXCoreServices* m_pCoreServices;

	U8 GetImageType(U8 eEncoding) ;
	U8 GetEncoding(U8 imageType);

	IFXInet* m_pReadBuffer;
	U32 m_uImageReadingState, m_uSucceededURL;
	U32 m_dirtyURLReading;

	U32	m_uContinuationImageCount;
	IFXContinuationImageFormat*	m_pContinuationFormats;
	U32 m_uCurrentContinuationImage;

	void* m_pCompressedImages[IFX_MAX_CONTINUATIONIMAGE_COUNT];
	U32 m_uCompressedImageSizes[IFX_MAX_CONTINUATIONIMAGE_COUNT];
};

#endif
