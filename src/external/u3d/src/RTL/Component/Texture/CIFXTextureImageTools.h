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
	@file	CIFXTextureImageTools.h

			Header file for the implementation of IFXTextureImageTools interface.
*/
#ifndef __CIFXTEXTURE_IMAGETOOLS_H__
#define __CIFXTEXTURE_IMAGETOOLS_H__

#include "IFXTextureObject.h"
#include "IFXTextureImageTools.h"

/*
 * The RGB data in the pixel format structure is valid.
 */
#define DDPF_RGB                                0x00000040l

/*
 * The surface has alpha channel information in the pixel format.
 */
#define DDPF_ALPHAPIXELS                        0x00000001l

class CIFXTextureImageTools : public IFXTextureImageTools
{
public:
	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef(void); 	
	virtual U32 IFXAPI  Release(void); 	
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void **ppv);

	virtual IFXRESULT IFXAPI  SetTexels(U32 uWidth, U32 uHeight, U8 uFormat, void* pinTexels);

	virtual IFXRESULT IFXAPI  CopyRenderImage( STextureOutputInfo* pRenderImageInfo);
	virtual BOOL	  IFXAPI  IsInitialize() {return m_bInitialized;}
	virtual IFXRESULT IFXAPI  Clear();


	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTextureImageTools_Factory( IFXREFIID interfaceId, void** ppInterface );

	class PixelFormat
	{
 	public:
			
		U32 dwSize; 
		U32 dwFlags; 
		U32 dwFourCC; 
		union 
		{ 
			U32 dwRGBBitCount; 
			U32 dwYUVBitCount; 
			U32 dwZBufferBitDepth; 
			U32 dwAlphaBitDepth; 
			U32 dwLuminanceBitCount;
			U32 dwBumpBitCount;
		}; 
		union 
		{ 
			U32 dwRBitMask; 
			U32 dwYBitMask; 
			U32 dwStencilBitDepth;
			U32 dwLuminanceBitMask;
			U32 dwBumpDuBitMask;
		};
		union 
		{ 
			U32 dwGBitMask; 
			U32 dwUBitMask; 
			U32 dwZBitMask;
			U32 dwBumpDvBitMask;
		};
		union 
		{ 
			U32 dwBBitMask; 
			U32 dwVBitMask; 
			U32 dwStencilBitMask;
			U32 dwBumpLuminanceBitMask;
		};
		union 
		{ 
			U32 dwRGBAlphaBitMask; 
			U32 dwYUVAlphaBitMask; 
			U32 dwLuminanceAlphaBitMask;
			U32 dwRGBZBitMask; 
			U32 dwYUVZBitMask; 
		};
	};

private:
	// constructor/destructor...
	CIFXTextureImageTools();
	virtual ~CIFXTextureImageTools();

	U32	ComputeBufferSize( U32 uWidth, U32 uHeight, U8 uFormat);

	IFXRESULT IFXAPI_CALLTYPE ResizeImage( U32 uNewWidth, U32 uNewHeight);
	IFXRESULT IFXAPI_CALLTYPE MipMap( STextureOutputInfo* pSrcOutputInfo,
						 STextureOutputInfo* pDstOutputInfo,
						 U32 uSrcX,U32 uSrcY, 
						 U32 uDstX, U32 uDstY);

	IFXRESULT IFXAPI_CALLTYPE ReformatImage(U8* pDst, IFXenum pSrcFormat, IFXenum eSrcOrder,
							 IFXenum pDstFormat, IFXenum eDstOrder,
							 U32 uSrcX, U32 uSrcY, U32 uSrcPitch,
							 U32 uDstX, U32 uDstY, U32 uDstPitch,
							 U32 uWidth, U32 uHeight );
	IFXRESULT ConvertToRenderFormat( STextureOutputInfo* pRenderImageInfo);


	// store the refcount of the instance here...
	U32 m_uRefCount;

	// store the allocated width, height and format here...
	U32 m_uWidth, m_uHeight, m_uPitch, m_uSize;
	U8	m_u8PixelSize;
	IFXenum m_eCurRenderFormat;
	IFXenum m_eCurOrder;
	U8	m_uFormat;

	BOOL m_bCorrectedFormat;
	BOOL m_bCorrectSized;
	BOOL m_bResized;
	BOOL m_bHasAlpha;
	BOOL m_bInitialized;
	// store the texel buffer pointer here...
	U8 *m_pBuffer;
};

#endif
