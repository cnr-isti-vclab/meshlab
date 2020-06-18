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
// CIFXDeviceBase.h
#ifndef CIFX_DEVICE_BASE__H
#define CIFX_DEVICE_BASE__H

#define CompareDevice(hr,a,b,func) {if(! (a == b)) { MakeDeviceDirty(); hr |= func(b); } }

class CIFXDeviceBase : virtual public IFXUnknown
{
public:
	/**
	 *	Gets the value of the m_bDirty data member.
	 *
	 *	@return TRUE if dirty, FALSE if not.
	 */
	BOOL IFXAPI IsDirty();

	BOOL IFXAPI GetEnabled() const;

	IFXRESULT IFXAPI SetEnabled(BOOL bEnabled);

	void IFXAPI MakeDeviceDirty();
protected:
	CIFXDeviceBase();
	virtual ~CIFXDeviceBase();

	virtual IFXRESULT IFXAPI SetHWEnabled(BOOL bEnabled);

	void IFXAPI InitData();

	BOOL m_bEnabled;

	/**
	 *	Holds which IFXRenderYYY was lastUsed in a CIFXDeviceYYY class.  
	 */
	U32 m_uLastSetId;

	/**
	 *	Dirty flag.  Used for different purposes in different inherited classes.
	 */
	BOOL m_bDirty;

};

IFXINLINE CIFXDeviceBase::CIFXDeviceBase()
{
	InitData();
}

IFXINLINE CIFXDeviceBase::~CIFXDeviceBase()
{
	// EMPTY
}

IFXINLINE void CIFXDeviceBase::InitData()
{
	m_bEnabled = FALSE;
	m_uLastSetId = 0;
	m_bDirty = TRUE;
}

IFXINLINE BOOL CIFXDeviceBase::IsDirty()
{
	return m_bDirty;
}

IFXINLINE void CIFXDeviceBase::MakeDeviceDirty()
{
#ifdef _DEBUG
	//OutputDebugString("Making Device Dirty!\n");
#endif // _DEBUG
	m_bDirty = TRUE;
	m_uLastSetId = 0;
}

IFXINLINE IFXRESULT CIFXDeviceBase::SetEnabled(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	if(m_bEnabled != bEnabled)
		rc = SetHWEnabled(bEnabled);

	return rc;
}

IFXINLINE IFXRESULT CIFXDeviceBase::SetHWEnabled(BOOL bEnabled)
{
	m_bEnabled = bEnabled;

	return IFX_OK;
}

IFXINLINE BOOL CIFXDeviceBase::GetEnabled() const
{
	return m_bEnabled;
}


#endif
