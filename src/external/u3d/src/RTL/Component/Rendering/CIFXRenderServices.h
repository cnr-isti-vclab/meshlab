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
// CIFXRenderServices interface class
#ifndef CIFX_RENDERSERVICES_H
#define CIFX_RENDERSERVICES_H

#include "IFXRenderServices.h"
#include "IFXRenderDevice.h"
#include "IFXRender.h"
class CIFXRenderServices;

class CIFXRenderType
{
public:
	friend class CIFXRenderServices;

	// To get rid of warnings:
	CIFXRenderType& IFXAPI operator=(const CIFXRenderType& operand);

private:
	CIFXRenderType(U32 uRenderId, IFXREFCID idRender, IFXREFCID idRenderDevice);
	~CIFXRenderType();

	BOOL IFXAPI GetSupported(CIFXRenderServices* pServices, U32 uDeviceNum);

	U32 m_uId;
	IFXREFCID m_idRender;
	IFXREFCID m_idRenderDevice;
	IFXRenderDevicePtr* m_pDevices;
	BOOL m_bSupported;
	BOOL m_bRecommended;
	BOOL m_bCheckedForSupport;

	CIFXRenderType* m_pNext;
};

class IFXRenderContext;
class CIFXRCListEntry
{
public:
	friend class CIFXRenderServices;

private:
	CIFXRCListEntry();
	~CIFXRCListEntry();

	IFXRenderContext* m_pRC;
	CIFXRCListEntry* m_pNext;
	CIFXRCListEntry* m_pPrev;

	void IFXAPI RemoveFromList();
	void IFXAPI InsertBefore(CIFXRCListEntry* pListEntry);
};

//===============================================
// CIFXRenderServices - singleton render manager
//===============================================
class CIFXRenderServices :	virtual public IFXRenderServices, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderServicesFactory(IFXREFIID intId, void** ppUnk);
	
	//============================
	// CIFXRenderServices Methods
	//============================
	IFXRESULT IFXAPI RegisterRenderer(	U32			uRenderId, 
										IFXREFCID	cidRender,
										IFXREFCID	cidRenderDevice );

	IFXRESULT IFXAPI CreateRenderer(U32& uRenderId,
									IFXRenderPtr& rRender,
									U32 uMonitorNum);

	IFXRESULT IFXAPI GetRenderDevice(	IFXREFCID cidRenderDevice,
										U32 uDeviceNum,
										IFXRenderDevicePtr& rRenderDevice);
	
	IFXRESULT IFXAPI ResetSupportedInfo();
	IFXRESULT IFXAPI SetNotSupported(U32 uRenderId);
	BOOL	  IFXAPI GetSupported(U32 uRenderId);
	BOOL	  IFXAPI GetRecommended(U32 uRenderId);
	IFXRESULT IFXAPI SetRecommended(	U32 uRenderId, 
										BOOL bRecommended);
	U32		  IFXAPI GetRequestedRenderId();
	IFXRESULT IFXAPI SetRequestedRenderId(U32 uRenderId);
	U32		  IFXAPI GetSafeRenderId();
	IFXRESULT IFXAPI SetSafeRenderId(U32 uRenderId);

	BOOL	  IFXAPI GetDefaultAAEnalbed();
	IFXRESULT IFXAPI SetDefaultAAEnabled(BOOL bEnabled);
	IFXenum	  IFXAPI GetDefaultAAMode();
	IFXRESULT IFXAPI SetDefaultAAMode(IFXenum eAAMode);

	IFXRESULT IFXAPI SetTextureRenderFormat(IFXenum eTexFormat);
	IFXRESULT IFXAPI SetPixelAspectRatio(F32 fRatio);
	IFXRESULT IFXAPI SetDepthBufferFormat(IFXenum eDepthFormat);
	IFXRESULT IFXAPI SetVSyncEnabled(BOOL bEnabled);
	
	IFXenum	  IFXAPI GetTextureRenderFormat();
	F32		  IFXAPI GetPixelAspectRatio();
	IFXenum	  IFXAPI GetDepthBufferFormat();
	BOOL	  IFXAPI GetVSyncEnabled();

	U32		  IFXAPI GetColorBufferDepth();
	
	U32		  IFXAPI GetNumDevices();
	const IFXMonitor* IFXAPI GetMonitors(U32& uNumMonitors) const;

	IFXRESULT IFXAPI DeleteTexture(U32 uTextureId);
	IFXRESULT IFXAPI DeleteStaticMesh(U32 uId);
	
	IFXRESULT IFXAPI RegisterRenderContext(IFXRenderContext* pRC);
	IFXRESULT IFXAPI UnregisterRenderContext(IFXRenderContext* pRC);
		
	IFXRESULT IFXAPI PurgeDevices();
	IFXRESULT IFXAPI Reset();

protected:
	CIFXRenderServices();
	virtual ~CIFXRenderServices();
	
	CIFXRenderType* IFXAPI FindRenderType(IFXREFCID cidRender);
	CIFXRenderType* IFXAPI FindRenderType(U32 uRenderId);

	CIFXRCListEntry* IFXAPI FindRC(IFXRenderContext* pRC);

	IFXRESULT IFXAPI CreateRenderer(IFXRenderPtr& rRender, U32 uDeviceNum,
									U32 uRenderId);

	IFXRESULT IFXAPI Construct();
	
	/**
	 *	'Are we initialized?' flag.
	 */
	IFXRESULT		m_iInitialized;

	/**
	 *	Number of rendering devices (video cards) in the system.
	 */
	U32				m_uNumDevices;

	/**
	 *	List of active render contexts.
	 */
	CIFXRCListEntry* m_pRenderContexts;

	/**
	 *	List of best render id's (lowest renderer register that
	 *	is supported).  There is one render id for every render device
	 *	in the system.
	 */
	U32*			m_puBestRenderers;
	
	/**
	 *	The user desired render type.
	 */
	U32				m_uRequestedRenderer;
	
	/**
	 *	The id of a renderer that is guaranteed to work (this is used
	 *	as a fallback if one of the other renderers fail for some reason).
	 *	This typically is a software rasterizer.
	 */
	U32				m_uSafeRenderId;
	
	/**
	 *	Number of different render types that are registered.
	 */
	U32				m_uNumRenderTypes;
	
	/**
	 *	List of different render types that are registered.
	 */
	CIFXRenderType* m_pRenderTypes;

	/**
	 *	Number of monitors (screens) that could be used for rendering.
	 */
	U32				m_uNumMonitors;

	/**
	 *	List of monitor information structures ('IFXMonitor's) - one for
	 *	each monitor that is attached to the system.
	 */
	IFXMonitor*		m_pMonitors;

	/**
	 *	The default texture format for the system.
	 */
	IFXenum			m_eTextureRenderFormat;
	
	/**
	 *	The default pixel aspect ratio for the system (typically 1).
	 */
	F32				m_fPixelAspectRatio;
	
	/**
	 *	The default depth buffer format for the system.
	 */
	IFXenum			m_eDepthBufferFormat;

	/**
	 *	Is VSync enabled?
	 */
	BOOL			m_bVSyncEnabled;

	/**
	 *	Default AA enalbed
	 */
	BOOL			m_bAAEnabled;

	/**
	 *	Default Anti-Aliasing mode
	 */
	IFXenum			m_eAAMode;

	static CIFXRenderServices* IFXAPI GetSingleton() { return ms_pSingleton; }
	static CIFXRenderServices* ms_pSingleton;
};

typedef IFXSmartPtr<CIFXRenderServices> CIFXRenderServicesPtr;

#endif
