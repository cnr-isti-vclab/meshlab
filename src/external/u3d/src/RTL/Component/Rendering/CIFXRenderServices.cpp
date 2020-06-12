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
// CIFXRenderServices.cpp

#include "IFXRenderPCH.h"
#include <string.h>
#include <ctype.h>
#include "CIFXRenderServices.h"
#include "IFXOSRender.h"
#include "IFXOSLoader.h"

CIFXRenderServices* CIFXRenderServices::ms_pSingleton = 0;

//===============================
// Factory Function - Singleton
//===============================
IFXRESULT IFXAPI_CALLTYPE CIFXRenderServicesFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if(ppUnk)
	{
		CIFXRenderServices* pPtr = CIFXRenderServices::GetSingleton();
		if(!pPtr)
		{
			pPtr = new CIFXRenderServices;
			if(pPtr)
			{
				rc = pPtr->Construct();
				if(IFXFAILURE(rc))
				{
					IFXDELETE(pPtr);
				}
			}
			else
			{
				rc = IFX_E_OUT_OF_MEMORY;
			}
		}

		if(IFXSUCCESS(rc))
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

U32 CIFXRenderServices::AddRef()
{
	return ++m_refCount;
}

U32 CIFXRenderServices::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		ms_pSingleton = NULL;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXRenderServices::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXRenderServices)
		{
			*(IFXRenderServices**)ppInterface = (IFXRenderServices*) this;
		}
		else
		if (interfaceId == CID_IFXRenderServices)
		{
			*(CIFXRenderServices**)ppInterface = (CIFXRenderServices*) this;
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

//===============================
// Public Methods
//===============================
CIFXRenderServices::CIFXRenderServices()
{
	m_refCount = 0;
}

CIFXRenderServices::~CIFXRenderServices()
{
	IFXDELETE(m_pRenderTypes);
	IFXDELETE(m_pRenderContexts);
	IFXDELETE_ARRAY(m_pMonitors);
	IFXDELETE_ARRAY(m_puBestRenderers);

	ms_pSingleton = 0;
}

IFXRESULT CIFXRenderServices::Construct()
{
	IFXRESULT rc = IFX_OK;

	ms_pSingleton = this;
	m_pMonitors = 0;
	m_uNumMonitors = 0;
	m_uNumDevices = 0;
	m_pRenderTypes = 0;
	m_puBestRenderers = 0;
	m_pRenderContexts = 0;
	m_uSafeRenderId = 0;
	m_uRequestedRenderer = 0;
	m_bVSyncEnabled = FALSE;

	m_eTextureRenderFormat = IFX_RGBA_8888;

	m_eDepthBufferFormat = IFX_DEPTH_D32S0;

	m_iInitialized = IFX_E_INVALID_RANGE;

	m_eAAMode = IFX_AA_4X;
	m_bAAEnabled = FALSE;

	// The first thing we have to do is Determine how many video cards are
	// in the system.
	GetNumDevices();

	m_puBestRenderers = new U32[m_uNumDevices];
	U32 i;
	for( i = 0; i < m_uNumDevices; i++)
	{
		m_puBestRenderers[i] = 0;
	}

	if(IFXFAILURE(rc))
	{
		m_iInitialized = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::ResetSupportedInfo()
{
	IFXRESULT rc = m_iInitialized;

	CIFXRenderType* pRType = m_pRenderTypes;

	while(pRType)
	{
		pRType->m_bCheckedForSupport = FALSE;
		pRType->m_bRecommended = TRUE;
		pRType = pRType->m_pNext;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::SetNotSupported(U32 uRenderId)
{
	IFXRESULT rc = m_iInitialized;

	CIFXRenderType* pRType = FindRenderType(uRenderId);

	if(pRType)
	{
		pRType->m_bCheckedForSupport = TRUE;
		pRType->m_bRecommended = FALSE;
		pRType->m_bSupported = FALSE;
	}

	return rc;
}

const IFXMonitor* CIFXRenderServices::GetMonitors(U32& uNumMonitors) const
{
	uNumMonitors = m_uNumMonitors;
	return m_pMonitors;
}

IFXRESULT CIFXRenderServices::DeleteTexture(U32 uTextureId)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		CIFXRenderType* pRType = m_pRenderTypes;
		while (pRType)
		{
			U32 i;
			for( i = 0; i<m_uNumDevices; i++)
			{
				if(pRType->m_pDevices[i].IsValid())
				{
					pRType->m_pDevices[i]->DeleteTexture(uTextureId);
				}
			}
			pRType = pRType->m_pNext;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderServices::DeleteStaticMesh(U32 uId)
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		CIFXRenderType* pRType = m_pRenderTypes;
		while (pRType)
		{
			U32 i;
			for( i = 0; i<m_uNumDevices; i++)
			{
				if(pRType->m_pDevices[i].IsValid())
				{
					pRType->m_pDevices[i]->DeleteMesh(uId);
				}
			}
			pRType = pRType->m_pNext;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderServices::RegisterRenderer(U32 uRenderId,
											   IFXREFIID cidRender,
											   IFXREFIID cidRenderDevice )
{
	IFXRESULT rc = IFX_OK;

	// First search all registered renderers for this name
	CIFXRenderType* pRType = FindRenderType(uRenderId);
	if (!pRType) {
		//CIFXRenderType* pRType = new CIFXRenderType(uRenderId, cidRender, cidRenderDevice);
		pRType = new CIFXRenderType(uRenderId, cidRender, cidRenderDevice);
		pRType->m_pDevices = new IFXRenderDevicePtr[m_uNumDevices];
		pRType->m_pNext = m_pRenderTypes;
		m_pRenderTypes = pRType;
	} else {
//		rc = IFX_E_INVALID_RANGE;
		CIFXRenderType* pRType1 = new CIFXRenderType(uRenderId, cidRender, cidRenderDevice);
		pRType1->m_pDevices = new IFXRenderDevicePtr[m_uNumDevices];
		pRType1->m_pNext = pRType->m_pNext;
		CIFXRenderType* p = m_pRenderTypes;
		if (p == pRType) {
			m_pRenderTypes = pRType1;
		} else {
			while (p) {
				if (p->m_pNext == pRType)
					break;
				p = p->m_pNext;
			}
			p->m_pNext = pRType1;
		}
		delete pRType;
	}

	if(IFXSUCCESS(rc))
	{
		if(IFXFAILURE(m_iInitialized))
		{
			// Set the first supported renderer as the safe render id
			if(IFXSUCCESS(SetSafeRenderId(uRenderId)))
			{
				m_iInitialized = IFX_OK;
			}
		}
	}

	return rc;
}

IFXRESULT CIFXRenderServices::CreateRenderer(U32& uRenderId,
											 IFXRenderPtr& rRender,
											 U32 uMonitorNum )
{
	IFXRESULT rc = m_iInitialized;

	if(IFXSUCCESS(rc))
	{
		if(uMonitorNum < m_uNumMonitors)
		{
			U32 uDeviceNum = m_pMonitors[uMonitorNum].m_uDeviceNum;
			U32 uNewRenderId = m_uRequestedRenderer;

			if(uRenderId && FindRenderType(uRenderId))
			{
				uNewRenderId = uRenderId;
			}

			rc = CreateRenderer(rRender, uDeviceNum, uNewRenderId);

			if(IFXFAILURE(rc))
			{
				uNewRenderId = m_puBestRenderers[uDeviceNum];
				rc = CreateRenderer(rRender, uDeviceNum, uNewRenderId);
			}

			if(IFXFAILURE(rc))
			{
				uNewRenderId = m_uSafeRenderId;
				rc = CreateRenderer(rRender, uDeviceNum, uNewRenderId);
			}

			if(uRenderId != uNewRenderId)
			{
				uRenderId = 0;
			}

		}
		else
		{
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderServices::CreateRenderer(IFXRenderPtr& rRender,
											 U32 uDeviceNum,
											 U32 uRenderId)
{
	IFXRESULT rc = m_iInitialized;

	CIFXRenderType* pRType = FindRenderType(uRenderId);

	if(pRType)
	{
		if(pRType->GetSupported(this, uDeviceNum))
		{
			rc = rRender.Create(pRType->m_idRender, IID_IFXRender);
		}
		else
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}
	else
	{
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::GetRenderDevice(IFXREFCID cidRenderDevice,
											  U32 uDeviceNum,
											  IFXRenderDevicePtr& rRenderDevice)
{
	IFXRESULT rc = IFX_OK;

	if(uDeviceNum >= m_uNumDevices)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{

		CIFXRenderType* pRType = FindRenderType(cidRenderDevice);

		if(pRType == 0)
		{
			rc = IFX_E_INVALID_RANGE;
		}
		else
		{
			if(! pRType->m_pDevices[uDeviceNum].IsValid())
			{
				IFXRUN(rc, pRType->m_pDevices[uDeviceNum].Create(pRType->m_idRenderDevice, IID_IFXRenderDevice));
				IFXRUN(rc, pRType->m_pDevices[uDeviceNum]->Initialize(uDeviceNum));
			}
		}

		if(IFXSUCCESS(rc))
		{
			rRenderDevice = pRType->m_pDevices[uDeviceNum];
		}
	}

	return rc;
}

BOOL CIFXRenderServices::GetSupported(U32 uRenderId)
{
	IFXRESULT rc = IFX_OK;
	BOOL bSupported = FALSE;

	if(IFXSUCCESS(rc))
	{
		CIFXRenderType* pType = FindRenderType(uRenderId);

		if(pType)
		{
			U32 i;
			for( i = 0; i < m_uNumDevices; i++)
			{
				bSupported = pType->GetSupported(this, i);
				if(bSupported)
				{
					break;
				}
			}
		}
	}

	return bSupported;
}

BOOL CIFXRenderServices::GetRecommended(U32 uRenderId)
{
	BOOL bRec = FALSE;

	CIFXRenderType* pRType = FindRenderType(uRenderId);

	if(pRType)
	{
		bRec = pRType->m_bRecommended;
	}

	return bRec;
}

IFXRESULT CIFXRenderServices::SetRecommended(U32 uRenderId, BOOL bRecommended)
{
	IFXRESULT rc = m_iInitialized;

	CIFXRenderType* pRType = FindRenderType(uRenderId);

	if(pRType)
	{
		pRType->m_bRecommended = bRecommended;
	}

	return rc;
}

U32 CIFXRenderServices::GetRequestedRenderId()
{

	return m_uRequestedRenderer;
}

IFXRESULT CIFXRenderServices::SetRequestedRenderId(U32 uRenderId)
{
	IFXRESULT rc = m_iInitialized;

	if(uRenderId != m_uRequestedRenderer)
	{
		if(IFXSUCCESS(rc))
		{
			BOOL bSupported = GetSupported(uRenderId);
			if(bSupported)
			{
				m_uRequestedRenderer = uRenderId;
			}
			else
			{
				rc = IFX_E_UNSUPPORTED;
			}
		}

		if(IFXSUCCESS(rc))
		{
			CIFXRCListEntry* pLE = m_pRenderContexts;

			while(pLE)
			{
				if(!pLE->m_pRC->GetRenderer())
				{
					pLE->m_pRC->Reset();
				}
				pLE = pLE->m_pNext;
			}
		}
	}

	return rc;
}

U32 CIFXRenderServices::GetSafeRenderId()
{
	return m_uSafeRenderId;
}

IFXRESULT CIFXRenderServices::SetSafeRenderId(U32 uRenderId)
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		if(GetSupported(uRenderId))
		{
			m_uSafeRenderId = uRenderId;
		}
		else
		{
			rc = IFX_E_UNSUPPORTED;
		}
	}

	return rc;
}

IFXRESULT CIFXRenderServices::SetTextureRenderFormat(IFXenum eTexFormat)
{
	IFXRESULT rc = IFX_OK;

	switch(eTexFormat)
	{
	case IFX_RGBA_8888:
	case IFX_RGBA_8880:
	case IFX_RGBA_5650:
	case IFX_RGBA_5550:
	case IFX_RGBA_5551:
	case IFX_RGBA_4444:
		m_eTextureRenderFormat = eTexFormat;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::SetPixelAspectRatio(F32 fRatio)
{
	IFXRESULT rc = m_iInitialized;

	if(fRatio > 0)
	{
		m_fPixelAspectRatio = fRatio;
	}
	else
	{
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::SetDepthBufferFormat(IFXenum eDepthFormat)
{
	IFXRESULT rc = m_iInitialized;

	switch(eDepthFormat)
	{
	case IFX_DEPTH_D16S0:
	case IFX_DEPTH_D32S0:
	case IFX_DEPTH_D24S8:
		m_eDepthBufferFormat = eDepthFormat;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::SetVSyncEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bVSyncEnabled = bEnabled;

	CIFXRCListEntry* pLE = m_pRenderContexts;

	while(pLE && IFXSUCCESS(rc))
	{
		if(bEnabled)
		{
			rc = pLE->m_pRC->Enable(IFX_VSYNC);
		}
		else
		{
			rc = pLE->m_pRC->Disable(IFX_VSYNC);
		}

		pLE = pLE->m_pNext;
	}

	return rc;
}

BOOL CIFXRenderServices::GetVSyncEnabled()
{
	return m_bVSyncEnabled;
}


IFXenum CIFXRenderServices::GetTextureRenderFormat()
{
	return m_eTextureRenderFormat;
}

F32 CIFXRenderServices::GetPixelAspectRatio()
{
	return m_fPixelAspectRatio;
}

IFXenum CIFXRenderServices::GetDepthBufferFormat()
{
	return m_eDepthBufferFormat;
}

BOOL CIFXRenderServices::GetDefaultAAEnalbed()
{
	return m_bAAEnabled;
}

IFXRESULT CIFXRenderServices::SetDefaultAAEnabled(BOOL bEnabled)
{
	m_bAAEnabled = bEnabled;

	return IFX_OK;
}

IFXenum CIFXRenderServices::GetDefaultAAMode()
{
	return m_eAAMode;
}

IFXRESULT CIFXRenderServices::SetDefaultAAMode(IFXenum eAAMode)
{
	IFXRESULT rc = IFX_OK;

	switch(eAAMode)
	{
	case IFX_AA_4X_SW:
	case IFX_AA_4X:
	case IFX_AA_3X:
	case IFX_AA_2X:
		m_eAAMode = eAAMode;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::RegisterRenderContext(IFXRenderContext* pRC)
{
	IFXRESULT rc = IFX_OK;

	if(pRC)
	{
		CIFXRCListEntry* pLE = FindRC(pRC);
		if(0 == pLE)
		{
			pLE = new CIFXRCListEntry;
			if(pLE)
			{
				pLE->m_pRC = pRC;
				pLE->InsertBefore(m_pRenderContexts);
				m_pRenderContexts = pLE;
			}
			else
			{
				rc = IFX_E_OUT_OF_MEMORY;
			}
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::UnregisterRenderContext(IFXRenderContext* pRC)
{
	IFXRESULT rc = IFX_OK;

	if(pRC)
	{
		CIFXRCListEntry* pLE = FindRC(pRC);
		if(pLE)
		{
			if(pLE == m_pRenderContexts)
			{
				m_pRenderContexts = pLE->m_pNext;
			}
			pLE->RemoveFromList();
			IFXDELETE(pLE);
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}

	// Since a render context has gone away, we should purge any devices that are
	// no longer in use.
	IFXRUN(rc, PurgeDevices());

	return rc;
}

IFXRESULT CIFXRenderServices::PurgeDevices()
{
	IFXRESULT rc = IFX_OK;

	CIFXRenderType* pRType = m_pRenderTypes;

	while(pRType)
	{
		U32 i;
		for( i = 0; i < m_uNumDevices; i++)
		{
			// The DecRef() followed by IncRef() will delete any
			// devices where the render services object is the
			// sole reference holder.  All other devices will be
			// unaffected.
			pRType->m_pDevices[i].DecRef();
			pRType->m_pDevices[i].IncRef();
		}

		pRType = pRType->m_pNext;
	}

	return rc;
}

IFXRESULT CIFXRenderServices::Reset()
{
	IFXRESULT rc = IFX_OK;

	CIFXRCListEntry* pLE = m_pRenderContexts;

	while(pLE)
	{
		pLE->m_pRC->Reset();

		pLE = pLE->m_pNext;
	}

	PurgeDevices();

	return rc;
}


CIFXRenderType* CIFXRenderServices::FindRenderType(IFXREFCID cidRenderDevice)
{
	CIFXRenderType* pRType = m_pRenderTypes;

	while(pRType)
	{
		if(pRType->m_idRenderDevice == cidRenderDevice)
		{
			break;
		}
		pRType = pRType->m_pNext;
	}

	return pRType;
}

CIFXRenderType* CIFXRenderServices::FindRenderType(U32 uRenderId)
{
	CIFXRenderType* pRType = m_pRenderTypes;

	while(pRType)
	{
		if(pRType->m_uId == uRenderId)
		{
			break;
		}

		pRType = pRType->m_pNext;
	}

	return pRType;
}

CIFXRCListEntry* CIFXRenderServices::FindRC(IFXRenderContext* pRC)
{
	CIFXRCListEntry* pLE = m_pRenderContexts;

	while(pLE)
	{
		if(pLE->m_pRC == pRC)
		{
			break;
		}

		pLE = pLE->m_pNext;
	}

	return pLE;
}

//===============================
// CIFXRenderType
//===============================
CIFXRenderType::CIFXRenderType(U32 uRenderId,
							   IFXREFIID idRender,
							   IFXREFIID idRenderDevice) :
m_uId(uRenderId),
m_idRender(idRender),
m_idRenderDevice(idRenderDevice)
{
	m_pDevices = 0;
	m_pNext = 0;
	m_bRecommended = TRUE;
	m_bSupported = FALSE;
	m_bCheckedForSupport = FALSE;
}

CIFXRenderType::~CIFXRenderType()
{
	IFXDELETE_ARRAY(m_pDevices);
	IFXDELETE(m_pNext);
}

CIFXRenderType& CIFXRenderType::operator=(const CIFXRenderType& operand)
{
	return *this;
}

BOOL CIFXRenderType::GetSupported(CIFXRenderServices* pServices, U32 uDeviceNum/* =0 */)
{
	if(!m_bCheckedForSupport)
	{
		m_bCheckedForSupport = TRUE;

		IFXRESULT rc = IFX_OK;

		BOOL bCreated = FALSE;
		if(!m_pDevices[uDeviceNum].IsValid())
		{
			IFXRUN(rc, m_pDevices[uDeviceNum].Create(m_idRenderDevice, IID_IFXRenderDevice));
			bCreated = TRUE;
		}

		IFXRUN(rc, m_pDevices[uDeviceNum]->GetSupported(uDeviceNum, m_bSupported));

		if(IFXFAILURE(rc))
		{
			m_bSupported = FALSE;
		}

		if(bCreated)
		{
			if(m_bSupported)
			{
				IFXRUN(rc, m_pDevices[uDeviceNum]->Initialize(uDeviceNum));
				if(IFXFAILURE(rc))
				{
					m_bSupported = FALSE;
				}
			}

			if(!m_bSupported)
			{
				m_pDevices[uDeviceNum].DecRef();
			}
		}
	}

	return m_bSupported;
}

//===============================
// CIFXRCListEntry
//===============================
CIFXRCListEntry::CIFXRCListEntry()
{
	m_pRC = 0;
	m_pPrev = 0;
	m_pNext = 0;
}

CIFXRCListEntry::~CIFXRCListEntry()
{
	m_pRC = 0;
	IFXDELETE(m_pNext);
}

void CIFXRCListEntry::RemoveFromList()
{
	if(m_pPrev)
	{
		m_pPrev->m_pNext = m_pNext;
	}
	if(m_pNext)
	{
		m_pNext->m_pPrev = m_pPrev;
	}
	m_pNext = 0;
	m_pPrev = 0;
}

void CIFXRCListEntry::InsertBefore(CIFXRCListEntry* pListEntry)
{
	m_pNext = pListEntry;
	if(m_pNext)
	{
		m_pPrev = m_pNext->m_pPrev;
		m_pNext->m_pPrev = this;
	}
	if(m_pPrev)
	{
		m_pPrev->m_pNext = this;
	}
}


//***************************************************************************

U32 CIFXRenderServices::GetNumDevices()
{
	U32 i;

	IFXDISPLAY_DEVICE dd;

	U32 uDevNum = 0;
	m_uNumDevices = 0;
	
	U32 bVal = 1;

	// First count how many devices there are
	while(bVal)
	{
		memset(&dd, 0, sizeof(dd));
		dd.cb = sizeof(dd);
		IFXEnumDisplayDevices(NULL, uDevNum, &dd, 0, &bVal);
		if(bVal)
		{
			if(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP &&
				!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				m_uNumDevices++;
			}
		}
		uDevNum++;
	}

	// Now, record the string names for each device for posterity
	IFXString* pStrings = new IFXString[m_uNumDevices];
	bVal = 1;
	uDevNum = 0;
	U32 uTempDevNum = 0;
	while(bVal)
	{
		memset(&dd, 0, sizeof(dd));
		dd.cb = sizeof(dd);
		IFXEnumDisplayDevices(NULL, uDevNum, &dd, 0, &bVal);
		if(bVal)
		{
			if(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP &&
				!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				pStrings[uTempDevNum].Assign(dd.DeviceName);
				uTempDevNum++;
			}
		}
		uDevNum++;
	}
	
	// Now find out about all the monitors attached to the gathered devices
	m_uNumMonitors = IFXGetNumMonitors();
	IFXDELETE_ARRAY(m_pMonitors);
	m_pMonitors = new IFXMonitor[m_uNumMonitors];
	// TODO: Handle distinction between monitor and pseudo-monitor.
	// IFXGetNumMonitors returns number of actual monitors
	// IFXEnumDisplayMonitors goes through all monitors and pseudo-monitors.
	// NetMeeting adds a pseudo-monitor resulting in a memory overwrite.
	// To avoid the memory overwrite, the array size for pScreenArr and pDeviceArr
	// have been increased by one.
	IFXRect* pScreenArr = new IFXRect[m_uNumMonitors+1];
	U32* pDeviceArr = new U32[m_uNumMonitors+1];
	if( pScreenArr && pDeviceArr )
	{
		IFXEnumDisplayMonitors(NULL, 0, m_uNumDevices, pStrings, pScreenArr, pDeviceArr);
		for( i = 0; i < m_uNumMonitors; i++ )
		{
			m_pMonitors[i].m_rcScreen = pScreenArr[i];
			m_pMonitors[i].m_uDeviceNum = pDeviceArr[i];
		}
		delete [] pScreenArr;
		delete [] pDeviceArr;
	}
	IFXDELETE_ARRAY(pStrings);

	return m_uNumDevices;
}

U32 CIFXRenderServices::GetColorBufferDepth()
{
	return IFXGetColorBufferDepth();
}
