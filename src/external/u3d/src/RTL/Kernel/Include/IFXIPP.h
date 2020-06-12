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
	@file	IFXOSIPP.h

			This module defines an IPP integration layer.
*/

#ifndef IFXIPP_h
#define IFXIPP_h

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXMemory.h"
#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXEnums.h"

//***************************************************************************
//  Classes, structures and types
//***************************************************************************

typedef void (*pFunction)();

// Memory functions
#ifdef IPP_MEMORY

extern IFXAllocateFunction*	  gs_pAllocateFunction;
extern IFXDeallocateFunction* gs_pDeallocateFunction;
extern IFXReallocateFunction* gs_pReallocateFunction;

#endif

// Matrix functions

typedef void (*pIFXMatrix4x4_Multiply)(F32* m_data, const F32* m1, const F32* m2);
typedef void (*pIFXMatrix4x4_Multiply3x4)(F32* m_data, const F32* m1, const F32* m2);
typedef void (*pIFXMatrix4x4_MakeIdentity)(F32* m_data);
typedef void (*pIFXMatrix4x4_Scale)(F32* m_data, const F32* v);
typedef void (*pIFXMatrix4x4_Transpose)(F32* m_data);
typedef IFXRESULT (*pIFXMatrix4x4_Invert3x4)(F32* m_data, const F32* m);
typedef IFXRESULT (*pIFXMatrix4x4_Invert)(F32* m_data, const F32* m);
typedef F32 (*pIFXMatrix4x4_CalcDeterminant3x3)(const F32* m_data);
typedef F32 (*pIFXMatrix4x4_CalcDeterminant)(const F32* m_data);

extern pIFXMatrix4x4_Multiply _IFXMatrix4x4_Multiply;
extern pIFXMatrix4x4_Multiply3x4 _IFXMatrix4x4_Multiply3x4;
extern pIFXMatrix4x4_MakeIdentity _IFXMatrix4x4_MakeIdentity;
extern pIFXMatrix4x4_Scale _IFXMatrix4x4_Scale;
extern pIFXMatrix4x4_Transpose _IFXMatrix4x4_Transpose;
extern pIFXMatrix4x4_CalcDeterminant3x3 _IFXMatrix4x4_CalcDeterminant3x3;
extern pIFXMatrix4x4_CalcDeterminant _IFXMatrix4x4_CalcDeterminant;
extern pIFXMatrix4x4_Invert3x4 _IFXMatrix4x4_Invert3x4;
extern pIFXMatrix4x4_Invert _IFXMatrix4x4_Invert;

// Image processing functions
struct STextureOutputInfo;

typedef IFXRESULT (IFXAPI_CALLTYPE *pIFXTextureImageTools_ResizeImage)(U8* pSrc, U8* pDst, U8 uPixelSize, BOOL bHasAlpha, U32 uSrcWidth, U32 uSrcHeight, U32 uDstWidth, U32 uDstHeight);
typedef IFXRESULT (IFXAPI_CALLTYPE *pIFXTextureImageTools_ReformatImage)(U8* pSrc, U8* pDst, U8 uPixelSize, IFXenum eSrcFormat, IFXenum eSrcOrder, IFXenum eDstFormat, IFXenum eDstOrder, U32 uSrcX, U32 uSrcY, U32 uSrcPitch, U32 uDstX, U32 uDstY, U32 uDstPitch, U32 uWidth, U32 uHeight);
typedef void (IFXAPI_CALLTYPE *pIFXTextureImageTools_MipMap)(U8* pSrc, U8* pDst, U8 uPixelSize, BOOL bHasAlpha, STextureOutputInfo* pSrcOutputInfo, STextureOutputInfo* pDstOutputInfo, U32 uSrcX, U32 uSrcY, U32 uDstX, U32 uDstY);

extern pIFXTextureImageTools_ResizeImage _IFXTextureImageTools_ResizeImage;
extern pIFXTextureImageTools_ReformatImage _IFXTextureImageTools_ReformatImage;
extern pIFXTextureImageTools_MipMap _IFXTextureImageTools_MipMap;

//***************************************************************************
//  Global function prototypes
//***************************************************************************

void IFXIPPInitialize();

void IFXIPPUninitialize();

#endif
