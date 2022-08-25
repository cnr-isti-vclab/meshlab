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
//	CIFXTaskManagerView.cpp
//
//	DESCRIPTION
//		This module defines the CIFXTaskManagerView class.  It is used to...
//
//	NOTES
//
//***************************************************************************

#include "CIFXTaskManagerView.h"
#include "IFXAutoRelease.h"
#include "IFXErrorInfo.h"
#include "IFXNotificationManager.h"
#include "IFXScheduler.h"
#include "IFXSchedulingCIDs.h"

static const U32 View_DefaultStepSize = 32;

//---------------------------------------------------------------------
// Factory function
//---------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXTaskManagerView_Factory(IFXREFIID riid, void** ppv)
{
	CIFXTaskManagerView *pObject;

	if (ppv == NULL)
		return IFX_E_INVALID_POINTER;

	BOOL allocated = FALSE;
	pObject = new CIFXTaskManagerView();

	if (!pObject)
	{
		// allocation failed
		*ppv = NULL;
		return IFX_E_OUT_OF_MEMORY;
	}

	IFXRESULT rc = pObject->QueryInterface(riid, ppv);

	if (rc != IFX_OK && allocated)
	{
		delete pObject;
		pObject = NULL;
	}
	return rc;
}

//---------------------------------------------------------------------
// PRIVATE constructor
//---------------------------------------------------------------------
CIFXTaskManagerView::CIFXTaskManagerView()
{
	m_refcount = 0;
	m_initialized = FALSE;

	m_pCoreServices = NULL;

	m_allocStepSize = View_DefaultStepSize;
	m_allocated = 0;
	m_size = 0;
	m_ppView = NULL;
}

//---------------------------------------------------------------------
// PRIVATE destructor
//---------------------------------------------------------------------
CIFXTaskManagerView::~CIFXTaskManagerView()
{
	if (m_ppView) {
		Reset(); // releases all IFXTaskManagerNode pointers
		IFXDELETE_ARRAY( m_ppView );
	}

	IFXRELEASE(m_pCoreServices);
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::AddRef
//---------------------------------------------------------------------
U32 CIFXTaskManagerView::AddRef()
{
	return ++m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::Release
//---------------------------------------------------------------------
U32 CIFXTaskManagerView::Release()
{
	if (1 == m_refcount) 
	{
		delete this;
		return 0;
	}
	return --m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXUnknown::QueryInterface
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT rc = IFX_OK;

	if (ppv == NULL)
	{
		rc = IFX_E_INVALID_POINTER;
	}
	else if (riid == IID_IFXUnknown)
	{
		*ppv = this; // do not typecast to IFXUnknown, multiple inheritance confuses the compiler
		AddRef();
	}
	else if (riid == IID_IFXTaskManagerView)
	{
		*ppv = (IFXTaskManagerView*) this;
		AddRef();
	}
	else
	{
		*ppv = NULL;
		rc = IFX_E_UNSUPPORTED;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::Initialize
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::Initialize(IFXCoreServices * pCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (m_initialized)
		rc = IFX_E_ALREADY_INITIALIZED;
	else if (!pCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		m_pCoreServices = pCoreServices;
		m_pCoreServices->AddRef();
	}

	if (IFXSUCCESS(rc))
	{
		m_initialized = TRUE;
	}
	else
	{
		IFXRELEASE(m_pCoreServices);
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::Reset
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::Reset()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	// clear View List but do not delete array
	if (IFXSUCCESS(rc))
	{
		if (m_ppView)
		{
			U32 i;
			for (i=0; i<m_size; i++)
			{
				IFXRELEASE(m_ppView[i]);
			}
			m_size = 0;
		}
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::GetCoreServices
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::GetCoreServices(IFXCoreServices ** ppCoreServices)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppCoreServices)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*ppCoreServices = m_pCoreServices;
		if (*ppCoreServices)
			(*ppCoreServices)->AddRef();
	}

	return rc;
}


//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::GetSize
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::GetSize(U32 * pSize)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pSize)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		*pSize = m_size;
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::GetElementAt
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::GetElementAt(U32 n, IFXTaskManagerNode ** ppNode)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!ppNode)
		rc = IFX_E_INVALID_POINTER;
	else if (n >= m_size)
		rc = IFX_E_INVALID_RANGE;

	if (IFXSUCCESS(rc))
	{
		*ppNode = m_ppView[n];
		if (*ppNode)
			(*ppNode)->AddRef();
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::GetRefCount
//---------------------------------------------------------------------
U32 CIFXTaskManagerView::GetRefCount()
{
	return m_refcount;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::IncreaseAlloc
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::IncreaseAlloc()
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		U32 newSize = m_allocated + m_allocStepSize;

		IFXTaskManagerNode ** ppTemp = NULL;
		ppTemp = (IFXTaskManagerNode**) new IFXTaskManagerNode*[newSize];

		if (ppTemp)
		{
			U32 i;
			for (i=0; i<m_size; i++)
				ppTemp[i] = m_ppView[i];

			for (i=m_size; i<newSize; i++)
				ppTemp[i] = NULL;

			IFXDELETE_ARRAY( m_ppView );
			m_allocated = newSize;
			m_ppView = ppTemp;
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}

	return rc;
}

IFXRESULT CIFXTaskManagerView::SetAlloc(U32 n)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc))
	{
		m_allocStepSize = n;

		if (m_allocated == 0)
		{
			rc = IncreaseAlloc();
		}
	}

	return rc;
}

//---------------------------------------------------------------------
// PUBLIC IFXTaskManagerView::AddTask
//---------------------------------------------------------------------
IFXRESULT CIFXTaskManagerView::AddTask(IFXTaskManagerNode * pNode)
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pNode)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		if (m_size == m_allocated)
			rc = IncreaseAlloc();
	}

	if (IFXSUCCESS(rc))
	{
		IFXASSERT(m_size < m_allocated);

		m_ppView[m_size] = pNode;
		m_ppView[m_size]->AddRef();
		m_size++;
	}

	return rc;
}

IFXRESULT CIFXTaskManagerView::ExecuteAll(IFXTaskData * pData, BOOL submitErrors )
{
	IFXRESULT rc = IFX_OK;

	if (!m_initialized)
		rc = IFX_E_NOT_INITIALIZED;
	else if (!pData)
		rc = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(rc))
	{
		if (m_ppView)
		{
			IFXDECLARELOCAL( IFXErrorInfo, pInfo );
			IFXDECLARELOCAL( IFXNotificationManager, pNotificationManager );

			U32 i;
			for (i=0; i<m_size; i++)
			{
				IFXASSERT( m_ppView[ i ] );

				IFXRESULT	taskResult	= m_ppView[ i ]->Execute( pData );

				if ( IFXFAILURE( taskResult ) && 
					 taskResult != IFX_E_ABORTED && 
					 submitErrors )
				{
					IFXRESULT	transparentResult	= IFX_OK;

					IFXASSERT( m_pCoreServices );

					// Create an IFXErrorInfo supporting component once that 
					// can be reused.
					if ( !pInfo )
					{
						transparentResult = IFXCreateComponent(
												CID_IFXErrorInfo, 
												IID_IFXErrorInfo, 
												( void** ) &pInfo );
						if ( IFXSUCCESS( transparentResult ) )
							transparentResult = pInfo->Initialize( m_pCoreServices );
					}

					// Store the task execution failure details.
					if ( IFXSUCCESS( transparentResult ) )
					{
						IFXASSERT( pInfo );

						IFXDECLARELOCAL( IFXTask, pTask );

						transparentResult = m_ppView[ i ]->GetTask( &pTask );
						IFXASSERT( IFXSUCCESS( transparentResult ) );
						transparentResult = pInfo->SetErrorTask( pTask );
						IFXASSERT( IFXSUCCESS( transparentResult ) );
						IFXRELEASE( pTask );

						IFXTaskHandle	hTask	= 0;

						transparentResult = m_ppView[ i ]->GetTaskHandle( &hTask );
						IFXASSERT( IFXSUCCESS( transparentResult ) );
						transparentResult = pInfo->SetErrorTaskHandle( hTask );
						IFXASSERT( IFXSUCCESS( transparentResult ) );

						transparentResult = pInfo->SetErrorCode( taskResult );
						IFXASSERT( IFXSUCCESS( transparentResult ) );
					}

					// Get a reference to the Notification Manager's main 
					// interface once.
					if ( !pNotificationManager )
					{
						transparentResult = m_pCoreServices->GetNotificationManager( 
																IID_IFXNotificationManager, 
																( void** ) &pNotificationManager );
					}

					// Now submit the unknown block type skipped notification.  This 
					// involves getting a temporary reference to the Scheduler and 
					// Notification Manager first.
					if ( IFXSUCCESS( transparentResult ) )
					{
						IFXASSERT( pNotificationManager );

						transparentResult = pNotificationManager->SubmitError( pInfo );
					}
				}
			}

			IFXRELEASE( pNotificationManager );
			IFXRELEASE( pInfo );
		}
	}

	return rc;
}


