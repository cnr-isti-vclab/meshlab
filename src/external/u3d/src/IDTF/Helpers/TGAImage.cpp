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
@file TGAImage.cpp

This file defines the implementation of the TGAImage class.

@note 
Here is the description of supported TGA format

DATA TYPE 2: Unmapped RGB

| Offset | Length |                     Description                            |
|--------|--------|------------------------------------------------------------|
|    0   |     1  |  Number of Characters in Identification Field.             |
|        |        |                                                            |
|        |        |  This field is a one-byte unsigned integer, specifying     |
|        |        |  the length of the Image Identification Field.  Its value  |
|        |        |  is 0 to 255.  A value of 0 means that no Image            |
|        |        |  Identification Field is included.                         |
|--------|--------|------------------------------------------------------------|
|    1   |     1  |  Color Map Type.                                           |
|        |        |                                                            |
|        |        |  This field contains 0.                                    |
|--------|--------|------------------------------------------------------------|
|    2   |     1  |  Image Type Code.                                          |
|        |        |                                                            |
|        |        |  This field will always contain a binary 2.                |
|        |        |  ( That's what makes it Data Type 2 ).                     |
|--------|--------|------------------------------------------------------------|
|    3   |     5  |  Color Map Specification.                                  |
|        |        |                                                            |
|        |        |  Ignored	                                               |
|--------|--------|------------------------------------------------------------|
|    8   |    10  |  Image Specification.                                      |
|        |        |                                                            |
|    8   |     2  |  X Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) X coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   10   |     2  |  Y Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) Y coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   12   |     2  |  Width of Image.                                           |
|        |        |  Integer ( lo-hi ) width of the image in pixels.           |
|   14   |     2  |  Height of Image.                                          |
|        |        |  Integer ( lo-hi ) height of the image in pixels.          |
|   16   |     1  |  Image Pixel Size.                                         |
|        |        |  Number of bits in a pixel.  This is 24 for Targa 24,      |
|        |        |  32 for Targa 32                                           |
|   17   |     1  |  Image Descriptor Byte.                                    |
|        |        |  Not used                                                  |
|--------|--------|------------------------------------------------------------|
|   18   | varies |  Image Identification Field.                               |
|        |        |                                                            |
|        |        |  Contains a free-form identification field of the length   |
|        |        |  specified in byte 1 of the image record.  It's usually    |
|        |        |  omitted ( length in byte 1 = 0 ), but can be up to 255    |
|        |        |  characters.  If more identification information is        |
|        |        |  required, it can be stored after the image data.          |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Image Data Field.                                         |
|        |        |                                                            |
|        |        |  This field specifies (width) x (height) pixels.  Each     |
|        |        |  pixel specifies an RGB color value, which is stored as    |
|        |        |  an integral number of bytes.                              |
|        |        |  The 3 byte entry contains 1 byte each of blue, green,     |
|        |        |  and red.                                                  |
|        |        |  The 4 byte entry contains 1 byte each of blue, green,     |
|        |        |  red, and attribute.                                       |
--------------------------------------------------------------------------------

*/

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "TGAImage.h"
#include "IFXOSLoader.h"

using namespace U3D_IDTF;

namespace U3D_IDTF
{
typedef struct _TgaHeader
{
    U8 numIden;
    U8 colorMapType;
    U8 imageType;
    U8 colorMapSpec[5]; // not used, just here to take up space
    U8 origX[2];
    U8 origY[2];
    U8 width[2];
    U8 height[2];
    U8 bpp;
    U8 imageDes; // don't use, space eater
} TgaHeader;

}

TGAImage::TGAImage()
{
	m_Width = 0;
	m_Height = 0;
	m_Channels = 0;
	m_RGBPixels = NULL;
}

TGAImage::~TGAImage()
{
	Deallocate();
}

TGAImage::TGAImage(const TGAImage& other )
{
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	m_Channels = other.m_Channels;
	if ( other.m_RGBPixels != NULL )
	{
		m_RGBPixels =  new U8[m_Width * m_Height * m_Channels];
		memcpy( m_RGBPixels, other.m_RGBPixels, m_Width * m_Height * m_Channels );
	}
	else
		m_RGBPixels =  NULL;
}

TGAImage& TGAImage::operator = (const TGAImage& other)
{
	if (this != &other) // protect against invalid self-assignment
	{
		Deallocate();
		m_Width = other.m_Width;
		m_Height = other.m_Height;
		m_Channels = other.m_Channels;
		if ( other.m_RGBPixels != NULL )
		{
			m_RGBPixels =  new U8[m_Width * m_Height * m_Channels];
			memcpy( m_RGBPixels, other.m_RGBPixels, m_Width * m_Height * m_Channels );
		}
	}
	// by convention, always return *this
	return *this;
}

IFXRESULT TGAImage::Deallocate()
{
	if(m_RGBPixels)
	{
		delete[] m_RGBPixels;
	}

	m_RGBPixels = NULL;

	m_Width = 0;
	m_Height = 0;
	m_Channels = 0;

	return IFX_OK;
}

IFXRESULT TGAImage::Read( const IFXCHAR* pFileName )
{
    FILE* inFile = 0;
    TgaHeader header;
	IFXRESULT ret = IFX_OK;

    Deallocate();

    if( !pFileName )
    {
        ret = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( ret ) )
    {
		inFile = IFXOSFileOpen( pFileName, L"rb" );

		if( !inFile )
		{
			ret = IFX_E_INVALID_FILE;
		}
    }

    if( IFXSUCCESS( ret ) )
    {
		// read the file header
		size_t count = 
			fread( &header, sizeof(TgaHeader), 1, inFile );

		// if file header was not successfully read
		if( 1 != count )
		{
			ret = IFX_E_READ_FAILED;
		}
    }

    if( IFXSUCCESS( ret ) )
    {
		if( header.colorMapType != 0 )
		  ret = IFX_E_UNDEFINED;

		if( header.imageType != 2 )
		  ret = IFX_E_UNDEFINED;

		// make sure we are loading a supported type
		if( header.bpp!= 32 && header.bpp!= 24 )
		  ret = IFX_E_UNDEFINED;
    }

    if( IFXSUCCESS( ret ) )
    {
		m_Width = header.width[0] + header.width[1] * 256;
		m_Height = header.height[0] + header.height[1] * 256;
		m_Channels = header.bpp/8;
		m_RGBPixels = new U8[m_Width * m_Height * m_Channels];
		if( !m_RGBPixels )
			ret = IFX_E_OUT_OF_MEMORY;
    }

	if( IFXSUCCESS( ret ) && 0 != header.numIden )
	{
		// seek file pointer after image identification field
		fseek( inFile, header.numIden, SEEK_CUR ); 
	}

	if( IFXSUCCESS( ret ) )
	{
		// read the pixel data in BGR format
		size_t count = 
			fread( m_RGBPixels, m_Width * m_Height * m_Channels, 1, inFile );
		
		// if file was successfully read
		if( 1 == count )
		{
			// R and B channels reordering
			U32 i;
			for( i = 0; i < m_Width * m_Height * m_Channels; i += m_Channels )
			{    
				U8 tmp = m_RGBPixels[i];
				m_RGBPixels[i] = m_RGBPixels[i+2];
				m_RGBPixels[i+2] = tmp;
			}
		}
		else
		{
			ret = IFX_E_READ_FAILED;
		}
	}

	if( inFile )
	{
		fclose( inFile );
	}

	if( IFXFAILURE( ret ) )
	{
		Deallocate();
	}

    return ret;
}

IFXRESULT TGAImage::Write( const IFXCHAR* pFileName ) const
{
	TgaHeader header;
	U8* BGRPixels = NULL; // BGRA

    IFXRESULT ret = IFX_OK;
	FILE* outFile = 0;

	if( !m_RGBPixels )
	{
		ret = IFX_E_INVALID_POINTER;
	}

	if( IFXSUCCESS( ret ) )
	{
		outFile = IFXOSFileOpen( pFileName, L"wb" );

		if( !outFile )
			ret = IFX_E_INVALID_FILE;
	}

	if( IFXSUCCESS( ret ) )
	{
		// first attemp to write TGA image
		BGRPixels = new U8[ m_Width * m_Height * m_Channels ];
		if( NULL != BGRPixels && NULL != m_RGBPixels )
		{
			// R and B channels reordering
			U32 i;
			for( i = 0; i < m_Width * m_Height * m_Channels; i += m_Channels )
			{    
				BGRPixels[i] = m_RGBPixels[i+2]; // R->B
				BGRPixels[i+1] = m_RGBPixels[i+1]; // G->G
				BGRPixels[i+2] = m_RGBPixels[i]; // B->R
				if( 4 == m_Channels )
					BGRPixels[i+3] = m_RGBPixels[i+3]; // A->A
			}
		}
		else
			ret = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( ret ) )
	{
		memset(&header,0, sizeof(TgaHeader));
		header.imageType = 2;
		header.width[0] = m_Width % 256; header.width[1] = m_Width / 256;
		header.height[0] = m_Height % 256; header.height[1] = m_Height / 256;
		header.bpp = m_Channels*8;

		size_t count = fwrite( &header, sizeof(TgaHeader), 1, outFile );
		// if file header was not successfully written
		if( 1 != count )
		{
			ret = IFX_E_WRITE_FAILED;
		}
	}

	if( IFXSUCCESS( ret ) )
	{
		size_t count = 
			fwrite( BGRPixels, m_Width * m_Height * m_Channels, 1, outFile );
		
		// if file data was not successfully written
		if( 1 != count )
		{
			ret = IFX_E_WRITE_FAILED;
		}
	}

	if( outFile )
		fclose( outFile );

	if( BGRPixels )
	{
		delete[] BGRPixels;
	}
	
	return ret;
}

IFXRESULT TGAImage::Initialize( U32 in_Width, U32 in_Height, U32 in_Channels )
{
	IFXRESULT result = IFX_OK;

	if(in_Width < 1 || in_Height < 1 || (in_Channels != 3 && in_Channels != 4))
	{
		result = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_Height = in_Height;
		m_Width = in_Width;
		m_Channels = in_Channels;

		m_RGBPixels = new U8[ in_Width * in_Height * in_Channels ];
		if( !m_RGBPixels )
			result = IFX_E_OUT_OF_MEMORY;
	}
	
	return result;
}

void TGAImage::SetData(const U8* data )
{
	if ( data != NULL )
		memcpy( m_RGBPixels, data, m_Width * m_Height * m_Channels );
	else
	{
		if(m_RGBPixels)
			delete[] m_RGBPixels;
		m_RGBPixels =  NULL;
	}
}
