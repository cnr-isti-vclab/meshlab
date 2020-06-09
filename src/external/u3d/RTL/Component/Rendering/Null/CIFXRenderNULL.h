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
// CIFXRenderNULL.h
#ifndef CIFX_RENDERNULL_H
#define CIFX_RENDERNULL_H

#include "CIFXRender.h"
#include "CIFXDeviceLightNULL.h"
#include "CIFXDeviceTextureNULL.h"
#include "CIFXDeviceTexUnitNULL.h"
#include "CIFXRenderDeviceNULL.h"

#define NULL_NAME  "DirectX8"

class CIFXRenderNULL : public CIFXRender
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderNULLFactory(IFXREFIID intId, void** ppUnk);

	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	IFXRESULT IFXAPI GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals) {return IFX_OK;}
		
	IFXRESULT IFXAPI Swap();

protected:
	CIFXRenderNULL();
	virtual ~CIFXRenderNULL();

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	IFXRESULT IFXAPI CreateTexture(CIFXDeviceTexturePtr& rTexture);

	// Window Settings
	IFXRESULT IFXAPI SetHWAntiAliasingMode(IFXenum eAAMode);
	IFXRESULT IFXAPI SetHWDepthStencilFormat(IFXenum eDepthStencil);
	IFXRESULT IFXAPI MakeHWCurrent();
	IFXRESULT IFXAPI ClearHW(const IFXRenderClear& rClear);
	IFXRESULT IFXAPI SetVSyncEnabled(BOOL bVSyncEnabled);
	IFXRESULT IFXAPI SetHWWindow(BOOL bDirty);

	// Window Data
	BOOL					m_bNeedClear;
	
	CIFXDeviceLightNULLPtr		m_pspLightsNULL[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitNULLPtr	m_pspTexUnitsNULL[IFX_MAX_TEXUNITS];
	CIFXRenderDeviceNULLPtr		m_spDeviceNULL;
};

typedef IFXSmartPtr<CIFXRenderNULL> CIFXRenderNULLPtr;


#endif // CIFX_RENDERNULL_H
