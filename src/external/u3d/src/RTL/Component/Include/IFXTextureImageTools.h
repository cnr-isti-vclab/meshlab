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
	@file	IFXTextureImageTools.h

			Header file for the interfaces classes for the texture image tools.
*/
#ifndef __IFXTEXTURE_IMAGE_TOOLS_H__
#define __IFXTEXTURE_IMAGE_TOOLS_H__

#include "IFXUnknown.h"

class IFXTextureImageTools : public IFXUnknown 
{
public:
	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef(void)  = 0; 	
	virtual U32 IFXAPI  Release(void) = 0; 	
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void **ppv) = 0;

	// class-specific constants...
	enum 
	{ 
		IFXTEXTUREMAP_FORMAT_LUMINANCE  = 1,
		IFXTEXTUREMAP_FORMAT_RGB24		= 3,
		IFXTEXTUREMAP_FORMAT_RGBA32		= 4,
		IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA  = 5,
		IFXTEXTUREMAP_FORMAT_ALPHA		= 6

	};
	virtual IFXRESULT IFXAPI  SetTexels(U32 uWidth, U32 uHeight, U8 uFormat, void* pinTexels) = 0;
	virtual IFXRESULT IFXAPI  CopyRenderImage( STextureOutputInfo* pRenderImageInfo) = 0;
	virtual BOOL	  IFXAPI  IsInitialize() = 0;
	virtual IFXRESULT IFXAPI  Clear() = 0;
};
#endif
