//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// CIFXDeviceTextureOGL.h
#ifndef CIFX_Device_TEXTUREOGL_H
#define CIFX_Device_TEXTUREOGL_H

#include "CIFXDeviceTexture.h"
#include "CIFXRenderDeviceOGL.h"

//==============================
// CIFXDeviceTextureOGL
//==============================
class CIFXDeviceTextureOGL : public CIFXDeviceTexture
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTextureOGLFactory(IFXREFIID intId, void** ppUnk);

	//====================================
	// CIFXDeviceTextureOGL methods
	//====================================
	IFXRESULT IFXAPI Initialize(IFXOpenGL* pOGL, CIFXRenderDeviceOGL& rDevice);

	static void IFXAPI DeleteSurfaceCache();

	GLuint Id() { return m_uTexId; }

	IFXenum IFXAPI GetType() const { return m_eType; }
	
protected:
	CIFXDeviceTextureOGL();
	virtual ~CIFXDeviceTextureOGL();
	
	virtual IFXRESULT IFXAPI SetHWType(IFXenum eType);
	virtual IFXRESULT IFXAPI SetHWRenderFormat(IFXenum eFormat);
	virtual IFXRESULT IFXAPI SetHWWidth(U32 uWidth);
	virtual IFXRESULT IFXAPI SetHWHeight(U32 uHeight);
	virtual IFXRESULT IFXAPI SetHWDepth(U32 uDepth);
	virtual IFXRESULT IFXAPI SetHWMaxMipLevel(U32 uMipLevel);
	virtual IFXRESULT IFXAPI SetHWMinFilter(IFXenum eFilter);
	virtual IFXRESULT IFXAPI SetHWMagFilter(IFXenum eFilter);
	virtual IFXRESULT IFXAPI SetHWMipMode(IFXenum eMode);
	virtual IFXRESULT IFXAPI SetHWDynamic(BOOL bDyanmic);

	virtual IFXRESULT IFXAPI SetHWTexture();

	virtual IFXRESULT IFXAPI Lock(	IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	);

	virtual IFXRESULT IFXAPI Unlock(IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	);

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	GLenum IFXAPI GetGLTextureType(IFXenum eType, GLenum* glBinding = 0);
	GLenum m_eGLType;
	GLenum m_eGLBindingType;

	GLuint	m_uTexId;

	static U8*	ms_pSurface;
	static U32  ms_uSurfaceSize;
	
	IFXOpenGL* m_pOGL;

	CIFXRenderDeviceOGLPtr m_spDevice;
};
typedef IFXSmartPtr<CIFXDeviceTextureOGL> CIFXDeviceTextureOGLPtr;

#endif // CIFX_Device_TEXTUREOGL_H

// END OF FILE

