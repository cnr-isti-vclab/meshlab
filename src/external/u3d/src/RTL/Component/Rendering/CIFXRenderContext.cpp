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
// CIFXRenderContext.cpp
#include "IFXRenderPCH.h"
#include "CIFXRenderServices.h"
#include "CIFXRenderContext.h"

//=========================
// Factory Function
//=========================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderContextFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXRenderContext* pPtr = new CIFXRenderContext;
		if (pPtr)
		{
			rc = pPtr->Construct();
			if (IFXFAILURE(rc))
				IFXDELETE(pPtr);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if (IFXSUCCESS(rc))
		{
			pPtr->AddRef();
			rc = pPtr->QueryInterface(intId, ppUnk);
			pPtr->Release();
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}
	return rc;
}

U32 CIFXRenderContext::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderContext::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderContext::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRenderContext)
		{
			*(IFXRenderContext**)ppInterface = (IFXRenderContext*) this;
		}
		else
		if (interfaceId == IID_IFXUnknown)
		{
			*(IFXUnknown**)ppInterface = (IFXUnknown*) this;
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
		if (IFXSUCCESS(result))
			AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}
	return result;
}

//=========================
// Public Methods
//=========================
CIFXRenderContext::CIFXRenderContext()
{
	m_refCount = 0;
}

CIFXRenderContext::~CIFXRenderContext()
{
	Destroy();
}

IFXRESULT CIFXRenderContext::Construct()
{
	IFXRESULT rc = IFX_OK;

	InitData();

	rc = m_pRS.Create(CID_IFXRenderServices, CID_IFXRenderServices);

	if(IFXSUCCESS(rc))
	{
		m_pRS->RegisterRenderContext(this);
	}

	if(IFXSUCCESS(rc))
	{
		const IFXMonitor* pMons = m_pRS->GetMonitors(m_uNumMonitors);
		m_pMonitors = new IFXMonitor[m_uNumMonitors];
		if(m_pMonitors)
		{
			memcpy(m_pMonitors, pMons, sizeof(IFXMonitor)*m_uNumMonitors);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(rc))
	{
		m_ppRender = new IFXRenderPtr[m_uNumMonitors];
		if(0 == m_ppRender)
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Construct", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::Destroy()
{
	IFXDELETE_ARRAY(m_ppRender);
	IFXDELETE_ARRAY(m_pMonitors);

	if(m_pRS.IsValid())
	{
		m_pRS->UnregisterRenderContext(this);
	}

	return IFX_OK;
}

void CIFXRenderContext::InitData()
{
	m_ppRender = 0;
	m_pMonitors = 0;
	m_uNumMonitors = 0;
	m_uCurrentDevice = 0;
	m_uRenderId = 0;

	U32 i;
	for( i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		m_pvConstantColors[i].Set(1,1,1,1);
	}

	U32 l;
	for( l = 0; l < IFX_MAX_LIGHTS; l++)
	{
		m_pbLightEnabled[l] = FALSE;
	}

	m_bBlendEnabled = FALSE;
	m_bLighting = TRUE;
	m_bAlphaTestEnabled = FALSE;
	m_bCurrLighting = FALSE;
	m_bCurrBlendEnabled = FALSE;
	m_bCurrAlphaTestEnabled = FALSE;
	m_bSeparateSpecularPass = FALSE;
}

IFXRESULT CIFXRenderContext::Initialize()
{
	U32 i = 0;
	for(i = 0; i < m_uNumMonitors; i++)
	{
		m_ppRender[i] = 0;
	}

	for(i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		m_pbTexUnitEnabled[i] = FALSE;
	}

	m_bBlendEnabled = FALSE;
	m_bAlphaTestEnabled = FALSE;

	m_uNumRenderPasses = 0;

	return IFX_OK;
}

IFXRESULT CIFXRenderContext::SetRenderer(U32 uRenderId)
{
	IFXRESULT rc = IFX_OK;

	if(uRenderId != m_uRenderId)
	{
		Reset();
		m_uRenderId = uRenderId;
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetRenderer", FALSE, TRUE);

	return rc;
}

U32 CIFXRenderContext::CalculateNumPasses()
{
	m_ppRender[m_uCurrentDevice]->GetCaps(m_Caps);

	// Clear the current texture render pass map
	memset(&(m_ppiTexUnitMap[0][0]), 0xff, sizeof(I32) * IFX_MAX_TEXUNITS * IFX_MAX_TEXUNITS);

	// Map generation
	//
	// Determine how many passes are required and generate the texture render pass map.
	// This map tells us what texture units (virtual) are to be used during which
	// render passes.
	//
	// We make all of these calculations based on the rgb blend function.
	// If we end up doing multipass, alpha blending is screwed anyway, so
	// we don't want to generate more passes because of alpha blending problems
	// (there's no point).
	//
	// ASSUMPTION: Implicit multipass rendering (i.e. we return a value greater
	// than 1 from this function) is only meant to be used by shaders constraining
	// the blend function to IFX_SELECT_ARG0, IFX_MODULATE, IFX_ADD, and IFX_INTERPOLATE.
	// The use of any other blend functions cannot be simulated with multiple passes.
	// It is intended that operations using blend functions not mentioned above will
	// be sure that all textures are in the first pass.  Also, shaders using implicit
	// multipass rendering should ALWAYS have RGB and Alpha Input0 be IFX_TEXTURE and
	// RGB and Alpha Input1 be IFX_INCOMING.  Using other values for the inputs will
	// result in undefined behavior (not a crash, just wierd output) on successive
	// implicit rendering passes.

	IFXenum eRGBFunc = 0;
	IFXenum eLastRGBFunc = 0;
	U32 uNumTexturesThisPass = 0;
	m_uNumRenderPasses = 0;
	m_bSeparateSpecularPass = FALSE;
	BOOL bConstantUsed = FALSE;
	BOOL bConstantUsedThisTexture = FALSE;

	U32 iTexUnit;
	for(iTexUnit = 0; iTexUnit < IFX_MAX_TEXUNITS; iTexUnit++)
	{
		if(m_pbTexUnitEnabled[iTexUnit])
		{
			eRGBFunc = m_pTexUnits[iTexUnit].GetRGBBlendFunc();

			// For the first pass, almost anything goes.  Since the order of
			// operations is in-order, then we don't need to worry about
			// blending order issues until at least the second pass.
			//
			// However, since DX can only have 1 constant color per
			// rendering (instead of 1 per texture unit), we need
			// to break the chain if more than two constant colors
			// are needed in the same pass.

			// So, let's do the constant color determination first.
			bConstantUsedThisTexture =
				(
				 (m_pTexUnits[iTexUnit].GetRGBInput0() & (IFX_CONSTANT == IFX_CONSTANT)) ||
				 (m_pTexUnits[iTexUnit].GetRGBInput1() & (IFX_CONSTANT == IFX_CONSTANT)) ||
				 (m_pTexUnits[iTexUnit].GetAlphaInput0() & (IFX_CONSTANT == IFX_CONSTANT)) ||
				 (m_pTexUnits[iTexUnit].GetAlphaInput1() & (IFX_CONSTANT == IFX_CONSTANT)) ||
				 (
				   (
				     (m_pTexUnits[iTexUnit].GetRGBInterpolatorSource() & (IFX_CONSTANT == IFX_CONSTANT)) ||
					 (m_pTexUnits[iTexUnit].GetAlphaInterpolatorSource() & (IFX_CONSTANT == IFX_CONSTANT))
					) &&
				    eRGBFunc == IFX_INTERPOLATE
				  )
				 );

			if (bConstantUsedThisTexture)
			{
				// This will never be true on the first texture of a given pass
				if(bConstantUsed)
				{
					if(! (m_pvConstantColors[m_uNumRenderPasses] == m_pTexUnits[iTexUnit].GetConstantColor()))
					{
						m_uNumRenderPasses++;
						m_pvConstantColors[m_uNumRenderPasses] = m_pTexUnits[iTexUnit].GetConstantColor();
						m_ppiTexUnitMap[m_uNumRenderPasses][0] = iTexUnit;
						uNumTexturesThisPass = 1;
						eLastRGBFunc = eRGBFunc;

						// We need to break early to keep this texture from being
						// allocated in the map twice.
						continue;
					}
				}

				m_pvConstantColors[m_uNumRenderPasses] = m_pTexUnits[iTexUnit].GetConstantColor();
				bConstantUsed = TRUE;
			}

			// Anything else in the first pass is just fine.
			if(m_uNumRenderPasses == 0)
			{
				m_ppiTexUnitMap[0][uNumTexturesThisPass] = iTexUnit;
				uNumTexturesThisPass++;
			}
			else
			{
				// For the rest of the passes, we may not be able to
				// use all available HW texture units due to blending
				// order enforcements (the blend with the previous
				// pass happens AFTER the internal texture blending,
				// but we need to maintain the in-order blending operations
				// of the vertual texture chain).

				// No blending issues on the first texture.  The RGBBlend func
				// for this texture unit will determine how the geometry is blended
				// into the frame buffer for this pass.  All successive texture
				// blends for this pass must be communitative with this blend func.
				// In other words, only textures with the same blend func (and NOT
				// successive IFX_INTERPOLATE's) will work.
				if(uNumTexturesThisPass == 0)
				{
					m_ppiTexUnitMap[m_uNumRenderPasses][uNumTexturesThisPass] = iTexUnit;
					uNumTexturesThisPass++;
					eLastRGBFunc = eRGBFunc;
				}

				// Now, compare the blending function to see if we can add this texture
				// on to this pass.  If they are equal, we can combine these two
				// textures in the same pass.  Keep in mind the assumptions listed above,
				// this only works with IFX_SELECT_ARG0, IFX_ADD, and IFX_MODULATE.  However,
				// we don't check that here (we'll just render some ... interesting ... results).
				//
				// Also remember that we can't combine multiple IFX_INTERPOLATE's together...
				else if((eLastRGBFunc == eRGBFunc) && (eRGBFunc != IFX_INTERPOLATE))
				{
					m_ppiTexUnitMap[m_uNumRenderPasses][uNumTexturesThisPass] = iTexUnit;
					uNumTexturesThisPass++;
				}

				// If we can't combine it into this pass, start a new pass and add this texture
				// as the first entry.
				else
				{
					bConstantUsed = bConstantUsedThisTexture;
					m_uNumRenderPasses++;
					if(bConstantUsed)
					{
						m_pvConstantColors[m_uNumRenderPasses] = m_pvConstantColors[m_uNumRenderPasses-1];
					}
					m_ppiTexUnitMap[m_uNumRenderPasses][0] = iTexUnit;
					uNumTexturesThisPass = 1;
					eLastRGBFunc = eRGBFunc;
				}
			}

			// Check to see if we've maxed out the number of texture units yet.
			// If we have, add another pass.
			if(uNumTexturesThisPass == m_Caps.m_uNumHWTexUnits)
			{
				bConstantUsed = FALSE;
				m_uNumRenderPasses++;
				uNumTexturesThisPass = 0;
			}
		}
	}


	// Add another pass for any remaining textures or the diffuse material
	if(uNumTexturesThisPass || (m_uNumRenderPasses == 0))
	{
		m_uNumRenderPasses++;
	}

	// If we have more than 1 pass, and the last pass isn't ADD, then we
	// need a final pass for specular hilights only.
	if(m_uNumRenderPasses > 1)
	{
		// eRGBFunc should be the blend function used in the last render pass.
		if(eRGBFunc != IFX_ADD && m_bLighting)
		{
			m_bSeparateSpecularPass = TRUE;
			m_uNumRenderPasses++;
		}
	}

	return m_uNumRenderPasses;
}

IFXRESULT CIFXRenderContext::ConfigureRenderPass(U32 uRenderPass)
{
	IFXRESULT rc = IFX_OK;

	if(uRenderPass >= m_uNumRenderPasses)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		m_CurrBlend = m_Blend;
		m_CurrMaterial = m_Material;

		m_bCurrBlendEnabled = m_bBlendEnabled;
		m_bCurrLighting = m_bLighting;
		m_bCurrAlphaTestEnabled = m_bAlphaTestEnabled;

		if(uRenderPass == (m_uNumRenderPasses-1) && m_bSeparateSpecularPass)
		{
			rc = ConfigureSpecularPass();
		}
		else
		{
			rc = ConfigureTexturePass(uRenderPass);
		}
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Configure Render Pass", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::ConfigureSpecularPass()
{
	IFXRESULT rc = IFX_OK;

	m_CurrBlend.SetBlendFunc(IFX_FB_ADD);
	m_bCurrBlendEnabled = TRUE;
	m_CurrMaterial.SetEmissive(IFXVector4(0,0,0,0));
	m_CurrMaterial.SetAmbient(IFXVector4(0,0,0,0));
	m_CurrMaterial.SetDiffuse(IFXVector4(0,0,0,0));

	if(m_bCurrBlendEnabled)
	{
		m_ppRender[m_uCurrentDevice]->Enable(IFX_FB_BLEND);
	}
	else
	{
		m_ppRender[m_uCurrentDevice]->Disable(IFX_FB_BLEND);
	}

	if(m_bCurrAlphaTestEnabled)
	{
		m_ppRender[m_uCurrentDevice]->Enable(IFX_FB_ALPHA_TEST);
	}
	else
	{
		m_ppRender[m_uCurrentDevice]->Disable(IFX_FB_ALPHA_TEST);
	}

	if(m_bCurrLighting)
	{
		m_ppRender[m_uCurrentDevice]->Enable(IFX_LIGHTING);
		U32 i;
		for( i = 0; i < IFX_MAX_LIGHTS; i++)
		{
			if(m_pbLightEnabled[i])
				m_ppRender[m_uCurrentDevice]->Enable(IFX_LIGHT0 + i);
			else
				m_ppRender[m_uCurrentDevice]->Disable(IFX_LIGHT0 + i);
		}
	}
	else
	{
		m_ppRender[m_uCurrentDevice]->Disable(IFX_LIGHTING);
		U32 i;
		for( i = 0; i < IFX_MAX_LIGHTS; i++)
		{
			m_ppRender[m_uCurrentDevice]->Disable(IFX_LIGHT0 + i);
		}
	}

	m_ppRender[m_uCurrentDevice]->SetBlend(m_CurrBlend);
	m_ppRender[m_uCurrentDevice]->SetMaterial(m_CurrMaterial);

	U32 i;
	for( i = 0; i < m_Caps.m_uNumHWTexUnits; i++)
	{
		m_ppRender[m_uCurrentDevice]->Disable(IFX_TEXUNIT0 + i);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Configure Specular Pass", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::ConfigureTexturePass(U32 uRenderPass)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		if(uRenderPass < (m_uNumRenderPasses-1) || m_bSeparateSpecularPass)
		{
			m_CurrMaterial.SetSpecular(IFXVector4(0,0,0,0));
			m_CurrMaterial.SetShininess(0);

			if(uRenderPass)
			{
				m_bCurrLighting = FALSE;
			}
		}

		if(uRenderPass)
		{
			m_bCurrAlphaTestEnabled = FALSE;
			IFXenum eRGBBlend = m_pTexUnits[m_ppiTexUnitMap[uRenderPass][0]].GetRGBBlendFunc();

			switch(eRGBBlend)
			{
			case IFX_SELECT_ARG0:
				m_bCurrBlendEnabled = FALSE;
				break;
			case IFX_MODULATE:
			case IFX_MODULATE2X:
			case IFX_MODULATE4X:
				m_bCurrBlendEnabled = TRUE;
				m_CurrBlend.SetBlendFunc(IFX_FB_MULT);
				break;
			case IFX_ADD:
			case IFX_ADDSIGNED:
			case IFX_ADDSIGNED2X:
				m_bCurrBlendEnabled = TRUE;
				m_CurrBlend.SetBlendFunc(IFX_FB_ADD);
				break;
			case IFX_INTERPOLATE:
				m_bCurrBlendEnabled = TRUE;
				m_CurrBlend.SetBlendFunc(IFX_FB_ALPHA_BLEND);
				break;
			}
		}

		if(m_bCurrBlendEnabled)
		{
			m_ppRender[m_uCurrentDevice]->Enable(IFX_FB_BLEND);
		}
		else
		{
			m_ppRender[m_uCurrentDevice]->Disable(IFX_FB_BLEND);
		}

		if(m_bCurrAlphaTestEnabled)
		{
			m_ppRender[m_uCurrentDevice]->Enable(IFX_FB_ALPHA_TEST);
		}
		else
		{
			m_ppRender[m_uCurrentDevice]->Disable(IFX_FB_ALPHA_TEST);
		}

		if(m_bCurrLighting)
		{
			m_ppRender[m_uCurrentDevice]->Enable(IFX_LIGHTING);
			U32 i;
			for( i = 0; i < IFX_MAX_LIGHTS; i++)
			{
				if(m_pbLightEnabled[i])
					m_ppRender[m_uCurrentDevice]->Enable(IFX_LIGHT0 + i);
				else
					m_ppRender[m_uCurrentDevice]->Disable(IFX_LIGHT0 + i);
			}
		}
		else
		{
			m_ppRender[m_uCurrentDevice]->Disable(IFX_LIGHTING);
			U32 i;
			for( i = 0; i < IFX_MAX_LIGHTS; i++)
			{
				m_ppRender[m_uCurrentDevice]->Disable(IFX_LIGHT0 + i);
			}
		}

		m_ppRender[m_uCurrentDevice]->SetBlend(m_CurrBlend);
		m_ppRender[m_uCurrentDevice]->SetMaterial(m_CurrMaterial);

		m_ppRender[m_uCurrentDevice]->SetConstantColor(m_pvConstantColors[uRenderPass]);

		U32 i;
		for( i = 0; i < m_Caps.m_uNumHWTexUnits; i++)
		{
			if(m_ppiTexUnitMap[uRenderPass][i] >= 0)
			{
				m_CurrTexUnit = m_pTexUnits[m_ppiTexUnitMap[uRenderPass][i]];

				m_ppRender[m_uCurrentDevice]->Enable(IFX_TEXUNIT0 + i);

				if(i == 0 && uRenderPass)
				{
					m_CurrTexUnit.SetRGBBlendFunc(IFX_SELECT_ARG0);
					m_CurrTexUnit.SetAlphaBlendFunc(IFX_SELECT_ARG0);
					IFXenum eRGBBlendInterpSrc =
						m_pTexUnits[m_ppiTexUnitMap[uRenderPass][0]].GetRGBInterpolatorSource();
					IFXenum eRGBBlend =
						m_pTexUnits[m_ppiTexUnitMap[uRenderPass][0]].GetRGBBlendFunc();

					if(eRGBBlend == IFX_INTERPOLATE)
					{
						if((eRGBBlendInterpSrc & ~IFX_ALPHA & ~IFX_INVERSE ) == IFX_CONSTANT)
							m_CurrTexUnit.SetAlphaInput0(eRGBBlendInterpSrc);
					}
				}

				m_ppRender[m_uCurrentDevice]->SetTextureUnit(IFX_TEXUNIT0 + i, m_CurrTexUnit);
			}
			else
			{
				m_ppRender[m_uCurrentDevice]->Disable(IFX_TEXUNIT0 + i);
			}
		}
	}
	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Configure Texture Pass", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetRenderServices(IFXRenderServices*& pServices)
{
	pServices = m_pRS.GetPointerNR();

	return IFX_OK;
}

IFXRESULT CIFXRenderContext::GetCaps(IFXRenderCaps& rCaps)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		m_ppRender[m_uCurrentDevice]->GetCaps(m_Caps);
		rCaps = m_Caps;
	}
	else
		rc = IFX_E_NOT_INITIALIZED;

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed GetCaps", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetPerformanceData(IFXenum eData, U32& ruData)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->GetPerformanceData(eData, ruData);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed GetPerformance Data", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::ClearPerformanceData()
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->ClearPerformanceData();
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Clear Performance Data", FALSE, TRUE);

	return rc;
}

IFXenum CIFXRenderContext::GetColorFormat()
{
	IFXenum retVal = IFX_RGBA;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		retVal = m_ppRender[m_uCurrentDevice]->GetColorFormat();
	}

	return retVal;
}


IFXRESULT CIFXRenderContext::Enable(IFXenum eParam)
{
	IFXRESULT rc = IFX_OK;

	if ((eParam >= IFX_LIGHT0) && (eParam < (IFX_LIGHT0+IFX_MAX_LIGHTS))) {
		m_pbLightEnabled[eParam - IFX_LIGHT0] = TRUE;
	} else

	if ((eParam >= IFX_TEXUNIT0) && (eParam < (IFX_TEXUNIT0+IFX_MAX_TEXUNITS))) {
		m_pbTexUnitEnabled[eParam - IFX_TEXUNIT0] = TRUE;
	} else

	switch(eParam)
	{
	case IFX_FB_BLEND:
		m_bBlendEnabled = TRUE;
		break;
	case IFX_FB_ALPHA_TEST:
		m_bAlphaTestEnabled = TRUE;
		break;
	case IFX_LIGHTING:
		m_bLighting = TRUE;
		break;
	default:
		if(m_ppRender[m_uCurrentDevice].IsValid())
		{
			rc = m_ppRender[m_uCurrentDevice]->Enable(eParam);
		}
	};

	if(rc == IFX_E_NEED_RESET)
	{
		Reset();
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Enable", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::Disable(IFXenum eParam)
{
	IFXRESULT rc = IFX_OK;

	if ((eParam >= IFX_LIGHT0) && (eParam < (IFX_LIGHT0+IFX_MAX_LIGHTS))) {
		m_pbLightEnabled[eParam - IFX_LIGHT0] = FALSE;
	} else

	if ((eParam >= IFX_TEXUNIT0) && (eParam < (IFX_TEXUNIT0+IFX_MAX_TEXUNITS))) {
		m_pbTexUnitEnabled[eParam - IFX_TEXUNIT0] = FALSE;
	} else

	switch(eParam)
	{
	case IFX_FB_BLEND:
		m_bBlendEnabled = FALSE;
		break;
	case IFX_FB_ALPHA_TEST:
		m_bAlphaTestEnabled = FALSE;
		break;
	case IFX_LIGHTING:
		m_bLighting = FALSE;
		break;
	default:
		if(m_ppRender[m_uCurrentDevice].IsValid())
		{
			rc = m_ppRender[m_uCurrentDevice]->Disable(eParam);
		}
	};

	if(rc == IFX_E_NEED_RESET)
	{
		Reset();
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Disable", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetBlend(IFXRenderBlend& rBlend)
{
	IFXRESULT rc = IFX_OK;

	m_Blend = rBlend;

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetBlend", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetFog(IFXRenderFog& rFog)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetFog(rFog);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetFog", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetLight(U32 uLightId, IFXRenderLight& rLight)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetLight(uLightId, rLight);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetLight", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetMaterial(IFXRenderMaterial& rMat)
{
	IFXRESULT rc = IFX_OK;

	m_Material = rMat;

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetMaterial", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetStencil(IFXRenderStencil& rStencil)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetStencil(rStencil);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetStencil", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetTexture(IFXTextureObject& rTexture)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetTexture(rTexture);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetTexture", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetTextureUnit(IFXenum eTexUnit, IFXRenderTexUnit& rTexUnit)
{
	IFXRESULT rc = IFX_OK;

	U32 uTexUnitNum = eTexUnit - IFX_TEXUNIT0;

	if(uTexUnitNum >= IFX_MAX_TEXUNITS)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_pTexUnits[uTexUnitNum] = rTexUnit;
		if(m_pTexUnits[uTexUnitNum].GetTextureCoordinateSet() < 0)
			m_pTexUnits[uTexUnitNum].SetTextureCoordinateSet(uTexUnitNum);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetTexUnit", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetConstantColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetConstantColor(vColor);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetConstantColor", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetView(IFXRenderView& rView)
{
	IFXRESULT rc = IFX_OK;

	m_View = rView;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetView(rView);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetView", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetTransformMatrix(IFXMatrix4x4& rMatrix)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->GetTransformMatrix(rMatrix);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed GetTransformMatrix", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetNumDevices(IFXRenderWindow& rWindow, U32& uNumDevices)
{
	IFXRESULT rc = IFX_OK;

	// This will be in virtual coordinates (screen coordinates among all monitors)
	IFXRect VisibleSpriteRect;
	rWindow.GetWindowSizeVC(VisibleSpriteRect);

	uNumDevices = 0;

	// Only proceed further if some portion of the sprite is visible.
	if(VisibleSpriteRect.m_Width > 0 && VisibleSpriteRect.m_Height > 0)
	{
		m_rcWindowVC = VisibleSpriteRect;

		IFXRect rcWin;

		// Cycle over all monitors to find which of them the render context area
		// appears on.
		U32 i;
		for( i = 0; i < m_uNumMonitors; i++)
		{
			rcWin = VisibleSpriteRect;
			if((m_pMonitors[i].m_rcScreen.GetIntersection(rcWin) && !rWindow.GetDTS())
				|| (i == 0 && rWindow.GetDTS()) )
			{
				// The above if statement will succeed if a portion of the render context
				// area appears on monitor i.
				uNumDevices++;

				if(!m_ppRender[i].IsValid())
				{
					// Create a render object for this monitor if there isn't one yet.
					rc = CreateRender(i, rWindow);
				}

				// Do an addref of the render object that we are using.
				m_ppRender[i].IncRef();
			}

			// DecRef will decrement the reference count of the object
			// only if the pointer is valid.  So, if the render object
			// at i has already been destroyed, this will not break.
			// Also, since we have already addref'ed the objects that
			// we are currently using, only render objects that we are no
			// long using will end up with a lower reference count.
			m_ppRender[i].DecRef();
		}
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed GetNumDevices", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetWindow(IFXRenderWindow& rWindow)
{
	IFXRESULT rc = IFX_OK;
	static BOOL bInReset = FALSE;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetWindow(rWindow);
	}

	if(IFX_E_NEED_RESET == rc)
	{
		if(!bInReset)
		{
			bInReset = TRUE;
			m_pRS->Reset();

			U32 uNumDevices = 0;
			rc = GetNumDevices(rWindow, uNumDevices);

			if(IFXSUCCESS(rc))
			{
				rc = SetWindow(rWindow);
			}
		}
		else
		{
			bInReset = FALSE;
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXFAILURE(rc))
	{
		m_pRS->Reset();
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetWindow", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetWindow(IFXRenderWindow& rWindow)
{
	IFXRESULT rc = IFX_E_NOT_INITIALIZED;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->GetWindow(rWindow);
	}

	return rc;
}

IFXRESULT CIFXRenderContext::SetDevice(U32 uDevice)
{
	IFXRESULT rc = IFX_OK;

	m_uCurrentDevice = 0;
	U32 uNumValid = 0;
	U32 i;
	for( i = 0; i < m_uNumMonitors; i++)
	{
		if(m_ppRender[i].IsValid())
		{
			m_uCurrentDevice = i;
			if(uNumValid == uDevice)
			{
				rc = m_ppRender[i]->MakeCurrent();
				break;
			}
			else
			{
				uNumValid++;
			}
		}
	}

	if(uNumValid != uDevice)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(rc == IFX_E_NEED_RESET)
	{
		m_pRS->Reset();
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetDevice", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::Clear(const IFXRenderClear& rClear)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->Clear(rClear);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Clear", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::DrawMeshLines(IFXMesh& rMesh)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->DrawMeshLines(rMesh);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed DrawMeshLines", FALSE, 1);

	return rc;
}

IFXRESULT CIFXRenderContext::DrawMeshPoints(IFXMesh& rMesh)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->DrawMeshPoints(rMesh);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed DrawMeshPoints", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::DrawMesh(IFXMesh& rMesh)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->DrawMesh(rMesh);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed DrawMesh", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::DeleteTexture(U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->DeleteTexture(uTexId);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed DeleteTexture", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetViewMatrix(const IFXMatrix4x4& mView)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetViewMatrix(mView);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetViewMatrix", FALSE, TRUE);

	return rc;
}

const IFXMatrix4x4& CIFXRenderContext::GetViewMatrix()
{
	return m_ppRender[m_uCurrentDevice]->GetViewMatrix();
}



IFXRESULT CIFXRenderContext::SetWorldMatrix(const IFXMatrix4x4& mView)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetWorldMatrix(mView);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetWorldMatrix", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetDepthMode(IFXenum eDepthMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetDepthMode(eDepthMode);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetDepthMode", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetCullMode(IFXenum eCullMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetCullMode(eCullMode);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetCullMode", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetLineOffset(U32 uOffset)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetLineOffset(uOffset);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetLineOffset", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::SetGlobalAmbient(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->SetGlobalAmbient(vColor);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed SetGlobalAmbient", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetGlobalAmbient(IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->GetGlobalAmbient(vColor);
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed GetGlobalAmbient", FALSE, TRUE);

	return rc;
}

IFXRESULT CIFXRenderContext::GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals)
{
	IFXRESULT rc = IFX_OK;

	// Need to get total frustum plane normals!

	return rc;
}

IFXRESULT CIFXRenderContext::Swap()
{
	IFXRESULT rc = IFX_OK;

	if(m_ppRender[m_uCurrentDevice].IsValid())
	{
		rc = m_ppRender[m_uCurrentDevice]->Swap();

		if(IFX_E_NEED_RESET == rc)
		{
			m_pRS->Reset();
		}
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed Swap", FALSE, TRUE);

	return rc;
}

void CIFXRenderContext::Reset()
{
	U32 i;
	for( i = 0; i < m_uNumMonitors; i++)
	{
		m_ppRender[i] = 0;
	}

	m_rcWindowVC.Set(0,0,0,0);
}

IFXRESULT CIFXRenderContext::CreateRender(U32 uMonitorNum, IFXRenderWindow& rWindow)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		rc = m_pRS->CreateRenderer(m_uRenderId, m_ppRender[uMonitorNum], uMonitorNum);
	}

	if(IFXSUCCESS(rc))
	{
		rc = m_ppRender[uMonitorNum]->Initialize(uMonitorNum);
	}

	if(IFXSUCCESS(rc))
		rc = m_ppRender[uMonitorNum]->SetWindow(rWindow);

	if(IFXSUCCESS(rc))
	{
		BOOL bVSync = m_pRS->GetVSyncEnabled();

		if(bVSync)
		{
			m_ppRender[uMonitorNum]->Enable(IFX_VSYNC);
		}
		else
		{
			m_ppRender[uMonitorNum]->Disable(IFX_VSYNC);
		}
	}

	IFXASSERTBOXEX(IFXSUCCESS(rc), "Failed CreateRender", FALSE, TRUE);

	return rc;
}

// END OF FILE


