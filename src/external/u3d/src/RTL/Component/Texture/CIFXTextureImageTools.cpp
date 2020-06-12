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
//
//  CIFXTextureImageTools.cpp
//
//  DESCRIPTION
//    Converts Image format to render format.
//
//  NOTES
//      None.
//
//***************************************************************************

#include "CIFXTextureImageTools.h"
#include "CIFXUtilities.h"
#include "IFXTextureErrors.h"
#include "IFXIPP.h"

//***************************************************************************
//	Local function prototypes
//***************************************************************************

IFXRESULT IFXAPI_CALLTYPE IFXTextureImageTools_ResizeImage(
								U8* pSrc, U8* pDest, U8 m_u8PixelSize, 
								BOOL m_bHasAlpha, U32 uSrcWidth, U32 uSrcHeight,
								U32 uDstWidth, U32 uDstHeight);

IFXRESULT IFXAPI_CALLTYPE IFXTextureImageTools_ReformatImage(U8* pSrc, U8* pDst, U8 u8PixelSize,
											   IFXenum eSrcFormat, IFXenum eSrcOrder,
											   IFXenum eDstFormat, IFXenum eDstOrder,
											   U32 uSrcX, U32 uSrcY, U32 uSrcPitch,
											   U32 uDstX, U32 uDstY, U32 uDstPitch,
											   U32 uWidth, U32 uHeight);

void IFXAPI_CALLTYPE IFXTextureImageTools_MipMap(
								U8* pSrc, U8* pRenderBuffer, U8 m_u8PixelSize, BOOL m_bHasAlpha,
								 STextureOutputInfo* pSrcOutputInfo, STextureOutputInfo* pDstOutputInfo,
								 U32 uSrcX, U32 uSrcY, U32 uDstX, U32 uDstY);

//***************************************************************************
//	Local data
//***************************************************************************

pIFXTextureImageTools_ResizeImage _IFXTextureImageTools_ResizeImage = IFXTextureImageTools_ResizeImage;
pIFXTextureImageTools_ReformatImage _IFXTextureImageTools_ReformatImage = IFXTextureImageTools_ReformatImage;
pIFXTextureImageTools_MipMap _IFXTextureImageTools_MipMap = IFXTextureImageTools_MipMap;


//***************************************************************************
//	Public methods
//***************************************************************************

// Constructor
CIFXTextureImageTools::CIFXTextureImageTools()
{
	m_uRefCount   = 0;
	m_uWidth    = m_uHeight = m_uFormat = m_uSize = 0;
	m_uPitch    = 0;
	m_u8PixelSize = 3;
	m_pBuffer   = NULL;
	m_bCorrectedFormat  = FALSE;
	m_bHasAlpha       = FALSE;
	m_bCorrectSized   = FALSE;
	m_eCurRenderFormat  = 0;
	m_eCurOrder     = IFX_RGBA;
	m_bInitialized    = FALSE;
	m_bResized      = FALSE;
}

// Destructor.
CIFXTextureImageTools::~CIFXTextureImageTools()
{
	if (m_pBuffer)
	{
		IFXDeallocate(m_pBuffer);
		m_pBuffer = 0;
	}

	return;
}


// IFXUnknown methods...

U32 CIFXTextureImageTools::AddRef(void) {
	return ++m_uRefCount;
}

U32 CIFXTextureImageTools::Release(void) {
	if (--m_uRefCount == 0) {
		delete this;
		return 0;
	}
	return m_uRefCount;
}

IFXRESULT CIFXTextureImageTools::Clear()
{
	if (m_pBuffer)
	{
		IFXDeallocate(m_pBuffer);
		m_pBuffer = 0;
	}
	m_uWidth = m_uHeight = m_uFormat = m_uSize = 0;
	m_uPitch = 0;
	m_u8PixelSize = 3;
	m_bCorrectedFormat= FALSE;
	m_bHasAlpha     = FALSE;
	m_bCorrectSized   = FALSE;
	m_eCurRenderFormat = 0;
	m_eCurOrder      = IFX_RGBA;
	m_bInitialized    = FALSE;

	return IFX_OK;
}


IFXRESULT CIFXTextureImageTools::QueryInterface(IFXREFIID interfaceId, void **ppInterface) {
	IFXRESULT result  = IFX_OK;

	if (ppInterface)
	{
		if (IID_IFXUnknown == interfaceId) {
			*ppInterface = (IFXUnknown*) this;
			this->AddRef();
		} else if (interfaceId == IID_IFXTextureImageTools) {
			*ppInterface = (IFXTextureImageTools*) this;
			this->AddRef();
		} else {
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//---------------------------------------------------------------------------
//  CIFXTextureImageTools_Factory (non-singleton)
//
//  This is the CIFXTextureImageTools component factory function.  The
//  CIFXTextureImageTools component is NOT a singleton.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXTextureImageTools_Factory(IFXREFIID interfaceId, void** ppInterface){
	IFXRESULT result;

	if (ppInterface)
	{
		// Try to create it.
		CIFXTextureImageTools *pComponent = new CIFXTextureImageTools;

		if (pComponent)
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface(interfaceId, ppInterface);

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


IFXRESULT CIFXTextureImageTools::SetTexels(U32 uWidth, U32 uHeight, U8 uFormat, void* pinTexels)
{
	IFXRESULT iResult = IFX_OK;
	U32 uSize=0;

	if (NULL == pinTexels)
	{
		iResult = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(iResult))
	{
		uSize = ComputeBufferSize(uWidth,uHeight,uFormat);
		if (uSize > 0)
		{
			{
				//Don't want to copy this buffer, just
				//use as original image. The texture object
				//will clean this up.
				m_pBuffer = (U8*)pinTexels;
				m_uWidth  = uWidth;
				m_uHeight = uHeight;
				m_uFormat = uFormat;
				m_uSize   = uSize;
				if (IFXTEXTUREMAP_FORMAT_LUMINANCE == m_uFormat)
				{
					m_u8PixelSize = 1;
				}
				else if (IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24 == m_uFormat ||
					IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24 == m_uFormat)
				{
					m_u8PixelSize = 3;
					m_eCurRenderFormat = IFX_RGBA_8880;
					if (IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24 == m_uFormat)
						m_eCurOrder = IFX_RGBA;
					else 
						m_eCurOrder = IFX_BGRA;

				}
				else if (IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32 == m_uFormat ||
					     IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32 == m_uFormat)
				{
					m_u8PixelSize = 4;
					m_bHasAlpha = TRUE;
					m_eCurRenderFormat = IFX_RGBA_8888;
					if (IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32 == m_uFormat)
						m_eCurOrder = IFX_RGBA;
					else 
						m_eCurOrder = IFX_BGRA;
				}
				m_uPitch  = m_uWidth*m_u8PixelSize;
				m_bInitialized = TRUE;
			}
		}
		else
		{
			iResult = IFX_TEXTURE_MAP_NOT_ALLOCATED; /// @todo: Use other error value.
		}
	}

	return iResult;
}


U32 CIFXTextureImageTools::ComputeBufferSize(U32 uWidth, U32 uHeight, U8 uFormat) {
	U32 uSize = 0;
	if (IFXTEXTUREMAP_FORMAT_LUMINANCE == uFormat)
	{
		uSize = uWidth * uHeight;
	}
	else if (IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24 == uFormat ||
			 IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGR24 == uFormat)
	{
		uSize = uWidth * uHeight * 3;
	}
	else if (IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32 == uFormat ||
		     IFXTextureObject::IFXTEXTUREMAP_FORMAT_BGRA32 == uFormat)
	{
		uSize = uWidth * uHeight * 4;
		m_bHasAlpha = TRUE;
	}
	return uSize;
}


IFXRESULT IFXAPI_CALLTYPE CIFXTextureImageTools::ReformatImage(U8* pDst,
											   IFXenum eSrcFormat, IFXenum eSrcOrder,
											   IFXenum eDstFormat, IFXenum eDstOrder,
											   U32 uSrcX, U32 uSrcY, U32 uSrcPitch,
											   U32 uDstX, U32 uDstY, U32 uDstPitch,
											   U32 uWidth, U32 uHeight)
{
	if (!((eSrcFormat == IFX_RGBA_8888) || (eSrcFormat == IFX_RGBA_8880)) || !((eDstFormat == IFX_RGBA_8888) || (eDstFormat == IFX_RGBA_8880)))
		return IFXTextureImageTools_ReformatImage(
						m_pBuffer, pDst, m_u8PixelSize, 
						eSrcFormat, eSrcOrder, 
						eDstFormat, eDstOrder, 
						uSrcX, uSrcY, uSrcPitch, 
						uDstX, uDstY, uDstPitch, uWidth, uHeight);
	else
		return _IFXTextureImageTools_ReformatImage(
						m_pBuffer, pDst, m_u8PixelSize, 
						eSrcFormat, eSrcOrder, 
						eDstFormat, eDstOrder, 
						uSrcX, uSrcY, uSrcPitch, 
						uDstX, uDstY, uDstPitch, uWidth, uHeight);
}


IFXRESULT IFXAPI_CALLTYPE CIFXTextureImageTools::MipMap(STextureOutputInfo* pSrcOutputInfo,
										STextureOutputInfo* pDstOutputInfo,
										U32 uSrcX, U32 uSrcY,
										U32 uDstX, U32 uDstY)
{
	IFXRESULT iResult = IFX_OK;

	if(pSrcOutputInfo->m_pData == NULL || pDstOutputInfo->m_pData == NULL )
		iResult = IFX_E_INVALID_POINTER;
	else
	{
		_IFXTextureImageTools_MipMap(m_pBuffer, pDstOutputInfo->m_pData, m_u8PixelSize, m_bHasAlpha, pSrcOutputInfo, pDstOutputInfo, uSrcX, uSrcY, uDstX, uDstY);
		m_eCurOrder = pDstOutputInfo->eChannelOrder;
		m_eCurRenderFormat = pDstOutputInfo->eRenderFormat;
		m_uPitch  = pDstOutputInfo->m_pitch;
		m_uHeight = pDstOutputInfo->m_height;
		m_uWidth  = pDstOutputInfo->m_width;
		pSrcOutputInfo->m_pitch = 0;
		//We don't delete the initial buffer to save to memcpy calls.
		//This will reduce the memory foot prints.
		memcpy(m_pBuffer, pDstOutputInfo->m_pData, sizeof(U8)*m_uHeight*m_uPitch);
	}

	return iResult;
}


IFXRESULT IFXAPI_CALLTYPE CIFXTextureImageTools::ResizeImage(U32 uNewWidth, U32 uNewHeight)
{
	U32 uSize = uNewWidth*uNewHeight*m_u8PixelSize;
	U8* pNewBuffer = (U8*)IFXAllocate(uSize);
	if (NULL == pNewBuffer)
		return IFX_E_OUT_OF_MEMORY;

	_IFXTextureImageTools_ResizeImage(m_pBuffer, pNewBuffer, m_u8PixelSize, m_bHasAlpha, m_uWidth, m_uHeight, uNewWidth, uNewHeight);

	//We point this buffer to reformated image buf.
	//We don't need to release the original buf.  The Texture Obj will taking care of
	//it.
	m_pBuffer = pNewBuffer;
	m_uWidth = uNewWidth;
	m_uHeight = uNewHeight;
	m_uSize = uSize;
	m_uPitch = m_uWidth*m_u8PixelSize;
	m_bResized = TRUE;
	return IFX_OK;
}


IFXRESULT CIFXTextureImageTools::ConvertToRenderFormat(STextureOutputInfo* pRenderImageInfo)
{
	IFXRESULT iResult = IFX_OK;

	if (pRenderImageInfo->m_width != m_uWidth || pRenderImageInfo->m_height!= m_uHeight)
		iResult = ResizeImage( pRenderImageInfo->m_width, pRenderImageInfo->m_height);

	if (IFXSUCCESS(iResult))
	{
		ReformatImage ( pRenderImageInfo->m_pData,
			m_eCurRenderFormat, m_eCurOrder,
			pRenderImageInfo->eRenderFormat,
			pRenderImageInfo->eChannelOrder,
			0, 0, m_uPitch, 0, 0,
			pRenderImageInfo->m_pitch, pRenderImageInfo->m_width,
			pRenderImageInfo->m_height);
		m_uWidth  = pRenderImageInfo->m_width;
		m_uHeight = pRenderImageInfo->m_height;
		m_uPitch  = pRenderImageInfo->m_pitch;
		m_eCurRenderFormat = pRenderImageInfo->eRenderFormat;
		m_eCurOrder = pRenderImageInfo->eChannelOrder;

		//If the image resized in ResizeImage function, the m_pBuffer points
		//at new allocated buffer, not the buffer from Texture obj.
		//Therefor it needs to be deleted here.  Otherwise the m_pBuffer points
		//at the same buffers in the Texture obj, then we let the it clean
		//up.
		if (m_bResized)
		{
			IFXDeallocate(m_pBuffer);
			m_pBuffer = 0;
		}
		m_pBuffer = (U8*)IFXAllocate(m_uHeight*m_uPitch);
		memcpy(m_pBuffer,pRenderImageInfo->m_pData,m_uPitch*m_uHeight);
	}

	if (IFXSUCCESS(iResult))
		m_bCorrectedFormat = TRUE;
	return iResult;
}

IFXRESULT CIFXTextureImageTools::CopyRenderImage(STextureOutputInfo* pRenderImageInfo)
{
	IFXRESULT iResult = IFX_OK;
	if (!m_bCorrectedFormat)
	{
		iResult = ConvertToRenderFormat(pRenderImageInfo);
	}
	else
	{
		STextureOutputInfo RenderImgInfo;
		RenderImgInfo.m_pData = (U8*)m_pBuffer;
		RenderImgInfo.m_width = m_uWidth;
		RenderImgInfo.m_height = m_uHeight;
		RenderImgInfo.m_pitch  = m_uPitch;
		RenderImgInfo.eRenderFormat = m_eCurRenderFormat;
		RenderImgInfo.eChannelOrder = m_eCurOrder;
		iResult = MipMap (&RenderImgInfo, pRenderImageInfo, 0, 0, 0, 0);
	}

	return iResult;
}

//***************************************************************************
//	Local functions
//***************************************************************************

//---------------------------------------------------------------------------
//  CIFXTextureImageTools::CalcOffset
//
//  This method will return the number bits unset before the most significant
//  set bit.
//---------------------------------------------------------------------------
U32 IFXTextureImageTools_CalcOffset(U32 mask)
{
	U32 offset = 0;
	if (mask) {
		while (!(mask&1)) {
			offset++;
			mask >>= 1;
		}
	}
	return offset;
}


//---------------------------------------------------------------------------
//  CIFXTextureImageTools::CountBits
//
//  This method will return the number of set bits in the argument
//---------------------------------------------------------------------------
U32 IFXTextureImageTools_CountBits(U32 bits)
{
	U32 numBits = 0;
	while (bits) {
		if (bits & 1)
			numBits++;
		bits >>= 1;
	}
	return numBits;
}


IFXRESULT IFXTextureImageTools_GetPixelFormat(IFXenum eInBufferFormat, IFXenum eInOrder, CIFXTextureImageTools::PixelFormat* pddpf)
{
	IFXRESULT iResult = IFX_OK;

	if (pddpf == NULL)
		iResult = IFX_E_INVALID_POINTER;
	else
	{
		memset(pddpf, 0, sizeof(CIFXTextureImageTools::PixelFormat));
		pddpf->dwSize = sizeof(CIFXTextureImageTools::PixelFormat);
		pddpf->dwFlags = DDPF_RGB;

		switch (eInBufferFormat){
			case IFX_RGBA_8888:
				pddpf->dwRGBBitCount = 32;
				pddpf->dwRBitMask = 0x00ff0000;
				pddpf->dwGBitMask = 0x0000ff00;
				pddpf->dwBBitMask = 0x000000ff;
				pddpf->dwFlags |= DDPF_ALPHAPIXELS;
				pddpf->dwRGBAlphaBitMask = 0xff000000;
				break;
			case IFX_RGBA_8880:
				pddpf->dwRGBBitCount = 24;
				pddpf->dwRBitMask = 0x00ff0000;
				pddpf->dwGBitMask = 0x0000ff00;
				pddpf->dwBBitMask = 0x000000ff;
				pddpf->dwRGBAlphaBitMask = 0x00000000;
				break;
			case IFX_RGBA_5551:
				pddpf->dwRGBBitCount = 16;
				pddpf->dwRBitMask = 0x7c00;
				pddpf->dwGBitMask = 0x03e0;
				pddpf->dwBBitMask = 0x001f;
				pddpf->dwFlags |= DDPF_ALPHAPIXELS;
				pddpf->dwRGBAlphaBitMask = 0x8000;
				break;
			case IFX_RGBA_5550:
				pddpf->dwRGBBitCount = 16;
				pddpf->dwRBitMask = 0x7c00;
				pddpf->dwGBitMask = 0x03e0;
				pddpf->dwBBitMask = 0x001f;
				pddpf->dwRGBAlphaBitMask = 0x0000;
				break;
			case IFX_RGBA_5650:
				pddpf->dwRGBBitCount = 16;
				pddpf->dwRBitMask = 0xf800;
				pddpf->dwGBitMask = 0x07e0;
				pddpf->dwBBitMask = 0x001f;
				pddpf->dwRGBAlphaBitMask = 0x0000;
				break;
			case IFX_RGBA_4444:
				pddpf->dwRGBBitCount = 16;
				pddpf->dwRBitMask = 0x0f00;
				pddpf->dwGBitMask = 0x00f0;
				pddpf->dwBBitMask = 0x000f;
				pddpf->dwFlags |= DDPF_ALPHAPIXELS;
				pddpf->dwRGBAlphaBitMask = 0xf000;
				break;
			default:
				iResult = IFX_E_UNSUPPORTED;
		}
	}

	if (IFXSUCCESS(iResult) && (eInOrder == IFX_RGBA)) {
		U32 temp;
		temp = pddpf->dwRBitMask;
		pddpf->dwRBitMask = pddpf->dwBBitMask;
		pddpf->dwBBitMask = temp;
	}

	return iResult;
}

U32 IFXTextureImageTools_GetBitsPerPixel(IFXenum eInBufferFormat)
{
	U32 bits = 0;
	switch (eInBufferFormat) {
		case IFX_RGBA_8888:
			bits = 32; break;
		case IFX_RGBA_8880:
			bits = 24;break;
		case IFX_RGBA_5550:
		case IFX_RGBA_5650:
		case IFX_RGBA_5551:
		case IFX_RGBA_4444:
			bits = 16; break;
	}
	return bits;
}

//---------------------------------------------------------------------------
//  CIFXTextureImageTools::ReformatImage
//
// Convert the RGB(A) 8-bits to requested render format.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE IFXTextureImageTools_ReformatImage(U8* pSrc, U8* pDst, U8 u8PixelSize,
											   IFXenum eSrcFormat, IFXenum eSrcOrder,
											   IFXenum eDstFormat, IFXenum eDstOrder,
											   U32 uSrcX, U32 uSrcY, U32 uSrcPitch,
											   U32 uDstX, U32 uDstY, U32 uDstPitch,
											   U32 uWidth, U32 uHeight)
{
	IFXRESULT iResult = IFX_OK;

	if (pSrc == NULL || pDst == NULL)
		iResult = IFX_E_INVALID_POINTER;
	else
	{
		U32* pSrcPixelLong;
		U32* pDstPixelLong;
		U16* pDstPixelShort;
		U8*  pSrcPixelShort;
		CIFXTextureImageTools::PixelFormat srcFrmt, dstFrmt;

		IFXTextureImageTools_GetPixelFormat(eSrcFormat, eSrcOrder, &srcFrmt);
		IFXTextureImageTools_GetPixelFormat(eDstFormat, eDstOrder, &dstFrmt);
		U32 redSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwRBitMask);
		U32 greenSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwGBitMask);
		U32 blueSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwBBitMask);
		U32 alphaSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwRGBAlphaBitMask);
		U32 redDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwRBitMask);
		U32 greenDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwGBitMask);
		U32 blueDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwBBitMask);
		U32 alphaDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwRGBAlphaBitMask);
		I32 redOffset = redDstOffset -
			redSrcOffset +
			IFXTextureImageTools_CountBits(dstFrmt.dwRBitMask) -
			IFXTextureImageTools_CountBits(srcFrmt.dwRBitMask);
		I32 greenOffset = greenDstOffset -
			greenSrcOffset +
			IFXTextureImageTools_CountBits(dstFrmt.dwGBitMask) -
			IFXTextureImageTools_CountBits(srcFrmt.dwGBitMask);
		I32 blueOffset = blueDstOffset -
			blueSrcOffset +
			IFXTextureImageTools_CountBits(dstFrmt.dwBBitMask) -
			IFXTextureImageTools_CountBits(srcFrmt.dwBBitMask);
		I32 alphaOffset = alphaDstOffset -
			alphaSrcOffset +
			IFXTextureImageTools_CountBits(dstFrmt.dwRGBAlphaBitMask) -
			IFXTextureImageTools_CountBits(srcFrmt.dwRGBAlphaBitMask);
		U32 value;
		U32 sBpp = srcFrmt.dwRGBBitCount >> 3;
		U32 dBpp = dstFrmt.dwRGBBitCount >> 3;
		BOOL bFillAlpha = FALSE;

		if(!(srcFrmt.dwRGBAlphaBitMask) && dstFrmt.dwRGBAlphaBitMask)
		{
			bFillAlpha = TRUE;
		}

		U32 sX, sY, dX, dY, bpp;
		U8 u8RIndex, u8GIndex, u8BIndex, u8AIndex = 0;
		if (IFX_RGBA == eSrcOrder)
		{
			u8RIndex = 0;
			u8GIndex = 1;
			u8BIndex = 2;
			u8AIndex = 3;
		}
		else
		{
			u8RIndex = 2;
			u8GIndex = 1;
			u8BIndex = 0;
			u8AIndex = 3;
		}

		bpp = IFXTextureImageTools_GetBitsPerPixel(eDstFormat)>>3;
		if(bpp == 4)
		{
			U32 uDstIndex = 0;
			U32 uSrcIndex = 0;

			// Convert from RGBA, RGB, GrayScale to BGRA render format.
			//
			{
				if(u8PixelSize == 4)
				{
					for(sY = uSrcY, dY = uDstY; sY < (uHeight + uSrcY); sY++, dY++)
					{
						for(sX = uSrcX, dX = uDstX; sX < (uWidth + uSrcX); sX++, dX++)
						{
							uDstIndex = dY*uDstPitch+dX*bpp;
							uSrcIndex = sY*uSrcPitch+sX*u8PixelSize;
							pSrcPixelShort = (U8*)&(pSrc[uSrcIndex]);
							pDstPixelLong = (U32*)&(pDst[uDstIndex]);
							*pDstPixelLong = MakeTexel(pSrcPixelShort[u8RIndex+redOffset/8],
								pSrcPixelShort[u8GIndex+greenOffset/8],
								pSrcPixelShort[u8BIndex+blueOffset/8],
								pSrcPixelShort[u8AIndex+alphaOffset/8]);
						}
					}
				}
				else if(u8PixelSize == 3)
				{
					//U32 uSrcPixelLong = 0;
					for(sY = uSrcY, dY = uDstY; sY < (uHeight + uSrcY); sY++, dY++)
					{
						for(sX = uSrcX, dX = uDstX; sX < (uWidth + uSrcX); sX++, dX++)
						{
							uDstIndex = dY*uDstPitch+dX*bpp;
							uSrcIndex = sY*uSrcPitch+sX*u8PixelSize;
							pSrcPixelShort = (U8*)&(pSrc[uSrcIndex]);
							pDstPixelLong = (U32*)&(pDst[uDstIndex]);
							*pDstPixelLong = MakeTexel(pSrcPixelShort[u8RIndex+redOffset/8],
								pSrcPixelShort[u8GIndex+greenOffset/8],
								pSrcPixelShort[u8BIndex+blueOffset/8],
								0xFF);
						}
					}
				}
				else
				{
					for(sY = uSrcY, dY = uDstY; sY < (uHeight + uSrcY); sY++, dY++)
					{
						for(sX = uSrcX, dX = uDstX; sX < (uWidth + uSrcX); sX++, dX++)
						{
							uDstIndex = dY*uDstPitch+dX*bpp;
							uSrcIndex = sY*uSrcPitch+sX*u8PixelSize;
							pSrcPixelShort = (U8*)&(pSrc[uSrcIndex]);
							pDstPixelLong = (U32*)&(pDst[uDstIndex]);
							*pDstPixelLong = MakeTexel(pSrcPixelShort[u8RIndex+redOffset/8],
								pSrcPixelShort[u8GIndex+greenOffset/8],
								pSrcPixelShort[u8BIndex+blueOffset/8],
								0xFF);
						}
					}
				}
			}

		}
		else
		{
			// The DX texture formats use 32 bit 8880, our texture manager uses 24 bit...
			if(eSrcFormat == IFX_RGBA_8880)
			{
				if (u8PixelSize == 1)
					sBpp = 1;
				else
					sBpp = 3;
			}

			// I would like to keep this general purpose, but in reality the sBpp is
			// either 3 or 4, never 2.  The dBpp is always 2 or 4, never 3.
			if(sBpp == 3 || sBpp == 1)
			{
				if (sBpp == 1)
				{
					u8RIndex = u8GIndex = u8BIndex = u8AIndex = 0;
				}
				else
					u8AIndex = 0;

				U32  uSrcPixelLong;
				if(dBpp == 2)
				{
					// Do a masked component copy -- ARGH!
					for(sY = uSrcY, dY = uDstY; sY < (uHeight + uSrcY); sY++, dY++)
					{
						for(sX = uSrcX, dX = uDstX; sX < (uWidth + uSrcX); sX++, dX++)
						{
							pDstPixelShort = (U16*)&(pDst[dY*uDstPitch+dX*dBpp]);
							pSrcPixelShort = (U8*)&(pSrc[sY*uSrcPitch+sX*sBpp]);
							(*pDstPixelShort) &= ~(dstFrmt.dwRBitMask|dstFrmt.dwGBitMask|
								dstFrmt.dwBBitMask|dstFrmt.dwRGBAlphaBitMask);
							uSrcPixelLong = MakeTexel(pSrcPixelShort[u8RIndex],
								pSrcPixelShort[u8GIndex],
								pSrcPixelShort[u8BIndex],
								0xFF);
							// Red component
							value = (uSrcPixelLong) & srcFrmt.dwRBitMask;
							(redOffset < 0) ? value >>= -redOffset : value <<= redOffset;
							value &= dstFrmt.dwRBitMask;
							(*pDstPixelShort) |= value;

							// Green component
							value = (uSrcPixelLong) & srcFrmt.dwGBitMask;
							(greenOffset < 0) ? value >>= -greenOffset : value <<= greenOffset;
							value &= dstFrmt.dwGBitMask;
							(*pDstPixelShort) |= value;

							// Blue component
							value = (uSrcPixelLong) & srcFrmt.dwBBitMask;
							(blueOffset < 0) ? value >>= -blueOffset : value <<= blueOffset;
							value &= dstFrmt.dwBBitMask;
							(*pDstPixelShort) |= value;

							// Alpha component
							(*pDstPixelShort) |= dstFrmt.dwRGBAlphaBitMask;
						}
					}
				}
			}
			else //convert from 32 bits to 16 bits
			{
				if(dBpp == 2)
				{
					// Do a masked component copy -- ARGH!
					for(sY = uSrcY, dY = uDstY; sY < (uHeight + uSrcY); sY++, dY++)
					{
						for(sX = uSrcX, dX = uDstX; sX < (uWidth + uSrcX); sX++, dX++)
						{
							pDstPixelShort = (U16*)&(pDst[dY*uDstPitch+dX*dBpp]);
							pSrcPixelLong = (U32*)&(pSrc[sY*uSrcPitch+sX*sBpp]);
							(*pDstPixelShort) &= ~(dstFrmt.dwRBitMask|dstFrmt.dwGBitMask|
								dstFrmt.dwBBitMask|dstFrmt.dwRGBAlphaBitMask);

							// Red component
							value = (*pSrcPixelLong) & srcFrmt.dwRBitMask;
							(redOffset < 0) ? value >>= -redOffset : value <<= redOffset;
							value &= dstFrmt.dwRBitMask;
							(*pDstPixelShort) |= value;

							// Green component
							value = (*pSrcPixelLong) & srcFrmt.dwGBitMask;
							(greenOffset < 0) ? value >>= -greenOffset : value <<= greenOffset;
							value &= dstFrmt.dwGBitMask;
							(*pDstPixelShort) |= value;

							// Blue component
							value = (*pSrcPixelLong) & srcFrmt.dwBBitMask;
							(blueOffset < 0) ? value >>= -blueOffset : value <<= blueOffset;
							value &= dstFrmt.dwBBitMask;
							(*pDstPixelShort) |= value;

							// Alpha component
							if(bFillAlpha)
							{
								(*pDstPixelShort) |= dstFrmt.dwRGBAlphaBitMask;
							}
							else
							{
								value = (*pSrcPixelLong) & srcFrmt.dwRGBAlphaBitMask;
								(alphaOffset < 0) ? value >>= -alphaOffset : value <<= alphaOffset;
								value &= dstFrmt.dwRGBAlphaBitMask;
								(*pDstPixelShort) |= value;
							}
						}
					}
				}
			}

		}
	}
	return iResult;
}


//---------------------------------------------------------------------------
//  This method down sampling the image to next the next level.
//---------------------------------------------------------------------------
void IFXAPI_CALLTYPE IFXTextureImageTools_MipMap(
								U8* pSrc, U8* pRenderBuffer, U8 m_u8PixelSize, BOOL m_bHasAlpha,
								 STextureOutputInfo* pSrcOutputInfo, STextureOutputInfo* pDstOutputInfo,
								 U32 uSrcX, U32 uSrcY, U32 uDstX, U32 uDstY)
{
	CIFXTextureImageTools::PixelFormat srcFrmt, dstFrmt;
	U32 sX, sY, dX, dY;
	IFXenum eSrcFormat = pSrcOutputInfo->eRenderFormat;
	IFXenum eSrcOrder  = pSrcOutputInfo->eChannelOrder;
	IFXenum eDstFormat = pDstOutputInfo->eRenderFormat;
	IFXenum eDstOrder  = pDstOutputInfo->eChannelOrder;

	//U32 bpp = IFXTextureImageTools_GetBitsPerPixel(eSrcFormat)>>3;
	IFXTextureImageTools_GetPixelFormat(eSrcFormat, eSrcOrder, &srcFrmt);
	IFXTextureImageTools_GetPixelFormat(eDstFormat, eDstOrder, &dstFrmt);
	//U32 redSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwRBitMask);
	//U32 greenSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwGBitMask);
	//U32 blueSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwBBitMask);
	//U32 alphaSrcOffset = IFXTextureImageTools_CalcOffset(srcFrmt.dwRGBAlphaBitMask);
	//U32 redDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwRBitMask);
	//U32 greenDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwGBitMask);
	//U32 blueDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwBBitMask);
	//U32 alphaDstOffset = IFXTextureImageTools_CalcOffset(dstFrmt.dwRGBAlphaBitMask);
	//I32 redOffset = redDstOffset -
	//	redSrcOffset +
	//	IFXTextureImageTools_CountBits(dstFrmt.dwRBitMask) -
	//	IFXTextureImageTools_CountBits(srcFrmt.dwRBitMask);
	//I32 greenOffset = greenDstOffset -
	//	greenSrcOffset +
	//	IFXTextureImageTools_CountBits(dstFrmt.dwGBitMask) -
	//	IFXTextureImageTools_CountBits(srcFrmt.dwGBitMask);
	//I32 blueOffset = blueDstOffset -
	//	blueSrcOffset +
	//	IFXTextureImageTools_CountBits(dstFrmt.dwBBitMask) -
	//	IFXTextureImageTools_CountBits(srcFrmt.dwBBitMask);
	//I32 alphaOffset = alphaDstOffset -
	//	alphaSrcOffset +
	//	IFXTextureImageTools_CountBits(dstFrmt.dwRGBAlphaBitMask) -
	//	IFXTextureImageTools_CountBits(srcFrmt.dwRGBAlphaBitMask);
	U32 sBpp = srcFrmt.dwRGBBitCount >> 3;
	if (sBpp == 0)
		sBpp = m_u8PixelSize;
	U32 dBpp = dstFrmt.dwRGBBitCount >> 3;

	BOOL bFillAlpha = FALSE;

	if(!(srcFrmt.dwRGBAlphaBitMask) && dstFrmt.dwRGBAlphaBitMask)
	{
		bFillAlpha = TRUE;
	}
	if (!m_bHasAlpha)
		bFillAlpha = TRUE;

	// I would like to keep this general purpose, but in reality the sBpp is
	// either 3 or 4, never 2.  The dBpp is always 2 or 4, never 3.

	U32 sY2 = 0;
	U32 uCurRow = 0;
	U32 uNxtRow = 0;
	U32 uDstCurRowd = 0;
	U8 sBpp2 = sBpp*2;
	//U32 uPixelValue = 0;
	U8  u8ShiftBits = 0;
	if(dBpp == 2)
	{
		U16* pDstPixelShort=NULL;
		U16* pRenderPixelShort=NULL;
		U32 RValue=0;
		U32 GValue=0;
		U32 BValue=0;
		U32 AValue=0;
		// Do a masked component copy -- ARGH!

		for(sY = uSrcY, dY = uDstY; dY < (pDstOutputInfo->m_height + uSrcY); sY++, dY++)
		{
			sY2 = sY*2;
			uCurRow= sY2*pSrcOutputInfo->m_pitch;
			uNxtRow= (sY2+1)*pSrcOutputInfo->m_pitch;
			uDstCurRowd = dY*pDstOutputInfo->m_pitch;
			for(sX = uSrcX, dX = uDstX; dX < (pDstOutputInfo->m_width+ uSrcX); sX++, dX++)
			{
				pDstPixelShort = (U16*)&(pRenderBuffer[uDstCurRowd+dX*dBpp]);
				pRenderPixelShort = (U16*)&(pRenderBuffer[uDstCurRowd+dX*dBpp]);
				(*pDstPixelShort) &= ~(dstFrmt.dwRBitMask|dstFrmt.dwGBitMask|
					dstFrmt.dwBBitMask|dstFrmt.dwRGBAlphaBitMask);
				if ((pDstOutputInfo->m_height < pSrcOutputInfo->m_height &&
					pDstOutputInfo->m_width < pSrcOutputInfo->m_width))
				{
					RValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwRBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwRBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwRBitMask)+
						((*(U16*)&(pSrc[uNxtRow + (sX*2+1)*sBpp])) & srcFrmt.dwRBitMask);
					GValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwGBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwGBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwGBitMask)+
						((*(U16*)&(pSrc[uNxtRow + (sX*2+1)*sBpp])) & srcFrmt.dwGBitMask);
					BValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwBBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwBBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwBBitMask)+
						((*(U16*)&(pSrc[uNxtRow + (sX*2+1)*sBpp])) & srcFrmt.dwBBitMask);
					AValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwRGBAlphaBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwRGBAlphaBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwRGBAlphaBitMask)+
						((*(U16*)&(pSrc[uNxtRow + (sX*2+1)*sBpp])) & srcFrmt.dwRGBAlphaBitMask);
					u8ShiftBits = 2;
				}
				else if(pDstOutputInfo->m_height < pSrcOutputInfo->m_height)
				{
					RValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwRBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwRBitMask);
					GValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwGBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwGBitMask);
					BValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwBBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwBBitMask);
					AValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwRGBAlphaBitMask)+
						((*(U16*)&(pSrc[uNxtRow + sX*sBpp2])) & srcFrmt.dwRGBAlphaBitMask);
					u8ShiftBits = 1;
				}
				else if(pDstOutputInfo->m_width < pSrcOutputInfo->m_width)
				{
					RValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwRBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwRBitMask);
					GValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwGBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwGBitMask);
					BValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwBBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwBBitMask);
					AValue = ((*(U16*)&(pSrc[uCurRow + sX*sBpp2])) & srcFrmt.dwRGBAlphaBitMask)+
						((*(U16*)&(pSrc[uCurRow + (sX*2+1)*sBpp])) & srcFrmt.dwRGBAlphaBitMask);
					u8ShiftBits = 1;
				}

				// Red component
				(*pDstPixelShort) |= (RValue>>u8ShiftBits)&dstFrmt.dwRBitMask;
				// Green component
				(*pDstPixelShort) |= (GValue>>u8ShiftBits)&dstFrmt.dwGBitMask;

				// Blue component
				(*pDstPixelShort) |= (BValue>>u8ShiftBits)&dstFrmt.dwBBitMask;

				// Alpha component
				if(bFillAlpha)
				{
					(*pDstPixelShort) |= dstFrmt.dwRGBAlphaBitMask;
				}
				else
				{
					(*pDstPixelShort) |= (AValue>>u8ShiftBits)&dstFrmt.dwRGBAlphaBitMask;
				}
				*pRenderPixelShort = *pDstPixelShort;
			}
		}
	}
	else //Since this dbpp is 4bits, then the sbpp has to be 32 bits
	{
		U8*  pDstPixelShort=NULL;
		// Do a masked component copy -- ARGH!

		for(sY = uSrcY, dY = uDstY; dY < (pDstOutputInfo->m_height + uSrcY); sY++, dY++)
		{
			sY2 = sY*2;
			uCurRow= sY2*pSrcOutputInfo->m_pitch;
			uDstCurRowd = dY*pDstOutputInfo->m_pitch;
			uNxtRow= (sY2+1)*pSrcOutputInfo->m_pitch;
			if (pDstOutputInfo->m_height < pSrcOutputInfo->m_height &&
				pDstOutputInfo->m_width < pSrcOutputInfo->m_width)
			{
				for(sX = uSrcX, dX = uDstX; dX < (pDstOutputInfo->m_width+ uSrcX); sX++, dX++)
				{
					pDstPixelShort = (U8*)&(pRenderBuffer[uDstCurRowd+dX*dBpp]);

					pDstPixelShort[0]=(pSrc[uCurRow + sX*sBpp2]+ pSrc[uCurRow + (sX*2+1)*sBpp]+
						pSrc[uNxtRow + sX*sBpp2]+ pSrc[uNxtRow + (sX*2+1)*sBpp])>>2;
					pDstPixelShort[1]=(pSrc[uCurRow + sX*sBpp2+1]+pSrc[uCurRow + (sX*2+1)*sBpp+1]+
						pSrc[uNxtRow + sX*sBpp2+1]+pSrc[uNxtRow + (sX*2+1)*sBpp+1])>>2;
					pDstPixelShort[2]=(pSrc[uCurRow + sX*sBpp2+2]+pSrc[uCurRow + (sX*2+1)*sBpp+2]+
						pSrc[uNxtRow + sX*sBpp2+2]+pSrc[uNxtRow + (sX*2+1)*sBpp+2])>>2;
					pDstPixelShort[3]=(pSrc[uCurRow + sX*sBpp2+3]+pSrc[uCurRow + (sX*2+1)*sBpp+3]+
						pSrc[uNxtRow + sX*sBpp2+3]+pSrc[uNxtRow + (sX*2+1)*sBpp+3])>>2;

				}
			}
			else if(pDstOutputInfo->m_height < pSrcOutputInfo->m_height)
			{
				for(sX = uSrcX, dX = uDstX; dX < (pDstOutputInfo->m_width+ uSrcX); sX++, dX++)
				{
					pDstPixelShort = (U8*)&(pRenderBuffer[uDstCurRowd+dX*dBpp]);
					pDstPixelShort[0]=(pSrc[uCurRow + sX*sBpp2]+pSrc[uNxtRow + sX*sBpp2])>>1;
					pDstPixelShort[1]=(pSrc[uCurRow + sX*sBpp2+1]+pSrc[uNxtRow + sX*sBpp2+1])>>1;
					pDstPixelShort[2]=(pSrc[uCurRow + sX*sBpp2+2]+pSrc[uNxtRow + sX*sBpp2+2])>>1;
					pDstPixelShort[3]=(pSrc[uCurRow + sX*sBpp2+3]+pSrc[uNxtRow + sX*sBpp2+3])>>1;
				}
			}
			else if(pDstOutputInfo->m_width < pSrcOutputInfo->m_width)
			{
				for(sX = uSrcX, dX = uDstX; dX < (pDstOutputInfo->m_width+ uSrcX); sX++, dX++)
				{
					pDstPixelShort = (U8*)&(pRenderBuffer[uDstCurRowd+dX*dBpp]);

					pDstPixelShort[0]=(pSrc[uCurRow + sX*sBpp2]+pSrc[uCurRow + (sX*2+1)*sBpp])>>1;
					pDstPixelShort[1]=(pSrc[uCurRow + sX*sBpp2+1]+pSrc[uCurRow + (sX*2+1)*sBpp+1])>>1;
					pDstPixelShort[2]=(pSrc[uCurRow + sX*sBpp2+2]+pSrc[uCurRow + (sX*2+1)*sBpp+2])>>1;
					pDstPixelShort[3]=(pSrc[uCurRow + sX*sBpp2+3]+pSrc[uCurRow + (sX*2+1)*sBpp+3])>>1;
				}
			}
		}
	}
}



// perform a horizontal bilinear interpolation shrink of the data in pSrc
// into pDest using the Bresenham method...

void IFXTextureImageTools_BIHShrink(U8 m_u8PixelSize, BOOL m_bHasAlpha, U8 *pDest, int nDest, U8 *pSrc, int nSrc)
{
	
	// The main idea of this algorithm is that each source sample has
	// nDest portions to contibute to the destination, and that each
	// destination sample requires exactly nSrc portions.
	// This is true because the duration of each source sample is nDest/nSrc
	// the duration of the destination samples (think about it. I drew a picture).
	

	// number of portions that the destination pixel still needs...
	int needed;

	// accumulated weighted sum of the component samples...
	// int accum;
	U32 red, green, blue, alpha;

	// number of portions that the source pixel still has to contribute,,,
	int remaining = nDest;

	// roundoff increment...
	int round = nSrc >> 1;

	// pointer to place to stop...
	U8 *pEnd = pDest + (nDest*m_u8PixelSize);

	if (m_u8PixelSize > 1)
	{
		// loop over each pixel in the destination...

		while (pDest < pEnd) {
			// for each destination pixel the accum starts
			// at zero, and we need nSrc portions...
			// accum = round;
			red = green = blue = alpha = round;
			needed = nSrc;

			// as long as more portions remain than we need
			// the source samples contribute all the portions
			// that they have left...
			// this will continue to loop when remaining==0
			// if needed > 0 but this is okay because
			// adding zero portions of pSrc does no harm and
			// pSrc will be advanced to the next sample...
			while (needed > remaining) {
				// add a weighted amount to the accumulated sum...
				// accum += remaining * (int)(*pSrc);
				red += remaining * (pSrc[0]);
				green += remaining * (pSrc[1]);
				blue += remaining * (pSrc[2]);
				if (m_bHasAlpha)
					alpha += remaining * (pSrc[3]);

				needed -= remaining;
				pSrc += m_u8PixelSize;
				// advance to the next source sample...
				remaining = nDest;
			}

			// here we need only some of the portions left
			// of the current source sample...
			if (needed) {
				// accum += needed * (int)(*pSrc);
				red += needed * (pSrc[0]);
				green += needed * (pSrc[1]);
				blue += needed * (pSrc[2]);
				if (m_bHasAlpha)
					alpha += needed * (pSrc[3]);
				remaining -= needed;
			}

			// normalize the result and store it into
			// the destination sample...

			red /= nSrc;
			pDest[0] = red;
			green /= nSrc;
			blue /= nSrc;
			pDest[1] = green;
			pDest[2] = blue;
			if (m_bHasAlpha)
			{
				alpha /= nSrc;
				pDest[3] = alpha;
			}

			pDest += m_u8PixelSize;
		}
	}
	else
	{
		while (pDest < pEnd)
		{
			red = green = blue = alpha = round;
			needed = nSrc;
			while (needed > remaining) {
				// add a weighted amount to the accumulated sum...
				// accum += remaining * (int)(*pSrc);
				red += remaining * (pSrc[0]);
				needed -= remaining;
				pSrc += m_u8PixelSize;
				// advance to the next source sample...
				remaining = nDest;
			}

			// here we need only some of the portions left
			// of the current source sample...
			if (needed) {
				// accum += needed * (int)(*pSrc);
				red += needed * (pSrc[0]);
				remaining -= needed;
			}

			// normalize the result and store it into
			// the destination sample...

			red /= nSrc;
			pDest[0] = red;
			pDest += m_u8PixelSize;
		}
	}
}


// perform a vertical bilinear interpolation shrink of the data in pSrc
// into pDest using the Bresenham method...

void IFXTextureImageTools_BIVShrink(U8 m_u8PixelSize, BOOL m_bHasAlpha, U8 *pDest, int nDest, U8 *pSrc, int nSrc, int nSpan)
{
	
	// The main idea of this algorithm is that each source sample has
	// nDest portions to contibute to the destination, and that each
	// destination sample requires exactly nSrc portions.
	// This is true because the duration of each source sample is nDest/nSrc
	// the duration of the destination samples (think about it. I drew a picture).
	

	// number of portions that the destination pixel still needs...
	int needed;

	// accumulated weighted sum of the component samples...
	// int accum;
	U32 red, green, blue, alpha;

	// number of portions that the source pixel still has to contribute,,,
	int remaining = nDest;

	// roundoff increment...
	int round = nSrc >> 1;

	// pointer to place to stop...
	U8 *pEnd = pDest + nDest*nSpan;

	if (m_u8PixelSize > 1)
	{
		// loop over each pixel in the destination...
		while (pDest < pEnd) {
			// for each destination pixel the accum starts
			// at zero, and we need nSrc portions...
			// accum = round;
			red = green = blue = alpha = round;
			needed = nSrc;

			// as long as more portions remain than we need
			// the source samples contribute all the portions
			// that they have left...
			// this will continue to loop when remaining==0
			// if needed > 0 but this is okay because
			// adding zero portions of pSrc does no harm and
			// pSrc will be advanced to the next sample...
			while (needed > remaining) {
				// add a weighted amount to the accumulated sum...
				red += remaining * (pSrc[0]);
				green += remaining * (pSrc[1]);
				blue += remaining * (pSrc[2]);
				if (m_bHasAlpha)
					alpha += remaining * (pSrc[3]);

				needed -= remaining;

				// advance to the next source sample...
				pSrc += nSpan;
				remaining = nDest;
			}

			// here we need only some of the portions left
			// of the current source sample...
			if (needed) {
				// accum += needed * (int)(*pSrc);
				red += needed * (pSrc[0]);
				green += needed * (pSrc[1]);
				blue += needed * (pSrc[2]);
				if (m_bHasAlpha)
					alpha += needed * (pSrc[3]);
				remaining -= needed;
			}

			// normalize the result and store it into
			// the destination sample...

			red /= nSrc;
			green /= nSrc;
			blue /= nSrc;
			pDest[0] = red;
			pDest[1] = green;
			pDest[2] = blue;
			if (m_bHasAlpha)
			{
				alpha /= nSrc;
				pDest[3] = alpha;
			}

			pDest += nSpan;
		}
	}
	else
	{
		// loop over each pixel in the destination...
		while (pDest < pEnd)
		{
			red = green = blue = alpha = round;
			needed = nSrc;
			while (needed > remaining) {
				// add a weighted amount to the accumulated sum...
				red += remaining * (pSrc[0]);
				needed -= remaining;

				// advance to the next source sample...
				pSrc += nSpan;
				remaining = nDest;
			}

			// here we need only some of the portions left
			// of the current source sample...
			if (needed) {
				// accum += needed * (int)(*pSrc);
				red += needed * (pSrc[0]);
			}

			// normalize the result and store it into
			// the destination sample...

			red /= nSrc;
			pDest[0] = red;

			pDest += nSpan;
		}
	}
}


// perform a horizontal bilinear interpolation stretch of the data in pSrc
// into pDest using the Bresenham method...

void IFXTextureImageTools_BIHStretch(U8 m_u8PixelSize, BOOL m_bHasAlpha, U8 *pDest, int nDest, U8 *pSrc, int nSrc)
{
	// compute number of intervals in the source and destination...
	int srcIntervals = nSrc - 1;
	int destIntervals = nDest - 1;

	// roundoff increment...
	int round = destIntervals >> 1;

	// first destination pixel is equal to the first source pixel...
	pDest[0] = pSrc[0];
	pDest[1] = pSrc[1];
	pDest[2] = pSrc[2];
	if (m_bHasAlpha)
		pDest[3] = pSrc[3];
	//Advance to next pixel
	pDest += m_u8PixelSize;

	// track how far have we moved towards the next pixel...
	int currentIntervals = srcIntervals;

	// pointer to the place to stop...
	U8 *pEnd = pDest + (nDest - 2)*m_u8PixelSize;

	U32 red,green,blue,alpha=0;
	// loop over each pixel in the destination...
	while (pDest < pEnd) {
		// linear interpolate between current pixel and the next...
		red = (*(pSrc+m_u8PixelSize)*currentIntervals+(*pSrc)*(destIntervals - currentIntervals) + round) / destIntervals;
		green = (*(pSrc+m_u8PixelSize+1)*currentIntervals+(*(pSrc+1))*(destIntervals - currentIntervals) + round) / destIntervals;
		blue = (*(pSrc+m_u8PixelSize+2)*currentIntervals+(*(pSrc+2))*(destIntervals - currentIntervals) + round) / destIntervals;
		if (m_bHasAlpha)
			alpha = (*(pSrc+m_u8PixelSize+3)* currentIntervals+(*(pSrc+3))*(destIntervals - currentIntervals) + round) / destIntervals;
		pDest[0] = red;
		if (m_u8PixelSize > 1)
		{
			pDest[1] = green;
			pDest[2] = blue;
			if (m_bHasAlpha)
				pDest[3] = alpha;
		}
		pDest += m_u8PixelSize;
		currentIntervals += srcIntervals;
		if (currentIntervals >= destIntervals) {
			// advance to next source pixel...
			pSrc += m_u8PixelSize;
			currentIntervals -= destIntervals;
		}
	}

	// last destination pixel is equal to the last source pixel...
	pDest[0] = pSrc[0];
	pDest[1] = pSrc[1];
	pDest[2] = pSrc[2];
	
	if (m_bHasAlpha)
		pDest[3] = pSrc[3];
}


// perform a vertical bilinear interpolation stretch of the data in pSrc
// into pDest using the Bresenham method...

void IFXTextureImageTools_BIVStretch(U8 m_u8PixelSize, BOOL m_bHasAlpha, U8 *pDest, int nDest, U8 *pSrc, int nSrc, int nSpan)
{
	// compute number of intervals in the source and destination...
	int srcIntervals = nSrc -1;
	int destIntervals = nDest-1;

	// roundoff increment...
	int round = destIntervals >> 1;

	// first destination pixel is equal to the first source pixel...
	pDest[0] = pSrc[0];
	pDest[1] = pSrc[1];
	pDest[2] = pSrc[2];
	if (m_bHasAlpha)
		pDest[3] = pSrc[3];
	// track how far have we moved towards the next pixel...
	int currentIntervals = srcIntervals;

	// pointer to the place to stop...
	U8 *pEnd = pDest + ((nDest - 2)*nSpan);

	U32 red,green,blue,alpha;
	// loop over each pixel in the destination...
	while (pDest < pEnd) {
		// linear interpolate between current pixel and the next...
		red = (pSrc[nSpan]*currentIntervals+pSrc[0]*(destIntervals - currentIntervals) + round) / destIntervals;
		pDest[0] = red;
		if (m_u8PixelSize > 1)
		{
			green = (pSrc[nSpan+1]*currentIntervals+pSrc[1]*(destIntervals - currentIntervals) + round) / destIntervals;
			blue = (pSrc[nSpan+2]*currentIntervals +(pSrc[2])*(destIntervals - currentIntervals) + round) / destIntervals;
			pDest[1] = green;
			pDest[2] = blue;
			if (m_bHasAlpha)
			{
				alpha = (pSrc[nSpan+3]*currentIntervals+pSrc[3]*(destIntervals - currentIntervals) + round) / destIntervals;
				pDest[3] = alpha;
			}
		}
		pDest += nSpan;
		currentIntervals += srcIntervals;
		if (currentIntervals >= destIntervals) {
			// advance to next source pixel...
			pSrc += nSpan;
			currentIntervals -= destIntervals;
		}
	}

	// last destination pixel is equal to the last source pixel...
	pDest[0] = pSrc[0];
	pDest[1] = pSrc[1];
	pDest[2] = pSrc[2];

	if (m_bHasAlpha)
		pDest[3] = pSrc[3];
}

IFXRESULT IFXAPI_CALLTYPE IFXTextureImageTools_ResizeImage(U8* pSrc, U8* pDest, U8 m_u8PixelSize, BOOL m_bHasAlpha, U32 uSrcWidth, U32 uSrcHeight, U32 uDstWidth, U32 uDstHeight)
{
	int src_width  = uSrcWidth;
	int src_height = uSrcHeight;
	int dest_width = uDstWidth;
	int dest_height= uDstHeight;
	int rows, cols;

	U8* pTempBuffer = NULL;
	if (dest_width > src_width) {
		// vertical first then horizontal...
		// allocate channel temporary space...
		pTempBuffer = (U8*)IFXAllocate(dest_height * src_width*m_u8PixelSize);
		if (!pTempBuffer)
			return IFX_E_OUT_OF_MEMORY;

		// bilinear interpolate columns...
		U8 *pTemp = pTempBuffer;
		if (dest_height > src_height) {
			for (cols = 0; cols < src_width; cols++) {
				IFXTextureImageTools_BIVStretch(m_u8PixelSize, m_bHasAlpha, pTemp, dest_height, pSrc, src_height, src_width*m_u8PixelSize);
				pSrc += m_u8PixelSize;
				pTemp += m_u8PixelSize;
			}
		}
		else  {
			for (cols = 0; cols < src_width; cols++) {
				IFXTextureImageTools_BIVShrink(m_u8PixelSize, m_bHasAlpha, pTemp, dest_height, pSrc, src_height, src_width*m_u8PixelSize);
				pSrc += m_u8PixelSize;
				pTemp += m_u8PixelSize;
			}
		}

		// bilinear interpolate rows...
		pTemp = pTempBuffer;
		for (rows = 0; rows < dest_height; rows++) {
			IFXTextureImageTools_BIHStretch(m_u8PixelSize, m_bHasAlpha, pDest, dest_width, pTemp, src_width);
			pTemp += (src_width*m_u8PixelSize);
			pDest += (dest_width*m_u8PixelSize);
		}
	}
	else
	{
		// horizontal first then vertical...
		// allocate channel temporary space...
		pTempBuffer = (U8*)IFXAllocate(src_height * dest_width*m_u8PixelSize);
		if (!pTempBuffer)
			return IFX_E_OUT_OF_MEMORY;

		// bilinear interpolate rows...
		U8 *pTemp = pTempBuffer;

		for (rows = 0; rows < src_height; rows++) {
			IFXTextureImageTools_BIHShrink(m_u8PixelSize, m_bHasAlpha, pTemp, dest_width, pSrc, src_width);
			pSrc += (src_width*m_u8PixelSize);
			pTemp += (dest_width*m_u8PixelSize);
		}

		// bilinear interpolate columns...
		pTemp = pTempBuffer;
		if (dest_height >= src_height) {
			for (cols = 0; cols < dest_width; cols++) {
				IFXTextureImageTools_BIVStretch(m_u8PixelSize, m_bHasAlpha, pDest, dest_height, pTemp, src_height, dest_width*m_u8PixelSize);
				pDest += m_u8PixelSize;
				pTemp += m_u8PixelSize;
			}
		}
		else {
			for (cols = 0; cols < dest_width; cols++) {
				IFXTextureImageTools_BIVShrink(m_u8PixelSize, m_bHasAlpha, pDest, dest_height, pTemp, src_height, dest_width*m_u8PixelSize);
				pDest += m_u8PixelSize;
				pTemp += m_u8PixelSize;
			}
		}
	}
	IFXDeallocate(pTempBuffer);
	return IFX_OK;
}
