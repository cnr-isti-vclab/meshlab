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
//	IFXAAFilter.cpp
//
//	DESCRIPTION:
//		Defines a utility function for filtering supersampled images
//
//	NOTES
//		This filter is basically a 2x2 box filter that handles both 16 and
//		32 bit images.
//
//***************************************************************************

#include "IFXRenderPCH.h"

#define PIXEL_SIZE_32BIT	4
#define PIXEL_SIZE_16BIT	2

IFXRESULT IFXAAFilterImage(	U8* pSrc, U8* pDst, 
							U32 srcPitch, U32 dstPitch, 
							U32 dstWidth, U32 dstHeight, 
							IFXenum eBufferFormat)
{
	IFXRESULT rc = IFX_OK;

	if((!pSrc) || (!pDst))
	{
		rc = IFX_E_INVALID_POINTER;
	}

	BOOL b32Bit = FALSE;
	switch(eBufferFormat)
	{
	case IFX_RGBA_8888:
	case IFX_RGBA_8880:
		b32Bit = TRUE;
		break;
	case IFX_RGBA_5650:
	case IFX_RGBA_5551:
	case IFX_RGBA_5550:
	case IFX_RGBA_4444:
		b32Bit = FALSE;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		U32 ySrcOffset = 0;
		U32 xSrcOffset = 0;
		U32 yDstOffset = 0;
		U32 xDstOffset = 0;

		U32 uValue = 0;

		if(b32Bit)
		{
			U8* pcPixel = 0;
			
			U32 y;
			for( y = 0; y < dstHeight; y++)
			{
				ySrcOffset = y * srcPitch * 2;
				yDstOffset = y * dstPitch;

				U32 x;
				for( x = 0; x < dstWidth; x++)
				{
					xSrcOffset = x * PIXEL_SIZE_32BIT * 2;
					xDstOffset = x * PIXEL_SIZE_32BIT;
					pcPixel = &pSrc[ySrcOffset + xSrcOffset];

					// Red Channel
					uValue = (U32)*pcPixel;
					uValue += (U32)pcPixel[PIXEL_SIZE_32BIT];
					uValue += (U32)pcPixel[srcPitch];
					uValue += (U32)pcPixel[srcPitch+PIXEL_SIZE_32BIT];

					pDst[yDstOffset+xDstOffset] = (U8)((uValue>>2) & 0xFF);

					pcPixel++;

					// Green Channel
					uValue = (U32)*pcPixel;
					uValue += (U32)pcPixel[PIXEL_SIZE_32BIT];
					uValue += (U32)pcPixel[srcPitch];
					uValue += (U32)pcPixel[srcPitch+PIXEL_SIZE_32BIT];

					pDst[yDstOffset+xDstOffset+1] = (U8)((uValue>>2) & 0xFF);

					pcPixel++;

					// Blue Channel
					uValue = (U32)*pcPixel;
					uValue += (U32)pcPixel[PIXEL_SIZE_32BIT];
					uValue += (U32)pcPixel[srcPitch];
					uValue += (U32)pcPixel[srcPitch+PIXEL_SIZE_32BIT];

					pDst[yDstOffset+xDstOffset+2] = (U8)((uValue>>2) & 0xFF);

					//pcPixel++;

					// Alpha Channel - This will never happen for the frame buffer!
					/*
					uValue = (U32)*pcPixel;
					uValue += (U32)pcPixel[PIXEL_SIZE_32BIT];
					uValue += (U32)pcPixel[srcPitch];
					uValue += (U32)pcPixel[srcPitch+PIXEL_SIZE_32BIT];

					pDst[yDstOffset+xDstOffset] = (U8)((uValue>>2) & 0xFF);
					*/
				} // for x
			} // for y
		}
		else // 16 bit image
		{
			U16* psPixel = 0;
			U16 sPixel = 0;
			U32 uRed = 0;
			U32 uGreen = 0;
			U32 uBlue = 0;

			U32 uGreenMask = 0x7E0;
			U32 uBlueMask = 0xF800;
			//U32 uBlueShift = 11;

			// Frame buffer images are either 555 or 565.  Need to check!
			// If the buffer format is 555, then we need to change the green
			// mask and blue mask.
			if(eBufferFormat != IFX_RGBA_5650)
			{
				uGreenMask = 0x3E0;
				uBlueMask = 0x7C00;
			}

			U32 y;
			for( y = 0; y < dstHeight; y++)
			{
				ySrcOffset = y * srcPitch * 2;
				yDstOffset = y * dstPitch;

				U32 x;
				for( x = 0; x < dstWidth; x++)
				{
					xSrcOffset = x * PIXEL_SIZE_16BIT * 2;
					xDstOffset = x * PIXEL_SIZE_16BIT;

					psPixel = (U16*)(&pSrc[ySrcOffset + xSrcOffset]);
					
					sPixel = *psPixel;

					uRed = sPixel & 0x1F;
					uGreen = sPixel & uGreenMask;
					uBlue = sPixel & uBlueMask;

					sPixel = psPixel[1];

					uRed += sPixel & 0x1F;
					uGreen += sPixel & uGreenMask;
					uBlue += sPixel & uBlueMask;

					psPixel = (U16*)(&pSrc[ySrcOffset+srcPitch+xSrcOffset]);
					sPixel = *psPixel;

					uRed += sPixel & 0x1F;
					uGreen += sPixel & uGreenMask;
					uBlue += sPixel & uBlueMask;

					sPixel = psPixel[1];

					uRed += sPixel & 0x1F;
					uGreen += sPixel & uGreenMask;
					uBlue += sPixel & uBlueMask;

					psPixel = (U16*)(&pDst[yDstOffset+xDstOffset]);

					uRed >>= 2;
					uBlue >>= 2;
					uGreen >>= 2;

					sPixel = (uRed & 0x1F) | (uGreen & uGreenMask) | (uBlue & uBlueMask);

					*psPixel = sPixel;
				} // for x
			} // for y
		} // if 32 bit
	} // if ifxsuccess

	return rc;
}

