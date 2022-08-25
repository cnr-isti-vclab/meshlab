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
  @file TextureConverter.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include <wchar.h>
#include "TextureConverter.h"
#include "SceneUtilities.h"
#include "TextureResource.h"
#include "MetaDataConverter.h"
#include "TGAImage.h"
#include "Tokens.h"

#include "IFXTextureObject.h"
#include "IFXCheckX.h"

#ifndef LIBIDTF
extern FILE *stdmsg;
#endif

using namespace U3D_IDTF;

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


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

TextureConverter::TextureConverter( 
								   const Texture* pTexture, 
								   SceneUtilities* pSceneUtils )
: m_pTexture( pTexture ), m_pSceneUtils( pSceneUtils )
{
	IFXCHECKX_RESULT( NULL != pTexture, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pSceneUtils, IFX_E_INVALID_POINTER );

	m_quality = 100;
	m_limit = 0;
}

TextureConverter::~TextureConverter()
{
	m_pTexture = NULL;
	m_pSceneUtils = NULL;
}


void TextureConverter::SetQuality( U32 quality, U32 limit )
{
	m_quality = quality;
	m_limit = limit;
}


IFXRESULT TextureConverter::Convert()
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXTextureObject, pTextureObject );
	STextureSourceInfo sourceImageInfo;
	const IFXString& rImageType = m_pTexture->GetImageType();

	// Create an IFX Texture from the data we loaded into memory
	result = m_pSceneUtils->CreateTexture(
								m_pTexture->GetName(),
								1024,
								&pTextureObject );

	if( IFXSUCCESS( result ) )
		sourceImageInfo.m_name = m_pTexture->GetName();

	// define image type
	if( IFXSUCCESS( result ) )
	{
		/// @todo add support for 1 and 2 channel textures
		if( rImageType == IDTF_IMAGE_TYPE_RGB )
			sourceImageInfo.m_imageType = IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24;
		else if( rImageType == IDTF_IMAGE_TYPE_RGBA )
			sourceImageInfo.m_imageType = IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32;
		/*else if( rImageType == IDTF_IMAGE_TYPE_ALPHA )
			sourceImageInfo.m_imageType = IFXTextureObject::IFXTEXTUREMAP_FORMAT_ALPHA;
		else if( rImageType == IDTF_IMAGE_TYPE_LUMINANCE )
			sourceImageInfo.m_imageType = IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE;
		else if( rImageType == IDTF_IMAGE_TYPE_LUMINANCE_AND_ALPHA )
			sourceImageInfo.m_imageType = IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA;*/
		else
		{
			result = IFX_E_UNSUPPORTED;
			IFXASSERT( IFXSUCCESS( result ) );
		}
	}

	if( IFXSUCCESS( result ) )
	{
		// define URL params if image has any external data
		const U32 imageCount = m_pTexture->GetImageFormatCount();

		U32 i;
		for( i = 0; i < imageCount; ++i )
		{
			const ImageFormat& rImageFormat = m_pTexture->GetImageFormat(i);

			if( rImageFormat.m_urlList.GetUrlCount() > 0 )
			{
				sourceImageInfo.m_URLFileRef[i] = TRUE;
				sourceImageInfo.m_imageURLCount++;
			}
		}

		if( m_pTexture->IsExternal() == FALSE )
		{
			const IFXString& rPath = m_pTexture->GetPath();
			IFXCHAR* file = (IFXCHAR*)rPath.Raw();
			TGAImage textureImage;

#ifdef LIBIDTF
			if ( m_pTexture->m_textureImage.IsSet() )
				textureImage = m_pTexture->m_textureImage;
			// Load the TGA Texture file
			else if( IFXFAILURE( textureImage.Read( file ) ) )
			{
				IFXString dbgMessage = L"[Converter] -- Error - could not load ";
				dbgMessage+= rPath;
				dbgMessage+= L"\n";
				IFXTRACE_GENERIC( dbgMessage.Raw() );
				result = IFX_E_INVALID_FILE;
			}
#else
			// Load the TGA Texture file
			if( IFXFAILURE( textureImage.Read( file ) ) )
			{
				fwprintf(stdmsg, L" -- Error - could not load %ls\n", file );
				result = IFX_E_INVALID_FILE;
			}
#endif

			#ifdef DEBUG_TGA_WRITE
			if( IFXSUCCESS( result ) )
			{
				IFXString outTGAFile( rPath );
				outTGAFile.Concatenate( L"_out.tga" );
				textureImage.Write( outTGAFile.Raw() );
			}
			#endif

			if( IFXSUCCESS( result ) )
			{
				const U32 height = textureImage.GetHeight();
				const U32 width = textureImage.GetWidth();
				const U32 channels = textureImage.GetChannels();

				// Fill texture info structure
				sourceImageInfo.m_height = height;
				sourceImageInfo.m_width	 = width;
				sourceImageInfo.m_size	 = height * width * channels;
				sourceImageInfo.m_compressionQuality = (U8)m_quality;
				sourceImageInfo.m_pCodecCID = NULL;

				if( ( rImageType == IDTF_IMAGE_TYPE_RGB && channels != 3 ) || 
					( rImageType == IDTF_IMAGE_TYPE_RGBA && channels != 4 ) /*||
					rImageType == IDTF_IMAGE_TYPE_ALPHA && channels != 1 ||
					rImageType == IDTF_IMAGE_TYPE_LUMINANCE && channels != 1 ||
					rImageType == IDTF_IMAGE_TYPE_LUMINANCE_AND_ALPHA && channels != 2*/ )
				{
#ifndef LIBIDTF
					fprintf(stdmsg, "Error: Format defined do not match actual format of TGA\n");
#else
					IFXTRACE_GENERIC( L"Error: Format defined do not match actual format of TGA\n" );
#endif
					result = IFX_E_UNDEFINED;
					IFXASSERT( IFXSUCCESS( result ) );
				}

				if( IFXSUCCESS( result ) )
				{
					pTextureObject->SetQualityFactorX( sourceImageInfo.m_compressionQuality );

					result = pTextureObject->SetRawImage( &sourceImageInfo, textureImage.GetData() );
				}

				// If the user placed an upper limit on texture dimensions, enforce them
				if( IFXSUCCESS( result ) && 0 != m_limit )
				{
					// If the image is wider than the size limit, force it to shrink to fit.
					if( sourceImageInfo.m_width > m_limit )
					{
						result = pTextureObject->SetWidth( m_limit );
					}

					// If the image is taller than the size limit, force it to shrink to fit.
					if( IFXSUCCESS( result ) && sourceImageInfo.m_height > m_limit )
					{
						result = pTextureObject->SetHeight( m_limit );
					}

					if( ( sourceImageInfo.m_width > m_limit ) || 
						( sourceImageInfo.m_height > m_limit ) )
#ifndef LIBIDTF
						fprintf(stdmsg, "-- Warning: texture size was changed because it exceeds"
						"upper limit on texture dimensions\n" );
#else
						IFXTRACE_GENERIC( L"-- Warning: texture size was changed because it exceeds"
						L"upper limit on texture dimensions\n" );
#endif
					// Note that there is no visual indication that this happened other than a decrease
					// in image quality along the re-scaled vertical or horizontal axis.    If you don't
					// let you users know that this recalling happened, odds are they won't even notice
					// 99% of the time.    It does let you reduce the size of the U3D file, however,
					// which means your scene will take less time to load over the Internet.
				}
			}
		}
		else
		{
			const U32 height = m_pTexture->GetHeight();
			const U32 width  = m_pTexture->GetWidth();  
			U32 channels = 0;

			/// @todo add support for 1 and 2 channel textures
			if( rImageType == IDTF_IMAGE_TYPE_RGB )
				channels = 3;
			else if( rImageType == IDTF_IMAGE_TYPE_RGBA )
				channels = 4;
			/*
			else if( rImageType == IDTF_IMAGE_TYPE_ALPHA )
				channels = 1;
			else if( rImageType == IDTF_IMAGE_TYPE_LUMINANCE )
				channels = 1;
			else if( rImageType == IDTF_IMAGE_TYPE_LUMINANCE_AND_ALPHA )
				channels = 2;
			*/

			sourceImageInfo.m_height		= height;
			sourceImageInfo.m_width			= width;
			sourceImageInfo.m_size			= height*width*channels;

#ifndef USE_EMPTY_BUFFER
			result = pTextureObject->SetRawImage( &sourceImageInfo, NULL );
#else
			sourceImageInfo.m_imageURLCount = 1;
			sourceImageInfo.m_URLFileRef[0] = 1;

			U8* pImage = new U8[sourceImageInfo.m_size];

			if( NULL != pImage )
  			{
  				result = pTextureObject->SetRawImage( &sourceImageInfo, pImage );
  				delete [] pImage;
  			}
  			else
  				result = IFX_E_OUT_OF_MEMORY;
#endif
		}
	}

	if( IFXSUCCESS( result ) )
	{
		// set output image properties
		result = SetImageProperties( pTextureObject );
	}

	if( IFXSUCCESS( result ) )
	{
		// Cause the pTextureObject to compress the raw texture data according
		// to the settings in the STextureSourceInfo structure we used above.
		result = pTextureObject->ForceCompressionState( IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED );
	}

	// convert texture meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		result = pTextureObject->QueryInterface( 
									IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( m_pTexture, pTextureObject );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************

IFXRESULT TextureConverter::SetImageProperties( IFXTextureObject* pTextureObject )
{
	IFXRESULT result = IFX_OK;
	const U32 imageCount = m_pTexture->GetImageFormatCount();

	if( IFX_MAX_CONTINUATIONIMAGE_COUNT >= imageCount )
	{
		U8 BlockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT]; 
		IFXTextureObject::ChannelType 
			ImageChannelFlagsList[IFX_MAX_CONTINUATIONIMAGE_COUNT];
		IFXArray<IFXString*>* pFileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT] = { NULL };
		BOOL isFileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT] = { FALSE };

		U32 i, j;

		for( i = 0; i < imageCount && IFXSUCCESS( result ); ++i )
		{
			const ImageFormat& rImageFormat = m_pTexture->GetImageFormat(i);

			if( rImageFormat.m_compressionType == IDTF_IMAGE_COMPRESSION_TYPE_JPEG24 )
				BlockCompressionType[i] = IFXTextureObject::TextureType_Jpeg24;
			else if( rImageFormat.m_compressionType == IDTF_IMAGE_COMPRESSION_TYPE_JPEG8 )
				BlockCompressionType[i] = IFXTextureObject::TextureType_Jpeg8;
			else if( rImageFormat.m_compressionType == IDTF_IMAGE_COMPRESSION_TYPE_PNG )
				BlockCompressionType[i] = IFXTextureObject::TextureType_Png;
			else
				result = IFX_E_UNSUPPORTED;

			if( IFXSUCCESS( result ) )
			{
				U32 channelFlags = 0;
				if( rImageFormat.m_alpha == IDTF_TRUE )
					channelFlags |= IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
				
				if( rImageFormat.m_blue == IDTF_TRUE )
					channelFlags |= IFXTextureObject::IFXIMAGECHANNEL_BLUE;

				if( rImageFormat.m_red == IDTF_TRUE )
					channelFlags |= IFXTextureObject::IFXIMAGECHANNEL_RED;

				if( rImageFormat.m_green == IDTF_TRUE )
					channelFlags |= IFXTextureObject::IFXIMAGECHANNEL_GREEN;

				if( rImageFormat.m_luminance == IDTF_TRUE )
					channelFlags |= IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE;

				ImageChannelFlagsList[i] = (IFXTextureObject::ChannelType)channelFlags;

				const U32 urlCount = rImageFormat.m_urlList.GetUrlCount();

				if( urlCount > 0 )
				{
					pFileRef[i] = new IFXArray<IFXString*>;

					if( NULL != pFileRef[i] )
					{
						isFileRef[i] = TRUE;
						const IFXArray<IFXString>& rUrlList = rImageFormat.GetUrlList();

						for( j = 0; j < urlCount && IFXSUCCESS( result ); ++j )
						{
							const IFXString& rFileReference = rUrlList.GetElementConst( j );
							IFXString*& rpFileRef = pFileRef[i]->CreateNewElement();

							rpFileRef = new IFXString (rFileReference);

							if( NULL == rpFileRef )
								result = IFX_E_OUT_OF_MEMORY;
						}
					}
					else
						result = IFX_E_OUT_OF_MEMORY;
				}
			}
		}

		if( IFXSUCCESS( result ) )
			result = pTextureObject->SetImageCompressionProperties( 
										imageCount, 
										BlockCompressionType, 
										ImageChannelFlagsList, 
										isFileRef, 
										pFileRef );

		for( i = 0; i < IFX_MAX_CONTINUATIONIMAGE_COUNT; ++i )
		{
			if( NULL != pFileRef[i] )
			{
				const U32 urlCount = pFileRef[i]->GetNumberElements();

				for( j = 0; j < urlCount; ++j )
				{
					IFXString*& rpString = pFileRef[i]->GetElement( j );
					delete rpString;
					rpString = NULL;
				}

				delete pFileRef[i];
				pFileRef[i] = NULL;
			}
		}
	}
	else
		result = IFX_E_UNSUPPORTED;
	
	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
