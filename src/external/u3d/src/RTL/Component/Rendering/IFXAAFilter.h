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
//	IFXAAFilter.h
//
//	DESCRIPTION:
//		Declares a utility function for filtering supersampled images
//
//	NOTES
//		This filter is basically a 2x2 box filter that handles both 16 and
//		32 bit images.
//
//***************************************************************************

#ifndef IFXAAFILTER_H
#define IFXAAFILTER_H

IFXRESULT IFXAAFilterImage(	U8* pSrc, U8* pDst, 
							U32 srcPitch, U32 dstPitch, 
							U32 dstWidth, U32 dstHeight, 
							IFXenum eBufferFormat);

#define IFX_AA_SS_FACTOR	2

#endif // IFXAAFILTER_H

// END OF FILE

