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
// CIFXRenderOGL.h
#ifndef CIFX_RENDEROGL_H
#define CIFX_RENDEROGL_H

#include "CIFXRender.h"
#include "CIFXDeviceLightOGL.h"
#include "CIFXDeviceTextureOGL.h"
#include "CIFXDeviceTexUnitOGL.h"

#define OGL_NAME  "OpenGL"

class IFXOpenGLOS;

class CIFXRenderOGL : public CIFXRender
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderOGLFactory(IFXREFIID intId, void** ppUnk);

	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	// We need to handle enable and disable of VSync slightly differently
	// than the other systems.
	IFXRESULT IFXAPI Enable(IFXenum eParam);
	IFXRESULT IFXAPI Disable(IFXenum eParam);

	IFXRESULT IFXAPI SetDepthMode(IFXenum eDepthMode);
	
	IFXRESULT IFXAPI GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals) {return IFX_OK;}
	
	IFXRESULT IFXAPI DeleteTexture(U32 uTexId);
	IFXRESULT IFXAPI GetTextureState(U32 uTexId, U32& uState);

	IFXRESULT IFXAPI Swap();

protected:
	CIFXRenderOGL();
	virtual ~CIFXRenderOGL();
	
	IFXRESULT IFXAPI CreateTexture(CIFXDeviceTexturePtr& rTexture);

	IFXRESULT IFXAPI Construct();

	void IFXAPI InitData();

	// Window Settings
	IFXRESULT IFXAPI SetHWDTS(BOOL bDTS) { return SetHWDTSOS(bDTS); }	
	IFXRESULT IFXAPI SetHWWindow(BOOL bDirty);
	IFXRESULT IFXAPI ClearHW(const IFXRenderClear& rClear) { return ClearOS(rClear); }
	IFXRESULT IFXAPI MakeHWCurrent() { return MakeCurrentOS(); }

	// OS Specific Implementations
	IFXRESULT IFXAPI SetHWDTSOS(BOOL bDTS);
	IFXRESULT IFXAPI InitializeOS();
	IFXRESULT IFXAPI ClearOS(const IFXRenderClear& rClear);
	IFXRESULT IFXAPI SwapOS();
	IFXRESULT IFXAPI SetHWWindowOS(BOOL bDirty);
	IFXRESULT IFXAPI DestroyOS();
	IFXRESULT IFXAPI MakeCurrentOS();
	IFXRESULT IFXAPI SetVSyncEnabledOS(BOOL bEnabled);

	IFXRESULT IFXAPI ResizeBufferOS();

	// Window Data
	IFXOGLData m_glData;
	U8*		m_pTempBuffer;
	U32		m_uTempBufferSize;
	BOOL	m_bNeedClear;

	CIFXDeviceLightOGLPtr		m_pspLightsOGL[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitOGLPtr		m_pspTexUnitsOGL[IFX_MAX_TEXUNITS];
	CIFXRenderDeviceOGLPtr		m_spDeviceOGL;
	
	IFXOpenGL* m_pOGL;
	IFXOpenGLOS* m_pOSOGL;
};

typedef IFXSmartPtr<CIFXRenderOGL> CIFXRenderOGLPtr;

#endif // CIFX_RENDEROGL_H
