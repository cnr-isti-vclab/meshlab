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

// CIFXRender.h
// This is an abstract implementation class common to all IFXRender implementations
#ifndef CIFX_RENDER_H
#define CIFX_RENDER_H

#include "CIFXDeviceLight.h"
#include "CIFXDeviceTexture.h"
#include "CIFXDeviceTexUnit.h"
#include "CIFXRenderDevice.h"
#include "CIFXRenderServices.h"
#include "IFXRenderWindow.h"

class CIFXRender : virtual public IFXRender, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	//=========================
	// IFXRender Methods
	//=========================
	virtual IFXRESULT IFXAPI Initialize(U32 uMonitorNum);
	
	// This is an IFXRenderContext method, we do not need to handle it.
	virtual IFXRESULT IFXAPI Initialize() { return IFX_E_UNSUPPORTED; }
	virtual U32		  IFXAPI CalculateNumPasses() { return 0; }
	virtual IFXRESULT IFXAPI ConfigureRenderPass(U32 uRenderPass) { return IFX_OK; }
	virtual IFXRESULT IFXAPI SetRenderer(U32 uRenderId) { return IFX_OK; }
	virtual U32		  IFXAPI GetRenderer() { return 0; }
	virtual const IFXRenderView& IFXAPI GetView() const { IFXRenderView* pView = 0; return *pView; }

	virtual IFXRESULT IFXAPI Clear(const IFXRenderClear& rClear);
	
	virtual IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps);

	virtual IFXRESULT IFXAPI GetPerformanceData(IFXenum eData, U32& ruData);
	
	virtual IFXRESULT IFXAPI ClearPerformanceData();

	virtual IFXRESULT IFXAPI MakeCurrent();

	virtual IFXRESULT IFXAPI GetRenderServices(IFXRenderServices*& pServices);

	virtual IFXenum   IFXAPI GetColorFormat();

	virtual IFXRESULT IFXAPI Enable(IFXenum eparam);
	
	virtual IFXRESULT IFXAPI Disable(IFXenum eParam);
	
	virtual IFXRESULT IFXAPI GetEnabled(IFXenum eParam, BOOL& bEnabled);

	virtual IFXRESULT IFXAPI DrawMeshLines(IFXMesh& rMesh);

	virtual IFXRESULT IFXAPI DrawMeshPoints(IFXMesh& rMesh);

	virtual IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh);

	virtual IFXRESULT IFXAPI SetBlend(IFXRenderBlend& rBlend);

	virtual IFXRESULT IFXAPI SetFog(IFXRenderFog& rFog);

	virtual IFXRESULT IFXAPI SetLight(IFXenum eLight, IFXRenderLight& rLight);

	virtual IFXRESULT IFXAPI SetGlobalAmbient(const IFXVector4& vColor);
	
	virtual IFXRESULT IFXAPI GetGlobalAmbient(IFXVector4& vColor);

	virtual IFXRESULT IFXAPI SetMaterial(IFXRenderMaterial& rMat);
	
	virtual IFXRESULT IFXAPI SetStencil(IFXRenderStencil& rStencil);

	virtual IFXRESULT IFXAPI SetTexture(IFXTextureObject& rTexture);

	virtual IFXRESULT IFXAPI SetTextureUnit(IFXenum eTexUnit, IFXRenderTexUnit& rTexUnit);

	virtual IFXRESULT IFXAPI SetConstantColor(const IFXVector4& vColor);

	virtual IFXRESULT IFXAPI SetView(IFXRenderView& rView);

	virtual IFXRESULT IFXAPI SetWindow(IFXRenderWindow& rWindow);
	virtual IFXRESULT IFXAPI GetWindow(IFXRenderWindow& rWindow);

	virtual IFXRESULT IFXAPI GetNumDevices(IFXRenderWindow& rWindow, U32& uNumDevices) { return IFX_OK; }
	virtual IFXRESULT IFXAPI SetDevice(U32 uDevice) { return IFX_OK; }

	virtual IFXRESULT IFXAPI SetViewMatrix(const IFXMatrix4x4& mViewMatrix);

	virtual const IFXMatrix4x4& IFXAPI GetViewMatrix();

	virtual IFXRESULT IFXAPI SetWorldMatrix(const IFXMatrix4x4& mViewMatrix);

	virtual IFXRESULT IFXAPI SetCullMode(IFXenum eCullMode);

	virtual IFXRESULT IFXAPI SetLineOffset(U32 uOffset);

	virtual void IFXAPI Reset() {}

	virtual IFXRESULT IFXAPI SetDepthMode(IFXenum eDepthMode);

	virtual IFXRESULT IFXAPI DeleteTexture(U32 uTexId);

	virtual IFXRESULT IFXAPI GetTransformMatrix(IFXMatrix4x4& rMatrix);

protected:
	CIFXRender() ;	
	virtual ~CIFXRender() ;

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	virtual IFXRESULT IFXAPI CreateTexture(CIFXDeviceTexturePtr& rTexture)=0;
	virtual IFXRESULT IFXAPI SetVSyncEnabled(BOOL bVSyncEnabled);

	// IFXWindow operations
	virtual IFXRESULT IFXAPI SetHWAntiAliasingMode(IFXenum eAAMode);
	virtual IFXRESULT IFXAPI SetHWDTS(BOOL bDTS);
	virtual IFXRESULT IFXAPI SetHWDepthStencilFormat(IFXenum eDepthStencil);
	virtual IFXRESULT IFXAPI SetHWTransparent(BOOL bTransparent);
	virtual IFXRESULT IFXAPI SetHWWindowSize(const IFXRect& rcWindow);
	virtual IFXRESULT IFXAPI SetHWDeviceNum(U32 uDeviceNum);
	virtual IFXRESULT IFXAPI SetHWWindowPtr(void* pvWindow);
	virtual IFXRESULT IFXAPI ClearHW(const IFXRenderClear& rClear)=0;
	virtual IFXRESULT IFXAPI MakeHWCurrent()=0;
	virtual IFXRESULT IFXAPI SetHWWindow(BOOL bDirty)=0;
	void IFXAPI CalcVisibleWindow();
	IFXRESULT IFXAPI SizeBackBuffer(const IFXRect& rcMonitor);
	IFXRESULT IFXAPI CopyImageData(	U8* pSrc, U8* pDst, U32 uSrcPitch, U32 uDstPitch, 
									U32 uWidth, U32 uHeight, U32 uBpp, BOOL bFlip);

	IFXenum IFXAPI GetCompatibleAAMode(IFXenum eAAMode, IFXenum eAAEnabled);
	IFXenum IFXAPI GetCompatibleDepthMode(IFXenum eDepthMode);

	// IFXWindow Data
	IFXRenderWindow m_Window;
	IFXenum			m_eAAMode;
	IFXenum			m_eDepthStencil;
	IFXRect			m_rcVisibleWindow;
	IFXRect			m_rcBackBuffer;
	IFXRect			m_rcDevice;
	
	IFXRESULT m_iInitialized;

	CIFXRenderDevicePtr		m_spDevice;
	
	IFXSimpleHashPtr		m_spTextures;
	CIFXDeviceLightPtr		m_pspLights[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitPtr	m_pspTexUnits[IFX_MAX_TEXUNITS];

	CIFXRenderServices* m_pServices;

	U32	m_uNumHWTexUnits;
	
	U32 m_uDeviceNum;

	IFXGUID m_idRenderDevice;

	BOOL m_bVSyncEnabled;

	IFXenum m_eColorFormat;

	IFXRenderCaps m_RCaps;

	// Performance data
	U32 m_uNumFacesRendered;
	U32 m_uNumVerticesRendered;
	U32 m_uNumMeshesRendered;
	U32 m_uNumRenderCalls;
};

IFXINLINE IFXRESULT CIFXRender::GetRenderServices(IFXRenderServices*& pServices)
{
	pServices = m_pServices;

	return IFX_OK;
}

#define IFX_AA_SS_FACTOR	2

#endif
