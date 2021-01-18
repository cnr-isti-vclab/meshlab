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
//  CIFXShaderLitTexture.h
//

#ifndef __CIFXSHADERLITTEXTURE_H__
#define __CIFXSHADERLITTEXTURE_H__

#include "IFXShaderLitTexture.h"
#include "IFXTextureObject.h"
#include "IFXSceneGraph.h"
#include "IFXView.h"
#include "CIFXShader.h"
#include "IFXUVGenerator.h"
#include "IFXSimpleHash.h"
#include "IFXDids.h"

#include "IFXRenderContext.h"

class IFXMaterialResource;
class IFXModifierDataPacket;

class CIFXShaderLitTexture : public  CIFXShader,
					 virtual public   IFXShaderLitTexture
{
public:
	friend IFXRESULT IFXAPI_CALLTYPE CIFXShaderLitTexture_Factory(IFXREFIID iid, void** ppv);
	
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);
	
	// IFXMarker
	IFXRESULT IFXAPI   Mark();
	void      IFXAPI   SetPriority( U32 uInPriority, BOOL bRecursive, BOOL bPromotionOnly );
	
	// IFXMarkerX
	IFXRESULT IFXAPI   SetSceneGraph(IFXSceneGraph* pInSceneGraph);
	void IFXAPI       GetEncoderX (IFXEncoderX*& rpEncoderX);
	
	
	// IFXShader
	IFXShaderOpacity IFXAPI Opacity();
	IFXRESULT IFXAPI Render( IFXRenderContext& rInRenderer, U32 uRenderPass );
	
	// IFXShaderLitTexture
	IFXRESULT UpdateAlphaCacheBits( U32 uInLayer, U32 uInID );
	
	U32       IFXAPI   GetChannels();
	IFXRESULT IFXAPI   SetChannels(U32 uInEnabledFlags);

	U32       IFXAPI   GetFlags();
	IFXRESULT  IFXAPI 	 SetFlags(U32 uInEnabledFlags);

	U32       IFXAPI   GetAlphaTextureChannels();
	IFXRESULT  IFXAPI 	 SetAlphaTextureChannels( U32  uInEnabledFlags );
	
	IFXRenderBlend&	 IFXAPI GetRenderBlend();
	BOOL	  IFXAPI   GetAlphaTestEnabled();
	IFXRESULT  IFXAPI 	 SetAlphaTestEnabled( BOOL bInEnabled );
	
	BOOL	  IFXAPI   GetLightingEnabled();
	IFXRESULT  IFXAPI 	 SetLightingEnabled( BOOL bInEnabled );
	
	IFXRenderMaterial& IFXAPI GetRenderMaterial();
	
	U32       IFXAPI   GetMaterialID();
	IFXRESULT  IFXAPI 	 SetMaterialID( U32 uInMaterialID );
	
	IFXRESULT  IFXAPI 	 GetTextureID( U32  uInLayer, U32* puOutID );
	IFXRESULT  IFXAPI 	 SetTextureID( U32  uInLayer, U32  uInID );
	
	IFXRESULT  IFXAPI 	 GetTextureIntensity( U32  uInLayer, F32* pfOutIntensity );
	IFXRESULT  IFXAPI 	 SetTextureIntensity( U32  uInLayer, F32  fInIntensity   );
	
	U32       GetOpacityID();
	IFXRESULT  IFXAPI 	 SetOpacityID( U32 uInOpacityMapID );
	
	F32       GetOpacityIntensity();
	IFXRESULT  IFXAPI 	 SetOpacityIntensity( F32 fInIntensity    );
	
	IFXRESULT  IFXAPI 	 GetBlendFunction( U32 uInLayer, BlendFunction* peOut );
	IFXRESULT  IFXAPI 	 SetBlendFunction( U32 uInLayer, BlendFunction eInFn  );
	
	IFXRESULT  IFXAPI 	 GetBlendSource( U32 uInLayer, BlendSource* peOut   );
	IFXRESULT  IFXAPI 	 SetBlendSource( U32 uInLayer, BlendSource eIn      );
	
	IFXRESULT  IFXAPI 	 GetBlendConstant( U32 uInLayer, F32* pfOutConstant );
	IFXRESULT  IFXAPI 	 SetBlendConstant( U32 uInLayer, F32 fInConstant );
	
	IFXRESULT  IFXAPI 	 GetTextureMode( U32 uInLayer, TextureMode* peOut   );
	IFXRESULT  IFXAPI 	 SetTextureMode( U32 uInLayer, TextureMode  eInMode );
	
	IFXRESULT  IFXAPI 	 GetTextureTransform( U32 uInLayer, IFXMatrix4x4** ppmOutTextureTransform );
	IFXRESULT  IFXAPI 	 SetTextureTransform( U32 uInLayer, IFXMatrix4x4* pmInTextureTransform  );
	
	IFXRESULT  IFXAPI 	 GetWrapTransform( U32 uInLayer, IFXMatrix4x4** ppmOutWrapTransform );
	IFXRESULT  IFXAPI 	 SetWrapTransform( U32 uInLayer, IFXMatrix4x4* pmInWrapTransform  );
	
	IFXRESULT  IFXAPI 	 GetTextureRepeat( U32 uInLayer, U8* puOutTextureRepeat   );
	IFXRESULT  IFXAPI 	 SetTextureRepeat( U32 uInLayer, U8  uInTextureRepeat );
	
	BOOL IFXAPI   GetUseDiffuse( void )
	{
		return (m_uFlags&USEDIFFUSE)>0;
	}

	void IFXAPI   SetUseDiffuse( BOOL bUseDiffuse ) 
	{ 
		if (bUseDiffuse)
			m_uFlags |= USEDIFFUSE;
		else 
			m_uFlags &= (0xffffffff^USEDIFFUSE);
	}

	IFXRESULT  IFXAPI 	 SetInvertTexTransforms(BOOL bInvert)
	{
		m_bInvertTrans = bInvert;
		return IFX_OK;
	}
	BOOL   IFXAPI   GetInvertTexTransforms() const
	{
		return m_bInvertTrans;
	}

	// These are for the #standard shader only.
	IFXRESULT  IFXAPI 	 SetDrawFaces(BOOL bEnable);
	BOOL	IFXAPI   GetDrawFaces();

	IFXRESULT  IFXAPI 	 SetDrawLines(BOOL bEnable);
	BOOL	IFXAPI   GetDrawLines();

	IFXRESULT  IFXAPI 	 SetDrawPoints(BOOL bEnable);
	BOOL	IFXAPI   GetDrawPoints();
	
protected:
	CIFXShaderLitTexture();
	virtual  ~CIFXShaderLitTexture();

	IFXRESULT Construct();

	IFXRESULT RenderShaderLitTexture(IFXRenderContext& rRenderer);
	IFXRESULT RenderShaderPainter(IFXRenderContext& rRenderer);
	IFXRESULT RenderShaderInker(IFXRenderContext& rRenderer);
	IFXRESULT RenderShaderNPR(IFXRenderContext& rRenderer);

	// IFXUnknown
	U32                 m_uRefCount;

	// IFXShaderLitTexture
	U32                 m_uChannels;
	U32                 m_uFlags;
	U32                 m_uTexturesWithAlphaChannel;
	U32	                m_uMaterialID;
	IFXRenderMaterial   m_Material;

	IFXRenderTexUnit	m_pTexUnits[IFX_MAX_TEXUNITS];
	U32                 m_uTextureID[IFX_MAX_TEXUNITS];
	F32                 m_fTextureIntensity[IFX_MAX_TEXUNITS];
	BlendFunction       m_eBlendFunction[IFX_MAX_TEXUNITS];
	BlendSource         m_eBlendSource[IFX_MAX_TEXUNITS];
	TextureMode         m_eTextureMode[IFX_MAX_TEXUNITS];
	F32                 m_fBlendConstant[IFX_MAX_TEXUNITS];
	IFXUVGenerator*     m_pUVMapper[IFX_MAX_TEXUNITS];
	IFXUVMapParameters  m_sUVMapperParams[IFX_MAX_TEXUNITS];
	IFXMatrix4x4		m_pTexTransforms[IFX_MAX_TEXUNITS];
	U8	                m_uTextureRepeat[IFX_MAX_TEXUNITS];
	BOOL				m_bInvertTrans;
	
	U32                 m_uOpacityID;
	F32                 m_fOpacityIntensity;
	
	BOOL				m_bAlphaTestEnabled;
	IFXRenderBlend		m_Blend;
	
	BOOL				m_bLighting;

	// Line, face and points enables
	BOOL				m_bDrawFaces;
	BOOL				m_bDrawLines;
	BOOL				m_bDrawPoints;
};

#endif // __CIFXSHADERLITTEXTURE_H__
