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
//  CIFXDevice.cpp
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************
#include "IFXSceneGraphPCH.h"
#include "CIFXDevice.h"
#include "IFXView.h"
#include <float.h>
#include "IFXCoreCIDs.h"
#include "IFXRenderingCIDs.h"

// Globals
F32 g_fCLODMultiplier = 1.0;


CIFXDevice::CIFXDevice()
{
	// IFXUnknown attributes...
	m_uRefCount = 0;

	// IFXDevice attributes...
	// Note:  On construction, the m_spRenderContext smart pointer is 
	// automatically NULL.
	m_uNumDevices			= 0;
	m_uRenderId				= 0;
	m_pViewDataList			= NULL;
	m_pTimer				= NULL;
	m_fFramerate			= 0;
	m_uTimeStampRingHead	= 0;
	m_uFrameRatesToAverage	= 0;
	m_uUseTargetFramerate	= 0;
	m_fTargetFramerate		= DEFAULT_TARGETFRAMERATE;
	m_fTempo				= FLT_MAX;
	m_fPixelAspect			= 1.0f;
}


CIFXDevice::~CIFXDevice()
{
	// Note:  The IFXRenderContext interface pointer managed by the 
	// m_spRenderContext smart pointer will automatically be released.

	IFXRELEASE( m_pTimer );

	if ( m_pViewDataList )
	{
		U32	viewCount	= m_pViewDataList->size(),
			viewIndex	= 0;

		while ( viewIndex < viewCount )
		{
			IFXView	*pView	= ( *m_pViewDataList )[ viewIndex ].m_pView;

			IFXRELEASE( pView );

			++viewIndex;
		}

		delete m_pViewDataList;
	}
}


IFXRESULT IFXAPI_CALLTYPE CIFXDevice_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXDevice *pDevice = new CIFXDevice;

		if ( pDevice )
		{
			// Perform a temporary AddRef for our usage of the component.
			pDevice->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pDevice->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pDevice->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown interface...
U32 CIFXDevice::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXDevice::Release()
{
	if (--m_uRefCount) 
		return m_uRefCount;
	delete this;
	return 0;
}


IFXRESULT CIFXDevice::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXUnknown )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXDevice )
			*ppv = (IFXDevice*)this;
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS(result) )
			AddRef();
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXDevice interface...
IFXRESULT CIFXDevice::Initialize()
{
	IFXRESULT	result	= IFX_OK;

	if ( !m_spRenderContext.IsValid() )
	{
		// Allocate the view instance list.
		IFXASSERT( !m_pViewDataList );
		m_pViewDataList = new ViewDataList;

		if ( !m_pViewDataList )
			result = IFX_E_OUT_OF_MEMORY;

		// Create a timer and start it.
		if ( IFXSUCCESS( result ) )
		{
			IFXASSERT( !m_pTimer )
			result = IFXCreateComponent( 
						CID_IFXPerformanceTimer, 
						IID_IFXPerformanceTimer, 
						( void** ) &m_pTimer );

			if ( IFXSUCCESS( result ) )
				m_pTimer->StartTimer( 0 );
		}

		// Create a render context and initialize it.
		if ( IFXSUCCESS( result ) )
		{
			result = m_spRenderContext.Create( 
											CID_IFXRenderContext, 
											IID_IFXRenderContext );

			if ( IFXSUCCESS( result ) )
				result = m_spRenderContext->Initialize();
		}

		// If any errors occur, reverse the initialization work performed.
		if ( IFXFAILURE( result ) )
		{
			m_spRenderContext = NULL;
			IFXRELEASE( m_pTimer );
			IFXDELETE( m_pViewDataList );
		}
	}
	else
		result = IFX_E_ALREADY_INITIALIZED;

	return result;
}


IFXRESULT CIFXDevice::AppendView( 
						IFXView*	pView,
						U32			viewInstance )
{
	IFXRESULT	result	= IFX_OK;

	if ( m_spRenderContext.IsValid() )
	{
		IFXASSERT( m_pViewDataList );

		// Validate parameters.
		if ( pView )
		{
			// Make sure that the view instance specified is valid at least 
			// for this momement in time.

			U32	instanceCount	= 0;
			result = pView->GetNumberOfInstances(&instanceCount);
			if (IFXSUCCESS( result ))
			{
				if ( viewInstance < instanceCount )
					result = IFX_OK;
				else
					result = IFX_E_INVALID_RANGE;
			}
		}
		else
			result = IFX_E_INVALID_POINTER;

		if ( IFXSUCCESS( result ) )
		{
			// Since we'll keep a reference to the IFXView interface, 
			// increment the reference count.
			pView->AddRef();

			// Now add another view data entry to the array.
			ViewData	viewData	= { pView, viewInstance };
			m_pViewDataList->push_back( viewData );
		}
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT CIFXDevice::GetView( 
						U32			index, 
						IFXView**	ppView, 
						U32*		pViewInstance )
{
	IFXRESULT	result	= IFX_OK;

	if ( m_spRenderContext.IsValid() )
	{
		IFXASSERT( m_pViewDataList );

		// Validate parameters.
		if ( index >= m_pViewDataList->size() )
			result = IFX_E_INVALID_RANGE;

		if ( IFXSUCCESS( result ) )
		{
			ViewData	viewData	= ( *m_pViewDataList )[ index ];

			// Hand back the view node if the client wanted it, making sure 
			// to add a reference so it for the client to release when 
			// they're done with it.
			if ( ppView )
			{
				*ppView = viewData.m_pView;

				IFXASSERT( *ppView );
				( *ppView )->AddRef();
			}

			// Hand back the view node instance number if the client wanted	
			// it.
			if ( pViewInstance )
				*pViewInstance = viewData.m_instance;
		}
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT CIFXDevice::GetViewCount( 
						U32&	rViewCount )
{
	IFXRESULT	result	= IFX_OK;

	if ( m_spRenderContext.IsValid() )
	{
		IFXASSERT( m_pViewDataList );

		rViewCount = m_pViewDataList->size();
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT CIFXDevice::RemoveLastView()
{
	IFXRESULT	result	= IFX_OK;

	if ( m_spRenderContext.IsValid() )
	{
		IFXASSERT( m_pViewDataList );

		// Get the number of view data entries available.
		U32	viewCount	= m_pViewDataList->size();

		if ( viewCount )
		{
			// There's a last view data entry, so determine its index.
			U32		lastViewIndex	= viewCount - 1;
			IFXView	*pLastView		= ( *m_pViewDataList )[ lastViewIndex ].m_pView;

			// Release the IFXView interface reference associated with it.
			IFXASSERT( pLastView );
			pLastView->Release();

			// Now remove the actual entry from the array.
#ifdef _DEBUG			
			BOOL removeResult = 
#endif			
				m_pViewDataList->remove( lastViewIndex );
				
			IFXASSERT( removeResult );
		}
		else
			result = IFX_E_CANNOT_FIND;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT CIFXDevice::SetRenderer(U32 uRenderId)
{
	m_uRenderId = uRenderId;

	return IFX_OK;
}

IFXRESULT CIFXDevice::GetRenderer(U32& uRenderId)
{
	uRenderId = m_uRenderId;

	return IFX_OK;
}

IFXRESULT CIFXDevice::GetAAEnabled(IFXenum& eAAEnabled)
{
	eAAEnabled = m_RenderWindow.GetAntiAliasingEnabled();

	return IFX_OK;
}

IFXRESULT CIFXDevice::SetAAEnabled(IFXenum eAAEnabled)
{
	return m_RenderWindow.SetAntiAliasingEnabled(eAAEnabled);
}

IFXRESULT CIFXDevice::GetAAMode(IFXenum& eAAMode)
{
	eAAMode = m_RenderWindow.GetAntiAliasingMode();

	return IFX_OK;
}

IFXRESULT CIFXDevice::SetAAMode(IFXenum eAAMode)
{
	return m_RenderWindow.SetAntiAliasingMode(eAAMode);
}

IFXRESULT CIFXDevice::GetRenderCaps(IFXRenderCaps& rCaps)
{
	IFXRESULT rc = IFX_OK;

	if(m_spRenderContext.IsValid())
	{
		rc = m_spRenderContext->GetCaps(rCaps);
	}
	else
	{
		rc = IFX_E_NOT_INITIALIZED;
	}

	return rc;
}

IFXRESULT CIFXDevice::GetWindowSize(IFXRect& rcWindow) const
{
	IFXRESULT result = IFX_OK;

	rcWindow = m_RenderWindow.GetWindowSize();

	return result;
}

IFXRESULT CIFXDevice::Render()
{
	IFXRESULT	result	= IFX_OK;

	if ( m_spRenderContext.IsValid() )
	{
		//BOOL bAnyValidViews = FALSE;

		IFXRUN(result, m_spRenderContext->SetRenderer(m_uRenderId));
		IFXRUN(result, m_spRenderContext->GetNumDevices(m_RenderWindow, m_uNumDevices));

		if(IFXSUCCESS(result))
		{
			IFXASSERT( m_pViewDataList );

			if ( m_pViewDataList->size() && m_uUseTargetFramerate )
			{
				IFXASSERT( m_pTimer );

				U32 currentTime;
				m_pTimer->GetElapsed(0, currentTime);

				if ( m_uFrameRatesToAverage )
				{
					U32 uStartTimeIndex = 0;
					if  ( m_uFrameRatesToAverage == MAX_FRAME_RATES_TO_AVERAGE )
						uStartTimeIndex = m_uTimeStampRingHead;

					F32 milliseconds = (F32)(  currentTime - m_uTimeStampRingValues[uStartTimeIndex]  );

					if ( milliseconds )
						m_fFramerate = 1000.0f * (F32)m_uFrameRatesToAverage / (F32)milliseconds;

					F32 internalTargetFrameRate = IFXMIN(m_fTargetFramerate, m_fTempo);

					if (m_fFramerate > internalTargetFrameRate)
						g_fCLODMultiplier = 1.0f + m_fFramerate - internalTargetFrameRate;
					else
						g_fCLODMultiplier = 1.0f / ( 1.0f + IFXMIN((internalTargetFrameRate - m_fFramerate),9) );
				}

				m_uTimeStampRingValues[m_uTimeStampRingHead] = currentTime;

				if ( m_uFrameRatesToAverage < MAX_FRAME_RATES_TO_AVERAGE )
					m_uFrameRatesToAverage++;

				if ( ++m_uTimeStampRingHead == MAX_FRAME_RATES_TO_AVERAGE )
					m_uTimeStampRingHead = 0;
			}
			else
				g_fCLODMultiplier = 1;

			U32 uNumDevices = m_uNumDevices;
			while(uNumDevices--)
			{
				IFXRUN(result, m_spRenderContext->SetDevice(uNumDevices));
				IFXRUN(result, m_spRenderContext->SetWindow(m_RenderWindow));

				if(IFXSUCCESS(result))
				{
					// Attempt to render each associated view for this 
					// specific device.

					U32	viewCount	= m_pViewDataList->size(),
						viewIndex	= 0;

					while ( viewIndex < viewCount && IFXSUCCESS( result ) )
					{
						ViewData	viewData			= ( *m_pViewDataList )[ viewIndex ];
						U32			viewInstanceCount	= 0;

						IFXASSERT( viewData.m_pView );

						// Determine the current number of view instances 
						// that exist.
						result = viewData.m_pView->GetNumberOfInstances(&viewInstanceCount);

						// Verify that the requested instance exists 
						// then render it.
						if(IFXSUCCESS(result))
						{
							if ( viewData.m_instance < viewInstanceCount )
							{
								SetRenderFrameCount( viewData.m_pView );

								result = viewData.m_pView->RenderView( 
									m_spRenderContext.GetPointerNR(), 
									viewData.m_instance );
							}
							else
							{
								result = IFX_E_INVALID_RANGE;

								IFXTRACE_GENERIC( 
									L"CIFXDevice could not render view %p "
									L"instance %d since the total instance "
									L"count is %d\n", 
									viewData.m_pView, 
									viewData.m_instance, 
									viewInstanceCount );
							}

							// Note:  The IFXView interface contained in the 
							// viewData variable was not incremented, so it does 
							// not need to be decremented/released here.

							++viewIndex;
						}
					}
				}
					
				if(IFXSUCCESS(result))
					result = m_spRenderContext->Swap();
			}
		}
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


void CIFXDevice::SetRenderFrameCount(IFXView* pView)
{
	U32 uFrame = 0;

	if(pView)
	{
		IFXSceneGraph* pSG = 0;
		pView->GetSceneGraph(&pSG);

		IFXCoreServices* pCS = 0;
		if(pSG)
		{
			pSG->GetCoreServices(&pCS);
			pSG->Release();
		}

		IFXScheduler* pSched = 0;
		if(pCS)
		{
			pCS->GetScheduler(IID_IFXScheduler, (void**)&pSched);
			pCS->Release();
		}

		if(pSched)
		{
			pSched->GetSimulationFrame(&uFrame);
			pSched->Release();
		}
	}

	//IFXRenderServices::SetTimeStamp(uFrame);
}

IFXRESULT CIFXDevice::SetWindow(IFXRenderWindow& rWindow)
{
	IFXRESULT result = IFX_OK;

	m_RenderWindow = rWindow;

	return result;
}

IFXRESULT CIFXDevice::SetDTS(BOOL bDTS)
{
	IFXRESULT result = IFX_OK;
	IFXRUN(result, m_RenderWindow.SetDTS(bDTS));

	return result;
}

IFXRESULT CIFXDevice::SetWindowPtr(IFXHANDLE pvWindow)
{
	IFXRESULT result = IFX_OK;

	IFXRUN(result, m_RenderWindow.SetWindowPtr(pvWindow));

	return result;
}


IFXRESULT CIFXDevice::SetWindowSize(const IFXRect& rcWindow) 
{
	IFXRESULT result = IFX_OK;

	IFXRUN(result, m_RenderWindow.SetWindowSize(rcWindow));

	return result;
}


IFXRESULT CIFXDevice::SetTransparent(BOOL bTransparent)
{
	IFXRESULT result = IFX_OK;

	IFXRUN(result, m_RenderWindow.SetTransparent(bTransparent));

	return result;
}


IFXRESULT CIFXDevice::GetTargetFramerate(F32 *pfTargetFramerate)
{
	IFXRESULT result = IFX_OK;

	if (pfTargetFramerate)
		*pfTargetFramerate = m_fTargetFramerate;
	else
		result = IFX_E_INVALID_POINTER;
	
	return result;
}


IFXRESULT CIFXDevice::SetTargetFramerate (F32 fTargetFramerate)
{
	IFXRESULT result = IFX_OK;

	m_fTargetFramerate = fTargetFramerate;
	
	return result;
}


IFXRESULT CIFXDevice::GetUseTargetFramerate(U32 *puUseTargetFramerate)
{
	IFXRESULT result = IFX_OK;

	if (puUseTargetFramerate)
		*puUseTargetFramerate = m_uUseTargetFramerate;
	else
		result = IFX_E_INVALID_POINTER;
	
	return result;
}


IFXRESULT CIFXDevice::SetUseTargetFramerate (U32 uUseTargetFramerate)
{
	IFXRESULT result = IFX_OK;

	m_uUseTargetFramerate = uUseTargetFramerate;
	
	return result;
}



IFXRESULT CIFXDevice::SetPixelAspect(F32 fAspect)
{
	IFXRESULT result = IFX_OK;

	if(fAspect <= 0)
		result = IFX_E_INVALID_RANGE;
	else
		m_fPixelAspect = fAspect;

	return result;
}

IFXRESULT CIFXDevice::ClearRenderInfo()
{
	IFXRESULT rc = IFX_OK;

	if(m_spRenderContext.IsValid())
	{
		rc = m_spRenderContext->ClearPerformanceData();
	}

	return rc;
}

U32 CIFXDevice::GetRenderInfo(IFXenum eRenderInfo)
{
	U32 uResult = 0;

	if(m_spRenderContext.IsValid())
	{
		m_spRenderContext->GetPerformanceData(eRenderInfo, uResult);
	}

	return uResult;
}


