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
  @file TextureResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef TextureResource_H
#define TextureResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "IFXString.h"
#include "UrlList.h"
#include "Resource.h"
#include "TGAImage.h"

namespace U3D_IDTF
{
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

/**
	This structure defines Texture Image Format
*/
class ImageFormat
{
public:
	ImageFormat();
	virtual ~ImageFormat() {};

	/**
	*/
	void AddUrlList( const UrlList& rUrlList );

	/**
	*/
	const IFXArray<IFXString>& GetUrlList() const;

	IFXString m_compressionType;
	IFXString m_alpha;
	IFXString m_red;
	IFXString m_green;
	IFXString m_blue;
	IFXString m_luminance;
	UrlList m_urlList;
};

/**
*/

class Texture : public Resource
{
public:
	Texture();
	virtual ~Texture() {};

	void SetHeight( const I32& rHeight );
	const I32& GetHeight() const;

	void SetWidth( const I32& rWidth );
	const I32& GetWidth() const;
	
	/**
	Set image type
	*/
	void SetImageType( const IFXString& rType );

	/**
	Get image type
	*/
	const IFXString& GetImageType() const;

	/**
	*/
	void AddImageFormat( const ImageFormat& rImageFormat );

	/**
	*/
	const ImageFormat& GetImageFormat( U32 index ) const;

	/**
	*/
	const U32& GetImageFormatCount() const;

	/**
	Set image type
	*/
	void SetPath( const IFXString& rPath );

	/**
	Get image type
	*/
	const IFXString& GetPath() const;

	/**
	Is external?
	*/
	BOOL IsExternal() const;
	
	/**
	Set texture as external
	*/
	void SetExternal( BOOL isExternal );

	TGAImage m_textureImage;

private:
	I32 m_height;
	I32 m_width;
	IFXString m_imageType;
	IFXArray< ImageFormat > m_imageFormatList;
	IFXString m_path; // path to texture file
	BOOL m_isExternal;
};


//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE ImageFormat::ImageFormat()
: m_compressionType("JPEG24"),
  m_alpha("FALSE"), m_red("FALSE"), m_green("FALSE"), m_blue("FALSE"),
  m_luminance("FALSE")
{
}

IFXFORCEINLINE void ImageFormat::AddUrlList( const UrlList& rUrlList )
{
	m_urlList = rUrlList;
}

IFXFORCEINLINE const IFXArray<IFXString>& ImageFormat::GetUrlList() const
{
	return m_urlList.GetUrlList();
}

IFXFORCEINLINE Texture::Texture()
: m_height(0), m_width(0), m_imageType("RGB"), m_isExternal( TRUE )
{
}

IFXFORCEINLINE BOOL Texture::IsExternal() const
{
	return m_isExternal;
}

IFXFORCEINLINE void Texture::SetExternal( BOOL isExternal )
{
	m_isExternal = isExternal;
}

IFXFORCEINLINE void Texture::SetHeight( const I32& rHeight )
{
	m_height = rHeight;
}

IFXFORCEINLINE const I32& Texture::GetHeight() const
{
	return m_height;
}

IFXFORCEINLINE void Texture::SetWidth( const I32& rWidth )
{
	m_width = rWidth;
}

IFXFORCEINLINE const I32& Texture::GetWidth() const
{
	return m_width;
}

IFXFORCEINLINE void Texture::SetImageType( const IFXString& rType )
{
	m_imageType = rType;
}

IFXFORCEINLINE const IFXString& Texture::GetImageType() const
{
	return m_imageType;
}

IFXFORCEINLINE void Texture::AddImageFormat( const ImageFormat& rImageFormat )
{
	ImageFormat& imageFormat = m_imageFormatList.CreateNewElement();
	imageFormat = rImageFormat;
}

IFXFORCEINLINE const ImageFormat& Texture::GetImageFormat( U32 index ) const
{
	return m_imageFormatList.GetElementConst( index );
}

IFXFORCEINLINE const U32& Texture::GetImageFormatCount() const
{
	return m_imageFormatList.GetNumberElements();
}

IFXFORCEINLINE void Texture::SetPath( const IFXString& rPath )
{
	m_path = rPath;
}

IFXFORCEINLINE const IFXString& Texture::GetPath() const
{
	return m_path;
}


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}


#endif
