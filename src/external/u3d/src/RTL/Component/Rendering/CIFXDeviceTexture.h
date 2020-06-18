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
// CIFXDeviceTexture.h
#ifndef CIFX_DEVICE_TEXTURE_H
#define CIFX_DEVICE_TEXTURE_H

#include "IFXTextureObject.h"

//==============================
// CIFXDeviceTexture
//==============================
class CIFXDeviceTexture : public CIFXDeviceBase
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	//=================================
	// CIFXDeviceTexture methods
	//=================================
	virtual IFXRESULT IFXAPI SetTexture(IFXTextureObject& rTexture);

	IFXenum IFXAPI GetMipMode() const { return m_eMipMode; }
	IFXenum IFXAPI GetMinFilter() const { return m_eMinFilter; }
	IFXenum IFXAPI GetMagFilter() const { return m_eMagFilter; }
	
protected:
	CIFXDeviceTexture();
	virtual ~CIFXDeviceTexture();
	
	IFXRESULT IFXAPI SetDefaults();

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

	U32	IFXAPI	GetMaxRenderMipLevel() const;

	virtual IFXRESULT IFXAPI Lock(	IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	)=0;

	virtual IFXRESULT IFXAPI Unlock(IFXenum eType, 
									U32 uMipLevel, 
									STextureOutputInfo& rTex	)=0;

	IFXenum IFXAPI GetCompatibleRenderFormat(IFXenum eFormat);

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();


	IFXenum	m_eType;			///< Texture type
	IFXenum	m_eRenderFormat;	///< Texture render format
	U32		m_uWidth;			///< Width of the texture in texels
	U32		m_uHeight;			///< Height of the texture in texels
	U32		m_uDepth;			///< Depth of the 3D texture in texels
	U32		m_uMaxMipLevel;		///< Maximum mipmap level to use
	IFXenum	m_eMipMode;			///< Mipmap mode
	IFXenum	m_eMinFilter;		///< Minification filter
	IFXenum	m_eMagFilter;		///< Magnification filter
	BOOL	m_bDynamic;			///< If TRUE, this texture will be updated frequently.
	BOOL	m_bNeedData;
	IFXRenderCaps m_RCaps;
};
typedef IFXSmartPtr<CIFXDeviceTexture> CIFXDeviceTexturePtr;

#endif // CIFX_DEVICE_TEXTURE_H

// END OF FILE


