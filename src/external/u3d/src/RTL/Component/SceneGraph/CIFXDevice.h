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
//  CIFXDevice.h
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************


#ifndef __CIFXDEVICE_H__
#define __CIFXDEVICE_H__


#include "CArrayList.h"
#include "IFXDevice.h"
#include "IFXView.h"
#include "IFXPerformanceTimer.h"
#include "IFXScheduler.h"
#include "IFXRenderWindow.h"

#define MAX_FRAME_RATES_TO_AVERAGE 128


class CIFXDevice : public IFXDevice
{
	CIFXDevice();
	virtual ~CIFXDevice();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXDevice_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown interface...
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// IFXDevice interface...
	IFXRESULT IFXAPI Initialize();

	IFXRESULT IFXAPI AppendView( 
						IFXView*	pView,
						U32			viewInstance );
	IFXRESULT IFXAPI GetView( 
						U32			index, 
						IFXView**	ppView, 
						U32*		pViewInstance );
	IFXRESULT IFXAPI GetViewCount( 
						U32&		rViewCount );
	IFXRESULT IFXAPI RemoveLastView();

	IFXRESULT IFXAPI GetWindowSize(IFXRect& rcWindow) const;
	IFXRESULT IFXAPI Render();

	IFXRESULT IFXAPI SetWindow(IFXRenderWindow& rWindow);
	IFXRESULT IFXAPI SetDTS(BOOL bDTS);
	IFXRESULT IFXAPI SetWindowPtr(IFXHANDLE pvWindow);
	IFXRESULT IFXAPI SetWindowSize(const IFXRect& rcWindow);
	IFXRESULT IFXAPI SetTransparent(BOOL bTransparent);

	IFXRESULT IFXAPI GetAAEnabled(IFXenum& bAAEnabled);
	IFXRESULT IFXAPI SetAAEnabled(IFXenum  bAAEnabled);

	IFXRESULT IFXAPI GetAAMode(IFXenum& eAAMode);
	IFXRESULT IFXAPI SetAAMode(IFXenum  eAAMode);
	
	IFXRESULT IFXAPI GetRenderCaps(IFXRenderCaps& rCaps);

	IFXRESULT IFXAPI SetRenderer(U32 uRenderId);
	IFXRESULT IFXAPI GetRenderer(U32& uRenderId);

	IFXRESULT IFXAPI GetTargetFramerate(F32 *pfTargetFramerate);
	IFXRESULT IFXAPI SetTargetFramerate (F32 fTargetFramerate);
	IFXRESULT IFXAPI GetUseTargetFramerate(U32 *puUseTargetFramerate);
	IFXRESULT IFXAPI SetUseTargetFramerate (U32 uUseTargetFramerate);

	IFXRESULT IFXAPI SetPixelAspect(F32 fAspect);

	IFXRESULT IFXAPI ClearRenderInfo();
	U32		  IFXAPI GetRenderInfo(IFXenum eRenderInfo);

private:

	/**
		Identifies a single view node instance that is rendered when 
		IFXDevice::Render is called.

		@note	When a node has more than one parent, or one of its 
				ancestors has more than one parent, the node will have more 
				than one instance in the world.
	*/
	struct ViewData
	{
		IFXView	*m_pView;	///< Identifies a view node to be rendered.
		U32		m_instance;	///< Identifies the view node instance to be rendered.
	};

	/**
		Template specialization for a list of type CIFXDevice::ViewData.

		@todo	Eventually replace CArrayList usage with a more capable 
				linear array implementation (such as IFXArray) or improve 
				CArrayList so that the device view API functionality can 
				provide arbitrary insertion and removal.  The current 
				underlying implementation does not support insertion, or 
				removal that maintains element ordering.
	*/
	typedef CArrayList<ViewData>	ViewDataList;


	void IFXAPI SetRenderFrameCount(IFXView* pView);


	// IFXUnknown attributes...
	U32 m_uRefCount;

	// IFXDevice attributes...
	IFXRenderContextPtr m_spRenderContext;	///< IFXRenderContext interface pointer that if valid, indicates the device is initialized.
	IFXRenderWindow		m_RenderWindow;
	U32					m_uRenderId;
	U32					m_uNumDevices;
	ViewDataList		*m_pViewDataList;	///< Pointer to a CArrayList that contains ViewData entries drawn during rendering.  Refer to ViewDataList description for current usage limitations.
	IFXPerformanceTimer	*m_pTimer;			///< Pointer to a high performance timer.
	F32					m_fFramerate;
	U32					m_uUseTargetFramerate;
	F32					m_fTargetFramerate;
	F32					m_fTempo;
	U32					m_uTimeStampRingValues[MAX_FRAME_RATES_TO_AVERAGE];
	U32					m_uTimeStampRingHead;
	U32					m_uFrameRatesToAverage;
	F32					m_fPixelAspect;
};

#endif
