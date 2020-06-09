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
// CIFXRenderDeviceDX8.h
#ifndef CIFX_RENDER_DEVICE_DX8_H
#define CIFX_RENDER_DEVICE_DX8_H

#include "CIFXRenderDevice.h"
#include "CIFXDeviceLightDX8.h"
#include "CIFXDeviceTextureDX8.h"
#include "CIFXDeviceTexUnitDX8.h"

#ifdef _DEBUG

#define SetRS(rsType, rs) SetRenderStateDebug(__FILE__,__LINE__,rsType,rs)
#define SetTSS(uSn, tsType, ts) SetTexStageStateDebug(__FILE__,__LINE__,uSn, tsType,ts)

#else

#define SetRS(rsType, rs) SetRenderState(rsType,rs)
#define SetTSS(uSn, tsType, ts) SetTexStageState(uSn, tsType,ts)

#endif // DEBUG

//================================
// Helper data types
//================================
struct IFXDX8VB
{
	LPDIRECT3DVERTEXBUFFER8	m_pVB;
	U32						m_uVertsUsed;

	IFXDX8VB()
	{
		m_pVB = 0;
		m_uVertsUsed = 0;
	}

	~IFXDX8VB()
	{
		IFXRELEASE(m_pVB);
		m_uVertsUsed = 0;
	}
};

struct IFXDX8StaticVB
{
	LPDIRECT3DVERTEXBUFFER8	m_pVB;
	U32						m_uSrcDataVersion;

	IFXDX8StaticVB()
	{
		m_pVB = 0;
		m_uSrcDataVersion = 0;
	}

	~IFXDX8StaticVB()
	{
		IFXRELEASE(m_pVB);
		m_uSrcDataVersion = 0;
	}
};


class CIFXRenderDX8;

class IFXRenderDX8StackEntry
{
public:
	IFXRenderDX8StackEntry() : m_pRender(NULL), m_pNext(NULL), m_pPrev(NULL) {}
	~IFXRenderDX8StackEntry() { IFXDELETE(m_pNext); }
	CIFXRenderDX8* m_pRender;
	IFXRenderDX8StackEntry* m_pNext;
	IFXRenderDX8StackEntry* m_pPrev;
};

class CIFXRenderDeviceDX8 : public CIFXRenderDevice
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderDeviceDX8Factory(IFXREFIID intId, void** ppUnk);

	IFXRESULT IFXAPI Initialize(U32 uDeviceNum);

	IFXRESULT IFXAPI GetSupported(U32 uDeviceNum, BOOL& bSupported);

	IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps) const;

	IFXRESULT IFXAPI GetDXCaps(D3DCAPS8& rCaps) const;
	

	IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshType);
	
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
	LPDIRECT3DDEVICE8	IFXAPI GetD3DDevice();
	LPDIRECT3D8			IFXAPI GetD3D();

	IFXRESULT IFXAPI BeginScene();
	IFXRESULT IFXAPI EndScene();

	IFXRESULT IFXAPI ResetDevice();
	IFXRESULT IFXAPI ReleaseVidMem();
	IFXRESULT IFXAPI InitVidMem();
	IFXRESULT IFXAPI RegisterWindow(CIFXRenderDX8* pRender);
	IFXRESULT IFXAPI UnregisterWindow(CIFXRenderDX8* pRender);
	
	U32 IFXAPI GetDeviceNum() { return m_uDeviceNum; }

	U32 IFXAPI GetNumHWTexUnits() { return m_Caps.MaxSimultaneousTextures; }

	IFXRESULT IFXAPI DeleteMesh(U32 uInMeshId) { return DeleteStaticVB(uInMeshId); }

	const IFXMatrix4x4& IFXAPI GetViewMatrix() const { return m_mViewMatrix; }
	const IFXMatrix4x4& IFXAPI GetWorldMatrix() const { return m_mWorldMatrix; }
	
protected:
	CIFXRenderDeviceDX8();
	virtual ~CIFXRenderDeviceDX8();
	
	void IFXAPI InitData();
	IFXRESULT IFXAPI Create();

	IFXRESULT IFXAPI Construct();

	void IFXAPI FillIFXRenderCaps();

	IFXRenderDX8StackEntry* IFXAPI FindStackEntry(CIFXRenderDX8* pRender);
	void IFXAPI RemoveStackEntry(CIFXRenderDX8* pRender);

	DWORD IFXAPI BuildFVF(IFXVertexAttributes vaAttribs);
	IFXRESULT IFXAPI GetVertexBuffer(	DWORD fvf, 
										U32 uStride,
										IFXDX8VB*& pVerts);
	IFXRESULT IFXAPI GetStaticVertexBuffer(	DWORD fvf, 
											U32 uId,
											U32 uStride,
											U32 uNumVerts,
											IFXDX8StaticVB*& pVerts);
	IFXRESULT IFXAPI CopyIndicesToBuffer(	IFXInterleavedData& rIndices,
											U32& uIBStart,
											U32 uPrimitiveStart,
											U32 uNumPrimitives,
											U8 uMeshDrawType);
	IFXRESULT IFXAPI CopyVerticesToBuffer(	IFXInterleavedData& rVertices, 
											IFXDX8VB& rVertBuf, 
											U32& uVBStart, 
											U32 uStart, 
											U32 uNumVertices	);
	IFXRESULT IFXAPI CopyVerticesToStaticBuffer(	IFXInterleavedData& rSrc,
													U32 uNumVertices,
													IFXDX8StaticVB& rVerts );

	IFXRESULT IFXAPI SetStreamSource(LPDIRECT3DVERTEXBUFFER8 pVerts, U32 uStride);
	IFXRESULT IFXAPI SetFVF(U32 uFVF);
	IFXRESULT IFXAPI SetIndices(LPDIRECT3DINDEXBUFFER8 pIndices, U32 uOffset);

	IFXRESULT IFXAPI DrawBigMesh(IFXMesh& rMesh, U32& uNumRenderCalls, U8 uMeshDrawType);
	IFXRESULT IFXAPI DrawSmallMesh(	IFXMesh& rMesh, 
									IFXInterleavedData& rVerts,
									U32& uNumRenderCalls, 
									U8 uMeshDrawType);
	IFXRESULT IFXAPI DrawStaticSmallMesh(	IFXMesh& rMesh, 
											IFXInterleavedData& rVerts,
											U32& uNumRenderCalls, 
											U8 uMeshDrawType);

	IFXRESULT IFXAPI RenderSmallMesh(	IFXMesh& rMesh, 
										U32 uVBStart, 
										U32& uNumRenderCalls, 
										U8 uMeshDrawType);
	IFXRESULT IFXAPI DeleteStaticVB(U32 uMeshId);

	// Blend State Settings
	IFXRESULT IFXAPI SetHWTestRef(F32 fRef);
	IFXRESULT IFXAPI SetHWBlendFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWTestFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWAlphaTestEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWBlendEnabled(BOOL bEnabled);

	// Fog Settings
	IFXRESULT IFXAPI SetHWFogEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWMode(IFXenum eMode);
	IFXRESULT IFXAPI SetHWColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWLinearNear(F32 fNear);
	IFXRESULT IFXAPI SetHWFar(F32 fFar);

	// Material Settings
	IFXRESULT IFXAPI SetHWEmissive(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWAmbient(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWDiffuse(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWSpecular(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWShininess(F32 fShininess);
	IFXRESULT IFXAPI SetHWFastSpecular(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWRenderStyle(IFXenum eRenderStyle);
	IFXRESULT IFXAPI SetHWShadeModel(IFXenum eShadeModel);
	IFXRESULT IFXAPI SetHWMaterial();

	// Miscellaneous State Settings
	IFXRESULT IFXAPI SetHWLighting(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthTest(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthWrite(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWDepthCompare(IFXenum eCompare);
	IFXRESULT IFXAPI SetHWCull(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWCullMode(IFXenum eCullMode);
	IFXRESULT IFXAPI SetHWViewMatrix(const IFXMatrix4x4& mViewMatrix);
	IFXRESULT IFXAPI SetHWWorldMatrix(const IFXMatrix4x4& mViewMatrix);
	IFXRESULT IFXAPI SetHWGlobalAmbient(const IFXVector4& vGlobalAmbient);
	IFXRESULT IFXAPI SetHWConstantColor(const IFXVector4& vColor);
	IFXRESULT IFXAPI SetHWNormalization(BOOL bNormalization);

	// Stencil Settings
	IFXRESULT IFXAPI SetHWStencilEnabled(BOOL bEnabled);
	IFXRESULT IFXAPI SetHWFailureOp(IFXenum eOp);
	IFXRESULT IFXAPI SetHWZFailureOp(IFXenum eOp);
	IFXRESULT IFXAPI SetHWPassOp(IFXenum eOp);
	IFXRESULT IFXAPI SetHWStencilFunc(IFXenum eFunc);
	IFXRESULT IFXAPI SetHWTestMask(U32 uMask);
	IFXRESULT IFXAPI SetHWWriteMask(U32 uMask);
	IFXRESULT IFXAPI SetHWStencilRef(U32 uRef);
	U32 IFXAPI GetStencilOp(IFXenum eOp);

	// View Settings
	IFXRESULT IFXAPI SetHWView();
	void IFXAPI CalcProjectionMatrix();

	// Blend Data
	DWORD m_uSrcBlend;
	DWORD m_uDestBlend;
	DWORD m_uTestFunc;

	// Material Data
	D3DMATERIAL8	m_D3DUnlitMaterial;
	D3DMATERIAL8	m_D3DMaterial;
	BOOL			m_bBlackSpecular;

	// View Data
	D3DVIEWPORT8	m_Viewport;

	BOOL m_bUsingVertColors;
	BOOL m_bUsingLighting;
	BOOL m_bUsingUnlitMaterial;

	U32							m_uCurrentFVF;
	U32							m_uCurrentIndexOffset;
	LPDIRECT3DVERTEXBUFFER8		m_pStreamSource;
	LPDIRECT3DINDEXBUFFER8		m_pCurrentIndexBuffer;
	LPDIRECT3DINDEXBUFFER8		m_pBigIndexBuffer;
	LPDIRECT3DINDEXBUFFER8		m_pSmallIndexBuffer;
	U32							m_uSmallIndicesUsed;
	IFXSimpleHashPtr			m_spVertexBuffers;
	IFXSimpleHashPtr			m_spStaticBuffers;

	IFXDirectX8Ptr				m_spDirectX8;

	BOOL						m_bInitialized;

	LPDIRECT3D8					m_pD3D;
	LPDIRECT3DDEVICE8			m_pDevice;
	D3DCAPS8					m_Caps;
	IFXRenderCaps				m_RCaps;
	HWND						m_hWnd;

	BOOL						m_bInScene;

	U32							m_uDeviceNum;

	IFXRenderDX8StackEntry*		m_pWindows;

	CIFXDeviceLightDX8Ptr		m_pspLightsDX8[IFX_MAX_LIGHTS];
	CIFXDeviceTexUnitDX8Ptr		m_pspTexUnitsDX8[IFX_MAX_TEXUNITS];
};
typedef IFXSmartPtr<CIFXRenderDeviceDX8> CIFXRenderDeviceDX8Ptr;

//================================
// CIFXRenderDeviceDX8 Methods
//================================
IFXINLINE LPDIRECT3DDEVICE8 CIFXRenderDeviceDX8::GetD3DDevice()
{
	return m_pDevice;
}

IFXINLINE LPDIRECT3D8 CIFXRenderDeviceDX8::GetD3D()
{
	return m_pD3D;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX8::SetRenderState(D3DRENDERSTATETYPE rsType, DWORD uRs)
{
	HRESULT hr = m_pDevice->SetRenderState(rsType, uRs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}
	
	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX8::SetTexStageState(U32 uStageNum, D3DTEXTURESTAGESTATETYPE tsType, DWORD uTs)
{
	HRESULT hr = m_pDevice->SetTextureStageState(uStageNum, tsType, uTs);
	if(SUCCEEDED(hr))
	{
		return IFX_OK;
	}
	
	return IFX_E_SUBSYSTEM_FAILURE;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX8::BeginScene()
{
	IFXASSERTBOX(0 != m_pDevice, "Begin Scene with invalid Direct3D device");
	if(!m_bInScene)
	{
		m_bInScene = TRUE;
		HRESULT hr = m_pDevice->BeginScene();
		if(FAILED(hr))
		{
			return IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	
	return IFX_OK;
}

IFXINLINE IFXRESULT CIFXRenderDeviceDX8::EndScene()
{
	IFXASSERTBOX(0 != m_pDevice, "End Scene with invalid Direct3D device");
	if(m_bInScene)
	{
		m_bInScene = FALSE;
		HRESULT hr = m_pDevice->EndScene();
		if(FAILED(hr))
		{
			return IFX_E_SUBSYSTEM_FAILURE;
		}
	}
	
	return IFX_OK;
}

/*
 Helper function to stuff a FLOAT into a DWORD argument
*/
IFXINLINE DWORD F2DW( FLOAT f )
{
	return *((DWORD*)&f); 
}



#endif // CIFX_RENDER_DEVICE_DX8_H



// END OF FILE


