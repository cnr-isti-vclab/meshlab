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
// CIFXRenderDeviceDX7.h
#ifndef CIFX_RENDER_DEVICE_DX7_H
#define CIFX_RENDER_DEVICE_DX7_H

#include "CIFXRenderDevice.h"
#include "CIFXDeviceLightDX7.h"
#include "CIFXDeviceTextureDX7.h"
#include "CIFXDeviceTexUnitDX7.h"

#ifdef _DEBUG

#define SetRS(rsType, rs) SetRenderStateDebug(__FILE__,__LINE__,rsType,rs)
#define SetTSS(uSn, tsType, ts) SetTexStageStateDebug(__FILE__,__LINE__,uSn, tsType,ts)

#else

#define SetRS(rsType, rs) SetRenderState(rsType,rs)
#define SetTSS(uSn, tsType, ts) SetTexStageState(uSn, tsType,ts)

#endif // DEBUG

#define IFX_DX7_NUM_VBS	1

//================================
// Helper data types
//================================
struct IFXDX7VB
{
	LPDIRECT3DVERTEXBUFFER7	m_pVB[IFX_DX7_NUM_VBS];
	U32						m_uVertsUsed[IFX_DX7_NUM_VBS];
	U32						m_uCurrentVB;

	IFXDX7VB()
	{
		for(U32 i = 0; i < IFX_DX7_NUM_VBS; i++)
		{
			m_pVB[i] = 0;
			m_uVertsUsed[i] = 0;
		}
		m_uCurrentVB = 0;
	}

	~IFXDX7VB()
	{
		for(U32 i = 0; i < IFX_DX7_NUM_VBS; i++)
		{
			IFXRELEASE(m_pVB[i]);
			m_uVertsUsed[i] = 0;
		}
	}

	U32 NewVB()
	{
		m_uCurrentVB++;
		if(m_uCurrentVB >= IFX_DX7_NUM_VBS)
		{
			m_uCurrentVB = 0;
		}

		return m_uCurrentVB;
	}
};

struct IFXDX7StaticVB
{
	LPDIRECT3DVERTEXBUFFER7	m_pVB;
	U32						m_uSrcDataVersion;

	IFXDX7StaticVB()
	{
		m_pVB = 0;
		m_uSrcDataVersion = 0;
	}

	~IFXDX7StaticVB()
	{
		IFXRELEASE(m_pVB);
		m_uSrcDataVersion = 0;
	}
};

class CIFXRenderDX7;

class IFXRenderDX7StackEntry
{
public:
	IFXRenderDX7StackEntry() : m_pRender(NULL), m_pNext(NULL), m_pPrev(NULL) {}
	~IFXRenderDX7StackEntry() { IFXDELETE(m_pNext); }
	CIFXRenderDX7* m_pRender;
	IFXRenderDX7StackEntry* m_pNext;
	IFXRenderDX7StackEntry* m_pPrev;
};

class CIFXRenderDeviceDX7 : public CIFXRenderDevice
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceDX7Factory(IFXREFIID intId, void** ppUnk);
	
	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	IFXRESULT IFXAPI GetSupported(U32 uDeviceNum, BOOL& bSupported);

	IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps) const;

	IFXRESULT IFXAPI GetDXCaps(D3DDEVICEDESC7& rCaps) const;
	
	IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType);
	
	IFXRESULT IFXAPI SetTexStageState(	U32 uStageNum, 
										D3DTEXTURESTAGESTATETYPE tsType, 
										DWORD uTs);
	IFXRESULT IFXAPI SetTexStageStateDebug(	char* szFileName, 
											U32 uLineNum, 
											U32 uStageNum, 
											D3DTEXTURESTAGESTATETYPE tsType, 
											DWORD uTs);
	IFXRESULT IFXAPI SetRenderState(D3DRENDERSTATETYPE rsType, 
									DWORD uRs);
	IFXRESULT IFXAPI SetRenderStateDebug(	char* szFileName, 
											U32 uLineNum, 
											D3DRENDERSTATETYPE rsType, 
											DWORD uRs);
	U32 IFXAPI GetD3DCompare(IFXenum eCompare);
	LPDIRECT3DDEVICE7	IFXAPI GetD3DDevice();
	LPDIRECT3D7			IFXAPI GetD3D();
	LPDIRECTDRAW7		IFXAPI GetDD();
	LPDIRECTDRAWSURFACE7 IFXAPI GetPrimarySurface();
	IFXDirectX7Ptr & IFXAPI GetDX();
	
	IFXRESULT IFXAPI BeginScene();
	IFXRESULT IFXAPI EndScene();
	
	IFXRESULT IFXAPI ResetDevice();
	IFXRESULT IFXAPI RegisterWindow(CIFXRenderDX7* pRender);
	IFXRESULT IFXAPI UnregisterWindow(CIFXRenderDX7* pRender);
	
	void IFXAPI GetDepthFormat(IFXenum eDepthFormat, DDPIXELFORMAT& ddpf);
	
	U32 IFXAPI GetNumHWTexUnits() { return m_Caps.wMaxSimultaneousTextures; }

	void IFXAPI SetSWAA(BOOL bEnabled) { m_bSoftwareAA = bEnabled; }

	IFXRESULT IFXAPI DeleteMesh(U32 uInMeshId) { return DeleteStaticVB(uInMeshId); }

	const IFXMatrix4x4& IFXAPI GetViewMatrix() const { return m_mViewMatrix; }
	const IFXMatrix4x4& IFXAPI GetWorldMatrix() const { return m_mWorldMatrix; }

protected:
	CIFXRenderDeviceDX7();
	virtual ~CIFXRenderDeviceDX7();
	
	void IFXAPI InitData();
	IFXRESULT IFXAPI Construct();
	
	IFXRenderDX7StackEntry* IFXAPI FindStackEntry(CIFXRenderDX7* pRender);
	void IFXAPI RemoveStackEntry(CIFXRenderDX7* pRender);
	
	DWORD IFXAPI BuildFVF(IFXVertexAttributes vaAttribs);
	IFXRESULT IFXAPI GetVertexBuffer(	DWORD fvf, 
										U32 uStride,
										IFXDX7VB*& pVerts);
	IFXRESULT IFXAPI GetStaticVertexBuffer(	DWORD fvf, 
											U32 uId,
											U32 uStride,
											U32 uNumVerts,
											IFXDX7StaticVB*& pVerts);
	IFXRESULT IFXAPI CopyVerticesToBuffer(	IFXInterleavedData& rData,
											IFXDX7VB& rVerts,
											U32& uVBStart,
											U32 uStart,
											U32 uNumVertices,
											U8 uMeshDrawType);
	
	IFXRESULT IFXAPI CopyVerticesToStaticBuffer(	IFXInterleavedData& rSrc,
													U32 uNumVertices,
													IFXDX7StaticVB& rVerts );
	
	IFXRESULT IFXAPI DrawBigMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType);
	IFXRESULT IFXAPI DrawSmallMesh(	IFXMesh& rMesh, 
									IFXInterleavedData& rVerts,
									U32& uNumRenderCalls, 
									U8 uMeshDrawType	);
	IFXRESULT IFXAPI DrawStaticSmallMesh(	IFXMesh& rMesh, 
											IFXInterleavedData& rVerts,
											U32& uNumRenderCalls, 
											U8 uMeshDrawType);

	IFXRESULT IFXAPI RenderSmallMesh(	IFXMesh& rMesh, 
										LPDIRECT3DVERTEXBUFFER7 pVerts,
										U32 uVBStart, 
										U32& uNumRenderCalls, 
										U8 uMeshDrawType);
	IFXRESULT IFXAPI DeleteStaticVB(U32 uMeshId);

	IFXRESULT IFXAPI CreateD3D();
	IFXRESULT IFXAPI ReleaseD3D();

	void IFXAPI FillIFXRenderCaps();
	IFXRESULT IFXAPI TryDepthFormat(IFXenum eDepthFormat);

	// Blend State Settings
	IFXRESULT IFXAPI SetHWTestRef(F32 fRef);
	IFXRESULT IFXAPI SetHWBlendFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWTestFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWAlphaTestEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWBlendEnabled(BOOL bEnabled);

	// Fog Settings
	virtual IFXRESULT IFXAPI SetHWFogEnabled(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWMode(IFXenum eMode);
	virtual IFXRESULT IFXAPI SetHWColor(const IFXVector4& vColor);
	virtual IFXRESULT IFXAPI SetHWLinearNear(F32 fNear);
	virtual IFXRESULT IFXAPI SetHWFar(F32 fFar);

	// Material Settings
	virtual IFXRESULT IFXAPI SetHWEmissive(const IFXVector4& vColor);
	virtual IFXRESULT IFXAPI SetHWAmbient(const IFXVector4& vColor);
	virtual IFXRESULT IFXAPI SetHWDiffuse(const IFXVector4& vColor);
	virtual IFXRESULT IFXAPI SetHWSpecular(const IFXVector4& vColor);
	virtual IFXRESULT IFXAPI SetHWShininess(F32 fShininess);
	virtual IFXRESULT IFXAPI SetHWFastSpecular(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWRenderStyle(IFXenum eRenderStyle);
	virtual IFXRESULT IFXAPI SetHWShadeModel(IFXenum eShadeModel);
	virtual IFXRESULT IFXAPI SetHWMaterial();

	// Miscellaneous Settings
	IFXRESULT IFXAPI SetHWLighting(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthTest(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthWrite(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthCompare(IFXenum eCompare);
	IFXRESULT IFXAPI SetHWCull(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWCullMode(IFXenum eCullMode);
	IFXRESULT IFXAPI SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix);
	IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mWorldMatrix);
	IFXRESULT IFXAPI SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient);
	IFXRESULT IFXAPI SetHWConstantColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWNormalization(BOOL bNormalization);

	// Stencil Settings
	virtual IFXRESULT IFXAPI SetHWStencilEnabled(BOOL bEnabled);
	virtual IFXRESULT IFXAPI SetHWFailureOp(IFXenum eOp);
	virtual IFXRESULT IFXAPI SetHWZFailureOp(IFXenum eOp);
	virtual IFXRESULT IFXAPI SetHWPassOp(IFXenum eOp);
	virtual IFXRESULT IFXAPI SetHWStencilFunc(IFXenum eFunc);
	virtual IFXRESULT IFXAPI SetHWTestMask(U32 uMask);
	virtual IFXRESULT IFXAPI SetHWWriteMask(U32 uMask);
	virtual IFXRESULT IFXAPI SetHWStencilRef(U32 uRef);
	U32 IFXAPI GetStencilOp(IFXenum eOp);

	// View Settings
	IFXRESULT IFXAPI SetHWView();
	void IFXAPI CalcProjectionMatrix();

	// Blend Data
	DWORD m_uSrcBlend;
	DWORD m_uDestBlend;
	DWORD m_uTestFunc;

	// Material Data
	D3DMATERIAL7	m_D3DUnlitMaterial;
	D3DMATERIAL7	m_D3DMaterial;
	BOOL			m_bBlackSpecular;

	BOOL			m_bUsingLighting;
	BOOL			m_bUsingVertColors;
	BOOL			m_bUsingUnlitMaterial;

	// View Data
	D3DVIEWPORT7	m_Viewport;
	BOOL			m_bSoftwareAA;

	CIFXDeviceLightDX7Ptr		m_pspLightsDX7[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitDX7Ptr		m_pspTexUnitsDX7[IFX_MAX_TEXUNITS];
	
	IFXSimpleHashPtr			m_spVertexBuffers;
	IFXSimpleHashPtr			m_spStaticBuffers;

	IFXDirectX7Ptr				m_spDirectX7;
	
	BOOL						m_bInitialized;
	
	IFXRenderCaps				m_RCaps;
	D3DDEVICEDESC7				m_Caps;
	LPDIRECTDRAW7				m_pDD;
	LPDIRECT3D7					m_pD3D;
	LPDIRECT3DDEVICE7			m_pDevice;
	LPDIRECTDRAWSURFACE7		m_pScreen;
	LPDIRECTDRAWSURFACE7		m_pBackBuffer;
	LPDIRECTDRAWSURFACE7		m_pDepthBuffer;
	HWND						m_hWnd;
	
	BOOL						m_bInScene;
	
	U32							m_uDeviceNum;
	
	IFXRenderDX7StackEntry*		m_pWindows;

};
typedef IFXSmartPtr<CIFXRenderDeviceDX7> CIFXRenderDeviceDX7Ptr;

//================================
// CIFXRenderDeviceDX7 Methods
//================================
IFXINLINE LPDIRECT3DDEVICE7 CIFXRenderDeviceDX7::GetD3DDevice()
{
	return m_pDevice;
}

IFXINLINE LPDIRECT3D7 CIFXRenderDeviceDX7::GetD3D()
{
	return m_pD3D;
}

IFXINLINE LPDIRECTDRAW7 CIFXRenderDeviceDX7::GetDD()
{
	return m_pDD;
}

IFXINLINE LPDIRECTDRAWSURFACE7 CIFXRenderDeviceDX7::GetPrimarySurface()
{
	return m_pScreen;
}

IFXINLINE IFXDirectX7Ptr& CIFXRenderDeviceDX7::GetDX()
{
	return m_spDirectX7;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX7::SetRenderState(D3DRENDERSTATETYPE rsType, DWORD uRs)
{
	HRESULT hr = m_pDevice->SetRenderState(rsType, uRs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}
	IFXASSERTBOXEX(SUCCEEDED(hr), "SetRenderState Failed", FALSE, TRUE);
	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX7::SetTexStageState(U32 uStageNum, D3DTEXTURESTAGESTATETYPE tsType, DWORD uTs)
{
	HRESULT hr = m_pDevice->SetTextureStageState(uStageNum, tsType, uTs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}
	IFXASSERTBOXEX(SUCCEEDED(hr), "SetTextureStageState Failed", FALSE, TRUE);
	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX7::BeginScene()
{
	IFXASSERTBOX(0 != m_pDevice, "Begin Scene with invalid Direct3D device");
	if(!m_bInScene)
	{
		m_bInScene = TRUE;
		HRESULT hr = m_pDevice->BeginScene();
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "BeginScene Failed", FALSE, TRUE);
			return IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	
	return IFX_OK;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX7::EndScene()
{
	IFXASSERTBOX(0 != m_pDevice, "End Scene with invalid Direct3D device");
	if(m_bInScene)
	{
		m_bInScene = FALSE;
		HRESULT hr = m_pDevice->EndScene();
		if(FAILED(hr))
		{
			IFXASSERTBOXEX(SUCCEEDED(hr), "EndScene Failed", FALSE, TRUE);
			return IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	
	return IFX_OK;
}

IFXINLINE DWORD F2DW( FLOAT f )
{
	return *((DWORD*)&f); 
}


#endif // CIFX_RENDER_DEVICE_DX7_H

// END OF FILE

