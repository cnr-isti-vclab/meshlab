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

#include "CIFXImageTools.h"
#include "IFXBitStreamX.h"
#include "IFXTextureErrors.h"
#include "IFXAutoRelease.h"
#include "IFXImportingCIDs.h"
#include "IFXReadBufferX.h"

// local functions for PNG
static void png_read_data_fn(
				png_structp png_ptr, 
				png_bytep data, 
				png_size_t length);
static void png_write_data_fn(
				png_structp png_ptr, 
				png_bytep data, 
				png_size_t length);
static void png_flush(png_structp png_ptr);


// this flag avoids redeclarations inside the
// IJG of types U32 and FAR
#define IFX_INCLUDE_JPEG_EXCLUDED_DEFINITIONS

static const U32 JPEG_HEADER_BLOCK_MULTIPLIER = 512;


IFXRESULT IFXAPI_CALLTYPE IFXTextureImageTools_ResizeImage(
						U8* pSrc, U8* pDest, U8 m_u8PixelSize, BOOL m_bHasAlpha,
						U32 uSrcWidth, U32 uSrcHeight, U32 uDstWidth, 
						U32 uDstHeight);

//---------------------------------------------------------------------------
//  CIFXImageTools default constructor
//
// This method creates and initializes the member data for this component
//---------------------------------------------------------------------------
CIFXImageTools::CIFXImageTools()
{
	m_uRefCount=0;
	m_pCoreServices = NULL;
	m_uContinuationImageCount = 0;
	m_pContinuationFormats = NULL;
	m_dirtyURLReading = 0;
	m_uImageReadingState = IFXIMAGEREADING_NOTSTARTED;
	m_uCurrentContinuationImage = 0;

	U32 i;
	for (i = 0; i < IFX_MAX_CONTINUATIONIMAGE_COUNT; i++) 
	{
		m_pCompressedImages[i] = NULL;
		m_uCompressedImageSizes[i] = 0;
	}
	m_pReadBuffer = NULL;
}

//---------------------------------------------------------------------------
//  CIFXImageTools default constructor
//
// This method clears and frees the member data and memory for this component
//---------------------------------------------------------------------------
CIFXImageTools::~CIFXImageTools()
{
	m_uRefCount=0;
	// NOTE: m_pCoreServices IS NOT REFCOUNTED TO PREVENT CIRCULAR REF PROBLEM
	// IFXRELEASE(m_pCoreServices);

	CleanContinuationFormat();
	
	IFXRELEASE(m_pReadBuffer);
}



// IFXUnknown methods

//---------------------------------------------------------------------------
//  CIFXImageTools::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXImageTools::AddRef(void)
{
  return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXImageTools::Release(void)
{
  if ( !( --m_uRefCount ) )
  {
    delete this;

    // This second return point is used so that the deleted object's
    // reference count isn't referenced after the memory is released.
    return 0;
  }

  return m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT result  = IFX_OK;

  if ( ppInterface )
    {
    if ( IID_IFXImageCodec == interfaceId || IID_IFXUnknown == interfaceId )
        {
      *ppInterface = ( IFXImageCodec* ) this;
      AddRef();
    }
        else
        {
      *ppInterface = NULL;
      result = IFX_E_UNSUPPORTED;
    }
  }
    else
    {
    result = IFX_E_INVALID_POINTER;
  }

  IFXRETURN(result);
}

//---------------------------------------------------------------------------
//  CIFXImageTools_Factory (non-singleton)
//
//  This is the CIFXImageTools component factory function.  The
//  CIFXImageTools component is NOT a singleton.  This function
//  creates the local image decompressor object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXImageTools_Factory( IFXREFIID interfaceId,
                void**    ppInterface )
{
  IFXRESULT result;

  if ( ppInterface )
  {
    // It doesn't exist, so try to create it.
    CIFXImageTools  *pComponent = new CIFXImageTools;

    if ( pComponent )
    {
      // Perform a temporary AddRef for our usage of the component.
      pComponent->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      result = pComponent->QueryInterface( interfaceId, ppInterface );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pComponent->Release();
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}



// CIFXImageTools methods

//---------------------------------------------------------------------------
//  CIFXImageTools::Initialize
//
//  This method does nothing in the local case.  No callbacks need to be
//  registered.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::Initialize(void* pCallbacks, IFXCoreServices* pCoreServices)
{
  IFXRESULT iResult = IFX_OK;

  if(NULL == pCoreServices)
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) 
  {
  // NOTE: m_pCoreServices IS NOT REFCOUNTED TO PREVENT CIRCULAR REF PROBLEM
    m_pCoreServices = pCoreServices;
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::CompressImageToBlockQueue
//
//  This method takes raw image data and returns a raw image.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::CompressImageToBlockQueue(
							IFXDataBlockQueueX* pDestinationQueue, 
							STextureSourceInfo* pImageInfo, 
							void* pImage, U32 uPriority)
{
	IFXRESULT iResult=IFX_OK;
	U32 contInd1, contInd2, contInd3, contInd4, iImage;	

	if( NULL == pImageInfo ) 
	{
		iResult = IFX_E_INVALID_POINTER;
	}
	else
	{
		if( !(IsWholeImageFromExternalFile(pImageInfo)) ) 
		{
			if(	NULL == pDestinationQueue || NULL == pImage ) 
			{
				iResult = IFX_E_INVALID_POINTER;	
			}
		}
		else
		{
			if( NULL == pDestinationQueue ) 
			{
				iResult = IFX_E_INVALID_POINTER;	
			}	
		}
	}
  
	if(IFXSUCCESS(iResult)) 
	{
		iResult = EvaluateContinuationFormat(pImageInfo);
		IFXASSERT( iResult == IFX_OK );
	}

	if(IFXSUCCESS(iResult)) 
	{
		void *pColorChannels[IFX_MAX_CONTINUATIONIMAGE_COUNT], *pCompressedData = NULL;
		void *swapColorImage = NULL;
		void *pSourceImage   = pImage;
		BOOL bSourceImageOrderBGR = false;

		for (iImage = 0; iImage <IFX_MAX_CONTINUATIONIMAGE_COUNT; iImage++ ) 
		{
			pColorChannels[iImage] = NULL;
		}
		contInd1 = 0;
		contInd2 = 1;
		contInd3 = 2;
		contInd4 = 3;

		/// Check and set flag if we need swap to BGR: bSourceImageOrderBGR
		if (pImageInfo->m_imageType == IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24 ||
			pImageInfo->m_imageType == IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32) 
		{
			bSourceImageOrderBGR = true; 
		}

		if (m_uContinuationImageCount == 1 ) 
		{
			if (m_pContinuationFormats[0].m_uAttributes == 0) 
			{
				// if cont. image should be stored in U3D, otherwise it shiuld jst 
				// refer to external image

				if (bSourceImageOrderBGR) 
				{
					swapColorImage = SwapImageToRGB(pImageInfo, pImage);				
					if(NULL == swapColorImage)iResult = IFX_E_OUT_OF_MEMORY;		

					if (swapColorImage) 
						pSourceImage = swapColorImage;
				}

				if(IFXSUCCESS(iResult)) 
					iResult = CompressImage(
								pSourceImage, pImageInfo, contInd1, &pCompressedData);

				if(IFXSUCCESS(iResult)) 
				{
						iResult = BuildBlocksFromImageData(
										pCompressedData, pImageInfo, 
										contInd1, pDestinationQueue, 1 );
						if(NULL != pCompressedData) 
						{
							delete (U8 *) pCompressedData;
							pCompressedData = NULL;
						}

						//Compressed texture size 
						m_pContinuationFormats[0].m_uImageDataByteCount = 
							pImageInfo->m_size;

				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			}

		} 
		else if (m_uContinuationImageCount == 2 ) 
		{
			// 0. Swap image to BGR if needed
			if (bSourceImageOrderBGR) 
			{
				swapColorImage = SwapImageToRGB(pImageInfo, pImage);
				if(NULL == swapColorImage)iResult = IFX_E_OUT_OF_MEMORY;		
				if (swapColorImage) 
					pSourceImage = swapColorImage;
			}

			// 1. Split channels
			if(IFXSUCCESS(iResult))
				iResult = SplitColorChannels( pSourceImage, pImageInfo, pColorChannels);

			// 2. Compress them into blocks
			if( IFXSUCCESS(iResult) && 
				pColorChannels[0] && m_pContinuationFormats[0].m_uAttributes == 0) 
			{
				iResult = CompressImage(
								pColorChannels[0], pImageInfo, 
								contInd1, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, 
									pImageInfo, 
									contInd1, 
									pDestinationQueue, 1);
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}

					//Compressed texture size
					m_pContinuationFormats[0].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			}

			if( IFXSUCCESS(iResult) && pColorChannels[1] && 
				m_pContinuationFormats[1].m_uAttributes == 0) 
			{
				iResult = CompressImage(
							pColorChannels[1], pImageInfo, 
							contInd2, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd2, pDestinationQueue, 2 );
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}
			
					//Compressed texture size
					m_pContinuationFormats[1].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			} 
		} 
		else if (m_uContinuationImageCount == 3 ) 
		{
			// 0. Swap image to BGR if needed
			if (bSourceImageOrderBGR) 
			{
				swapColorImage = SwapImageToRGB(pImageInfo, pImage);
				if(NULL == swapColorImage)iResult = IFX_E_OUT_OF_MEMORY;		
				if (swapColorImage) 
					pSourceImage = swapColorImage;
			}
			// 1. Split channels
			if(IFXSUCCESS(iResult))
				iResult = SplitColorChannels( pSourceImage, pImageInfo, pColorChannels);

			// 2. Compress them into blocks
			if( IFXSUCCESS(iResult) && pColorChannels[0] && 
				m_pContinuationFormats[0].m_uAttributes == 0) 
			{
				iResult = CompressImage(
							pColorChannels[0], pImageInfo, 
							contInd1, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd1, pDestinationQueue, 1);
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}

					//Compressed texture size
					m_pContinuationFormats[0].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			}
			if( IFXSUCCESS(iResult) && pColorChannels[1] && 
				m_pContinuationFormats[1].m_uAttributes == 0) 
			{
				
				iResult = CompressImage(
								pColorChannels[1], pImageInfo, 
								contInd2, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd2, pDestinationQueue, 2);
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}

					//Compressed texture size
					m_pContinuationFormats[1].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			}
			if(IFXSUCCESS(iResult) && pColorChannels[2] && 
				m_pContinuationFormats[2].m_uAttributes == 0) 
			{
				iResult = CompressImage(
							pColorChannels[2], pImageInfo, 
							contInd3, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd3, pDestinationQueue, 3 );
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}
			
					//Compressed texture size
					m_pContinuationFormats[2].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			} 
		}
		else if (m_uContinuationImageCount == 4 ) 
		{
			// 0. Swap image to BGR if needed
			if (bSourceImageOrderBGR) 
			{
				swapColorImage = SwapImageToRGB(pImageInfo, pImage);
				if(NULL == swapColorImage)iResult = IFX_E_OUT_OF_MEMORY;		
				if (swapColorImage) 
					pSourceImage = swapColorImage;
			}
			// 1. Split channels
			if(IFXSUCCESS(iResult)) 
				iResult = SplitColorChannels( pSourceImage, pImageInfo, pColorChannels);
			

			// 2. Compress them into blocks
			if(IFXSUCCESS(iResult) && pColorChannels[0] && 
				m_pContinuationFormats[0].m_uAttributes == 0) 
			{
				
				iResult = CompressImage(
								pColorChannels[0], pImageInfo, 
								contInd1, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd1, pDestinationQueue, 1);
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}

					//Compressed texture size
					m_pContinuationFormats[0].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			}
			if( IFXSUCCESS(iResult) && pColorChannels[1] && 
				m_pContinuationFormats[1].m_uAttributes == 0) 
			{
				iResult = CompressImage(
								pColorChannels[1], pImageInfo, 
								contInd2, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd2, pDestinationQueue, 2);
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}

					//Compressed texture size
					m_pContinuationFormats[1].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			}
			if(IFXSUCCESS(iResult) && pColorChannels[2] && 
				m_pContinuationFormats[2].m_uAttributes == 0) 
			{
				iResult = CompressImage(
								pColorChannels[2], pImageInfo, 
								contInd3, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd3, pDestinationQueue, 3 );
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}
			
					//Compressed texture size
					m_pContinuationFormats[2].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			} 

			if( IFXSUCCESS(iResult) && pColorChannels[3] && 
				m_pContinuationFormats[3].m_uAttributes == 0) 
			{
				iResult = CompressImage(
								pColorChannels[3], pImageInfo, 
								contInd4, &pCompressedData);

				if(IFXSUCCESS(iResult))	
				{
					iResult = BuildBlocksFromImageData(
									pCompressedData, pImageInfo, 
									contInd4, pDestinationQueue, 4 );
					if(NULL != pCompressedData) 
					{
						delete (U8 *) pCompressedData;
						pCompressedData = NULL;
					}
			
					//Compressed texture size
					m_pContinuationFormats[3].m_uImageDataByteCount = pImageInfo->m_size;
				} 
				else 
				{
					iResult = IFX_E_UNSUPPORTED;
				}
			} 

	} 
	else 
	{
		IFXASSERT(0);
		iResult = IFX_E_UNSUPPORTED;
	}

	// 4. Cleanup
	if (swapColorImage)
		delete (U8 *) swapColorImage;
	if (IFXSUCCESS(iResult)) 
	{
		for (iImage = 0; iImage<m_uContinuationImageCount; iImage++) 
		{
			if( NULL != pColorChannels[iImage] && pImage != pColorChannels[iImage]) 
			{
				delete (U8 *) pColorChannels[iImage];
				pColorChannels[iImage] = NULL;
			}
		}
	}

	// Now write Declaration block but with 0 priority 
	if(IFXSUCCESS(iResult)) 
	{
		iResult = MakeDeclarationBlock(pDestinationQueue,pImageInfo);
	}
	
	IFXASSERT( iResult == IFX_OK );
  }
  IFXRETURN(iResult);
}

//---------------------------------------------------------------------------
//  CIFXImageTools::DecompressBlockQueueToImage
//
//  This method decompresses a block queue into raw image data.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::DecompressBlockQueueToImage(
								IFXDataBlockQueueX* pSourceQueueX, 
								STextureSourceInfo* pImageInfo, 
								void** ppImage)
{
	IFXRESULT iResult=IFX_OK;

	if(NULL == pSourceQueueX || NULL == pImageInfo || NULL == ppImage)  
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))  
	{
		IFXDECLARELOCAL(IFXDataBlockQueueX,pDataBlockQueueX);
		pSourceQueueX->CopyX(pDataBlockQueueX);

		void* pCompressedData = NULL; // will be allocated inside BuildImageFrom* calls
		void* pColorChannels[4] = {NULL, NULL, NULL, NULL};

		U32 contInd, i;
		U32 indURL[IFX_MAX_CONTINUATIONIMAGE_COUNT];

		//
		// 0. Process the Declaration block 
		//
		iResult = ProcessDeclarationBlock(pDataBlockQueueX, pImageInfo);
		IFXASSERT( iResult == IFX_OK );
		
		//
		// 1. Read images from continuation blocks
		//
		// max 4 when 4 channels combined RGBA or Luminance w/ Alpha
		IFXASSERT(m_uContinuationImageCount <= IFX_MAX_CONTINUATIONIMAGE_COUNT );

		if (m_uImageReadingState == IFXIMAGEREADING_PROCESS) 
		{
			iResult = IFX_W_READING_NOT_COMPLETE;
		}
		if (m_uImageReadingState == IFXIMAGEREADING_FAILED) 
		{
			iResult = IFX_E_CANNOT_FIND;
			U32 i;
			for ( i = 0; i < m_uContinuationImageCount; i++) 
			{
				if (m_pCompressedImages[i]) 
				{
					delete (U8*)m_pCompressedImages[i];
					m_pCompressedImages[i] = NULL;
				}
			}
		}
		while ( (m_uImageReadingState == IFXIMAGEREADING_NOTSTARTED) || 
				(m_uImageReadingState == IFXIMAGEREADING_SUCCESS)) 
		{
			if (m_uImageReadingState == IFXIMAGEREADING_SUCCESS) 
			{
				IFXRELEASE(m_pReadBuffer);
				indURL[m_uCurrentContinuationImage++] = m_uSucceededURL;
			}
			while ((m_uCurrentContinuationImage < m_uContinuationImageCount) && 
				(m_pContinuationFormats[m_uCurrentContinuationImage].m_uAttributes == 0))
			{
				m_uCurrentContinuationImage++;
			}
			if (m_uCurrentContinuationImage < m_uContinuationImageCount) 
			{
				m_uImageReadingState = IFXIMAGEREADING_PROCESS;
				iResult = IFXCreateComponent(
								CID_IFXInternetReadBuffer, IID_IFXInet, 
								(void**)&m_pReadBuffer);
				if (IFXSUCCESS(iResult))
					m_pReadBuffer->InitiateRead(
										m_pCoreServices, 
										(IFXReadingCallback*)this);
				if (IFXSUCCESS(iResult))
					iResult = IFX_W_READING_NOT_COMPLETE;
			} 
			else 
			{
				m_uImageReadingState = IFXIMAGEREADING_FINISHED;
				iResult = IFX_OK;
			}
		}

		if (iResult == IFX_W_READING_NOT_COMPLETE) 
			return iResult;

		// this is stored to remember if image was loaded from U3D or from external URL
		pImageInfo->m_imageLoadCount = m_uContinuationImageCount;
		pImageInfo->m_imageURLCount = 0;
		for (contInd=0; IFXSUCCESS(iResult) && 
			(contInd < m_uContinuationImageCount); contInd++) 
		{
			if (IFXSUCCESS(iResult)) 
			{
				pImageInfo->m_URLFileRef[contInd] = false;	
				if (m_pContinuationFormats[contInd].m_uAttributes == 0) 
				{
					// Continuation block is in the same file
					iResult = BuildImageFromBlocks( 
									pDataBlockQueueX, pImageInfo, 
									contInd, &pCompressedData );
					if(IFXSUCCESS(iResult) && pCompressedData) 
					{
						iResult = DecompressImage(
									pCompressedData, pImageInfo, 
									contInd, &(pColorChannels[contInd]));
					}
					if(pCompressedData) 
					{ 
						IFXDeallocate(pCompressedData); 
						pCompressedData = NULL; 
					}
				}
				if (IFXSUCCESS(iResult) && 
					(m_uCurrentContinuationImage == m_uContinuationImageCount)) 
				{
					U32 i;
					for (i = 0; i < m_uContinuationImageCount; i++) 
					{
						if (IFXSUCCESS(iResult)) 
						{
							if (m_pCompressedImages[i]) 
							{
								pImageInfo->m_URLFileRef[i] = true;
								pImageInfo->m_imageURLCount++;
								pImageInfo->m_blockCompressionType[i] = 
									m_pContinuationFormats[i].m_uCompressionType;
								
								pImageInfo->m_size = m_uCompressedImageSizes[i];
								iResult = DecompressImage(
												m_pCompressedImages[i], pImageInfo, 
												i, &pColorChannels[i]);
								delete (U8*)m_pCompressedImages[i];
								m_pCompressedImages[i] = NULL;
							}
						}
					}
				}
			}
		}
		
		if (IFXSUCCESS(iResult)) 
		{
			// 3. Merge (or just copy) the continuation images into one whole final image
		
			switch (pImageInfo->m_imageType) 
			{
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE: 
				IFXASSERT(m_uContinuationImageCount==1)
				iResult = MergeColorChannels(pColorChannels[0], pImageInfo, 
										m_pContinuationFormats[0].m_uImageChannels, 0,
										NULL, NULL,0, 0, 3, ppImage, pImageInfo);
				break;

			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24:
				IFXASSERT(m_uContinuationImageCount==1 || m_uContinuationImageCount<=3 );
				if (m_uContinuationImageCount==1) 
				{
					iResult = MergeColorChannels(pColorChannels[0], pImageInfo, 
										m_pContinuationFormats[0].m_uImageChannels, 0, 
										NULL, NULL, 0, 0, 3, ppImage, pImageInfo);
				} 
				else 
				{
					for (i=0; IFXSUCCESS( iResult ) && i<m_uContinuationImageCount; i++) 
					{
						iResult = MergeColorChannels(pColorChannels[i], pImageInfo, 
										m_pContinuationFormats[i].m_uImageChannels, i,
										NULL, NULL,0, 0, 3, ppImage, pImageInfo);
					}
				}
				break;

			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32:
				IFXASSERT(m_uContinuationImageCount>=2 || m_uContinuationImageCount<=4 );
				if (m_uContinuationImageCount==2) 
				{
					iResult = MergeColorChannels(
									pColorChannels[0], pImageInfo, 
									m_pContinuationFormats[0].m_uImageChannels, 0, 
									pColorChannels[1], pImageInfo, 
									m_pContinuationFormats[1].m_uImageChannels, 1, 4, 
									ppImage, pImageInfo); 
				} 
				else 
				{
					for (i=0; IFXSUCCESS( iResult )&& i<m_uContinuationImageCount; i++) 
					{
						iResult = MergeColorChannels(pColorChannels[i], pImageInfo, 
										m_pContinuationFormats[i].m_uImageChannels, i, 
										NULL, NULL,0, 0, 4, ppImage, pImageInfo);
					}
				}
				break;

			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_ALPHA:
				IFXASSERT(m_uContinuationImageCount==1)
				iResult = MergeColorChannels(
									pColorChannels[0], pImageInfo, 
									m_pContinuationFormats[0].m_uImageChannels, 0, 
									NULL, NULL,0, 0, 1, ppImage, pImageInfo);
				break;

			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA:
				for (i=0; IFXSUCCESS( iResult ) && i<m_uContinuationImageCount; i++) 
				{
					iResult = MergeColorChannels(pColorChannels[i], pImageInfo, 
									m_pContinuationFormats[i].m_uImageChannels, i, 
									NULL, NULL,0, 0, 4, ppImage, pImageInfo);
				}
				break;
			default:
				IFXASSERT(0);
				break;
			}
		}

		// 4. Put into image Source Info data about URL files images were loaded from
		if (IFXSUCCESS(iResult)) 
		{
#ifdef _DEBUG			
			U32 check = 0;
#endif
			for (contInd=0; contInd<m_uContinuationImageCount; contInd++) 
			{
				if (pImageInfo->m_URLFileRef[contInd]) 
				{
					pImageInfo->m_URLFileLoadedIndex[contInd] = indURL[contInd];
#ifdef _DEBUG
					check++;
#endif
				}				
			}
		}

		// 5. Cleanup
		for (i=0; i<4; i++) 
		{
			if(NULL != pColorChannels[i]) 
			{
				IFXDeallocate(pColorChannels[i]);
				pColorChannels[i] = NULL;
			}
		}
	}

	if (iResult == IFX_E_READ_FAILED) iResult = IFX_W_READING_NOT_COMPLETE;
	return iResult;
}

IFXRESULT CIFXImageTools::GetURLCount(U32& rUrlCount)
{
	rUrlCount = m_pContinuationFormats[m_uCurrentContinuationImage].m_uExtImageURLCount;
	return IFX_OK;
}

IFXRESULT CIFXImageTools::GetURL(U32 nUrl, IFXString& rUrl)
{
	IFXRESULT rc = IFX_OK;
	
	if ( 0 == m_pContinuationFormats[m_uCurrentContinuationImage].m_uExtImageURLCount )	
	{
		rc = IFX_E_INVALID_FILE;
	}
	if ( 0 == m_uContinuationImageCount  )	
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS( rc )) 
	{
		rUrl = 
			m_pContinuationFormats[m_uCurrentContinuationImage].m_pImageURLNames[nUrl];
		// transform file://c/dir/file.u3d to c:/dir/file.u3d 

		if( rUrl.IsEmpty() ) 
		{
			rc = IFX_E_PARAMETER_NOT_INITIALIZED;
		}
		if(IFXSUCCESS( rc )) 
		{
			U32 tmp = 0;
			if (IFXSUCCESS(rUrl.FindSubstring(L"file://", &tmp))) 
			{
				const IFXCHAR* pstr = rUrl;
				IFXCHAR p[] = L"x:";
				p[0] = pstr[7];
				rUrl = IFXString(p) + IFXString(&pstr[8]);
			}
			// because full path should contain ':', we should append 
			// relative path to base URL from Core Services
			tmp = 0;
			if (IFXFAILURE(rUrl.FindSubstring(L":", &tmp))) 
			{
				IFXString sBaseURL;
				m_pCoreServices->GetBaseURL(sBaseURL);
				rUrl = sBaseURL + rUrl;
			}
		}
	}
	return rc;
}

IFXRESULT CIFXImageTools::AcceptSuccess(U32 nUrl, void* pImage, U32 uImageSize)
{
	m_pCompressedImages[m_uCurrentContinuationImage] = pImage;
	m_uCompressedImageSizes[m_uCurrentContinuationImage] = uImageSize;
	m_uImageReadingState = IFXIMAGEREADING_SUCCESS;
	m_uSucceededURL = nUrl;
	return IFX_OK;
}

IFXRESULT CIFXImageTools::AcceptFailure(IFXRESULT uErrorCode)
{
	m_uImageReadingState = IFXIMAGEREADING_FAILED;
	return IFX_OK;
}


// CIFXImageTools private methods...

//---------------------------------------------------------------------------
//  CIFXImageTools::BuildImageFromBlocks
//
//  This method decompresses a queue of blocks into raw image data.  It does
//  this by extracting all the image data out of the blocks and placing it
//  into one contiguous memory block.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::BuildImageFromBlocks(
								IFXDataBlockQueueX* pSourceQueue, 
								STextureSourceInfo* pImageInfo, 
								U32& continuationIndex, void** ppImage)
{
  IFXRESULT iResult=IFX_OK;

  if(NULL == pSourceQueue || NULL == pImageInfo || NULL == ppImage )
    iResult =IFX_E_INVALID_POINTER;
  if(NULL == m_pCoreServices)
    iResult = IFX_E_NOT_INITIALIZED;

  if(IFXSUCCESS(iResult))   
  {
    U32       uNameLength = 0;
    U8*       pBlockImageData = NULL;
    U32       uBlockImageDataLength = 0;
    //U8*       pOutputImageData = NULL;
    U32       uOutputImageDataLength = 0;
	IFXString	sName;

    if(IFXSUCCESS(iResult))   
	{
      IFXDataBlockX *pDataBlock = NULL;
      BOOL rbDone = FALSE;

	  pSourceQueue->GetNextBlockX( pDataBlock, rbDone );
	  if (!pDataBlock) iResult = IFX_E_READ_FAILED;
      
	  IFXBitStreamX *pBitStream = NULL;
	  if(IFXSUCCESS(iResult))
		iResult = IFXCreateComponent(
						CID_IFXBitStreamX, IID_IFXBitStreamX, 
						(void**)&pBitStream);

	  // Wrap datablock in bitstream
      if(IFXSUCCESS(iResult))
          pBitStream->SetDataBlockX(*pDataBlock);

	  //  2. Determine the type of datablock - first or continuation
      U32 uBlockType=0;
	  if(IFXSUCCESS(iResult))
		pDataBlock->GetBlockTypeX( uBlockType );

	  //  3. Should be "continuation" block 
	  if (BlockType_ResourceTextureContinuationU3D == uBlockType) 
	  {
		// Read the name
	    if(IFXSUCCESS(iResult))
		  pBitStream->ReadIFXStringX( sName );

		IFXASSERT(pImageInfo->m_name == sName);

		// Read continuation image index
		if(IFXSUCCESS(iResult)) 
		{
		  pBitStream->ReadU32X(continuationIndex);

		  pImageInfo->m_blockCompressionType[continuationIndex] = 
			  m_pContinuationFormats[continuationIndex].m_uCompressionType;

		  if(IFXSUCCESS(iResult)  )	
		  {
				// get a pointer to the block data
				pDataBlock->GetPointerX( pBlockImageData );
	
				if(IFXSUCCESS(iResult))
					iResult = pImageInfo->m_name.GetLengthU8( &uNameLength );
	
				if(IFXSUCCESS(iResult))
					pDataBlock->GetSizeX( uBlockImageDataLength );

				if(IFXSUCCESS(iResult))	
				{
					// the start of the Image data is after the string, after a 
					// U16 that tells the size of the string, 
					// and after a U8 that tells the image block type
					pBlockImageData = pBlockImageData
							+ uNameLength
							+ sizeof(U16) /* name length tag*/
							+ sizeof(U32); /* continutaion image index */

					// subtract the header junk off the data length to get the
					// size of the compressed data block
					uBlockImageDataLength = 
						uBlockImageDataLength - 
						(uNameLength + sizeof(U16) + sizeof(U32));

					IFXASSERT( uBlockImageDataLength == 
						m_pContinuationFormats[continuationIndex].m_uImageDataByteCount);
				}
				pImageInfo->m_size += uBlockImageDataLength;

				// 7. append the block's image data
				if(IFXSUCCESS(iResult))	{
					// reallocate the output buffer with the new size
					*ppImage=IFXReallocate(
									*ppImage, 
									(uOutputImageDataLength + uBlockImageDataLength) );
					if( *ppImage == NULL && 
						((uOutputImageDataLength + uBlockImageDataLength) != 0))
						iResult = IFX_E_OUT_OF_MEMORY;

					if(IFXSUCCESS(iResult))	
					{
						U8* pImage=(U8*)*ppImage;
						U8* pTail=&(pImage[uOutputImageDataLength]);
						uOutputImageDataLength+=uBlockImageDataLength;
						memcpy( pTail, pBlockImageData, uBlockImageDataLength );
					}
				}
		   }
        }
        // 8. release the current datablock
        IFXRELEASE( pBitStream );
        IFXRELEASE(pDataBlock);
      }
    }

    // 9. cleanup and return
    // IFXRELEASE( pSourceQueue );
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::BuildBlocksFromImageData
//
//  This method takes a compressed image data, creates a properly
//  formatted block and puts it into the specified queue
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::BuildBlocksFromImageData(
								void* pCompressedImage, 
								STextureSourceInfo* pImageInfo, 
								U32 contInd,
								IFXDataBlockQueueX* pDestinationQueue, 
								U32 uPriority) 
{
  IFXRESULT iResult=IFX_OK;

  if(NULL == pCompressedImage || NULL == pImageInfo || NULL == pDestinationQueue)
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) 
  {
    IFXBitStreamX* pBitStream = NULL;
    iResult = IFXCreateComponent(
					CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStream);

    if(IFXSUCCESS(iResult)) 
	{
      IFXDataBlockX* pDataBlock = NULL;
      U32 uNameLength=0;
      U32 uNewBlockSize = 0;

      // 1. store the texture name
      pImageInfo->m_name.GetLengthU8(&uNameLength);
      if(IFXSUCCESS(iResult))
        pBitStream->WriteIFXStringX( pImageInfo->m_name );

	  // 2. write  continuation image index
      if(IFXSUCCESS(iResult))
        pBitStream->WriteU32X(contInd);

      switch( pImageInfo->m_blockCompressionType[contInd] ) 
	  {
        case IFXTextureObject::TextureType_Jpeg24:
		case IFXTextureObject::TextureType_Jpeg8:
		case IFXTextureObject::TextureType_Png:
			// 4a. Set the data block type
			if ( IFXSUCCESS(iResult) )
				pBitStream->GetDataBlockX( pDataBlock );

			// 4b. Calculate the new block size
			uNewBlockSize = uNameLength
							+ sizeof(U16)		 // ?? string length tag
							+ sizeof(U32)		 // length of continuation index type
							+ pImageInfo->m_size; // length of buffer

			// 4c. Reallocate the data block
			if ( IFXSUCCESS(iResult) )
				pDataBlock->SetSizeX( uNewBlockSize );

			// 4d. Get the datablock pointer and fill it with the compressed image data
			if(IFXSUCCESS(iResult)) 
			{
				U8 *pBuf;
				pDataBlock->GetPointerX( pBuf );
				pBuf += uNameLength 
						+ sizeof(U16)     // string length tag -?? 
						+ sizeof(U32);    // length of continuation index type 

				memcpy( pBuf, pCompressedImage, pImageInfo->m_size );
			}

			// 4e. set the block type
			if(IFXSUCCESS(iResult))
				pDataBlock->SetBlockTypeX( BlockType_ResourceTextureContinuationU3D );

			// set the block priority
			if(IFXSUCCESS(iResult)) 
			{
				pDataBlock->SetPriorityX(uPriority);
			}

			break;
		default:
			IFXASSERT(0);
			iResult = IFX_E_UNSUPPORTED;
      }

      // 4. Append newly created block into the block queue
      if(IFXSUCCESS(iResult))
        pDestinationQueue->AppendBlockX( *pDataBlock );

      IFXRELEASE(pDataBlock);
      IFXRELEASE(pBitStream);
    }
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::SplitColorChannels
//
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::SplitColorChannels(
								void* pSourceImage, 
								STextureSourceInfo* pImageInfo,
								void** ppColorChannels ) 
{

	IFXRESULT iResult = IFX_OK;

	if( NULL == pSourceImage ||  NULL == pImageInfo)
		iResult = IFX_E_INVALID_POINTER;
	if( NULL == ppColorChannels )
		iResult = IFX_E_INVALID_POINTER;

	if (m_uContinuationImageCount != 1) 
	{
		U32 iImage, uIndex, uChannel;
		U8	src_bpp = 0;
		U8*	pBufColorChannels[IFX_MAX_CONTINUATIONIMAGE_COUNT];
		U8*	pBufSourceImage = (U8*)pSourceImage;
		U32 SrcChannelIndex[IFX_MAX_CONTINUATIONIMAGE_COUNT][4];
		U32 DstChannelCount[IFX_MAX_CONTINUATIONIMAGE_COUNT];  

		for ( iImage = 0; iImage <m_uContinuationImageCount; iImage++ )  
		{
			ppColorChannels[iImage] = 
				(U8*) new U8 [ pImageInfo->m_width * 
				pImageInfo->m_height * 
				m_pContinuationFormats[iImage].m_bpp ];
			if( NULL != ppColorChannels[iImage])
			{
				//U32 sz = m_pContinuationFormats[iImage].m_bpp;
				pBufColorChannels[iImage] = (U8*) ppColorChannels[iImage];
				if(NULL == ppColorChannels[iImage])
				{
					iResult = IFX_E_OUT_OF_MEMORY;
				}
			}
		}

		if(IFXSUCCESS(iResult)) 
		{
			memset(DstChannelCount, 0, 4*sizeof(U32));

			// 2. check formats
			switch( pImageInfo->m_imageType ) 
			{

				// If alpha/8 bit, set that channel and corresponding channel info structure
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE:
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_ALPHA:
				src_bpp = 1;
				DstChannelCount[0]		= 1;
				SrcChannelIndex[0][0]	= 0;
				break;

				// If rgb/24 bit, set that channel and corresponding channel info structure
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24:
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32:
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32:
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24:
				src_bpp = 4;
				if (pImageInfo->m_imageType == 
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24 || 
					pImageInfo->m_imageType == 
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24 ) src_bpp = 3;
				for ( iImage = 0; iImage <m_uContinuationImageCount; iImage++ )  
				{
					if (m_pContinuationFormats[iImage].m_uImageChannels & 
						IFXIMAGECHANNEL_ALPHA)
					{
						SrcChannelIndex[iImage][DstChannelCount[iImage]] = 3;
						DstChannelCount[iImage]++;
					}
					if (m_pContinuationFormats[iImage].m_uImageChannels & 
						IFXIMAGECHANNEL_RED)
					{
						SrcChannelIndex[iImage][DstChannelCount[iImage]] = 0;
						DstChannelCount[iImage]++;
					}
					if (m_pContinuationFormats[iImage].m_uImageChannels & 
						IFXIMAGECHANNEL_GREEN)
					{
						SrcChannelIndex[iImage][DstChannelCount[iImage]] = 1;
						DstChannelCount[iImage]++;
					}
					if (m_pContinuationFormats[iImage].m_uImageChannels & 
						IFXIMAGECHANNEL_BLUE)
					{
						SrcChannelIndex[iImage][DstChannelCount[iImage]] = 2;
						DstChannelCount[iImage]++;
					}
				}
				break;
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA:
				src_bpp = 2;
				for ( iImage = 0; iImage <m_uContinuationImageCount; iImage++ )  
				{
					if (m_pContinuationFormats[iImage].m_uImageChannels &
						IFXIMAGECHANNEL_ALPHA)
					{
						SrcChannelIndex[iImage][DstChannelCount[iImage]] = 1;
						DstChannelCount[iImage]++;
					}
					if (m_pContinuationFormats[iImage].m_uImageChannels & 
						IFXIMAGECHANNEL_LUMINANCE)
					{
						SrcChannelIndex[iImage][DstChannelCount[iImage]] = 0;
						DstChannelCount[iImage]++;
					}
				}
				break;
			default:
				iResult = IFX_E_UNSUPPORTED;
			}

			U8 step_bpp = 0, dst_bpp;
			// U32 channelCount;
			for ( iImage = 0; iImage <m_uContinuationImageCount; iImage++ )  
			{
				// channelCount = 0;

				dst_bpp = m_pContinuationFormats[iImage].m_bpp;
				pBufSourceImage = (U8*)pSourceImage;

				for( uIndex=0; 
					uIndex < (pImageInfo->m_width * pImageInfo->m_height * src_bpp); 
					uIndex+=src_bpp) 
				{
					for (uChannel = 0; uChannel < DstChannelCount[iImage]; uChannel++)
					{
						pBufColorChannels[iImage][uChannel] = 
							pBufSourceImage[SrcChannelIndex[iImage][uChannel]];
					}
					pBufColorChannels[iImage] = pBufColorChannels[iImage] + dst_bpp;
					pBufSourceImage = pBufSourceImage + src_bpp;
				}
				step_bpp = step_bpp + dst_bpp;
			}
		} 
		else 
		{
			// free memory that was allocated 
			for ( iImage = 0; iImage <m_uContinuationImageCount; iImage++ )  
			{
				if (  ppColorChannels[iImage]) 
					delete (U8 *) ppColorChannels[iImage];
				ppColorChannels[iImage] = NULL;
			}
		}
	}
	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::MergeColorChannels
//
//  This method merges the color and alpha channels back into a RGBA32 image;
//  or if some are missing, it does its best.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::MergeColorChannels(
								void* pChannelsSrc1, 
								STextureSourceInfo* pChannelInfoSrc1, 
								U8 chanBit1, U32 contInd1,  
								void* pChannelsSrc2, 
								STextureSourceInfo* pChannelInfoSrc2, 
								U8 chanBit2, U32 contInd2, 
								U8 chanNumber, void** ppImage, 
								STextureSourceInfo* pImageInfo ) 
{
  IFXRESULT iResult=IFX_OK;
  U32 uIndex=0;
  U8* pDest = NULL;

  if( pChannelsSrc1 && NULL == pChannelInfoSrc1 )
    iResult = IFX_E_INVALID_POINTER;
  // it is OK for (pChannelsSrc2 && NULL == pChannelInfoSrc2)

  if(NULL == ppImage || NULL == pImageInfo)
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) {

	IFXASSERT(chanNumber==3 || chanNumber==4 || chanNumber==1);
	
	pImageInfo->m_width  = pChannelInfoSrc1->m_width;
	pImageInfo->m_height = pChannelInfoSrc1->m_height;
	pImageInfo->m_size = ( pImageInfo->m_width * pImageInfo->m_height * chanNumber);

	if (*ppImage == NULL) 
	{
		pDest = (U8*) new U8 [pImageInfo->m_size];
		if(NULL == pDest) 
		{
			iResult = IFX_E_OUT_OF_MEMORY;
		}
		else
		{
			*ppImage = pDest;
		}
	} 
	else 
	{
		pDest = (U8*)*ppImage;
	}
  }
  U8 src1_bpp=3, src2_bpp=3; // just default value

  if(IFXSUCCESS(iResult)) 
  {
    // copy image data

	if(pChannelsSrc1 && NULL == pChannelsSrc2) 
	{ 	// we have a 1st channel, but no 2nd 
		// copy the color channel
		if (chanBit1 != (U8)IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE) 
		{
			if (chanNumber != 1 ) 
			{  // this is for RGB or RGBA 
				src1_bpp = m_pContinuationFormats[contInd1].m_bpp;
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_RED) 
						pDest[uIndex + 0]   = ((U8*)pChannelsSrc1)[0];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN) 
						pDest[uIndex + 1] = ((U8*)pChannelsSrc1)[1];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE) 
						pDest[uIndex + 2]  = ((U8*)pChannelsSrc1)[2];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA) 
						pDest[uIndex + 3] = ((U8*)pChannelsSrc1)[3];
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + src1_bpp;
				}
			} 
			else 
			{ // this is for ALPHA only 
				IFXASSERT(chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA);
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA) 
						pDest[uIndex] = ((U8*)pChannelsSrc1)[0];
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + chanNumber;
				}
			}
		} 
		else 
		{ // if (chanBit1 == IFXIMAGECHANNEL_LUMINANCE) 
			for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
			{
				pDest[uIndex + 0] = ((U8*)pChannelsSrc1)[0];
				pDest[uIndex + 1] = ((U8*)pChannelsSrc1)[0];
				pDest[uIndex + 2] = ((U8*)pChannelsSrc1)[0];
				pChannelsSrc1 = ((U8*)pChannelsSrc1) + 1;
			}
		}
	} 
	else 
	{ //  if( pChannelsSrc1 && pChannelsSrc2)  
		// copy the color channel
		if (chanBit1 != (U8)IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE && 
			chanBit2 != (U8)IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE) 
		{ // no luminance
			if ( !(chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA) && 
				 !(chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA)) 
			{ // in scr1 and src2 color images
				IFXASSERT(chanNumber==3);
				src1_bpp = m_pContinuationFormats[contInd1].m_bpp;
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_RED) 
						pDest[uIndex + 0]   = ((U8*)pChannelsSrc1)[0];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN) 
						pDest[uIndex + 1] = ((U8*)pChannelsSrc1)[1];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE) 
						pDest[uIndex + 2]  = ((U8*)pChannelsSrc1)[2];
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + src1_bpp;
				}
				src2_bpp = m_pContinuationFormats[contInd2].m_bpp;
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_RED) 
						pDest[uIndex + 0]   = ((U8*)pChannelsSrc2)[0];
					if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN) 
						pDest[uIndex + 1] = ((U8*)pChannelsSrc2)[1];
					if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE) 
						pDest[uIndex + 2]  = ((U8*)pChannelsSrc2)[2];
					pChannelsSrc2 = ((U8*)pChannelsSrc2) + src2_bpp;
				}
			} 
			else if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA)
			{ // in src1 - ALPHA and in scr2 - Color
				IFXASSERT(chanNumber==4);
				src2_bpp = m_pContinuationFormats[contInd2].m_bpp;
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_RED) 
						pDest[uIndex + 0]   = ((U8*)pChannelsSrc2)[0];
					if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN) 
						pDest[uIndex + 1] = ((U8*)pChannelsSrc2)[1];
					if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE) 
						pDest[uIndex + 2]  = ((U8*)pChannelsSrc2)[2];
					pDest[uIndex + 3] = ((U8*)pChannelsSrc1)[0];
					pChannelsSrc2 = ((U8*)pChannelsSrc2) + src2_bpp;
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + 1;
				}
			} 
			else if (chanBit2 & (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA)
			{ // in src1 - Color and in scr2 - Alpha
				IFXASSERT(chanNumber==4);
				src1_bpp = m_pContinuationFormats[contInd1].m_bpp;
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_RED) 
						pDest[uIndex + 0]   = ((U8*)pChannelsSrc1)[0];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN) 
						pDest[uIndex + 1] = ((U8*)pChannelsSrc1)[1];
					if (chanBit1 & (U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE) 
						pDest[uIndex + 2]  = ((U8*)pChannelsSrc1)[2];
					pDest[uIndex + 3] = ((U8*)pChannelsSrc2)[0];
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + src1_bpp;
					pChannelsSrc2 = ((U8*)pChannelsSrc2) + 1;
				}
			} 
		} 
		else 
		{// there is a luminance 
			IFXASSERT(chanNumber==4);
			if (chanBit1 == (U8)IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE ) 
			{ // Src1 luminance, Src2 Alpha 
				IFXASSERT(chanBit2 == (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA);
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					pDest[uIndex + 0] = ((U8*)pChannelsSrc1)[0];
					pDest[uIndex + 1] = ((U8*)pChannelsSrc1)[0];
					pDest[uIndex + 2] = ((U8*)pChannelsSrc1)[0];
					pDest[uIndex + 3] = ((U8*)pChannelsSrc2)[0];
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + 1;
					pChannelsSrc2 = ((U8*)pChannelsSrc2) + 1;
				}
			} 
			else 
			{ // Src1 Alpha, Src2 Luminance 
				IFXASSERT(chanBit1 == (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA);
				for( uIndex=0; uIndex < pImageInfo->m_size; uIndex+=chanNumber) 
				{
					pDest[uIndex + 0] = ((U8*)pChannelsSrc2)[0];
					pDest[uIndex + 1] = ((U8*)pChannelsSrc2)[0];
					pDest[uIndex + 2] = ((U8*)pChannelsSrc2)[0];
					pDest[uIndex + 3] = ((U8*)pChannelsSrc1)[0];
					pChannelsSrc1 = ((U8*)pChannelsSrc1) + 1;
					pChannelsSrc2 = ((U8*)pChannelsSrc2) + 1;
				}
			}
		 } // 
	  } // copy image data
	}

  return iResult;
}


// CIFXImageTools private methods used by image compressor

//---------------------------------------------------------------------------
//  CIFXImageTools::CompressImage
//
//  This method
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::CompressImage(
								void* pSourceImage, STextureSourceInfo* pImageInfo, 
								U32& contInd, void** ppCompressedData) 
{
  IFXRESULT iResult=IFX_OK;

  if(NULL == pSourceImage || NULL == pImageInfo || NULL == ppCompressedData )
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) 
  {
    switch(pImageInfo->m_blockCompressionType[contInd]) 
	{
    case IFXTextureObject::TextureType_Jpeg24:
    case IFXTextureObject::TextureType_Jpeg8:
      iResult = CompressImageJPEG(pSourceImage, pImageInfo, contInd, ppCompressedData);
	  IFXASSERT( iResult == IFX_OK );
      break;
    case IFXTextureObject::TextureType_Png:
      iResult = CompressImagePng(pSourceImage, pImageInfo, contInd, ppCompressedData);
	  IFXASSERT( iResult == IFX_OK );
      break;

	default:       
      iResult = IFX_E_UNSUPPORTED;
	  IFXASSERT( iResult == IFX_OK );
    }
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::DecompressImage
//
//  This method dispatches the compressed data to the correct decompressor.
//  If there is no data, simply return ok.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::DecompressImage(
								void* pCompressedData, STextureSourceInfo* pImageInfo, 
								U32& contInd, void** ppDecompressedImage) 
{
  IFXRESULT iResult=IFX_OK;

  if( NULL == pCompressedData || NULL == pImageInfo || NULL == ppDecompressedImage )
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult) && NULL != pCompressedData) 
  {
    switch( pImageInfo->m_blockCompressionType[contInd] ) 
	{
	case IFXTextureObject::TextureType_Jpeg24:
	case IFXTextureObject::TextureType_Jpeg8:
      iResult = DecompressImageJPEG(
					pCompressedData, pImageInfo, contInd, ppDecompressedImage);
	  IFXASSERT( iResult == IFX_OK );
      break;
	case IFXTextureObject::TextureType_Png:
		iResult = DecompressImagePng(
					pCompressedData, pImageInfo, contInd, ppDecompressedImage);
		IFXASSERT( iResult == IFX_OK );
		break;

    default:
      iResult = IFX_E_UNSUPPORTED;
	  IFXASSERT( iResult == IFX_OK );
    }
  }
  return iResult;
}


IFXRESULT CIFXImageTools::MakeDeclarationBlock(
								IFXDataBlockQueueX *pDataBlockQueue, 
								STextureSourceInfo *pImageInfo)
{
  IFXRESULT rc = IFX_OK;
  IFXBitStreamX *pBitStream = NULL;
  IFXDataBlockX *pDataBlock = NULL;
  U32 iFile;

  if(NULL == pDataBlockQueue || NULL == pImageInfo) 
  {
    rc = IFX_E_INVALID_POINTER;
  }

  if(IFXSUCCESS(rc)) 
  {
    rc = IFXCreateComponent(CID_IFXBitStreamX,IID_IFXBitStreamX,(void**)&pBitStream);
  }
  if(IFXSUCCESS(rc)) 
  {
    pBitStream->WriteIFXStringX((pImageInfo->m_name));
    pBitStream->WriteU32X(pImageInfo->m_height);
    pBitStream->WriteU32X(pImageInfo->m_width);

	U8 textureImageType = GetImageType(pImageInfo->m_imageType);
    pBitStream->WriteU8X(textureImageType);
	// write info on continuation images
    pBitStream->WriteU32X(m_uContinuationImageCount);
  }

  // continutaion image formats are ready by that moment 
  U32 i;
  for( i=0; i<m_uContinuationImageCount; i++) 
  {
	pBitStream->WriteU8X(m_pContinuationFormats[i].m_uCompressionType);
	pBitStream->WriteU8X(m_pContinuationFormats[i].m_uImageChannels);
	pBitStream->WriteU16X(m_pContinuationFormats[i].m_uAttributes);
	if (m_pContinuationFormats[i].m_uAttributes==0) 
	{
		pBitStream->WriteU32X(m_pContinuationFormats[i].m_uImageDataByteCount);
	} 
	else 
	{
		pBitStream->WriteU32X(m_pContinuationFormats[i].m_uExtImageURLCount);
		for (iFile =0; iFile < m_pContinuationFormats[i].m_uExtImageURLCount; iFile++) 
		{
		    pBitStream->WriteIFXStringX(
								*(m_pContinuationFormats[i].m_pImageURLNames[iFile]));
		}
	}
  }

  if(IFXSUCCESS(rc)) 
  {
    pBitStream->GetDataBlockX(pDataBlock);
    pDataBlock->SetBlockTypeX(BlockType_ResourceTextureU3D);
    pDataBlock->SetPriorityX(0);
    pDataBlockQueue->AppendBlockX(*pDataBlock);
  }

  IFXRELEASE(pDataBlock);
  IFXRELEASE(pBitStream);
  IFXRETURN(rc);
}

IFXRESULT CIFXImageTools::ProcessDeclarationBlock(
									IFXDataBlockQueueX *pDataBlockQueueX, 
									STextureSourceInfo *pImageInfo )
{
	IFXRESULT rc = IFX_OK;
	IFXBitStreamX *pBitStream = NULL;
	IFXDataBlockX *pDataBlock = NULL;
	BOOL rbDone = FALSE;

	pDataBlockQueueX->GetNextBlockX( pDataBlock, rbDone );
	rc = IFXCreateComponent(
				CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStream);

	// Wrap datablock in bitstream
	if(IFXSUCCESS(rc))
		pBitStream->SetDataBlockX(*pDataBlock);

	//  Verify the type of datablock - declaration or continuation
	U32 uBlockType=0;
	pDataBlock->GetBlockTypeX( uBlockType );

	//  Should be "declaration"
	if (BlockType_ResourceTextureU3D == uBlockType) 
	{
		// Initialize the image info return structure
		pImageInfo->m_name.Assign(L"");
		pImageInfo->m_width = 0;
		pImageInfo->m_height = 0;
		pImageInfo->m_size = 0;

		pBitStream->ReadIFXStringX( pImageInfo->m_name );
		pBitStream->ReadU32X( pImageInfo->m_height);       
		pBitStream->ReadU32X( pImageInfo->m_width);

		U8 imageType;
		pBitStream->ReadU8X( imageType);
		pImageInfo->m_imageType = GetEncoding(imageType);

		// read Continuation Image Info
		pBitStream->ReadU32X(m_uContinuationImageCount);

		// do not create and fill continuation formats if it is already done
		if( NULL == m_pContinuationFormats )
		{
			m_pContinuationFormats = 
				new IFXContinuationImageFormat[m_uContinuationImageCount];

			U32 i;
			for( i=0; i<m_uContinuationImageCount; i++ ) 
			{
				pBitStream->ReadU8X(m_pContinuationFormats[i].m_uCompressionType);
				pBitStream->ReadU8X(m_pContinuationFormats[i].m_uImageChannels);
				
				m_pContinuationFormats[i].m_bpp = 
					CalculateBitsPerPixel(m_pContinuationFormats[i].m_uImageChannels);
				IFXASSERT( m_pContinuationFormats[i].m_bpp <= 4 && 
						m_pContinuationFormats[i].m_bpp>0 );

				pBitStream->ReadU16X(m_pContinuationFormats[i].m_uAttributes);
				

				if (m_pContinuationFormats[i].m_uAttributes == 0) 
				{
					pBitStream->ReadU32X(m_pContinuationFormats[i].m_uImageDataByteCount);
					// this is to make sure for future calls that there are NO URLs! 
					m_pContinuationFormats[i].m_uExtImageURLCount = 0; 
				} 
				else 
				{
					pBitStream->ReadU32X(m_pContinuationFormats[i].m_uExtImageURLCount);

					m_pContinuationFormats[i].m_pImageURLNames = 
						new IFXString* [m_pContinuationFormats[i].m_uExtImageURLCount];
					
					IFXASSERT(m_pContinuationFormats[i].m_uExtImageURLCount != 0);

					U32 count;
					for ( count = 0; 
						count < m_pContinuationFormats[i].m_uExtImageURLCount; 
						++count ) 
					{
						// read URL name
						IFXString urlName;
						pBitStream->ReadIFXStringX( urlName );

						m_pContinuationFormats[i].m_pImageURLNames[count] = 	
									new IFXString( &urlName );
					
					}
				}
			}
		}
	} 
	else 
	{
		rc = IFX_E_UNDEFINED;
	}

	IFXRELEASE( pBitStream );
	IFXRELEASE(pDataBlock);

	return rc;
}

//
IFXRESULT CIFXImageTools::DecompressImageDeclaration(
								STextureSourceInfo* pImageInfo, 
								void** ppDecompressedImage)
{
	IFXRESULT rc = IFX_OK;

	if(NULL == pImageInfo || NULL == ppDecompressedImage) 
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc)) 
	{
		pImageInfo->m_size = pImageInfo->m_width * pImageInfo->m_height;
		U8 uFormat=0;
		switch(pImageInfo->m_imageType) 
		{
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32:
				uFormat = 4;
				break;
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24:
				uFormat = 3;
				break;
			case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE:
				uFormat = 1;
				break;
			default:
				rc = IFX_E_UNSUPPORTED;
		}

		if(IFXSUCCESS(rc)) 
		{
			pImageInfo->m_size *= uFormat;
		}
	}
	if(IFXSUCCESS(rc)) 
	{
		*ppDecompressedImage = new U32[pImageInfo->m_size];

		if(*ppDecompressedImage == NULL) 
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if(IFXSUCCESS(rc)) 
		{
			memset(*ppDecompressedImage, 117, pImageInfo->m_size);
		}
	}

	IFXRETURN(rc);
}

//---------------------------------------------------------------------------
//  CIFXImageTools::DecompressImageJPEG
//
//  This method decompresses a block queue into raw image data.  This code is
//  all VERY specific to using the IJG decompression library.  The source and
//  docs for the library is located in /TextureManager/IJG.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::DecompressImageJPEG(
								void* pCompressedData, 
								STextureSourceInfo* pImageInfo, 
								U32& contInd, void** ppDecompressedImage)
{
	IFXRESULT iResult=IFX_OK;
	if(NULL == pCompressedData || NULL == pImageInfo || NULL == ppDecompressedImage )
	{
		iResult = IFX_E_INVALID_POINTER;
	}
	BOOL bNeedImageResize = FALSE;
	//int  bpp = 0;

	if(IFXSUCCESS(iResult)) 
	{
		void* pDestBuffer = NULL;
		void* pOutput     = NULL;

		// Here are the suggested steps for decompresing an image
		// from the IJG document: libjpeg.doc  page/line: 2/54
		// ugh, this function is too long...

		// IJG decompression structures...
		struct ifx_jpeg_memory_source_mgr mgr;
		struct jpeg_decompress_struct cinfo;
		struct ifx_jpeg_error_mgr jerr;

		// allocate and initialize the decompression object...
		// and the error handler...
		jpeg_create_decompress( &cinfo);

		// create the jpeg source manager...
		// note that we are using a structure derived from
		// the ""struct jpeg_source_mgr"" that the JPEG callback
		// routines want, this is to give us a place to store
		// the object "this" pointer so that we can get at the
		// compressed buffer and its length...
		cinfo.src = create_ifx_jpeg_memory_source_mgr( &mgr, this);

		// create the error handler, we need to use our own
		// instead of the default to override the default error
		// behavior of exiting the program...
		cinfo.err = create_ifx_jpeg_error_mgr( &jerr, this);

		// set the image source to the local buffer
		cinfo.src->next_input_byte = (const unsigned char*)pCompressedData;
		cinfo.src->bytes_in_buffer = 0;

		// actually try to decompress the image...
		// catch errors thrown in the IJG library and convert
		// to our own error code...
		if (setjmp(m_envErrorRecovery) == 0) 
		{
			// read the jpeg header...
			jpeg_read_header( &cinfo, TRUE);

			// start decompression...
			jpeg_start_decompress( &cinfo);

			// allocate the buffer to the proper size and requested
			// format for the JPEG image and set the pImageInfo data
			if (pImageInfo->m_width != cinfo.output_width || 
				pImageInfo->m_height != cinfo.output_height) 
			{
				IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_WARNING, 
					L"DecompressImageJPEG: JPEG image size (width and/or height) is "
					L"different from size specified in image declaration block. Image "
					L"will be resized.\n");
				bNeedImageResize = TRUE;	
			}

			if ( cinfo.output_components != m_pContinuationFormats[contInd].m_bpp) 
			{
				IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_WARNING, 
					L"DecompressImageJPEG: JPEG image channel number conflict could "
					L"not be resolve.\n");
				iResult = IFX_E_UNSUPPORTED;
			}

			if (IFXSUCCESS(iResult) ) 
			{
				pImageInfo->m_size = 
					cinfo.output_width * cinfo.output_height * cinfo.output_components;  
				// resize the buffer
				pDestBuffer = IFXReallocate( *ppDecompressedImage, pImageInfo->m_size);
				if(NULL == pDestBuffer && pImageInfo->m_size != 0) 
				{
					iResult = IFX_E_OUT_OF_MEMORY;
				}				
				if (bNeedImageResize) 
				{
					pOutput = IFXAllocate(
									pImageInfo->m_width *
									pImageInfo->m_height *
									m_pContinuationFormats[contInd].m_bpp );
				}
				// decompress the image - scan line by scan line
				if(IFXSUCCESS(iResult))
				{
					iResult = DecompressJPEGScanLines(
									(U8*)pDestBuffer, &cinfo, pImageInfo, contInd );
				}

				if ( IFXSUCCESS(iResult) && bNeedImageResize ) 
				{
					iResult = IFXTextureImageTools_ResizeImage(
										(U8*)pDestBuffer, (U8*)pOutput, 
										cinfo.num_components, FALSE, 
										cinfo.image_width, cinfo.image_height, 
										pImageInfo->m_width, pImageInfo->m_height);
	
					if(IFXSUCCESS(iResult))
					{	
						IFXDeallocate(pDestBuffer);
						pImageInfo->m_size = 
							pImageInfo->m_width * 
							pImageInfo->m_height * 
							m_pContinuationFormats[contInd].m_bpp;
						*ppDecompressedImage=(void*)pOutput;
					}
				} 
				else 
				{
					*ppDecompressedImage=(void*)pDestBuffer;
				}
			} 

			// clean up decompression object...
			jpeg_finish_decompress( &cinfo);
			jpeg_destroy_decompress( &cinfo);
		}  
		else 
		{
			// error was caught...
			// clean up cinfo object and return error...
			jpeg_destroy_decompress( &cinfo);
			iResult = IFX_TEXTURE_ERROR_DECODING_JPEG;
		}			
	}

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::CompressImageJPEG
//
//  This method compresses raw image data into compressed image data.  This code
//  is all VERY specific to using the IJG decompression library.  The source and
//  docs for the library is located in 3d/TextureManager/IJG.
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::CompressImageJPEG(
								void* pSourceImage, 
								STextureSourceInfo* pImageInfo, 
								U32& contInd, void** ppCompressedData)
{
  IFXRESULT iResult=IFX_OK;

  U32 OutputBufferSize = 0;
  if(NULL == ppCompressedData || NULL == pImageInfo || NULL == pSourceImage )
    iResult =IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) 
  {
    // Here are the suggested steps for Compresing an image
    // from the IJG document: libjpeg.doc  page/line: 5/53

    // IJG decompression structures...
    struct ifx_jpeg_memory_destination_mgr mgr;
    struct jpeg_compress_struct cinfo;
    struct ifx_jpeg_error_mgr jerr;

    // 1. Allocate and initialize a JPEG compression object
    // and the error handler...
    jpeg_create_compress( &cinfo);

    // 2. Specify the destination for the compressed data (eg, a file)
    // note that we are using a structure derived from
    // the ""struct jpeg_source_mgr"" that the JPEG callback
    // routines want, this is to give us a place to store
    // the object "this" pointer so that we can get at the
    // compressed buffer and its length...

    cinfo.dest = create_jpeg_memory_destination_mgr( &mgr, this);

    // create the error handler, we need to use our own
    // instead of the default to override the default error
    // behavior of exiting the program...
    cinfo.err = create_ifx_jpeg_error_mgr( &jerr, this);

    // 3. Set parameters for compression, including image size & colorspace
    cinfo.image_width = pImageInfo->m_width;
    cinfo.image_height = pImageInfo->m_height;

    // 3b. Size, create and set the destination buffer
    OutputBufferSize = (JPEG_HEADER_BLOCK_MULTIPLIER*pImageInfo->m_height) +
              (pImageInfo->m_width*pImageInfo->m_height*4)+JPEG_HEADER_BLOCK_MULTIPLIER;

	*ppCompressedData= new U8 [OutputBufferSize ];
    if(NULL == *ppCompressedData)
	{
      iResult = IFX_E_OUT_OF_MEMORY;
	}

    cinfo.optimize_coding = TRUE;

    if(IFXSUCCESS(iResult)) 
	{
      cinfo.dest->next_output_byte = (unsigned char*) *ppCompressedData;
      cinfo.dest->free_in_buffer =  OutputBufferSize;

      switch(m_pContinuationFormats[contInd].m_uCompressionType) 
	  {
      case IFXTextureObject::TextureType_Jpeg8:
        cinfo.input_components = 1;         // 1 component per pixel = alpha
        cinfo.in_color_space = JCS_GRAYSCALE;   // colorspace is single channel
        break;
      case IFXTextureObject::TextureType_Jpeg24:
        cinfo.input_components = 3;         // 3 components per pixel = rgb
        cinfo.in_color_space = JCS_RGB;       // colorspace is RGB for 24 bit images
        break;
      default:
        iResult = IFX_E_UNSUPPORTED;

      }	  
	  jpeg_set_defaults(&cinfo);

      if (setjmp(m_envErrorRecovery) == 0) 
	  {
        U8 uQualitySetting;
        uQualitySetting = pImageInfo->m_compressionQuality;
        if(uQualitySetting>100)
          uQualitySetting = 100;
        jpeg_set_quality(&cinfo, uQualitySetting, TRUE);

        // 4. jpeg_start_compress(...);
        jpeg_start_compress(&cinfo, TRUE);

        // 5. compress the scanlines
        iResult = CompressJPEGScanLines((U8*)pSourceImage, &cinfo, pImageInfo, contInd);

        // 6. jpeg_finish_compress(...);
        jpeg_finish_compress(&cinfo);

        // calculate the size of the compressed buffer
        pImageInfo->m_size = OutputBufferSize - (cinfo.dest->free_in_buffer);

        // 7. Release the JPEG compression object
        jpeg_destroy_compress(&cinfo);
      } 
	  else 
	  {
        // error was caught...
        // clean up cinfo object and return error...
        jpeg_destroy_compress( &cinfo);
        iResult = IFX_TEXTURE_ERROR_DECODING_JPEG;
      }

      // resize the output buffer to correct byte length
      //ppCompressedData;
    }
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::CompressJPEGScanLines
//
//  This method handles the individual scan lines for compression.
//---------------------------------------------------------------------------

IFXRESULT CIFXImageTools::CompressJPEGScanLines(
								U8* pSourceImageBuffer, jpeg_compress_struct* pcinfo, 
								STextureSourceInfo* pImageInfo, U32& continuationIndex)
{
  IFXRESULT iResult = IFX_OK;

  if( NULL == pSourceImageBuffer || NULL == pcinfo || NULL == pImageInfo)
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) 
  {
    U8* pTexels = pSourceImageBuffer;
    U8  uPixelSize=0;
    U32 uNumRows=pImageInfo->m_height;

	uPixelSize = m_pContinuationFormats[continuationIndex].m_bpp;

	if (uPixelSize!=1 && uPixelSize!=3 && uPixelSize!=4) 
	{
		IFXASSERT(0);
		iResult = IFX_E_UNSUPPORTED;
	}
    // point to last line of source image since compressor compresses upside-down
    pTexels = 
		&pSourceImageBuffer[ (pImageInfo->m_width * 
							 (pImageInfo->m_height-1) * 
							 uPixelSize) ];

    // decompress the JPEG image 1 scan line at a time
    // converting the pixels into the requested format...
    JSAMPROW pScanline=NULL;
    pScanline = new JSAMPLE[3 * pImageInfo->m_width];
    if(NULL == pScanline)
      iResult = IFX_E_OUT_OF_MEMORY;

    while(uNumRows!=0 && IFXSUCCESS(iResult)) 
	{
      // handle RGBA source - IFXTEXTUREMAP_FORMAT_RGBA32
      if ( 4 == uPixelSize) 
	  {
        if(IFXSUCCESS(iResult)) 
		{
          // copy the old data into this buffer
          U32 i;
          for ( i = 0; i < 3*pImageInfo->m_width; i += 3) 
		  {
            pScanline[i] = pTexels[0];
            pScanline[i+1] = pTexels[1];
            pScanline[i+2] = pTexels[2];
            pTexels=pTexels+uPixelSize;
          }
          jpeg_write_scanlines(pcinfo, &pScanline, 1);
          pTexels = pTexels - (pImageInfo->m_width*2*uPixelSize);
        }
      }
      // handle RGB source - IFXTEXTUREMAP_FORMAT_RGB24
      else if( 3 == uPixelSize) 
	  {
        U32 i;
        for ( i = 0; i < 3*pImageInfo->m_width; i += 3) 
		{
          pScanline[i] = pTexels[0];
          pScanline[i+1] = pTexels[1];
          pScanline[i+2] = pTexels[2];
          pTexels=pTexels+uPixelSize;
        }
        jpeg_write_scanlines(pcinfo, &pScanline, 1);
        pTexels = pTexels - (pImageInfo->m_width*2*uPixelSize);

      }
      // handle 8-bit source - IFXTEXTUREMAP_FORMAT_LUMINANCE
      else if( 1 == uPixelSize) 
	  {
        U32 i;
        for ( i = 0; i < 1*pImageInfo->m_width; i++) 
		{
          pScanline[i] = pTexels[0];
          pTexels++;
        }
        jpeg_write_scanlines(pcinfo, &pScanline, 1);
        pTexels = pTexels - (pImageInfo->m_width*2*uPixelSize);

      } 
	  else 
	  {
        // output only trace messages in debug build...
        IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, 
			IFXDEBUG_ERROR, 
			L"CompressImage: unrecognized number of channels (not 1, 3, or 4).\n");
        iResult = IFX_E_UNSUPPORTED;
      }
      uNumRows--;
    }

    delete [] pScanline;
  }

  return iResult;
}


// CIFXImageTools private methods used by image decompressor

//---------------------------------------------------------------------------
//  CIFXImageTools::DecompressJPEGScanLines
//
//  This method decompresses the scan lines
//---------------------------------------------------------------------------

IFXRESULT CIFXImageTools::DecompressJPEGScanLines(
								U8* pImageBase, jpeg_decompress_struct* pcinfo, 
								STextureSourceInfo* pImageInfo, U32& contInd )
{

  IFXRESULT iResult = IFX_OK;

  if( NULL == pImageBase )
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) 
  {
    U8* pTexels = pImageBase;
    U8* pBase = NULL;
    U8  uDestinationPixelSize=0;
   
	uDestinationPixelSize = m_pContinuationFormats[contInd].m_bpp;

	if (uDestinationPixelSize!=1 && 
		uDestinationPixelSize!=3 && 
		uDestinationPixelSize!=4) 
	{
		IFXASSERT(0);
		iResult = IFX_E_UNSUPPORTED;
	}

	U32  uScanLineWidth = uDestinationPixelSize * pcinfo->output_width;

    // set base to point to first pixel of last row...
    pBase = &pTexels[((pcinfo->output_height - 1) * uScanLineWidth)];

    // decompress the JPEG image 1 scan line at a time
    // converting the pixels into the requested format...
    JSAMPARRAY scanlines;
    JSAMPROW scanline;
    scanlines = &scanline;
    if (3 == pcinfo->out_color_components) 
	{
      // RGB image...
      scanline = new JSAMPLE[3 * pcinfo->output_width];
      U32 j;
      for (  j = 0; j < pcinfo->output_height; j++) 
	  {
        pTexels = pBase;  // added to fix upside-down problem

        jpeg_read_scanlines( pcinfo, scanlines, 1);
		if (uDestinationPixelSize == 1) 
		{
          // output to a luminance channel only
          U32 i;
          for ( i = 0; i < 3*pcinfo->output_width; i += 3) 
		  {
            *pTexels++ = scanline[i+1];
          }
        }
		else if (uDestinationPixelSize == 3) 
		{
          // output to RGB24...
          U32 i;
          for ( i = 0; i < 3*pcinfo->output_width; i += 3) 
		  {
            pTexels[0] = scanline[i];
            pTexels[1] = scanline[i + 1];
            pTexels[2] = scanline[i + 2];
            pTexels += 3;
          }
        }
        else 
		{
          // output to RGBA32...
          U32 i;
          for ( i = 0; i < 3*pcinfo->output_width; i += 3) 
		  {
            pTexels[0] = scanline[i];
            pTexels[1] = scanline[i + 1];
            pTexels[2] = scanline[i + 2];
            pTexels[3] = 0;
            pTexels += 4;
          }
        }
        pBase -= uScanLineWidth;  // added to fix upside-down problem
      }
      delete [] scanline;
    }
    else if (1 == pcinfo->out_color_components) 
	{
      // greyscale image
      scanline = new JSAMPLE[pcinfo->output_width];
      U32 j;
      for (  j = 0; j < pcinfo->output_height; j++) 
	  {
        pTexels = pBase;    // added to fix upside-down problem
        jpeg_read_scanlines( pcinfo, scanlines, 1);
		if (uDestinationPixelSize == 1) 
		{
          // output LUMINANCE...
          U32 i;
          for ( i = 0; i < pcinfo->output_width; i++) 
		  {
            *pTexels++ = scanline[i];
          }
        }
		else if (uDestinationPixelSize == 3) 
		{
          // output RGB24...
          U32 i;
          for ( i = 0; i < pcinfo->output_width; i++) 
		  {
            pTexels[0] = pTexels[1] = pTexels[2] = scanline[i];
            pTexels += 3;
          }
        }
        else 
		{
          // output RGBA32...
          U32 i;
          for ( i = 0; i < pcinfo->output_width; i++) 
		  {
            pTexels[0] = scanline[i];
            pTexels[1] = scanline[i];
            pTexels[2] = scanline[i];
            pTexels[3] = scanline[i];
            pTexels += 4;
          }
        }
        pBase -= uScanLineWidth;  // added to fix upside-down problem
      }
      delete [] scanline;
    } 
	else 
	{
      // clean up decompression object...
      jpeg_destroy_decompress( pcinfo );

      // output only trace messages in debug build...
      IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, 
		  IFXDEBUG_ERROR, 
		  L"DecompressImage: unrecognized number of channels (not 1 or 3).\n");

      // return decoding error...
      iResult = IFX_TEXTURE_ERROR_DECODING_JPEG;
    }

  }

  return iResult;
}


// IJG - source manager - handles decompression callbacks

//---------------------------------------------------------------------------
//  CIFXImageTools::create_ifx_jpeg_memory_source_mgr
//
//  This method
//---------------------------------------------------------------------------
struct jpeg_source_mgr* CIFXImageTools::create_ifx_jpeg_memory_source_mgr( 
											struct ifx_jpeg_memory_source_mgr *pmgr, 
											CIFXImageTools *pSource)
{
  pmgr->base.next_input_byte = NULL;
  pmgr->base.bytes_in_buffer = 0;
  pmgr->base.init_source = CIFXImageTools::ijg_init_source;
  pmgr->base.fill_input_buffer = CIFXImageTools::ijg_fill_input_buffer;
  pmgr->base.skip_input_data = CIFXImageTools::ijg_skip_input_data;
  pmgr->base.resync_to_restart = CIFXImageTools::ijg_resync_to_restart;
  pmgr->base.term_source = CIFXImageTools::ijg_term_source;
  pmgr->pSource = pSource;
  return (struct jpeg_source_mgr *) pmgr;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_init_source
//
//  This method initializes the source.  This is called by jpeg_read_header()
//  before any data is actually read.  Unlike init_destination(), it may leave
//  bytes_in_buffer set to 0 (in which case a fill_input_buffer() call
//  will occur immediately).
//---------------------------------------------------------------------------
void CIFXImageTools::ijg_init_source( j_decompress_ptr cinfo)
{
  cinfo->src->bytes_in_buffer = 0;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_fill_input_buffer
//
//  This method is called whenever bytes_in_buffer has reached zero and more
//  data is wanted.  In typical applications, it should read fresh data
//  into the buffer (ignoring the current state of next_input_byte and
//  bytes_in_buffer), reset the pointer & count to the start of the
//  buffer, and return TRUE indicating that the buffer has been reloaded.
//  It is not necessary to fill the buffer entirely, only to obtain at
//  least one more byte.  bytes_in_buffer MUST be set to a positive value
//  if TRUE is returned.  A FALSE return should only be used when I/O
//  suspension is desired
//---------------------------------------------------------------------------
boolean CIFXImageTools::ijg_fill_input_buffer( j_decompress_ptr cinfo)
{
  return TRUE;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_skip_input_data
//
//  This method skips num_bytes worth of data.  The buffer pointer and count
//  should be advanced over num_bytes input bytes, refilling the buffer as
//  needed.  This is used to skip over a potentially large amount of
//  uninteresting data (such as an APPn marker).  In some applications
//  it may be possible to optimize away the reading of the skipped data,
//  but it's not clear that being smart is worth much trouble; large
//  skips are uncommon.  bytes_in_buffer may be zero on return.
//  A zero or negative skip count should be treated as a no-op.
//---------------------------------------------------------------------------
void CIFXImageTools::ijg_skip_input_data( j_decompress_ptr cinfo, long num_bytes)
{
  cinfo->src->next_input_byte += num_bytes;
  cinfo->src->bytes_in_buffer -= num_bytes;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_resync_to_restart
//
//  This method is called only when the decompressor has failed to find
//  a restart (RSTn) marker where one is expected.  Its mission is to
//  find a suitable point for resuming decompression.  For most
//  applications, we recommend that you just use the default resync
//  procedure, jpeg_resync_to_restart().  However, if you are able to back
//  up in the input data stream, or if you have a-priori knowledge about
//  the likely location of restart markers, you may be able to do better.
//  Read the read_restart_marker() and jpeg_resync_to_restart() routines
//  in jdmarker.c if you think you'd like to implement your own resync
//  procedure.
//---------------------------------------------------------------------------
boolean CIFXImageTools::ijg_resync_to_restart( j_decompress_ptr cinfo, int desired)
{
  return jpeg_resync_to_restart( cinfo, desired);
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_term_source
//
//  This method terminates the source - called by jpeg_finish_decompress()
//  after all data has been read.  Often a no-op.
//---------------------------------------------------------------------------
void CIFXImageTools::ijg_term_source( j_decompress_ptr cinfo)
{
  return;
}



// IJG - destination manager - handles compression callbacks

//---------------------------------------------------------------------------
//  CIFXImageTools::create_ifx_jpeg_memory_source_mgr
//
//  This method
//---------------------------------------------------------------------------
struct jpeg_destination_mgr* CIFXImageTools::create_jpeg_memory_destination_mgr( 
											struct ifx_jpeg_memory_destination_mgr *pmgr,
											CIFXImageTools *pDestination)
{
  pmgr->base.init_destination = CIFXImageTools::init_destination;
  pmgr->base.empty_output_buffer = CIFXImageTools::empty_output_buffer;
  pmgr->base.term_destination = CIFXImageTools::term_destination;
  pmgr->pDestination = pDestination;
  return (struct jpeg_destination_mgr *) pmgr;
}


//---------------------------------------------------------------------------
//  CIFXImageTools::init_destination
//
//  This method initializes the destination buffer.  This is called by
//  jpeg_start_compress() before any data is actually written.  It must
//  initialize next_output_byte and free_in_buffer.  free_in_buffer must be
//  initialized to a positive value.
//---------------------------------------------------------------------------
void CIFXImageTools::init_destination( j_compress_ptr cinfo )
{
//  cinfo->dest->next_output_byte = NULL;
//  cinfo->dest->free_in_buffer = 0;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::empty_output_buffer
//
//  This method is called whenever the buffer has filled (free_in_buffer
//  reaches zero).  In typical applications, it should write out the
//  *entire* buffer (use the saved start address and buffer length;
//  ignore the current state of next_output_byte and free_in_buffer).
//  Then reset the pointer & count to the start of the buffer, and
//  return TRUE indicating that the buffer has been dumped.
//  free_in_buffer must be set to a positive value when TRUE is
//  returned.  A FALSE return should only be used when I/O suspension is
//  desired
//---------------------------------------------------------------------------
boolean CIFXImageTools::empty_output_buffer( j_compress_ptr cinfo )
{
  return TRUE;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::term_destination
//
//  This method terminates destination - called by jpeg_finish_compress() after
//  all data has been written.  In most applications, this must flush any
//  data remaining in the buffer.  Use either next_output_byte or
//  free_in_buffer to determine how much data is in the buffer.
//---------------------------------------------------------------------------
void CIFXImageTools::term_destination( j_compress_ptr cinfo )
{

}



// IJL error manager - handles error callbacks

//---------------------------------------------------------------------------
//  CIFXImageTools::create_ifx_jpeg_error_mgr
//
//  This method
//---------------------------------------------------------------------------
struct jpeg_error_mgr* CIFXImageTools::create_ifx_jpeg_error_mgr(
											struct ifx_jpeg_error_mgr *errmgr, 
											CIFXImageTools *pSource)
{
  jpeg_std_error(&errmgr->base);
  errmgr->base.error_exit = CIFXImageTools::ijg_error_exit;
  errmgr->base.emit_message = CIFXImageTools::ijg_emit_message;
  errmgr->pSource = pSource;
  return (struct jpeg_error_mgr *) errmgr;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_error_exit
//
//  This method
//---------------------------------------------------------------------------
void CIFXImageTools::ijg_error_exit( j_common_ptr cinfo)
{
  // output only trace messages in debug build...
  IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, 
	  IFXDEBUG_ERROR, L"CIFXImageTools: error while decompressing image.\n");
  CIFXImageTools *pSource = ((struct ifx_jpeg_error_mgr *)(cinfo->err))->pSource;
  longjmp( pSource->m_envErrorRecovery, 1);
  return;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::ijg_emit_message
//
//  This method
//---------------------------------------------------------------------------
void CIFXImageTools::ijg_emit_message( j_common_ptr cinfo, int msg_level)
{
  // output only error messages from library in debug build...
  // trace messages have (msg_level >= 0) error and warning
  // messages are less than zero...
  if (msg_level < 0) {
    if (msg_level == -1)
      IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_ERROR, 
		L"CIFXImageTools: IJG warning while decompressing image.\n");
    else
      IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_ERROR, 
		L"CIFXImageTools: IJG error while decompressing image.\n");
  }
  return;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::CompressImagePng
//
//  This method compresses raw image data into compressed image data.  This code
//  is all VERY specific to using the PNG library. 
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::CompressImagePng(
								void* pSourceImage, STextureSourceInfo* pImageInfo, 
								U32& contInd, void** ppCompressedData)

{
	IFXRESULT iResult = IFX_OK;
	if(NULL == ppCompressedData || NULL == pImageInfo || NULL == pSourceImage )
		iResult =IFX_E_INVALID_POINTER;

	U32 OutputBufferSize = 0;
	png_structp	png_ptr;
	png_infop	info_ptr;
	U32	ciChannels; 
	ciChannels = m_pContinuationFormats[contInd].m_bpp;
	if (ciChannels!=1 && ciChannels!=2 && ciChannels!=3 && ciChannels!=4) 
	{
		IFXASSERT(0);
		iResult = IFX_E_UNSUPPORTED;
	}

	if(IFXSUCCESS(iResult)) 
	{
		png_voidp		user_error_ptr		= NULL; 
		png_error_ptr	user_error_fn		= NULL;
		png_error_ptr	user_warning_fn		= NULL;
		static png_byte   **ppbRowPointers  = NULL;
	    const U32               ciBitDepth  = 8;
		
		// prepare the standard PNG structures

		png_ptr = png_create_write_struct(
							PNG_LIBPNG_VER_STRING, 
							user_error_ptr, 
							user_error_fn, 
							user_warning_fn);
		if (png_ptr == NULL)
		{
			iResult = IFX_FALSE;
			return iResult;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) 
		{
			png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
			iResult = IFX_FALSE;
			return iResult;
		}
		
		/* Set error handling.   */
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			/* If we get here, we had a problem reading the image */
			png_destroy_write_struct(&png_ptr, &info_ptr);
			iResult = IFX_FALSE;
			return iResult;
		}		

		// Size, create and set the destination buffer
		/**
			@todo - need to find out what is top limit for PNG buffer; 
			now 1024 is just "best guess" constant
		*/
	    OutputBufferSize	= (pImageInfo->m_width*pImageInfo->m_height*4) + 1024;
		
		*ppCompressedData	= new U8 [ OutputBufferSize];
		if(NULL == *ppCompressedData) 
		{
			png_destroy_write_struct(&png_ptr,  &info_ptr);
			iResult = IFX_E_OUT_OF_MEMORY;
			return iResult;
		}

		png_bytep io_ptr = (png_bytep)(*ppCompressedData);
		// Set our own write function 
        png_set_write_fn(
			png_ptr, (png_voidp)(&io_ptr), png_write_data_fn, png_flush);
		
        
		// Set PNG header 

		int iColorType = 2;
		switch (ciChannels) 
		{
		case 3:
			iColorType = PNG_COLOR_TYPE_RGB;
			break;
		case 4:
			iColorType = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		case 1:
			iColorType = PNG_COLOR_TYPE_GRAY;
			break;
		case 2:
			iColorType = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		default:
			IFXASSERT(0); 
			break;
		};

		png_set_IHDR(
			png_ptr, info_ptr, pImageInfo->m_width, 
			pImageInfo->m_height, ciBitDepth,
            iColorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
            PNG_FILTER_TYPE_BASE);
        
        // write the file header information
        png_write_info(png_ptr, info_ptr);
        
        // row_bytes is the width x number of channels
        U32 ulRowBytes = pImageInfo->m_width * ciChannels;
        
        // we can allocate memory for an array of row-pointers
		if ( NULL == 
				(ppbRowPointers = (png_bytepp) new png_bytep [pImageInfo->m_height]) ) 
		{
            IFXASSERT(0);
		}
        
        // set the individual row-pointers to point at the correct offsets
		U32 i;
        for (i = 0; i < pImageInfo->m_height; i++)
		{
			/// @todo need to investigate here where we need set origin
			ppbRowPointers[i] = 
				(png_bytep)pSourceImage + (pImageInfo->m_height - i - 1) * ulRowBytes;
			//ppbRowPointers[i] = (png_bytep)pSourceImage + i * ulRowBytes;
		}
        
        // write out the entire image data in one call
        png_write_image (png_ptr, ppbRowPointers);
        
        // write the additional chunks to the PNG file (not really needed)
        png_write_end(png_ptr, info_ptr);

		// calculate size of written PNG data
		pImageInfo->m_size = 
			(*(png_bytepp)(png_get_io_ptr(png_ptr))) - (png_bytep)(*ppCompressedData);
        // and we're done
        
        delete [] ppbRowPointers;
        ppbRowPointers = NULL;
        
        // clean up after the write, and free any memory allocated
        png_destroy_write_struct(&png_ptr, &info_ptr);
	}

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXImageTools::DecompressImagePng
//
//  This method decompresses  compressed PNG image data into raw image data into.  
//  This code is all VERY specific to using the PNG library. 
//---------------------------------------------------------------------------
IFXRESULT CIFXImageTools::DecompressImagePng(
								void* pCompressedData, STextureSourceInfo* pImageInfo, 
								U32& contInd, void** ppDecompressedImage)
{

	IFXRESULT iResult = IFX_OK;
	if(NULL == pCompressedData || NULL == pImageInfo || NULL == ppDecompressedImage )
	{
		iResult =IFX_E_INVALID_POINTER;
	}

	png_structp			png_ptr			= NULL;
	png_infop			info_ptr		= NULL;
	//png_voidp			user_error_ptr	= NULL;
	png_error_ptr		user_error_fn	= NULL;
	//png_error_ptr		user_warning_fn	= NULL;

	png_uint_32			width_png, height_png;
    int                 iBitDepth;
    int                 iColorType;
	png_byte*           pbImageData         = NULL; 
	static png_byte   **ppbRowPointers		= NULL;

	// will need resize if width and height in pImageInfo are 
	// different from them in PNG data
	BOOL				bNeedImageResize	= FALSE;

	BOOL				bHasAlpha			= FALSE;

    // first check the eight byte PNG signature - copressed image should start from it 
    if (png_sig_cmp((png_bytep)pCompressedData, 0, 8))  
	{
		// not PNG header 
	    IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_ERROR, 
			L"DecompressImagePng: not PNG signature at image data block.\n");
		return IFX_E_UNDEFINED;
    }

    // create the two png(-info) structures
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
      (png_error_ptr)user_error_fn, (png_error_ptr)NULL);

	if (!png_ptr)
	{
        return IFX_E_UNDEFINED;
	}

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
	{
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return IFX_E_UNDEFINED;
    }
	
	// Set error handling.  
	if (setjmp(png_jmpbuf(png_ptr))) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		iResult = IFX_E_UNDEFINED;
		return iResult;
	}

    // resize the destination buffer, size if stored as RGBA
    pImageInfo->m_size = pImageInfo->m_width * pImageInfo->m_height * 4;
	if( pImageInfo->m_size == 0) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        iResult = IFX_E_OUT_OF_MEMORY;
		return iResult;
	}

	png_bytep io_ptr = (png_bytep)(pCompressedData);
	png_set_read_fn(png_ptr, (png_voidp)(&io_ptr), png_read_data_fn);

    // read all PNG info up to image data
    png_read_info(png_ptr, info_ptr);
	
    // get width, height, bit-depth and color-type
    png_get_IHDR(
		png_ptr, info_ptr, &width_png, &height_png, 
		&iBitDepth, &iColorType, NULL, NULL, NULL);
	IFXASSERT(iBitDepth==8);
	IFXASSERT( // this is temp for debugging 
		iColorType==PNG_COLOR_TYPE_RGB || iColorType==PNG_COLOR_TYPE_RGB_ALPHA || 
		iColorType==PNG_COLOR_TYPE_GRAY || iColorType==PNG_COLOR_TYPE_GRAY_ALPHA);  

	if (pImageInfo->m_width != width_png || pImageInfo->m_height != height_png) 
	{
	    IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_WARNING, 
			L"DecompressImagePng: PNG image size (width and/or height) is different "
			L"from size specified in image declaration block. Image will be resized.\n");
		bNeedImageResize = TRUE;	
	}

	if ( png_get_channels(png_ptr, info_ptr) != m_pContinuationFormats[contInd].m_bpp) 
	{
	    IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_WARNING, 
			L"DecompressImagePng: PNG image channel number is different from one "
			L"specified in image declaration block. Trying to handle...\n");

		if (m_uContinuationImageCount==1) 
		{
		    IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_WARNING, 
				L"DecompressImagePng: Channel number and image type specified in "
				L"PNG image data will be used.\n");
	
			m_pContinuationFormats[contInd].m_bpp = png_get_channels(png_ptr, info_ptr);
			switch (iColorType) 
			{
			case PNG_COLOR_TYPE_RGB: 
				if (pImageInfo->m_imageType ==
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32) 
				{
					pImageInfo->m_imageType = 
						IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24;
				}
				if (pImageInfo->m_imageType == 
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32) 
				{
					pImageInfo->m_imageType = 
						IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24;
				}
				m_pContinuationFormats[contInd].m_uImageChannels ^= 
					IFXIMAGECHANNEL_ALPHA;
				pImageInfo->m_blockChannels[contInd] ^= IFXIMAGECHANNEL_ALPHA;
				break;
			case PNG_COLOR_TYPE_RGB_ALPHA: 
				if (pImageInfo->m_imageType == 
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24) 
				{
					pImageInfo->m_imageType = 
						IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32;
				}
				if (pImageInfo->m_imageType == 
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24) 
				{
					pImageInfo->m_imageType = 
						IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32;
				}
				m_pContinuationFormats[contInd].m_uImageChannels |= 
					IFXIMAGECHANNEL_ALPHA;
				pImageInfo->m_blockChannels[contInd] |= IFXIMAGECHANNEL_ALPHA;
				break;
			default: 
				// once we get here - need to think if 
				// this is right decision for other ...
				IFXASSERT(0);
				break;
			}
		} 
		else 
		{
			IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_ERROR, 
			L"DecompressImagePng: channel number conflict could not be reslove.\n");
			iResult = IFX_E_UNDEFINED;
		}
	}

	if (IFXSUCCESS(iResult)) 
	{
		if (iColorType==PNG_COLOR_TYPE_RGB_ALPHA || 
			iColorType==PNG_COLOR_TYPE_GRAY_ALPHA) 
		{
			bHasAlpha = TRUE;
		}

		// row_bytes is the width x number of channels
		png_uint_32 ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);

		if ((pbImageData = 
			(png_byte *) new png_byte [ulRowBytes * height_png ]) == NULL ) 
		{
			png_error(png_ptr, "PNG image : out of memory");
			iResult = IFX_E_OUT_OF_MEMORY;
		}
		// and allocate memory for an array of row-pointers        

		if (IFXSUCCESS(iResult)) 
		{
			if ((ppbRowPointers = (png_bytepp) new png_bytep [ height_png ]) == NULL ) 
			{
				png_error(png_ptr, "PNG image : out of memory");
				iResult = IFX_E_OUT_OF_MEMORY;
			}
		}

		if (IFXSUCCESS(iResult)) 
		{
			// set the individual row-pointers to point at the correct offsets
			U32 i;
			for ( i = 0; i < height_png; i++) 
			{			
				ppbRowPointers[i] = pbImageData + (height_png - i - 1 ) * ulRowBytes;				
			}

			// now we can go ahead and just read the whole image
			png_read_image(png_ptr, ppbRowPointers);
			
			if (!bNeedImageResize)	
			{
				*ppDecompressedImage = pbImageData;
			} 
			else 
			{
				*ppDecompressedImage = 
					IFXReallocate( *ppDecompressedImage, pImageInfo->m_size);
			}
		}
		// read the additional chunks in the PNG file (not really needed)
		png_read_end(png_ptr, NULL);

		if (IFXSUCCESS(iResult) && bNeedImageResize) 
		{
			iResult = IFXTextureImageTools_ResizeImage(
							pbImageData, (U8*)(*ppDecompressedImage), 
							png_get_channels(png_ptr, info_ptr), bHasAlpha, 
							width_png, height_png, 
							pImageInfo->m_width, pImageInfo->m_height);
			if (!IFXSUCCESS(iResult)) 
			{
				IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_IMAGE_TOOLS, IFXDEBUG_ERROR, 
					L"DecompressImagePng: Error at re-sizing image.\n");
			}
		}
	}
        
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	if (ppbRowPointers) 
	{
		delete [] ppbRowPointers;
	}
	if (bNeedImageResize && pbImageData ) 
	{
		delete pbImageData;
	}
	return iResult;
}


U8 CIFXImageTools::GetImageType(U8 eEncoding) 
{
	U8 imageType = 0;

	switch (eEncoding) 
	{
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE:
		imageType = IFXIMAGE_TYPE_LUMINANCE;
		break;
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24:
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24:
		imageType = IFXIMAGE_TYPE_RGB;
		break;
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32:
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32:
		imageType = IFXIMAGE_TYPE_RGBA;
		break;
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_ALPHA:
		imageType = IFXIMAGE_TYPE_ALPHA;
		break;
	case IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA:
		imageType = IFXIMAGE_TYPE_LUMINANCE_ALPHA;
		break;
	default:
		IFXASSERT(0);
		break;
	};

	return imageType;
}

U8 CIFXImageTools::GetEncoding(U8 imageType) 
{
	U8 encoding = 0;

	switch (imageType) 
	{
	case IFXIMAGE_TYPE_LUMINANCE: 
		encoding = IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE;
		break;
	case IFXIMAGE_TYPE_RGB:
		encoding = IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24;
		break;
	case IFXIMAGE_TYPE_RGBA:
		encoding = IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32;
		break;
	case IFXIMAGE_TYPE_ALPHA:
		encoding = IFXTextureObject::IFXTEXTUREMAP_FORMAT_ALPHA;
		break;
	case IFXIMAGE_TYPE_LUMINANCE_ALPHA:
		encoding = IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA;
		break;
	default:
		IFXASSERT(0);
		break;
	};

	return encoding;
}

IFXRESULT CIFXImageTools::EvaluateContinuationFormat(STextureSourceInfo* pImageInfo)
{
	IFXRESULT iResult = IFX_OK;
	U32 iContImg;

	IFXASSERT(pImageInfo);

	CleanContinuationFormat();
	
	m_uContinuationImageCount = pImageInfo->m_imageLoadCount;

	U8 imageType = GetImageType(pImageInfo->m_imageType);
	switch (imageType) 
	{
	case IFXIMAGE_TYPE_ALPHA:
		m_uContinuationImageCount = 1;
		m_pContinuationFormats = new IFXContinuationImageFormat[1];
		SetCompressionProperties(pImageInfo);
		m_pContinuationFormats[0].m_bpp = 1;
		// we can force it here because with 1 cont. image no 
		// other channel combinations possible	
		pImageInfo->m_blockChannels[0] = (U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
		m_pContinuationFormats[0].m_uImageChannels = pImageInfo->m_blockChannels[0];
		break;

	case IFXIMAGE_TYPE_RGB:
		m_uContinuationImageCount = pImageInfo->m_imageLoadCount;
		IFXASSERT(m_uContinuationImageCount<=3);
		m_pContinuationFormats = 
			new IFXContinuationImageFormat[m_uContinuationImageCount];

		SetCompressionProperties(pImageInfo);		

		switch(m_uContinuationImageCount) 
		{
			case 1:
				IFXASSERT(
					m_pContinuationFormats[0].m_uCompressionType == 
						IFXTextureObject::TextureType_Png ||
					m_pContinuationFormats[0].m_uCompressionType == 
						IFXTextureObject::TextureType_Jpeg24);
				// we can force it here because with 1 cont. image no other 
				// channel combinations possible
				pImageInfo->m_blockChannels[0] = 
					(U8)IFXTextureObject::IFXIMAGECHANNEL_RED | 
					(U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN |
					(U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE;

				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];

				m_pContinuationFormats[0].m_bpp = 3;
				break;

			case 2:
				IFXASSERT( 
					(m_pContinuationFormats[0].m_uCompressionType == 
									IFXTextureObject::TextureType_Jpeg8 &&
					m_pContinuationFormats[1].m_uCompressionType == 
									IFXTextureObject::TextureType_Png) ||
					(m_pContinuationFormats[0].m_uCompressionType == 
									IFXTextureObject::TextureType_Png &&
					m_pContinuationFormats[1].m_uCompressionType == 
									IFXTextureObject::TextureType_Jpeg8) ||
					(m_pContinuationFormats[0].m_uCompressionType == 
									IFXTextureObject::TextureType_Png &&
					m_pContinuationFormats[1].m_uCompressionType == 
									IFXTextureObject::TextureType_Png) );

				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];
				m_pContinuationFormats[1].m_uImageChannels = 
					pImageInfo->m_blockChannels[1];

				m_pContinuationFormats[0].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[0]);
				m_pContinuationFormats[1].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[1]);
				IFXASSERT(
					m_pContinuationFormats[0].m_bpp <= 2 && 
					m_pContinuationFormats[0].m_bpp > 0);
				IFXASSERT(
					m_pContinuationFormats[1].m_bpp <= 2 && 
					m_pContinuationFormats[1].m_bpp > 0);
				break;

			case 3:
				IFXASSERT(
					m_pContinuationFormats[0].m_uCompressionType != 
									IFXTextureObject::TextureType_Jpeg24 &&
					m_pContinuationFormats[1].m_uCompressionType != 
									IFXTextureObject::TextureType_Jpeg24 && 
					m_pContinuationFormats[2].m_uCompressionType != 
									IFXTextureObject::TextureType_Jpeg24 );

				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];
				m_pContinuationFormats[1].m_uImageChannels = 
					pImageInfo->m_blockChannels[1];
				m_pContinuationFormats[2].m_uImageChannels = 
					pImageInfo->m_blockChannels[2];
				
				m_pContinuationFormats[0].m_bpp = 1;
				m_pContinuationFormats[1].m_bpp = 1;
				m_pContinuationFormats[2].m_bpp = 1;
				break;

			default:
				IFXASSERT(0);
				break;
		}
		break;

	case IFXIMAGE_TYPE_RGBA:
		m_uContinuationImageCount = pImageInfo->m_imageLoadCount;
		IFXASSERT(m_uContinuationImageCount<=4);

		m_pContinuationFormats = 
			new IFXContinuationImageFormat[m_uContinuationImageCount];

		SetCompressionProperties(pImageInfo);		
		switch(m_uContinuationImageCount) 
		{
			case 1:
				IFXASSERT(
					m_pContinuationFormats[0].m_uCompressionType == 
						IFXTextureObject::TextureType_Png );
				// we can force it here because with 1 cont. image no 
				// other channel combinations possible
				pImageInfo->m_blockChannels[0] = 
					(U8)IFXTextureObject::IFXIMAGECHANNEL_RED | 
					(U8)IFXTextureObject::IFXIMAGECHANNEL_GREEN |
					(U8)IFXTextureObject::IFXIMAGECHANNEL_BLUE | 
					(U8)IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];
				
				m_pContinuationFormats[0].m_bpp = 4;
				break;
			case 2:

				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];
				m_pContinuationFormats[1].m_uImageChannels = 
					pImageInfo->m_blockChannels[1];
				m_pContinuationFormats[0].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[0]);
				m_pContinuationFormats[1].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[1]);
				IFXASSERT(
					m_pContinuationFormats[0].m_bpp<=3 && 
					m_pContinuationFormats[0].m_bpp>0);
				IFXASSERT(
					m_pContinuationFormats[1].m_bpp<=3 && 
					m_pContinuationFormats[1].m_bpp>0);	
				break;

			case 3:

				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];
				m_pContinuationFormats[1].m_uImageChannels = 
					pImageInfo->m_blockChannels[1];
				m_pContinuationFormats[2].m_uImageChannels = 
					pImageInfo->m_blockChannels[2];
				m_pContinuationFormats[0].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[0]);
				m_pContinuationFormats[1].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[1]);
				m_pContinuationFormats[2].m_bpp = 
					CalculateBitsPerPixel(pImageInfo->m_blockChannels[2]);
				IFXASSERT(
					m_pContinuationFormats[0].m_bpp<=2 && 
					m_pContinuationFormats[0].m_bpp>0);
				IFXASSERT(
					m_pContinuationFormats[1].m_bpp<=2 && 
					m_pContinuationFormats[1].m_bpp>0);
				IFXASSERT(
					m_pContinuationFormats[2].m_bpp<=2 && 
					m_pContinuationFormats[2].m_bpp>0);
				break;

			case 4:
				IFXASSERT(
					m_pContinuationFormats[0].m_uCompressionType!=
								IFXTextureObject::TextureType_Jpeg24 &&
					m_pContinuationFormats[1].m_uCompressionType!=
								IFXTextureObject::TextureType_Jpeg24 && 
					m_pContinuationFormats[2].m_uCompressionType!=
								IFXTextureObject::TextureType_Jpeg24 &&
					m_pContinuationFormats[3].m_uCompressionType!=
								IFXTextureObject::TextureType_Jpeg24 );

				m_pContinuationFormats[0].m_uImageChannels = 
					pImageInfo->m_blockChannels[0];
				m_pContinuationFormats[1].m_uImageChannels = 
					pImageInfo->m_blockChannels[1];
				m_pContinuationFormats[2].m_uImageChannels = 
					pImageInfo->m_blockChannels[2];
				m_pContinuationFormats[3].m_uImageChannels = 
					pImageInfo->m_blockChannels[3];

				m_pContinuationFormats[0].m_bpp = 1;
				m_pContinuationFormats[1].m_bpp = 1;
				m_pContinuationFormats[2].m_bpp = 1;
				m_pContinuationFormats[3].m_bpp = 1;
				break;
			default:
				IFXASSERT(0);
				break;
		}
		break;

		break;
	case IFXIMAGE_TYPE_LUMINANCE:
		m_uContinuationImageCount = 1;
		m_pContinuationFormats = new IFXContinuationImageFormat[1];
		SetCompressionProperties(pImageInfo);
		// we can force it here because with 1 cont. image no other 
		// channel combinations possible
		pImageInfo->m_blockChannels[0] = (U8)IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE;
		m_pContinuationFormats[0].m_uImageChannels	= pImageInfo->m_blockChannels[0];

		m_pContinuationFormats[0].m_bpp = 1;
		break;

	case IFXIMAGE_TYPE_LUMINANCE_ALPHA:
		m_uContinuationImageCount = pImageInfo->m_imageLoadCount;
		IFXASSERT(m_uContinuationImageCount<=2);
		m_pContinuationFormats = 
			new IFXContinuationImageFormat[m_uContinuationImageCount];

		SetCompressionProperties(pImageInfo);

		m_pContinuationFormats[0].m_uImageChannels = pImageInfo->m_blockChannels[0];			
		m_pContinuationFormats[0].m_bpp = 1;
		if (m_uContinuationImageCount==1) 
		{
			m_pContinuationFormats[0].m_uImageChannels = pImageInfo->m_blockChannels[0];
			m_pContinuationFormats[0].m_bpp = 2;
		} 
		else 
		{
			m_pContinuationFormats[1].m_uImageChannels = pImageInfo->m_blockChannels[1];
			m_pContinuationFormats[1].m_bpp = 1;
		}
		break;
	default:
		IFXASSERT(0);
		iResult = IFX_E_UNSUPPORTED;
		break;
	};

	if (IFXSUCCESS(iResult)) 
	{
		for (iContImg = 0; iContImg < m_uContinuationImageCount; iContImg++) 
		{
			U32 uFileNum = 
				(pImageInfo->m_pURLFileNameList[iContImg]).GetNumberElements();
			m_pContinuationFormats[iContImg].m_uExtImageURLCount = uFileNum;
			if (uFileNum>0) 
			{
				m_pContinuationFormats[iContImg].m_uAttributes = 1; 

				IFXDELETE_ARRAY( m_pContinuationFormats[iContImg].m_pImageURLNames );
				m_pContinuationFormats[iContImg].m_pImageURLNames = 
					new IFXString* [uFileNum];

				if( NULL != m_pContinuationFormats[iContImg].m_pImageURLNames )
				{
					U32 iFile;
					for ( iFile = 0; iFile < uFileNum; iFile++ ) 
					{
						IFXString*& pStr = 
							(pImageInfo->m_pURLFileNameList[iContImg]).GetElement(iFile);
						(m_pContinuationFormats[iContImg].m_pImageURLNames)[iFile] = 
							new IFXString(pStr);
					}
				}
				else
				{
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXASSERT(m_pContinuationFormats[iContImg].m_pImageURLNames != NULL);
				}
			} 
			else 
			{
				m_pContinuationFormats[iContImg].m_uAttributes = 0; 
			}
		}
	}

	return iResult;
}

void CIFXImageTools::SetCompressionProperties(STextureSourceInfo* pImageInfo)
{
	U32 iCount, uFileCount;

	if( pImageInfo )
	{
		for (iCount = 0; iCount <m_uContinuationImageCount; iCount++) 
		{
			m_pContinuationFormats[iCount].m_uCompressionType = 
				pImageInfo->m_blockCompressionType[iCount];	
			uFileCount = pImageInfo->m_URLFileRef[iCount];
			if (uFileCount && pImageInfo->m_keepExternalImagesAtEncoding) 
			{
				m_pContinuationFormats[iCount].m_uAttributes		= 1;
				m_pContinuationFormats[iCount].m_uExtImageURLCount	= 1;
			} 
			else 
			{
				m_pContinuationFormats[iCount].m_uAttributes		= 0;
				m_pContinuationFormats[iCount].m_uExtImageURLCount	= 0;
				m_pContinuationFormats[iCount].m_pImageURLNames		= NULL;
			}
		}
	}
}


void CIFXImageTools::CleanContinuationFormat()
{
	IFXDELETE_ARRAY( m_pContinuationFormats );
	m_uContinuationImageCount = 0;
}

CIFXImageTools::IFXContinuationImageFormat::IFXContinuationImageFormat()
{
	m_uCompressionType  = IFXTextureObject::TextureType_Jpeg24;
	m_bpp				= 0;
	m_uImageChannels	= (U8)(IFXTextureObject::IFXIMAGECHANNEL_RED | 
						  IFXTextureObject::IFXIMAGECHANNEL_BLUE | 
						  IFXTextureObject::IFXIMAGECHANNEL_GREEN);
	m_uImageDataByteCount = 0;
	m_uAttributes		= 0;
	m_uExtImageURLCount = 0;
	m_pImageURLNames	= NULL;
}

CIFXImageTools::IFXContinuationImageFormat::~IFXContinuationImageFormat()
{
	if (NULL != m_pImageURLNames)
	{
		U32 i;

		for( i = 0; i < m_uExtImageURLCount; ++i )
			IFXDELETE( m_pImageURLNames[i] );

		IFXDELETE_ARRAY( m_pImageURLNames );
		m_uExtImageURLCount = 0;
	}
}

U32 CIFXImageTools::CalculateBitsPerPixel(U8 channel)
{
	U8 val = channel;
	U32 bpp = 0;
	while (val>0) 
	{
		if ( 0 != (val&1) ) 
		{
			(bpp)++;
		}
		val = val>>1;
	}

	return bpp;
}

static void png_write_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	IFXASSERT(length>0);
	IFXASSERT(png_ptr!=0&&data!=NULL);
	IFXASSERT(png_get_io_ptr(png_ptr)!=NULL);
	png_bytepp pio_ptr = (png_bytepp)png_get_io_ptr(png_ptr);
	IFXASSERT(*pio_ptr!=NULL);

	//copy from data to *(png_ptr->io_ptr)
	memcpy(*pio_ptr, data, length );
	*pio_ptr = *pio_ptr + length;
}

static void png_flush(png_structp png_ptr)
{
}

static void png_read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	IFXASSERT(length>0);
	IFXASSERT(png_ptr!=0&&data!=NULL);
	IFXASSERT(png_get_io_ptr(png_ptr)!=NULL);
	png_bytepp pio_ptr = (png_bytepp)png_get_io_ptr(png_ptr);
	IFXASSERT(*pio_ptr!=NULL);

	//copy from *(png_ptr->io_ptr) (compressed PNG image) to data
	memcpy( data, *pio_ptr,  length );
	*pio_ptr = *pio_ptr + length;
}

void* CIFXImageTools::SwapImageToRGB(STextureSourceInfo* pImageInfo, void* pSrcImage) 
{
	void* pDstImage=NULL;
	U32 b_pp = 3;
	U32  uIndex;
	IFXRESULT iResult = IFX_OK;

	if ( pImageInfo && pSrcImage &&
		( pImageInfo->m_imageType==IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24 ||
		pImageInfo->m_imageType==IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32 ) )
	{
		pDstImage = (U8*) new U8 [pImageInfo->m_size];
		if(NULL == pDstImage) 
		{ 
			iResult = IFX_E_OUT_OF_MEMORY;
		}

		if(IFXSUCCESS(iResult)) 
		{

			if (pImageInfo->m_imageType==IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32)
					b_pp = 4;

			/// @todo use pointer arithmetic to improve performance
			for( uIndex=0; uIndex < (pImageInfo->m_size); uIndex = uIndex + b_pp) 
			{
				((U8*)pDstImage)[uIndex]   = ((U8*)pSrcImage)[uIndex+2];
				((U8*)pDstImage)[uIndex+2] = ((U8*)pSrcImage)[uIndex];
				((U8*)pDstImage)[uIndex+1] = ((U8*)pSrcImage)[uIndex+1];
				if (b_pp==4) 
				{
					((U8*)pDstImage)[uIndex+3] = ((U8*)pSrcImage)[uIndex+3];
				}
			}
		}
	}

	return pDstImage;
}

BOOL CIFXImageTools::IsWholeImageFromExternalFile(const STextureSourceInfo* pImageInfo) 
{
	BOOL res = FALSE; 
	if ( pImageInfo && pImageInfo->m_imageURLCount != 0) 
	{
		res = TRUE;
		U32 i;
		for ( i=0; i<pImageInfo->m_imageURLCount;i++) 
		{
			if ( !(pImageInfo->m_URLFileRef[i]) ) 
			{
				res = FALSE;
			}
		}
	} 
	return res;
}
