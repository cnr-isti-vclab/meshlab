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
//  CIFXShaderLitTexture.cpp
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************

#include "CIFXShaderLitTexture.h"
#include "IFXSceneGraph.h"
#include "IFXMaterialResource.h"
#include "IFXTextureObject.h"
#include "IFXPalette.h"
#include "IFXLightSet.h"
#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXException.h"
#include "IFXLight.h"
#include "IFXFrustum.h"
#include "IFXDids.h"
#include "IFXExportingCIDs.h"
#include "IFXReadBuffer.h"
#include "IFXAutoRelease.h"

CIFXShaderLitTexture::CIFXShaderLitTexture()
{
	m_uRefCount = 0;
	m_uMaterialID = 0;
	U32 i;
	for (  i=0; i<IFX_MAX_TEXUNITS; i++ ) 
		m_uTextureID[i] = 0;
}


CIFXShaderLitTexture::~CIFXShaderLitTexture()
{
	// Deallocate any attached UVMappers
	U32 i;
	for ( i=0; i<IFX_MAX_TEXUNITS; i++ )
	{
		if ( m_pUVMapper[i] != NULL )
		{
			IFXRELEASE(m_pUVMapper[i])
		}
	}
}

IFXRESULT CIFXShaderLitTexture::Construct()
{
	IFXRESULT rc = IFX_OK;

	// IFXShaderLitTexture
	m_uChannels = 0x00000000;
	m_uFlags = MATERIAL | USEDIFFUSE;
	m_uTexturesWithAlphaChannel = 0;

	m_bInvertTrans = TRUE;

	U32 i;
	for ( i=0; i<IFX_MAX_TEXUNITS && IFXSUCCESS(rc); i++ ) 
	{
		m_fTextureIntensity[i] = 1;
		m_eBlendFunction[i] = MULTIPLY;
		m_eBlendSource[i] = CONSTANT;
		m_fBlendConstant[i] = 0.5f;
		m_eTextureMode[i] = TM_NONE;
		rc = IFXCreateComponent( CID_IFXUVGenerator, 
			IID_IFXUVGenerator, 
			(void**)(&m_pUVMapper[i]) );
		m_sUVMapperParams[i].eWrapMode = IFX_UV_NONE;
		m_sUVMapperParams[i].eOrientation = IFX_UV_VERTEX;
		m_sUVMapperParams[i].uTextureLayer = i; 
		m_pTexTransforms[i].MakeIdentity();
		m_sUVMapperParams[i].mWrapTransformMatrix.MakeIdentity();
		m_uTextureRepeat[i] = 0x03;

		m_pTexUnits[i].SetConstantColor(IFXVector4(1,1,1,m_fBlendConstant[i]));
		SetTextureMode(i, TM_NONE);
	}

	m_uOpacityID           = 0;
	m_fOpacityIntensity    = 1;
	m_bAlphaTestEnabled    = FALSE;

	m_bLighting = TRUE;

	m_bDrawFaces = TRUE;
	m_bDrawLines = TRUE;
	m_bDrawPoints = TRUE;
	return rc;
}


IFXRESULT IFXAPI_CALLTYPE CIFXShaderLitTexture_Factory( IFXREFIID intId, void **ppUnk )
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXShaderLitTexture* pPtr = new CIFXShaderLitTexture;
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

// IFXUnknown
U32 CIFXShaderLitTexture::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXShaderLitTexture::Release()
{
	if( m_uRefCount == 1 )
	{
		CIFXSubject::PreDestruct();
		delete this;
		return 0;
	}
	else
		return --m_uRefCount;
}


IFXRESULT CIFXShaderLitTexture::QueryInterface( IFXREFIID riid, void **ppv )
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXShader )
			*ppv = (IFXShader*)this;
		else if ( riid == IID_IFXShaderLitTexture   )
			*ppv = (IFXShaderLitTexture*)this;
		else if ( riid == IID_IFXSubject )
			*ppv = (IFXSubject*)this;
		else if ( riid == IID_IFXUnknown )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXMarker )
			*ppv = (IFXMarker*)this;
		else if ( riid == IID_IFXMarkerX )
			*ppv = (IFXMarkerX*)this;
		else if ( riid == IID_IFXMetaDataX ) 
			*ppv = ( IFXMetaDataX* ) this;
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS(result) )
			AddRef();
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXMarker
IFXRESULT CIFXShaderLitTexture::Mark(void)
{
	IFXRESULT rc = IFX_OK;

	rc = CIFXShader::Mark();

	if ( (IFXSUCCESS(rc)) && (m_pSceneGraph != NULL) )
	{
		// Mark all components to which this component refers.
		if (m_uMaterialID)
		{
			IFXPalette* pPalette = NULL;
			rc = m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, 
				&pPalette );
			if ( IFXSUCCESS( rc ) )
			{
				IFXMarker* pMarker = NULL;
				rc = pPalette->GetResourcePtr( m_uMaterialID, 
					IID_IFXMarker, 
					(void**)&pMarker );

				if ( IFXSUCCESS( rc ) )
					rc = pMarker->Mark();

				// it's ok if there's no resource.
				else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc)
					|| (IFX_E_INVALID_RANGE == rc) )
					rc = IFX_OK ;


				IFXRELEASE(pMarker);
			}

			IFXRELEASE(pPalette);
		}

		U32 i;
		for ( i=0; i<IFX_MAX_TEXUNITS; i++ )
		{
			// For each map layer
			if (m_uTextureID[i])
			{
				if (IFXSUCCESS(rc))
				{
					IFXPalette* pPalette = NULL;
					rc = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, 
						&pPalette );
					if ( IFXSUCCESS( rc ) )
					{
						IFXMarker* pMarker = NULL;
						rc = pPalette->GetResourcePtr( m_uTextureID[i], 
							IID_IFXMarker, 
							(void**)&pMarker );

						if ( IFXSUCCESS( rc ) )
							rc = pMarker->Mark();

						// it's ok if there's no resource.
						else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc)
							|| (IFX_E_INVALID_RANGE == rc) )
							rc = IFX_OK ;

						IFXRELEASE(pMarker);
					}

					IFXRELEASE(pPalette);
				}
			}  // if the layer is in use
		} // for each map layer
	}
	else 
		rc = IFX_E_NOT_INITIALIZED;

	return rc;

} 


void CIFXShaderLitTexture::SetPriority( U32 uInPriority,
									   BOOL bRecursive,		// optional. default == IFX_TRUE.
									   BOOL bPromotionOnly )	// optional. default == IFX_TRUE.
{
	IFXRESULT rc = IFX_OK ;

	CIFXShader::SetPriority( uInPriority, bRecursive, bPromotionOnly );

	if (m_pSceneGraph != NULL)
	{
		// Mark all components to which this component refers.
		if (m_uMaterialID)
		{
			if (IFXSUCCESS(rc))
			{
				IFXPalette* pPalette = NULL;
				rc = m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, 
					&pPalette );
				if ( IFXSUCCESS( rc ) )
				{
					IFXMarker* pMarker = NULL;
					rc = pPalette->GetResourcePtr( m_uMaterialID, 
						IID_IFXMarker, 
						(void**)&pMarker );

					if ( IFXSUCCESS( rc ) )
						pMarker->SetPriority( uInPriority,
						bRecursive,			// optional. default == IFX_TRUE.
						bPromotionOnly );	// optional. default == IFX_TRUE.

					// it's ok if there's no resource.
					else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc)
						|| (IFX_E_INVALID_RANGE == rc) )
						rc = IFX_OK ;


					IFXRELEASE(pMarker);
				}

				IFXRELEASE(pPalette);
			}
		}

		U32 i;
		for ( i=0; i<IFX_MAX_TEXUNITS; i++ )
		{
			// For each map layer
			if (m_uTextureID[i])
			{
				if (IFXSUCCESS(rc))
				{
					IFXPalette* pPalette = NULL;
					rc = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, 
						&pPalette );
					if ( IFXSUCCESS( rc ) )
					{
						IFXMarker* pMarker = NULL;
						rc = pPalette->GetResourcePtr( m_uTextureID[i], 
							IID_IFXMarker, 
							(void**)&pMarker );

						if ( IFXSUCCESS( rc ) )
							pMarker->SetPriority( uInPriority * IFX_SHADERLITTEXTURE_PRIORITY_FACTOR,
							bRecursive,			// optional. default == IFX_TRUE.
							bPromotionOnly );	// optional. default == IFX_TRUE.

						// it's ok if there's no resource.
						else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc)
							|| (IFX_E_INVALID_RANGE == rc) )
							rc = IFX_OK ;


						IFXRELEASE(pMarker);
					}

					IFXRELEASE(pPalette);
				}
			} // If this layer is used
		} // for each map layer
	}
	else 
		rc = IFX_E_NOT_INITIALIZED;

	return ;
}


// IFXMarkerX
void CIFXShaderLitTexture::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXShaderLitTextureEncoder, rpEncoderX);
}


// IFXShader
IFXShader::IFXShaderOpacity CIFXShaderLitTexture::Opacity( void )
{
	IFXShaderOpacity result = IFX_SHADER_OPACITY_TRUE;

	if ( m_pSceneGraph )
	{
		IFXPalette* pPalette;
		m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pPalette );
		IFXUnknown* pUnknown = NULL;
		pPalette->GetResourcePtr( m_uMaterialID, &pUnknown );
		if ( pUnknown )
		{
			IFXMaterialResource* pMaterialResource = NULL;
			pUnknown->QueryInterface(IID_IFXMaterialResource, (void**)&pMaterialResource);
			if ( pMaterialResource )
			{
				F32 opacity;
				pMaterialResource->GetOpacity(&opacity);

				BOOL bTransparent;
				pMaterialResource->GetTransparent(&bTransparent);

				if ( bTransparent )
				{
					if ((opacity < 1) 
						||(m_uTexturesWithAlphaChannel & m_uChannels))
						result = IFX_SHADER_OPACITY_FALSE;
					else
						result = IFX_SHADER_OPACITY_DEPENDENT;
				}
				IFXRELEASE(pMaterialResource);
			}
			IFXRELEASE(pUnknown);
		}
		IFXRELEASE(pPalette);
	}

	return result;
}

IFXRenderBlend& CIFXShaderLitTexture::GetRenderBlend()
{
	return m_Blend;
}

BOOL CIFXShaderLitTexture::GetAlphaTestEnabled()
{
	return m_bAlphaTestEnabled;
}

IFXRESULT CIFXShaderLitTexture::SetAlphaTestEnabled(BOOL bInEnabled )
{
	m_bAlphaTestEnabled = bInEnabled;

	return IFX_OK;
}

BOOL CIFXShaderLitTexture::GetLightingEnabled()
{
	return m_bLighting;
}

IFXRESULT CIFXShaderLitTexture::SetLightingEnabled(BOOL bInEnabled)
{
	m_bLighting = bInEnabled;

	return IFX_OK;
}

IFXRenderMaterial& CIFXShaderLitTexture::GetRenderMaterial()
{
	return m_Material;
}

IFXRESULT CIFXShaderLitTexture::Render( IFXRenderContext& rInRenderer, U32 uRenderPass )
{
	if(! (m_uRenderPassFlags & (1 << uRenderPass)) )
		return IFX_OK;

	IFXRESULT rc = IFX_OK;
	rc = RenderShaderLitTexture( rInRenderer );

	return rc;
}

IFXRESULT CIFXShaderLitTexture::RenderShaderLitTexture( IFXRenderContext& rInRenderer )
{
	IFXRESULT result = IFX_OK;

	BOOL bMaterialTransparent = FALSE;

	if ( !m_pSceneGraph )
		return IFX_E_NOT_INITIALIZED;

	// Mode
	if ( m_uFlags & FLAT )
		m_Material.SetShadeModel(IFX_FLAT);
	else
		m_Material.SetShadeModel(IFX_SMOOTH);

	if ( m_uFlags & WIRE )
		m_Material.SetRenderStyle(IFX_WIREFRAME);
	else if ( m_uFlags & POINT )
		m_Material.SetRenderStyle(IFX_POINTS);
	else
		m_Material.SetRenderStyle(IFX_FILLED);

	// Materials
	IFXMaterialResource* pMaterialResource = NULL;
	if ( m_uFlags & MATERIAL )
	{
		IFXPalette* pPalette = NULL;
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pPalette );

		if ( IFXSUCCESS( result ) )
		{
			result = pPalette->GetResourcePtr( m_uMaterialID, 
				IID_IFXMaterialResource, 
				(void**)&pMaterialResource );
			if ( IFXFAILURE( result ) )
				result = pPalette->GetResourcePtr( 0, 
				IID_IFXMaterialResource, 
				(void**)&pMaterialResource );
		}

		if ( pMaterialResource )
		{
			pMaterialResource->GetTransparent(&bMaterialTransparent);

			// Set up the emmisive color
			IFXVector4 color;
			if ( IFXSUCCESS( result ) )
				result = pMaterialResource->GetEmission(&color);

			if ( IFXSUCCESS( result ) )
				result = m_Material.SetEmissive(*(IFXVector4*)&color.R());

			// Set up the ambient color
			if ( IFXSUCCESS( result ) )
				result = pMaterialResource->GetAmbient(&color);
			if ( IFXSUCCESS( result ) )
				result = m_Material.SetAmbient(*(IFXVector4*)&color.R());

			// Set up the diffuse color
			if ( IFXSUCCESS(result) )
				result = pMaterialResource->GetDiffuse(&color);
			if ( IFXSUCCESS( result ) )
				result = pMaterialResource->GetOpacity(&(color.H()));
			if ( IFXSUCCESS( result ) )
				result = m_Material.SetDiffuse(*(IFXVector4*)&color.R());

			// Set up the specular color
			if ( IFXSUCCESS( result ) )
				result = pMaterialResource->GetSpecular(&color);
			if ( IFXSUCCESS( result ) )
				result = m_Material.SetSpecular(*(IFXVector4*)&color.R());

			// Set up the shinyness
			F32 reflectivity;
			if ( IFXSUCCESS( result ) )
				result = pMaterialResource->GetReflectivity(&reflectivity);
			if ( IFXSUCCESS( result ) )
			{
				if ( reflectivity > 1.0f )
					result = m_Material.SetShininess(1.0f);
				else
					result = m_Material.SetShininess(reflectivity);
			}
		}
		IFXRELEASE(pPalette);
	}

	// Textures
	IFXPalette* pPalette = NULL;
	if( IFXSUCCESS(result) )
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pPalette);

	BOOL bTextures = FALSE;
	U32 i;
	//IFXMatrix4x4 mTrans;
	U32 uMaxTexUnit = 0;
	for ( i=0; i<IFX_MAX_TEXUNITS; i++ ) 
	{
		if ( m_uChannels & (1<<i) )
		{
			uMaxTexUnit = i+1;
			IFXTextureObject* pTextureObject = NULL;
			if ( IFXSUCCESS( result ) )
				result = pPalette->GetResourcePtr( m_uTextureID[i], 
				IID_IFXTextureObject, 
				(void**)&pTextureObject );

			if ( IFXSUCCESS( result ) )
			{
				//result = pTextureObject->UpdateAndMakeCurrent(pInRenderContext, i);
				result = rInRenderer.SetTexture(*pTextureObject);
				if (result == IFX_W_READING_NOT_COMPLETE) result = IFX_E_NOT_INITIALIZED;
				IFXRUN(result, m_pTexUnits[i].SetTextureId(pTextureObject->GetId()));
				IFXRELEASE( pTextureObject );

				// Report this to the output window and mask it internally so that
				// the rendered result is untextured instead of not rendering at all.
				if (IFXFAILURE(result))
				{
					IFXTRACE_CUSTOM( IFXRESULT_COMPONENT_SCENEGRAPH, 
						IFXDEBUG_MESSAGE, 
						L"UpdateAndMakeCurrent failed in IFXShaderLitTexture:(%p)\n", 
						this);
					result = IFX_OK;
					rInRenderer.Disable(IFX_TEXUNIT0 + i);
				}
				else
				{
					bTextures = TRUE;

					rInRenderer.Enable(IFX_TEXUNIT0 + i);

					if( m_uTextureRepeat[i] & IFX_TEXTURE_REPEAT_1 )
					{
						m_pTexUnits[i].SetTexRepeatU( TRUE );
					}
					else
					{
						m_pTexUnits[i].SetTexRepeatU( FALSE );
					}

					if( m_uTextureRepeat[i] & IFX_TEXTURE_REPEAT_2 )
					{
						m_pTexUnits[i].SetTexRepeatV( TRUE );
					}
					else
					{
						m_pTexUnits[i].SetTexRepeatV( FALSE );
					}
				}
			}

			if (  (result == IFX_E_PALETTE_NULL_RESOURCE_POINTER )
				||(result == IFX_E_INVALID_RANGE ) )
				result = IFX_OK;

			if( IFXSUCCESS( result ) )
			{
				F32 reflectivity = 1;
				if ( pMaterialResource && IFXSUCCESS( result ) )
					result = pMaterialResource->GetReflectivity(&reflectivity);
				if ( IFXSUCCESS( result ) )
				{
					if ( reflectivity > 1.0f )
					{
						m_sUVMapperParams[i].fShininess = 1.0f;
					}
					else
					{
						m_sUVMapperParams[i].fShininess = reflectivity;
					}
				}

				if( m_pUVMapper[i] )
				{
					if(m_bInvertTrans)
					{
						IFXMatrix4x4 mTrans;
						mTrans.Reset();
						if(IFXFAILURE(mTrans.Invert3x4(m_pTexTransforms[i])))
						{
							F32* fMat = m_pTexTransforms[i].Raw();
							fMat[2] = fMat[6] = 0.0f;
							fMat[10] = 1.0f;
							mTrans.Invert3x4(m_pTexTransforms[i]);
						}
						m_pTexUnits[i].SetTextureTransform(mTrans);
					}
					else
					{
						m_pTexUnits[i].SetTextureTransform(m_pTexTransforms[i]);
					}
				}
			}

			UpdateAlphaCacheBits( i, m_uTextureID[i] );
			// NOTE: UpdateAlphaCacheBits() returns an IFXRESULT, but we don't
			// care about it because it's possible that this shader doesn't
			// currently have a texture, in which case it would return an 
			// error because there's no texture in the texture palette at
			// that ID.  However - rest assured that this call will 
			// EITHER SET OR CLEAR the alpha cache bit for that layer (clear if 
			// there's no texture).
		}
		else 
		{
			m_uTexturesWithAlphaChannel &= ~(1<<i);
			rInRenderer.Disable(IFX_TEXUNIT0 + i);
		}

		rInRenderer.SetTextureUnit(IFX_TEXUNIT0 + i, m_pTexUnits[i]);
	}
	IFXRELEASE(pPalette);
	IFXRELEASE(pMaterialResource);

	// UseDiffuseWithTexture
	if ( !(m_uFlags&USEDIFFUSE) && bTextures && IFXSUCCESS( result ) )
	{
		IFXVector4 color = m_Material.GetDiffuse();
		color.R() = 1;
		color.G() = 1;
		color.B() = 1;
		result = m_Material.SetDiffuse(color);
	}
	IFXRUN(result, rInRenderer.SetMaterial(m_Material));

	/// @todo Fix this when dependent transparnecy is implemented.
	//BOOL bTransparent = Opacity() == IFX_SHADER_OPACITY_FALSE;
	BOOL bTransparent = Opacity() == IFX_SHADER_OPACITY_FALSE;
	BOOL bUsingBlending = FALSE;

	// Frame buffer blending
	if ((m_Blend.GetBlendFunc() == IFX_FB_ALPHA_BLEND)
		||(m_Blend.GetBlendFunc() == IFX_FB_INV_ALPHA_BLEND))
	{
		if (bTransparent)
		{
			bUsingBlending = TRUE;
			result = rInRenderer.Enable(IFX_FB_BLEND);
		}
		else
		{
			result = rInRenderer.Disable(IFX_FB_BLEND);
		}
	}
	else if(bMaterialTransparent)
	{
		bUsingBlending = TRUE;
		result = rInRenderer.Enable(IFX_FB_BLEND);
	}
	else 
	{
		result = rInRenderer.Disable(IFX_FB_BLEND);
	}

	if(IFXSUCCESS(result) && m_bAlphaTestEnabled)
	{
		result = rInRenderer.Enable(IFX_FB_ALPHA_TEST);
	}
	else if(IFXSUCCESS(result))
	{
		result = rInRenderer.Disable(IFX_FB_ALPHA_TEST);
	}

	IFXRUN(result, rInRenderer.SetBlend(m_Blend));

	// Lighting
	if (m_bLighting)
	{
		IFXRUN(result, rInRenderer.Enable(IFX_LIGHTING));
	}
	else
	{
		IFXRUN(result, rInRenderer.Disable(IFX_LIGHTING));
	}

	// Render
	U32 uNumPasses = rInRenderer.CalculateNumPasses();
	U32 passNum;
	for( passNum = 0; passNum < uNumPasses && IFXSUCCESS(result); passNum++)
	{
		result = rInRenderer.ConfigureRenderPass(passNum);

		IFXMesh* pMesh;
		IFXMeshGroup* pMeshGroup;
		IFXShadedElement* pElement;
		static IFXListContext context;
		IFXMatrix4x4 renderTransform, offsetTransform;

		m_shadedElementList.ToHead(context);
		const IFXLightSet* pLastLightSet = 0;
		const IFXFrustum* pLastFrustum = 0;
		IFXMatrix4x4 mFrustumInverse;
		mFrustumInverse.Reset();

		while ( (pElement = m_shadedElementList.PostIncrement(context)) && IFXSUCCESS(result))
		{
			if ( IFX_OK == pElement->pDataPacket->
				GetDataElement( pElement->u16DataElementIndex, 
				IID_IFXMeshGroup, 
				(void**)&pMeshGroup ) )
			{
				if ( IFX_OK == pMeshGroup->
					GetMesh( pElement->uRenderableElementIndex, pMesh ) )
				{
					if ((pMesh->GetNumFaces() && m_bDrawFaces)
						||(pMesh->GetNumLines() && m_bDrawLines) || (pMesh->GetNumVertices() && m_bDrawPoints))
					{
						// Lights
						if ( m_bLighting ) 
						{
							if(pElement->pLightSet != pLastLightSet)
							{
								pLastLightSet = pElement->pLightSet;
								static const F32 black[4] = { 0,0,0,0 };
								rInRenderer.SetGlobalAmbient((F32*)black);

								U32 nHardwareLightsUsed = 0;
								const U32 nLights = pElement->pLightSet->GetNumLights();
								U32 lightInstance;
								U32 lightIndex;
								for (  lightIndex = 0; (lightIndex < nLights); lightIndex++ )
								{
									IFXDECLARELOCAL(IFXLight, pLight);
									result = pElement->pLightSet->GetLight(lightIndex, pLight, lightInstance);
									if( IFXSUCCESS(result) && pLight )
									{
										IFXDECLARELOCAL(IFXLightResource, pLR);
										pLR = pLight->GetLightResource();

										if( pLR )
										{
											if ( pLR->GetType() == IFXLightResource::AMBIENT )
												pLight->Enable( &rInRenderer, 0, lightInstance );
											else if ( nHardwareLightsUsed < IFX_MAX_LIGHTS )
												pLight->Enable( &rInRenderer, &nHardwareLightsUsed, lightInstance );
											else
												break; // ...out of the <for> construct.
										}
										else
											break; // ...out of the <for> construct.
									}
								}
								while (nHardwareLightsUsed < IFX_MAX_LIGHTS)
									rInRenderer.Disable( IFX_LIGHT0 + nHardwareLightsUsed++ );
							}
						}

						// Texcoord generation
						if(pLastFrustum != pElement->pFrustum)
						{
							pLastFrustum = pElement->pFrustum;

							mFrustumInverse.Invert3x4(pLastFrustum->GetWorldTransform());
						}
						pMesh->GetOffsetTransform( &offsetTransform );
						renderTransform.Multiply( *(pElement->pfTransform), offsetTransform );

						U32 i;
						for( i = 0; i < uMaxTexUnit; i++)
						{
							result = m_pUVMapper[i]->Generate(	*pMesh, &m_sUVMapperParams[i], 
								&renderTransform, 
								&mFrustumInverse, 
								pElement->pLightSet);
						}

						rInRenderer.SetWorldMatrix( renderTransform );
					}

					if(pMesh->GetNumFaces() && m_bDrawFaces && IFXSUCCESS(result))
					{
						// Two sided and negativly scaled lighting support
						U32 bFrontFaceWindingOrder = 1 - pElement->pfTransform->NegativelyScaled();

						if( (pElement->u16Flags & (1 << 15)) || bUsingBlending)
							rInRenderer.Disable(IFX_DEPTH_WRITE);
						else
							rInRenderer.Enable(IFX_DEPTH_WRITE);

						switch (pElement->u16Flags&3)
						{
						case (1<<FRONT_FACE_VISIBILITY):
							{
								result = rInRenderer.SetCullMode( IFX_CULL_CCW - bFrontFaceWindingOrder );
								IFXRUN(result, rInRenderer.DrawMesh( *pMesh ));
								break;
							}

						case (1<<BACK_FACE_VISIBILITY):
							{
								result = rInRenderer.SetCullMode( IFX_CULL_CW + bFrontFaceWindingOrder );
								if ( bFrontFaceWindingOrder )
									pMesh->ReverseNormals();
								IFXRUN(result, rInRenderer.DrawMesh( *pMesh ));
								if ( bFrontFaceWindingOrder )
									pMesh->ReverseNormals();
								break;
							}

						case ((1<<FRONT_FACE_VISIBILITY) | (1<<BACK_FACE_VISIBILITY)):
							{
								result = rInRenderer.SetCullMode( IFX_CULL_CW + bFrontFaceWindingOrder );
								if ( bFrontFaceWindingOrder && pMesh->GetAttributes().m_uData.m_bHasNormals )
									pMesh->ReverseNormals();
								IFXRUN(result, rInRenderer.DrawMesh( *pMesh ));
								if ( bFrontFaceWindingOrder && pMesh->GetAttributes().m_uData.m_bHasNormals )
									pMesh->ReverseNormals();

								IFXRUN(result, rInRenderer.SetCullMode( IFX_CULL_CCW - bFrontFaceWindingOrder ));
								IFXRUN(result, rInRenderer.DrawMesh( *pMesh ));
								break;
							}

						case 0: // This case should be filtered at a higher level.
						default:
							IFXASSERT(0);
						}
						rInRenderer.SetCullMode( IFX_CULL_CW );
					}

					if(pMesh->GetNumLines() && m_bDrawLines && IFXSUCCESS(result))
						result = rInRenderer.DrawMeshLines( *pMesh );

					if(pMesh->GetNumVertices() && m_bDrawPoints && 
						(pMesh->GetNumFaces() == 0) && (pMesh->GetNumLines()==0)
						&& IFXSUCCESS(result))
					{
						result = rInRenderer.DrawMeshPoints( *pMesh ); 					}

					pMesh->Release();
				}

				pMeshGroup->Release();
			}
		}
	}

	return result;
}

// IFXShaderLitTexture
IFXRESULT CIFXShaderLitTexture::SetAlphaTextureChannels( U32 uInEnabledFlags )
{
	IFXRESULT result = IFX_OK;
	m_uTexturesWithAlphaChannel = uInEnabledFlags;
	return result;
}



IFXRESULT CIFXShaderLitTexture::SetChannels( U32 uInEnabledFlags )
{
	IFXRESULT result = IFX_OK;
	m_uChannels = uInEnabledFlags;
	return result;
}


IFXRESULT CIFXShaderLitTexture::SetFlags( U32 uInEnabledFlags )
{
	IFXRESULT result = IFX_OK;
	m_uFlags = uInEnabledFlags;
	return result;
}


IFXRESULT CIFXShaderLitTexture::UpdateAlphaCacheBits( U32 uInLayer, U32 uInID )
{
	IFXRESULT result = IFX_OK;

	if ( !m_pSceneGraph )
		return IFX_E_NOT_INITIALIZED;

	IFXPalette* pPalette = NULL ;
	if ( IFXSUCCESS(result) )
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pPalette );

	IFXUnknown* pUnknown = NULL ;
	if ( IFXSUCCESS(result) )
		result = pPalette->GetResourcePtr( uInID, &pUnknown );
	IFXTextureObject* pTextureObject = NULL ;
	if ( IFXSUCCESS(result) )
		result = pUnknown->QueryInterface(IID_IFXTextureObject, (void**)&pTextureObject);
	IFXRELEASE(pUnknown);

	BOOL bHasAlpha = FALSE;
	if (IFXSUCCESS(result))
		bHasAlpha = pTextureObject->HasAlphaColor();	

	IFXRELEASE( pTextureObject );

	if ( bHasAlpha )
		m_uTexturesWithAlphaChannel |= (1<<uInLayer);
	else
		m_uTexturesWithAlphaChannel &= ~(1<<uInLayer);

	IFXRELEASE( pPalette );

	return result;
}


IFXRESULT CIFXShaderLitTexture::SetTextureMode( U32         uInLayer,
											   TextureMode eInMode   )
{
	IFXRESULT result = IFX_OK;

	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	m_eTextureMode[uInLayer] = eInMode;

	if ( m_pUVMapper[uInLayer] == NULL )
	{
		result = IFXCreateComponent( CID_IFXUVGenerator, 
			IID_IFXUVGenerator, 
			(void**)(&m_pUVMapper[uInLayer]) );
	}

	if ( IFXSUCCESS( result ) )
	{
		switch ( eInMode )
		{
		case TM_NONE:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_NONE);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_PLANAR:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_NONE);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_PLANAR;
			break;
		case TM_CYLINDRICAL:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_NONE);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_CYLINDRICAL;
			break;
		case TM_SPHERICAL:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_NONE);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_SPHERICAL;
			break;
		case TM_REFLECTION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_REFLECTION_SPHERE);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
#ifdef SPEC_COVERS_CUBE_MAPS
		case TM_LOCALPOSITION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_LOCALPOSITION);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_LOCALNORMAL:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_LOCALNORMAL);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_LOCALREFLECTION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_LOCALREFLECTION);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_WORLDPOSITION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_WORLDPOSITION);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_WORLDNORMAL:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_WORLDNORMAL);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_WORLDREFLECTION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_WORLDREFLECTION);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_VIEWPOSITION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_VIEWPOSITION);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_VIEWNORMAL:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_VIEWNORMAL);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
		case TM_VIEWREFLECTION:
			m_pTexUnits[uInLayer].SetTexCoordGen(IFX_TEXGEN_VIEWREFLECTION);
			m_sUVMapperParams[uInLayer].eWrapMode = IFX_UV_NONE;
			break;
#endif
		default:
			result = IFX_E_INVALID_RANGE;
		}
	}

	return result;
}

U32 CIFXShaderLitTexture::GetChannels( void )
{
	return m_uChannels; 
}

U32 CIFXShaderLitTexture::GetFlags( void )
{
	return m_uFlags; 
}

U32 CIFXShaderLitTexture::GetAlphaTextureChannels( void )
{
	return m_uTexturesWithAlphaChannel; 
}



U32 CIFXShaderLitTexture::GetMaterialID( void )
{
	return m_uMaterialID; 
}


IFXRESULT CIFXShaderLitTexture::SetMaterialID( U32 uInMaterialID )
{
	m_uMaterialID = uInMaterialID;

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::GetTextureID( U32 uInLayer, U32* puOutID )
{
	if ( puOutID == NULL )
		return IFX_E_INVALID_POINTER;

	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	*puOutID = m_uTextureID[uInLayer];

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::SetTextureID( U32 uInLayer, U32 uInID )
{
	IFXRESULT result = IFX_OK;

	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		result = IFX_E_INVALID_RANGE;

	if ( IFXSUCCESS(result) )
		m_uTextureID[uInLayer] = uInID;

	return result;
}


IFXRESULT CIFXShaderLitTexture::GetTextureIntensity( U32 uInLayer, F32* pfOutIntensity )
{
	if ( pfOutIntensity == NULL )
		return IFX_E_INVALID_POINTER;

	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	*pfOutIntensity = m_fTextureIntensity[uInLayer];

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::SetTextureIntensity( U32 uInLayer, F32 fInIntensity )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	m_fTextureIntensity[uInLayer] = fInIntensity;

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::GetBlendFunction( U32 uInLayer,
												 BlendFunction* peOut )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	if ( peOut == NULL )
		return IFX_E_INVALID_POINTER;

	*peOut = m_eBlendFunction[uInLayer];
	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::SetBlendFunction( U32 uInLayer,
												 BlendFunction eInFn  )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	m_eBlendFunction[uInLayer] = eInFn;
	m_pTexUnits[uInLayer].SetRGBBlendFunc((IFXenum)eInFn);
	m_pTexUnits[uInLayer].SetAlphaBlendFunc((IFXenum)eInFn);

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::GetBlendSource( U32 uInLayer,
											   BlendSource* peOut   )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	if ( peOut == NULL )
		return IFX_E_INVALID_POINTER;

	*peOut = m_eBlendSource[uInLayer];
	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::SetBlendSource( U32 uInLayer,
											   BlendSource eIn      )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	m_eBlendSource[uInLayer] = eIn;

	if(eIn == ALPHA)
	{
		m_pTexUnits[uInLayer].SetRGBInterpolatorSource(IFX_TEXTURE | IFX_ALPHA);
		m_pTexUnits[uInLayer].SetAlphaInterpolatorSource(IFX_TEXTURE | IFX_ALPHA);
	}
	else
	{
		m_pTexUnits[uInLayer].SetRGBInterpolatorSource(IFX_CONSTANT | IFX_ALPHA);
		m_pTexUnits[uInLayer].SetAlphaInterpolatorSource(IFX_CONSTANT | IFX_ALPHA);
	}

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::GetBlendConstant( U32 uInLayer,
												 F32* pfOutConstant )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	if ( pfOutConstant == NULL )
		return IFX_E_INVALID_POINTER;

	*pfOutConstant = m_fBlendConstant[uInLayer];
	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::SetBlendConstant( U32 uInLayer,
												 F32 fInConstant )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	m_fBlendConstant[uInLayer] = fInConstant;
	m_pTexUnits[uInLayer].SetConstantColor(IFXVector4(1,1,1,fInConstant));

	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::GetTextureMode( U32 uInLayer,
											   TextureMode* peOut   )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	if ( peOut == NULL )
		return IFX_E_INVALID_POINTER;

	*peOut = m_eTextureMode[uInLayer];
	return IFX_OK;
}


IFXRESULT CIFXShaderLitTexture::GetTextureTransform(    U32 uInLayer,
													IFXMatrix4x4** ppmOutTextureTransform )
{
	if ( ppmOutTextureTransform == NULL )
		return IFX_E_INVALID_POINTER;

	*ppmOutTextureTransform = &(m_pTexTransforms[uInLayer]);

	return IFX_OK;
}

IFXRESULT CIFXShaderLitTexture::SetTextureTransform(    U32 uInLayer,
													IFXMatrix4x4* pmInTextureTransform  )
{
	m_pTexTransforms[uInLayer] = *pmInTextureTransform;
	return IFX_OK;
}

IFXRESULT CIFXShaderLitTexture::GetWrapTransform(       U32 uInLayer,
												 IFXMatrix4x4** ppmOutWrapTransform )
{
	if ( ppmOutWrapTransform == NULL )
		return IFX_E_INVALID_POINTER;

	*ppmOutWrapTransform = &(m_sUVMapperParams[uInLayer].mWrapTransformMatrix);

	return IFX_OK;
}

IFXRESULT CIFXShaderLitTexture::SetWrapTransform(       U32 uInLayer,
												 IFXMatrix4x4* pmInWrapTransform  )
{
	m_sUVMapperParams[uInLayer].mWrapTransformMatrix = *pmInWrapTransform;
	return IFX_OK;
}

IFXRESULT CIFXShaderLitTexture::GetTextureRepeat( U32 uInLayer,
												 U8* puOutTextureRepeat   )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	if ( puOutTextureRepeat == NULL )
		return IFX_E_INVALID_POINTER;

	*puOutTextureRepeat = m_uTextureRepeat[uInLayer];

	return IFX_OK;
}

IFXRESULT CIFXShaderLitTexture::SetTextureRepeat( U32 uInLayer,
												 U8 uInTextureRepeat )
{
	if ( uInLayer > (IFX_MAX_TEXUNITS-1) )
		return IFX_E_INVALID_RANGE;

	m_uTextureRepeat[uInLayer] = uInTextureRepeat;

	return IFX_OK;
}

// Drawing Faces and/or lines support
IFXRESULT CIFXShaderLitTexture::SetDrawFaces(BOOL bEnable)
{
	m_bDrawFaces = bEnable;

	return IFX_OK;
}

IFXRESULT CIFXShaderLitTexture::SetDrawLines(BOOL bEnable)
{
	m_bDrawLines = bEnable;

	return IFX_OK;
}

BOOL CIFXShaderLitTexture::GetDrawFaces()
{
	return m_bDrawFaces;
}

BOOL CIFXShaderLitTexture::GetDrawLines()
{
	return m_bDrawLines;
}

IFXRESULT CIFXShaderLitTexture::SetDrawPoints(BOOL bEnable)
{
	m_bDrawPoints = bEnable;

	return IFX_OK;
}

BOOL CIFXShaderLitTexture::GetDrawPoints()
{
	return m_bDrawPoints;
}

IFXRESULT CIFXShaderLitTexture::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT rc = CIFXMarker::SetSceneGraph( pInSceneGraph );

	return rc;
}
