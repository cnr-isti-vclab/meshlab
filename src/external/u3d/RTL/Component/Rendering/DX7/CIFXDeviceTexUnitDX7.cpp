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
// CIFXDeviceTexUnitDX7.h
#include "IFXRenderPCHDX7.h"
#include "CIFXDeviceTexUnitDX7.h"

//===========================
// Factory Function
//===========================
IFXRESULT IFXAPI_CALLTYPE CIFXDeviceTexUnitDX7Factory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXDeviceTexUnitDX7* pPtr = new CIFXDeviceTexUnitDX7;
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

U32 CIFXDeviceTexUnitDX7::AddRef()
{
	return ++m_refCount;
}

U32 CIFXDeviceTexUnitDX7::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXDeviceTexUnitDX7::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == CID_IFXDeviceTexUnit)
		{
			*(CIFXDeviceTexUnit**)ppInterface = (CIFXDeviceTexUnit*) this;
		}
		else
		if (interfaceId == CID_IFXDeviceTexUnitDX7)
		{
			*(CIFXDeviceTexUnitDX7**)ppInterface = (CIFXDeviceTexUnitDX7*) this;
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

//===========================
// Public Methods
//===========================
CIFXDeviceTexUnitDX7::CIFXDeviceTexUnitDX7()
{
	m_refCount = 0;
}

CIFXDeviceTexUnitDX7::~CIFXDeviceTexUnitDX7()
{
	m_pDevice = 0;
}

IFXRESULT CIFXDeviceTexUnitDX7::Construct()
{
	IFXRESULT rc = CIFXDeviceTexUnit::Construct();

	InitData();

	return rc;
}

void CIFXDeviceTexUnitDX7::InitData()
{
	m_pDevice = 0;
	m_uTexUnitNum = 0;
	m_uTexGenMode = 0;
	m_mTexGenMatrix.Reset();
	m_eTexMipMode = 0xffff;
	m_eTexMinFilter = 0xffff;
	m_eTexMagFilter = 0xffff;

	m_bCubeMap = FALSE;
	m_bNeedValidation = TRUE;
}

IFXRESULT CIFXDeviceTexUnitDX7::Initialize(U32 uTexUnitNum, CIFXRenderDeviceDX7Ptr& spDevice, BOOL bHW)
{
	IFXRESULT rc = IFX_OK;
	
	m_pDevice = spDevice.GetPointerNR();
	m_pDevice->GetDXCaps(m_Caps);
	m_uTexUnitNum = uTexUnitNum;

	m_uTexCoordSet = uTexUnitNum;

	if(bHW)
	{
		rc = SetDefaults();
	}
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWEnabled (BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bEnabled = bEnabled;

	rc |= SetHWRGBBlendFunc(m_eRGBBlendFunc);
	rc |= SetHWAlphaFunc(m_eABlendFunc);
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWRGBInput0 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		rc = m_pDevice->SetTSS(m_uTexUnitNum, 
								D3DTSS_COLORARG1, 
								GetTexUnitInput(eInput));
	}
	
	if(IFXSUCCESS(rc))
	{
		m_eRGBInput0 = eInput;
		m_bNeedValidation = TRUE;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWRGBInput1 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		rc = m_pDevice->SetTSS(m_uTexUnitNum, 
								D3DTSS_COLORARG2, 
								GetTexUnitInput(eInput));
	}
	
	if(IFXSUCCESS(rc))
	{
		m_eRGBInput1 = eInput;
		m_bNeedValidation = TRUE;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWRGBInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_eRGBInterpSrc = (eInput | IFX_ALPHA) & ~IFX_INVERSE;

	rc = SetHWRGBBlendFunc(m_eRGBBlendFunc);

	m_bNeedValidation = TRUE;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWRGBBlendFunc (IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		U32 uFunc = D3DTOP_DISABLE;
		if(m_bEnabled)
		{
			uFunc = GetTexUnitFunc(eFunc, m_eRGBInterpSrc);
		}
		rc = m_pDevice->SetTSS(m_uTexUnitNum, 
								D3DTSS_COLOROP, 
								uFunc);
	}
	
	if(IFXSUCCESS(rc))
	{
		m_eRGBBlendFunc = eFunc;
		m_bNeedValidation = TRUE;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWAlphaInput0 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		rc = m_pDevice->SetTSS(m_uTexUnitNum, 
								D3DTSS_ALPHAARG1, 
								GetTexUnitInput(eInput));
	}
	
	if(IFXSUCCESS(rc))
	{
		m_eAInput0 = eInput;
		m_bNeedValidation = TRUE;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWAlphaInput1 (IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		rc = m_pDevice->SetTSS(m_uTexUnitNum, 
								D3DTSS_ALPHAARG2, 
								GetTexUnitInput(eInput));
	}
	
	if(IFXSUCCESS(rc))
	{
		m_eAInput1 = eInput;
		m_bNeedValidation = TRUE;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWAlphaInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	m_eAInterpSrc = (eInput | IFX_ALPHA) & ~IFX_INVERSE;

	rc = SetHWAlphaFunc(m_eABlendFunc);

	m_bNeedValidation = TRUE;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWAlphaFunc (IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		U32 uFunc = D3DTOP_DISABLE;
		if(m_bEnabled)
		{
			uFunc = GetTexUnitFunc(eFunc, m_eAInterpSrc);
		}
		rc = m_pDevice->SetTSS(m_uTexUnitNum, 
								D3DTSS_ALPHAOP, 
								uFunc);
	}
	
	if(IFXSUCCESS(rc))
	{
		m_eABlendFunc = eFunc;
		m_bNeedValidation = TRUE;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTextureId (U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		CIFXDeviceTextureDX7* pTex = GetTexture(uTexId);

		if(pTex)
		{
			HRESULT hr = m_pDevice->GetD3DDevice()->SetTexture(m_uTexUnitNum, pTex->GetTexture());
			if(FAILED(hr))
			{
				IFXASSERTBOXEX(SUCCEEDED(hr), "SetTexture Failed", FALSE, TRUE);
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}
	
			if(IFXSUCCESS(rc))
			{
				BOOL bCubeMap = (pTex->GetType() == IFX_TEXTURE_CUBE);
				if(m_bCubeMap != bCubeMap)
				{
					m_bCubeMap = bCubeMap;
					if(m_uTexGenMode)
						SetHWTexCoordGen(m_eTexCoordGen);
				}
			}
		}
		else
		{
			HRESULT hr = m_pDevice->GetD3DDevice()->SetTexture(m_uTexUnitNum, NULL);
			if(FAILED(hr))
			{
				IFXASSERTBOXEX(SUCCEEDED(hr), "SetTexture Failed", FALSE, TRUE);
				rc = IFX_E_SUBSYSTEM_FAILURE;
			}

			m_bCubeMap = FALSE;
		}


		IFXRELEASE(pTex);
	}

	if(IFXSUCCESS(rc))
	{
		m_uTexId = uTexId;
	}
	
	return IFX_OK;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTexCoordGen(IFXenum eGenMode)
{
	IFXRESULT rc = IFX_OK;

	m_mTexGenMatrix.Reset();
	m_uTexGenMode = 0;
	switch(eGenMode)
	{
	case IFX_NONE:
		break;
	case IFX_TEXGEN_REFLECTION_SPHERE:
		m_uTexGenMode = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
		
		// This is a bias and scale matrix to shove the computed 
		// texture coordinates from (-1.0 - 1.0) -> (0.0 - 1.0).
		m_mTexGenMatrix[0] = 0.40f;
		m_mTexGenMatrix[5] = 0.40f;
		m_mTexGenMatrix[12] = 0.5f;
		m_mTexGenMatrix[13] = 0.5f;	
		break;
	case IFX_TEXGEN_VIEWPOSITION:
		m_uTexGenMode = D3DTSS_TCI_CAMERASPACEPOSITION;
		break;
	case IFX_TEXGEN_VIEWNORMAL:
		m_uTexGenMode = D3DTSS_TCI_CAMERASPACENORMAL;
		break;
	case IFX_TEXGEN_VIEWREFLECTION:
		m_uTexGenMode = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
		break;
	case IFX_TEXGEN_WORLDPOSITION:
		{
			// Texgen matrix is the camera->world matrix.
			// This is the inverse of the device's view matrix.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();

			// We negate the Z axis because that is what's submitted to DX
			// We need to replicate that.
			mView[2] = -mView[2];
			mView[6] = -mView[6];
			mView[10] = -mView[10];
			mView[14] = -mView[14];
			
			m_mTexGenMatrix.Invert3x4(mView);

			m_uTexGenMode = D3DTSS_TCI_CAMERASPACEPOSITION;
		}
		break;
	case IFX_TEXGEN_WORLDNORMAL:
		{
			// TexGenMatrix is inverse transpose of the camera->world matrix
			// The device View matrix is the Inverse of camera->world matrix
			m_mTexGenMatrix = m_pDevice->GetViewMatrix();

			// We negate the Z axis because that is what's submitted to DX
			// We need to replicate that.
			m_mTexGenMatrix[2] = -m_mTexGenMatrix[2];
			m_mTexGenMatrix[6] = -m_mTexGenMatrix[6];
			m_mTexGenMatrix[10] = -m_mTexGenMatrix[10];

			// Don't want to translate direction vectors
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Now for the transpose part of inverse transpose.
			m_mTexGenMatrix.Transpose();

			m_uTexGenMode = D3DTSS_TCI_CAMERASPACENORMAL;
		}
		break;
	case IFX_TEXGEN_WORLDREFLECTION:
		{
			// TexGenMatrix is inverse transpose of the camera->world matrix
			// The device View matrix is the Inverse of camera->world matrix
			m_mTexGenMatrix = m_pDevice->GetViewMatrix();

			// We negate the Z axis because that is what's submitted to DX
			// We need to replicate that.
			m_mTexGenMatrix[2] = -m_mTexGenMatrix[2];
			m_mTexGenMatrix[6] = -m_mTexGenMatrix[6];
			m_mTexGenMatrix[10] = -m_mTexGenMatrix[10];

			// Don't want to translate direction vectors
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Now for the transpose part of inverse transpose.
			m_mTexGenMatrix.Transpose();

			m_uTexGenMode = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
		}
		break;
	case IFX_TEXGEN_LOCALPOSITION:
		{
			// The texgen matrix is the inverse of the concatenated world
			// and view matrices.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();

			// We negate the Z axis because that is what's submitted to DX
			// We need to replicate that.
			mView[2] = -mView[2];
			mView[6] = -mView[6];
			mView[10] = -mView[10];
			mView[14] = -mView[14];
			IFXMatrix4x4 mModelView;
			mModelView.Reset();

			// Concatenate the world and view matrices.
			mModelView.Multiply3x4(mView, m_pDevice->GetWorldMatrix());

			// Invert 
			m_mTexGenMatrix.Invert3x4(mModelView);

			m_uTexGenMode = D3DTSS_TCI_CAMERASPACEPOSITION;
		}
		break;
	case IFX_TEXGEN_LOCALNORMAL:
		{
			// The texgen matrix is the inverse transpose of the camera->local
			// matrix.  This is the same as the transpose of the concatenated
			// world and view matrices sent to the device.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();

			// We negate the Z axis because that is what's submitted to DX
			// We need to replicate that.
			mView[2] = -mView[2];
			mView[6] = -mView[6];
			mView[10] = -mView[10];
			mView[14] = -mView[14];

			// Concatenate the world and view matrices.
			m_mTexGenMatrix.Multiply3x4(mView, m_pDevice->GetWorldMatrix());

			// Don't want to translate direction vectors.
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Finally, take the transpose.
			m_mTexGenMatrix.Transpose();

			m_uTexGenMode = D3DTSS_TCI_CAMERASPACENORMAL;
		}
		break;
	case IFX_TEXGEN_LOCALREFLECTION:
		{
			// The texgen matrix is the inverse transpose of the camera->local
			// matrix.  This is the same as the transpose of the concatenated
			// world and view matrices sent to the device.
			IFXMatrix4x4 mView = m_pDevice->GetViewMatrix();

			// We negate the Z axis because that is what's submitted to DX
			// We need to replicate that.
			mView[2] = -mView[2];
			mView[6] = -mView[6];
			mView[10] = -mView[10];
			mView[14] = -mView[14];

			// Concatenate the world and view matrices.
			m_mTexGenMatrix.Multiply3x4(mView, m_pDevice->GetWorldMatrix());

			// Don't want to translate direction vectors.
			m_mTexGenMatrix[12] = 0;
			m_mTexGenMatrix[13] = 0;
			m_mTexGenMatrix[14] = 0;

			// Finally, take the transpose.
			m_mTexGenMatrix.Transpose();

			m_uTexGenMode = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
		}
		break;
	}

	m_eTexCoordGen = eGenMode;

	IFXRUN(rc, SetHWTexCoordSet(m_uTexCoordSet));
	IFXRUN(rc, SetHWTextureTransform(m_mTexMatrix));

	return rc;	
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTextureTransform(const IFXMatrix4x4& mTrans)
{
	IFXRESULT rc = IFX_OK;
	IFXMatrix4x4 mIdent;
	mIdent.Reset();
	
	BOOL bProjected = FALSE;

	if((!(mTrans == mIdent)) || m_uTexGenMode)
	{
		DWORD Flags = D3DTTFF_COUNT2;
		if(m_bCubeMap)
			Flags = D3DTTFF_COUNT3;
		else if(mTrans.RawConst()[15] != 1)
		{
			bProjected = TRUE;
			Flags = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
		}

		rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_TEXTURETRANSFORMFLAGS, Flags);
	}
	else
	{
		rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}

	if(IFXSUCCESS(rc))
	{
		HRESULT hr = D3D_OK;
		IFXMatrix4x4 mTexture = mTrans;

		if(m_uTexGenMode)
		{
			mTexture.Multiply3x4(mTrans, m_mTexGenMatrix);
		}
		else
		{
			if(!bProjected)
			{
				mTexture[8] = mTexture[12];
				mTexture[9] = mTexture[13];
			}
		}

		if(bProjected)
		{
			mTexture[2] = mTexture[3];
			mTexture[6] = mTexture[7];
			mTexture[10] = mTexture[11];
			mTexture[14] = mTexture[15];
		}
		
		hr = m_pDevice->GetD3DDevice()->SetTransform(	(D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + m_uTexUnitNum), 
														(D3DMATRIX*)mTexture.Raw());
		if(FAILED(hr))
		{
			rc = IFX_E_SUBSYSTEM_FAILURE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		if ( &m_mTexMatrix != &mTrans )
			m_mTexMatrix = mTrans;
	}

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTexRepeatU(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		if(bEnabled)
		{
			rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		}
		else
		{
			rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		}
	}

	m_bTexRepeatU = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTexRepeatV(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		if(bEnabled)
		{
			rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		}
		else
		{
			rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	}

	m_bTexRepeatV = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTexRepeatW(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatW = bEnabled;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWMinFilter(IFXenum eMinFilter)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		U32 uVal = GetD3DFilterMode(eMinFilter);
		rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_MINFILTER, uVal);
	}

	m_eTexMinFilter = eMinFilter;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWMagFilter(IFXenum eMagFilter)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		U32 uVal = GetD3DFilterMode(eMagFilter);
		rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_MAGFILTER, uVal);
	}

	m_eTexMagFilter = eMagFilter;

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWMipMode(IFXenum eMipMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_pDevice->GetD3DDevice())
	{
		U32 uVal = GetD3DFilterMode(eMipMode);

		if(eMipMode != IFX_NONE)
		{
			uVal++;
		}
		
		if((eMipMode == IFX_LINEAR) && !(m_Caps.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR))
		{
			uVal = D3DTFN_POINT;	
		}
		rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_MIPFILTER, uVal);
	}

	m_eTexMipMode = eMipMode;
	
	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTexUnit()
{
	IFXRESULT rc = IFX_OK;
	HRESULT hr = D3D_OK;
	
	if(m_pDevice->GetD3DDevice())
	{
		CIFXDeviceTextureDX7* pTex = GetTexture(m_uTexId);

		if(pTex)
		{
			CompareDevice(rc, m_eTexMipMode, pTex->GetMipMode(), SetHWMipMode);
			CompareDevice(rc, m_eTexMinFilter, pTex->GetMinFilter(), SetHWMinFilter);
			CompareDevice(rc, m_eTexMagFilter, pTex->GetMagFilter(), SetHWMagFilter);

			m_bDirty = FALSE;
		}

		IFXRELEASE(pTex);

		if(m_bNeedValidation)
		{
			DWORD uNumPasses = 0;
			hr = m_pDevice->GetD3DDevice()->ValidateDevice(&uNumPasses);
			if(FAILED(hr) || uNumPasses != 1)
			{
				rc = IFX_E_INVALID_RANGE;
			}

			m_bNeedValidation = FALSE;
		}
	}

	return rc;
}

IFXRESULT CIFXDeviceTexUnitDX7::SetHWTexCoordSet(U32 uTCSet)
{
	IFXRESULT rc = IFX_OK;
	
	if(uTCSet > IFX_MAX_TEXUNITS)
	{
		uTCSet = 0;
	}
	
	m_uTexCoordSet = uTCSet;
	
	if(m_pDevice)
	{
		rc = m_pDevice->SetTSS(m_uTexUnitNum, D3DTSS_TEXCOORDINDEX, m_uTexCoordSet|m_uTexGenMode);
	}
	
	return rc;
}

U32 CIFXDeviceTexUnitDX7::GetTexUnitFunc(IFXenum eFunc, IFXenum eInterpSrc) const
{
	U32 uVal = D3DTOP_DISABLE;
	switch(eFunc)
	{
	case IFX_SELECT_ARG0:
		uVal = D3DTOP_SELECTARG1;
		break;
	case IFX_MODULATE:
		uVal = D3DTOP_MODULATE;
		break;
	case IFX_MODULATE2X:
		uVal = D3DTOP_MODULATE2X;
		break;
	case IFX_MODULATE4X:
		uVal = D3DTOP_MODULATE4X;
		break;
	case IFX_ADD:
		uVal = D3DTOP_ADD;
		break;
	case IFX_ADDSIGNED:
		uVal = D3DTOP_ADDSIGNED;
		break;
	case IFX_ADDSIGNED2X:
		uVal = D3DTOP_ADDSIGNED2X;
		break;
	case IFX_INTERPOLATE:
		{
			switch(eInterpSrc)
			{
			case (IFX_CONSTANT|IFX_ALPHA):
				uVal = D3DTOP_BLENDFACTORALPHA;
				break;
			case (IFX_TEXTURE|IFX_ALPHA):
				uVal = D3DTOP_BLENDTEXTUREALPHA;
				break;
			case (IFX_INCOMING|IFX_ALPHA):
				uVal = D3DTOP_BLENDCURRENTALPHA;
				break;
			case (IFX_DIFFUSE|IFX_ALPHA):
				uVal = D3DTOP_BLENDDIFFUSEALPHA;
				break;
			}
		}
		break;
	case IFX_DOTPRODUCT3:
		uVal = D3DTOP_DOTPRODUCT3;
		break;
	}

	return uVal;
}

U32 CIFXDeviceTexUnitDX7::GetTexUnitInput(IFXenum eInput) const
{
	U32 uVal = D3DTA_CURRENT;

	IFXenum eBase = eInput & ~(IFX_INVERSE | IFX_ALPHA);
	switch(eBase)
	{
	case IFX_CONSTANT:
		uVal = D3DTA_TFACTOR;
		break;
	case IFX_INCOMING:
		uVal = D3DTA_CURRENT;
		break;
	case IFX_DIFFUSE:
		uVal = D3DTA_DIFFUSE;
		break;
	case IFX_TEXTURE:
		uVal = D3DTA_TEXTURE;
		break;
	}

	if(eInput & IFX_INVERSE)
	{
		uVal |= D3DTA_COMPLEMENT;
	}

	if(eInput & IFX_ALPHA)
	{
		uVal |= D3DTA_ALPHAREPLICATE;
	}

	return uVal;
}

CIFXDeviceTextureDX7* CIFXDeviceTexUnitDX7::GetTexture(U32 uTexId)
{
	CIFXDeviceTextureDX7* pTex = 0;
	IFXRESULT rc = IFX_OK;

	if(m_pDevice)
	{
		IFXUnknown* pUnk = 0;
		rc = m_pDevice->GetTextures()->GetData(uTexId, pUnk);

		IFXRUN(rc, pUnk->QueryInterface(CID_IFXDeviceTextureDX7, (void**)&pTex));

		IFXRELEASE(pUnk);
	}

	return pTex;
}

DWORD CIFXDeviceTexUnitDX7::GetD3DFilterMode(IFXenum eFilter)
{
	DWORD uVal = D3DTFN_POINT;

	switch(eFilter)
	{
	case IFX_POINT:
		uVal = D3DTFN_POINT;
		break;
	case IFX_LINEAR:
		uVal = D3DTFN_LINEAR;
		break;
	}

	return uVal;
}

// END OF FILE























