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
  @file TextureParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "TextureParser.h"
#include "TextureResource.h"
#include "FileScanner.h"
#include "UrlListParser.h"
#include "Tokens.h"

#include "IFXCheckX.h"

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

TextureParser::TextureParser( 
							FileScanner* pScanner, 
							Texture* pTexture )
: BlockParser( pScanner ), m_pScanner( pScanner ), m_pTexture( pTexture )
{
	IFXCHECKX_RESULT( NULL != pScanner, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pTexture, IFX_E_INVALID_POINTER );
}

TextureParser::~TextureParser()
{
	m_pTexture = NULL;
	m_pScanner = NULL;
}

IFXRESULT TextureParser::Parse()
{
	IFXRESULT result = IFX_OK;
	IFXString path;
	I32 height = 0, width = 0;
	IFXString imageType;
	I32 imageCount = 0;

	result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_HEIGHT, &height );

	if( IFXSUCCESS( result ) )
		m_pTexture->SetHeight( height );
	else if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // set from tga file

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_TEXTURE_WIDTH, &width );

	if( IFXSUCCESS( result ) )
		m_pTexture->SetWidth( width );
  	else if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // set from tga file

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_TEXTURE_IMAGE_TYPE, &imageType );

	if( IFXSUCCESS( result ) )
		m_pTexture->SetImageType( imageType );
	else if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( 
								IDTF_TEXTURE_IMAGE_COUNT, &imageCount );

	if( IFXSUCCESS( result ) )
	{
		result = BlockBegin( IDTF_IMAGE_FORMAT_LIST );

		I32 i;
		for( i = 0; i < imageCount && IFXSUCCESS( result ); ++i )
		{
			I32 imageFormatNumber;

			result = BlockBegin( IDTF_IMAGE_FORMAT, &imageFormatNumber );

			if( IFXSUCCESS( result ) && i == imageFormatNumber )
				result = ParseImageFormat();

			if( IFXSUCCESS( result ) )
				result = BlockEnd();
		}

		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}
	else if( IFX_E_TOKEN_NOT_FOUND == result )
	{
		// there is no defined image format in idtf, so we have to create
		// one with default settings
		ImageFormat imageFormat;

		imageFormat.m_blue = "TRUE";
		imageFormat.m_green = "TRUE";
		imageFormat.m_red = "TRUE";

		m_pTexture->AddImageFormat( imageFormat );
		m_pTexture->SetExternal( FALSE );

		result = IFX_OK;
	}

	if( IFXSUCCESS( result ) )
	{
		result = m_pScanner->ScanStringToken( 
								IDTF_TEXTURE_PATH, &path );

		// if the texture path is defined but texture is fully external
		// we just skip the texture path
		if( IFXSUCCESS( result ) && m_pTexture->IsExternal() == FALSE )
			m_pTexture->SetPath( path );
		else if( IFX_E_TOKEN_NOT_FOUND == result )
			result = IFX_OK;
	}

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( m_pTexture );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************

IFXRESULT TextureParser::ParseImageFormat()
{
	IFXRESULT result = IFX_OK;

	ImageFormat imageFormat;

	result = m_pScanner->ScanStringToken( 
								IDTF_COMPRESSION_TYPE, 
								&imageFormat.m_compressionType );
	
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ALPHA_CHANNEL, &imageFormat.m_alpha );
		
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_BLUE_CHANNEL, &imageFormat.m_blue );
		
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_GREEN_CHANNEL, &imageFormat.m_green );
		
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_RED_CHANNEL, &imageFormat.m_red );
		
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_LUMINANCE, &imageFormat.m_luminance );
		
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // use default

	if( IFXSUCCESS( result ) )
	{
		UrlList urlList;

		UrlListParser urlListParser( m_pScanner, &urlList );

		result = urlListParser.Parse();

		if( IFXSUCCESS( result ) )
			imageFormat.AddUrlList( urlList );

		// if texture has any internal images then 
		// texture is considered as has internal data
		if( urlList.GetUrlCount() == 0 )
			m_pTexture->SetExternal( FALSE );
	}

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK; // no external references

	if( IFXSUCCESS( result ) )
		m_pTexture->AddImageFormat( imageFormat );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT TextureParser::ParseUrlList()
{
	IFXRESULT result = IFX_OK;

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
