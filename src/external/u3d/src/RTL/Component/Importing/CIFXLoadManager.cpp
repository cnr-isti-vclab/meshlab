//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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

/**
	@file	CIFXLoadManager.cpp

			Implementation of the CIFXLoadManager class.
			CIFXLoadManager implements IFXLoadManager, IFXTask, and IFXUnknown
			The CIFXLoadManager is used to load a file from an IFXReadBuffer
			into the scene graph.
*/

#include "CIFXLoadManager.h"
#include "IFXCoreCIDs.h"
#include "IFXScheduler.h"
#include "IFXBlockTypes.h"
#include "IFXBitStreamX.h"
#include "IFXModifierChain.h"
#include "IFXOSUtilities.h"
#include "IFXPalette.h"
#include "IFXDecoderChainX.h"
#include "IFXCheckX.h"
#include "IFXStdio.h"
#include "IFXFileReference.h"
#include "IFXDummyModifier.h"
#include "IFXSchedulingCIDs.h" // CID_IFXNotificationInfo
#include "IFXImportingCIDs.h"

// Constants
const U32 CIFXLoadManager::ReadTaskID = 0x00000001;
const U32 CIFXLoadManager::TransferTaskID = 0x00000002;
const U32 CIFXLoadManager::NotificationTaskID = 0x00000003;

const U32 CIFXLoadManager::m_uMinDecoderPaletteSize = 10;

// LoadManager constructor

CIFXLoadManager::CIFXLoadManager() :
	m_uRefCount(0),
	IFXDEFINEMEMBER(m_pCoreServices),
	IFXDEFINEMEMBER(m_pBlockReaderX),
	IFXDEFINEMEMBER(m_pNameMap),
	IFXDEFINEMEMBER(m_pBTTHash)
{
	m_uReadTimeLimit = 5;
	m_uTransferTimeLimit = 20;

	m_ReadTaskHandle = IFXTASK_HANDLE_INVALID;
	m_TransferTaskHandle = IFXTASK_HANDLE_INVALID;
	m_bReadDone = TRUE;
	m_bTransferDone = TRUE;
	m_uDeclarationSize = 0;

	m_ppDecoderPalettes = NULL;

	// State and Error codes
	m_eState = IFX_LOAD_STATE_UNLOADED;
	m_eOldState = IFX_LOAD_STATE_UNINITIALIZED;

	m_bTransferNow = FALSE;
	m_bAtomicLoad = FALSE;
	m_uLoadId = 0;
	m_bExternal = FALSE;
	m_CollisionPolicy = IFXCOLLISIONPOLICY_REPLACE;
}

CIFXLoadManager::~CIFXLoadManager()
{
	U32 i;

	for (i = 0; i < m_SubLoadManagers.GetNumberElements(); ++i )
	{
		IFXRELEASE( m_SubLoadManagers[i] );
	}

	for (i = 0; i < IFXSceneGraph::NUMBER_OF_PALETTES; i++)
	{
		/** 
		@todo when the palette entries represent
		'chains' of decoders, it is likely that RIGHT HERE
		you'll have to traverse each palette and manually
		release each decoder in the chain before releasing
		the palette itself.  Palettes treat their entries
		as IFXUnknown pointers, so if there's more to each
		entry than a single object, things won't get destructed
		properly.
		*/
		IFXRELEASE(m_ppDecoderPalettes[i]);
		// this will cause each decoder palette to destruct,
		// releasing each palette entry in the process, and
		// since the palette owns each decoder as a palette
		// entry, the decoders will destruct as well.
	}

	IFXDELETE_ARRAY(m_ppDecoderPalettes);
}

U32 CIFXLoadManager::AddRef(void)
{
	return ++m_uRefCount;
}

U32 CIFXLoadManager::Release(void)
{
	if (1 == m_uRefCount) 
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXLoadManager::QueryInterface(IFXREFIID interfaceId,
										  void**   ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if (ppInterface) {
		if (interfaceId == IID_IFXLoadManager) {
			*ppInterface = (IFXLoadManager*) this;
			this->AddRef();
		} else if (interfaceId == IID_IFXTask) {
			*ppInterface = (IFXTask*) this;
			this->AddRef();
		} else if (interfaceId == IID_IFXUnknown) {
			*ppInterface = (IFXUnknown*) this;
			this->AddRef();
		} else {
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
// Initialize - get a reference to a core services object.  The core services
// object will be used to get access to scene graph and scheduler
//-----------------------------------------------------------------------------
IFXRESULT CIFXLoadManager::Initialize(IFXCoreServices* pCoreServices)
{
	IFXRESULT rc = IFX_OK;
	U32     i = 0;

	try {
		if(NULL == pCoreServices) {
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}

		pCoreServices->AddRef();
		IFXRELEASE(m_pCoreServices);
		m_pCoreServices = pCoreServices;

		// create the block type table hash object
		IFXCHECKX(IFXCreateComponent(CID_IFXBTTHash, IID_IFXBTTHash,(void**)&m_pBTTHash));

		// changes for BTT support (BlockTypeTable)
		// Create the decoder palettes:
		m_ppDecoderPalettes = new IFXPalette*[ IFXSceneGraph::NUMBER_OF_PALETTES ];
		if (NULL == m_ppDecoderPalettes) {
			IFXCHECKX(IFX_E_OUT_OF_MEMORY);
		}

		// we're going to initialize our decoder palettes to the
		// same size as the current scenegraph palettes.

		IFXDECLARELOCAL(IFXSceneGraph,pScenegraph);
		IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph,(void**)&pScenegraph));

		for (i=0; i<IFXSceneGraph::NUMBER_OF_PALETTES; i++) {
			// get the corresponding scenegraph palette:
			IFXDECLARELOCAL(IFXPalette,pTmpPalette);
			IFXCHECKX(pScenegraph->GetPalette((IFXSceneGraph::EIFXPalette)i, &pTmpPalette));

			U32 uPalSize = 0;
			IFXCHECKX(pTmpPalette->GetPaletteSize(&uPalSize));
			uPalSize = IFXMAX(uPalSize, m_uMinDecoderPaletteSize);

			IFXCHECKX(IFXCreateComponent(CID_IFXPalette,IID_IFXPalette,(void**)&(m_ppDecoderPalettes[i])));
			IFXCHECKX(m_ppDecoderPalettes[i]->Initialize(uPalSize));
		}
	}
	catch (IFXException& e) {
		e.Log();
		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pBTTHash);
		/// @todo: more clean up might need to go here....
		rc = e.GetIFXResult();
	}

	IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
// Load - start loading data from the read buffer
//
//  The data block queue pointer may be valid or NULL.  If valid, the data
//  blocks pulled from the buffer will be appended to the end.
//-----------------------------------------------------------------------------
IFXRESULT CIFXLoadManager::Load(IFXReadBuffer* pReadBuffer, const BOOL bAtomicLoad, 
								const IFXString& sScope, const IFXCollisionPolicy uCollisionPolicy, const U32 uLoadId)
{
	IFXObjectFilters tmpObjectFilters;
	return Load(pReadBuffer, bAtomicLoad, sScope, uCollisionPolicy, uLoadId, 
		tmpObjectFilters, PALETTE_DEFAULT_GROUP, FALSE);
}

IFXRESULT CIFXLoadManager::Load(IFXReadBuffer* pReadBuffer, const BOOL bAtomicLoad, 
								const IFXString& sScope, const IFXCollisionPolicy uCollisionPolicy, 
								const U32 uLoadId, const IFXObjectFilters& rObjectFilters, 
								const IFXString& sWorldAlias, const BOOL bExternalLoad)
{
	IFXRESULT rc = IFX_OK;

	try {
		ValidateInputX(pReadBuffer);

		m_bAtomicLoad = bAtomicLoad;
		m_uLoadId = uLoadId;
		m_sScope = sScope;
		m_CollisionPolicy = uCollisionPolicy;
		m_ObjectFilters = rObjectFilters;
		m_bExternal = bExternalLoad;
		m_uPriority = 0;

		IFXCHECKX(m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&m_pNameMap));
		if (0 == m_uLoadId) {
			IFXCHECKX(m_pNameMap->GenerateScopeId(m_uLoadId));
			IFXCHECKX(m_pNameMap->NewScope(m_uLoadId, uCollisionPolicy));
		}
		IFXCHECKX(m_pNameMap->SetPrefix(m_uLoadId, m_sScope));
		IFXCHECKX(m_pNameMap->SetWorldAlias(m_uLoadId, sWorldAlias));

		// Create a block reader
		if (m_pBlockReaderX != NULL) {
			IFXCHECKX(IFX_E_UNDEFINED); /// @todo: Change to IFX_E_NOT_AVAILABLE
		} else {
			IFXCHECKX(IFXCreateComponent(CID_IFXBlockReaderX, IID_IFXBlockReaderX,(void**)&m_pBlockReaderX));
		}

		// Initialize the block reader
		IFXDECLARELOCAL(IFXReadBufferX, pReadBufferX);
		IFXCHECKX(pReadBuffer->QueryInterface(IID_IFXReadBufferX,(void**)&pReadBufferX));
		m_pBlockReaderX->InitializeX(*m_pCoreServices, *pReadBufferX);

		// Get the system manager (for registering tasks)
		IFXDECLARELOCAL(IFXSystemManager,pSysMgr);
		{
			IFXDECLARELOCAL(IFXScheduler,pScheduler);
			if (IFXSUCCESS(m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)))
				IFXCHECKX(pScheduler->GetSystemManager(&pSysMgr));
		}

		// Register the read task
		if (pSysMgr) {
			IFXDECLARELOCAL(IFXTask,pReadTask);
			IFXCHECKX(this->QueryInterface(IID_IFXTask, (void**) &pReadTask));
			IFXCHECKX(pSysMgr->RegisterTask(pReadTask, 1, (void*)ReadTaskID, &m_ReadTaskHandle));
		}
		m_bReadDone = FALSE;

		// Register the transfer task
		if (pSysMgr) {
			IFXDECLARELOCAL(IFXTask,pTransferTask);
			IFXCHECKX(this->QueryInterface(IID_IFXTask, (void**)&pTransferTask));
			IFXCHECKX(pSysMgr->RegisterTask(pTransferTask, 1, (void*)TransferTaskID, &m_TransferTaskHandle));
		}
		m_bTransferDone = FALSE;

		// Register notification task
		if (pSysMgr) {
			IFXDECLARELOCAL(IFXScheduler, pScheduler);
			IFXDECLARELOCAL(IFXNotificationManager, pNotificationManager);
			IFXDECLARELOCAL(IFXTask, pNotificationTask);
			IFXCHECKX(m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler));
			IFXCHECKX(pScheduler->GetNotificationManager(&pNotificationManager));
			IFXCHECKX(this->QueryInterface(IID_IFXTask, (void**) &pNotificationTask));
			IFXCHECKX(pNotificationManager->RegisterTask(pNotificationTask, IFXNotificationType_Update, IFXNotificationId_StreamState, &m_NotificationTaskHandle));
		}

		if (pSysMgr == NULL)
			m_bAtomicLoad = TRUE;
		// Call ExecuteRead and ExecuteTransfer...this will be the initial read.
		// Set the stream state to header loading.
		m_eState = IFX_LOAD_STATE_HEADER_LOADING;
		ExecuteReadX();
		ExecuteTransferX();
	}
	catch (IFXException& e) {
		// If there was a failure, then set the state to error
		e.Log();
		m_eState = IFX_LOAD_STATE_ERR;
		rc = e.GetIFXResult();
	}

	try {
		// Update the streaming status 'state' variable (whether success or failure)
		NotifyStreamStateX();
	}
	catch (IFXException& e) {
		e.Log();
		if(IFXSUCCESS(rc)) {			
			rc = e.GetIFXResult();
		}
	}

	IFXRETURN(rc);
}

//-----------------------------------------------------------------------------
// Cancel - stop the load operation, clean up, return to initialized state
//-----------------------------------------------------------------------------
IFXRESULT CIFXLoadManager::Cancel()
{
	IFXRESULT rc = IFX_OK;

	try {
		if (NULL == m_pCoreServices) {
			IFXCHECKX(IFX_E_NOT_INITIALIZED);
		}

		// unregister the tasks
		UnRegisterReadX();
		UnRegisterTransferX();
		// Unregistering the tasks also resets the task handles

		// Cancel all sub-load managers also
		U32 i, n = m_SubLoadManagers.GetNumberElements();
		for (i = 0; i < n; i++)
			m_SubLoadManagers[i]->Cancel();

		// Release the block reader
		IFXRELEASE(m_pBlockReaderX);
		m_uLoadId = 0;
		m_eState = IFX_LOAD_STATE_UNLOADED;
	}

	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXLoadManager::Reset()
{
	IFXRESULT rc = IFX_OK;

	EIFXLoadState eState;
	GetAggregateLoadState(eState);
	if ((eState <= IFX_LOAD_STATE_UNLOADED) || (eState == IFX_LOAD_STATE_MEDIA_LOADED))
		Cancel();
	else
		rc = IFX_E_NOT_DONE;

	IFXRETURN(rc);
}

IFXRESULT CIFXLoadManager::GetAggregateLoadState(EIFXLoadState& rLoadState)
{
	IFXRESULT rc = IFX_OK;

	try {
		// Get least complete state between us and our child load managers
		EIFXLoadState eState = m_eState;
		U32 i, n = m_SubLoadManagers.GetNumberElements();
		for (i = 0; i < n; i++) {
			EIFXLoadState eSubState;
			m_SubLoadManagers[i]->GetAggregateLoadState(eSubState);
			if (eSubState < eState)
				eState = eSubState;
		}
		rLoadState = eState;
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetAggregateFileSize(U64& rFileSize)
{
	IFXRESULT rc = IFX_OK;

	try {
		U64 s = 0, st = 0;
		if (NULL == m_pBlockReaderX)
			rc = IFX_E_NOT_INITIALIZED;

		EIFXLoadState eState;
		IFXCHECKX(GetAggregateLoadState(eState));
		if (eState < IFX_LOAD_STATE_HEADER_LOADED) {
			rFileSize = s = 0;
			rc = IFX_W_DATA_NOT_AVAILABLE;
		} else {
			U32 i, n = m_SubLoadManagers.GetNumberElements();
			m_pBlockReaderX->GetFileSizeX(s);
			for (i = 0; i < n; i++) {
				IFXCHECKX(m_SubLoadManagers[i]->GetAggregateFileSize(st));
				s += st;
			}
		}
		if (IFXSUCCESS(rc))
			rFileSize = s;
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetAggregateFileSizeLoaded(U64& rFileSize)
{
	IFXRESULT rc = IFX_OK;

	try {
		U64 s = 0, st = 0;
		if (NULL == m_pBlockReaderX)
			rc = IFX_E_NOT_INITIALIZED;

		EIFXLoadState eState;
		IFXCHECKX(GetAggregateLoadState(eState));
		if (eState < IFX_LOAD_STATE_HEADER_LOADED) {
			rFileSize = s = 0;
			rc = IFX_W_DATA_NOT_AVAILABLE;
		} else {
			U32 i, n = m_SubLoadManagers.GetNumberElements();
			m_pBlockReaderX->GetFileSizeLoadedX(s);
			for (i = 0; i < n; i++) {
				IFXCHECKX(m_SubLoadManagers[i]->GetAggregateFileSizeLoaded(st));
				s += st;
			}
		}
		if (IFXSUCCESS(rc))
			rFileSize = s;
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetFileSize(U64& rFileSize)
{
	IFXRESULT rc = IFX_OK;

	try {
		if (NULL == m_pBlockReaderX)
			rc = IFX_E_NOT_INITIALIZED;
		if (IFXSUCCESS(rc))
			m_pBlockReaderX->GetFileSizeX(rFileSize);
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetFileSizeLoaded(U64& rFileSize)
{
	IFXRESULT rc = IFX_OK;

	try {
		if (NULL == m_pBlockReaderX)
			rc = IFX_E_NOT_INITIALIZED;
		if (IFXSUCCESS(rc))
			m_pBlockReaderX->GetFileSizeLoadedX(rFileSize);
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetSubLoadManagersCount(U32& rCount)
{
	IFXRESULT rc = IFX_OK;

	try {
		rCount = m_SubLoadManagers.GetNumberElements();
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetSubLoadManager(U32 uIndex, IFXLoadManager*& rLoadManager)
{
	IFXRESULT rc = IFX_OK;

	try {
		U32 rCount = m_SubLoadManagers.GetNumberElements();
		if (uIndex >= rCount)
			rc = IFX_E_BAD_PARAM;
		if (IFXSUCCESS(rc))
			rc = m_SubLoadManagers[uIndex]->QueryInterface(IID_IFXLoadManager, (void**)&rLoadManager);
	}
	catch (IFXException& e) {
		e.Log();
		rc = e.GetIFXResult();
	}

	return rc;
}

IFXRESULT CIFXLoadManager::GetReadTimeLimit(U32& rReadTimeLimit)
{
	rReadTimeLimit = m_uReadTimeLimit;

	return IFX_OK;
}

IFXRESULT CIFXLoadManager::SetReadTimeLimit(const U32 rReadTimeLimit)
{
	m_uReadTimeLimit = rReadTimeLimit;

	return IFX_OK;
}

IFXRESULT CIFXLoadManager::GetTransferTimeLimit(U32& rTransferTimeLimit)
{
	rTransferTimeLimit = m_uTransferTimeLimit;

	return IFX_OK;
}

IFXRESULT CIFXLoadManager::SetTransferTimeLimit(const U32 rTransferTimeLimit)
{
	m_uTransferTimeLimit = rTransferTimeLimit;

	return IFX_OK;
}

IFXRESULT CIFXLoadManager::Execute(IFXTaskData* pTaskData)
{
	IFXRESULT rc = IFX_OK;

	// Check is it notification info
	IFXDECLARELOCAL(IFXNotificationInfo, pNotificationInfo);
	if (IFXSUCCESS(pTaskData->QueryInterface(IID_IFXNotificationInfo, (void**)&pNotificationInfo)))
	{
		// Get load manager where this notification info came from
		CIFXLoadManager* pSubLoadManager = NULL;
		pNotificationInfo->GetNotificationData((void**)&pSubLoadManager);

		// Search in our child load managers
		U32 i, n = m_SubLoadManagers.GetNumberElements();
		for (i = 0; i < n; i++)
			if (m_SubLoadManagers[i] == pSubLoadManager)
				break;
		// If not found, return
		if (i == n)
			goto l_exit;
		/// @todo: mark with 'complete' sign
		/*    // If submanager completed loading (may be with error), delete it
		U32 eState;
		pNotificationInfo->GetKeyFilter(&eState);
		if ((IFX_LOAD_STATE_MEDIA_LOADED == eState) || (IFX_LOAD_STATE_ERR == eState)) {
		m_SubLoadManagers[i]->Release();
		U32 j;
		for ( j = i; j < n-1; j++)
		m_SubLoadManagers[j] = m_SubLoadManagers[j+1];
		m_SubLoadManagers.ResizeToExactly(n-1);
		}*/
	}

	try {
		if (NULL == m_pCoreServices) {
			IFXCHECKX(IFX_E_NOT_INITIALIZED);
		}
		if (NULL == pTaskData) {
			IFXCHECKX(IFX_E_INVALID_POINTER);
		}

		U32 theTaskID = 0;
		IFXCHECKX(pTaskData->GetUserData((void**) &theTaskID));

		if (ReadTaskID == theTaskID) {
			ExecuteReadX();
		} else if (TransferTaskID == theTaskID) {
			ExecuteTransferX();
		}
	}
	catch (IFXException& e) {
		// If there was a failure, then set the state to error
		e.Log();
		m_eState = IFX_LOAD_STATE_ERR;
		rc = e.GetIFXResult();
	}

	try {
		// Update the streaming status 'state' variable (whether success or failure)
		NotifyStreamStateX();
	}
	catch (IFXException& e) {
		e.Log();
		if(IFXSUCCESS(rc)) {
			rc = e.GetIFXResult();
		}
	}
l_exit:
	IFXRETURN(rc);
}

void CIFXLoadManager::ExecuteReadX()
{
	if (!m_bReadDone) {
		U32 uStartTime = IFXOSGetTime();

		U32 debugcount = 0; /// @todo: Remove debugcount
		BOOL bDone = FALSE;
		IFXRESULT iWarningCode = IFX_OK;
		while(!bDone) {
			// The while loop will normally exit if any of the following conditions are true:
			// 1. ReadBlock did not read a block (inidicated by warning code
			//        IFX_W_DATA_NOT_AVAILABLE or IFX_W_END_OF_FILE)
			// 2. Atomic Load is false and time has expired and state is media loading
			// The while loop will also exit if it gets and an unexpected error code.
			IFXASSERT(m_pBlockReaderX);
			IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
			m_pBlockReaderX->ReadBlockX(pDataBlockX ,iWarningCode);
			debugcount++;

			// if ReadBlock got a datablock
			if(pDataBlockX && IFX_OK == iWarningCode) {
				// Process the datablock
				ProcessBlockX(*pDataBlockX);
				U64 uFileSizeLoaded = 0;
				m_pBlockReaderX->GetFileSizeLoadedX(uFileSizeLoaded);
				if ((m_eState < IFX_LOAD_STATE_HEADER_LOADED) && (uFileSizeLoaded == m_uDeclarationSize))
					m_eState = IFX_LOAD_STATE_HEADER_LOADED;
				if ((m_eState < IFX_LOAD_STATE_MEDIA_LOADING) && (uFileSizeLoaded > m_uDeclarationSize))
					m_eState = IFX_LOAD_STATE_MEDIA_LOADING;
			} else {
				// no datablock to process, -> Done
				bDone = TRUE;
			}

			if (FALSE == bDone  && FALSE == m_bAtomicLoad
				&& ((IFX_LOAD_STATE_MEDIA_LOADING == m_eState) || (IFX_LOAD_STATE_HEADER_LOADED == m_eState)))
			{
				// if time has expired, -> Done
				U32 uCurrentTime = IFXOSGetTime();
				if ((uCurrentTime - uStartTime) > m_uReadTimeLimit)
				{
					bDone = TRUE;
				}
			}
		}

		// if at end of file
		if (IFX_W_END_OF_FILE == iWarningCode) {
			// unregister the read task
			UnRegisterReadX();
			m_bTransferNow = TRUE;
		}

		if (m_bTransferNow) {
			ExecuteTransferX();
			m_bTransferNow = FALSE;
		}
	}
}

void CIFXLoadManager::ExecuteTransferX()
{
	if (!m_bTransferDone) {
		IFXRESULT iWarningCode = IFX_OK;
		ProcessTransferOrderX(iWarningCode);

		// The transfer task should be unregistered after the read task is done
		// and the ProcessTransferOrder call was successful and not a partial transfer
		if (m_bReadDone && IFX_OK == iWarningCode) {
			// unregister the transfer task
			UnRegisterTransferX();
			m_eState = IFX_LOAD_STATE_MEDIA_LOADED;
			m_pNameMap->DeleteScope(m_uLoadId);
		}
	}
}

void CIFXLoadManager::UnRegisterReadX()
{
	// unregister the read task
	if (!m_bReadDone) {
		IFXDECLARELOCAL(IFXScheduler,pScheduler);
		if (IFXSUCCESS(m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler))) {
			IFXDECLARELOCAL(IFXSystemManager,pSysMgr);
			IFXCHECKX(pScheduler->GetSystemManager(&pSysMgr));

			IFXCHECKX(pSysMgr->UnregisterTask(m_ReadTaskHandle));
			m_ReadTaskHandle = IFXTASK_HANDLE_INVALID;
		}
		m_bReadDone = TRUE;
	}
}

void CIFXLoadManager::UnRegisterTransferX()
{
	// unregister the transfer task
	if (!m_bTransferDone) {
		IFXDECLARELOCAL(IFXScheduler,pScheduler);
		if (IFXSUCCESS(m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler))) {
			IFXDECLARELOCAL(IFXSystemManager,pSysMgr);
			IFXCHECKX(pScheduler->GetSystemManager(&pSysMgr));

			IFXCHECKX(pSysMgr->UnregisterTask(m_TransferTaskHandle));
			m_TransferTaskHandle = IFXTASK_HANDLE_INVALID;
		}
		m_bTransferDone = TRUE;
	}
}

void CIFXLoadManager::ProcessBlockX(IFXDataBlockX& rDataBlockX, IFXSceneGraph::EIFXPalette ePalette)
{
	U32 uBlockType = 0;

	// Perform idling activities.
	ThumpX();

	// get the block type
	rDataBlockX.GetBlockTypeX(uBlockType);
	rDataBlockX.SetPriorityX(m_uPriority);

	// process the block
	switch (uBlockType) 
	{
		// process the file header
		case BlockType_FileHeaderU3D:
		{
			ProcessFileHeaderBlockX(rDataBlockX);
			break;
		}

		// process priority update block
		case BlockType_FilePriorityUpdateU3D:
		{
			ProcessPriorityUpdateBlockX(rDataBlockX);
			break;
		}

		// Process a block that defines non-intrinsic blocks which will
		// probably be read in subsequently. This will add a new entry
		// to the BTT.
		case BlockType_FileNewObjectTypeU3D:
		{
			if(m_uProfile & IFXPROFILE_EXTENSIBLE)
				ProcessNewBlockTypeDeclarationBlockX(rDataBlockX);
			break;
		}

		// file reference blocks
		case BlockType_FileReferenceU3D:
		{
			ProcessFileReferenceBlockX(rDataBlockX);
			break;
		}

		// modifier chain blocks
		case BlockType_FileModifierChainU3D:
		{
			ProcessModifierChainBlockX(rDataBlockX);
			break;
		}

		// All other types of blocks processed by looking up block type in BTT;
		// if a match is found, then take action based on record retrieved
		// from BTT, otherwise ignore the block.
		default:
		{
			ProcessGenericBlockX(rDataBlockX, ePalette);
			break;
		}
	}
}

void CIFXLoadManager::ProcessModifierChainBlockX(IFXDataBlockX& rDataBlockX)
{
	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX, pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));

	// Give the data block to the bitstream
	pBitStreamX->SetDataBlockX(rDataBlockX);

	// Read modifier chain name
	/// @todo: do somthing with this
	U32 uPosition = 0, length = 0;
	IFXString sModChainName;
	pBitStreamX->ReadIFXStringX(sModChainName);
	IFXCHECKX( sModChainName.GetLengthU8( &length ) );
	uPosition += 2 + length;

	// Read modifer chain type (0 - nodes, 1 - model resources, 2 - textures)
	U32 uModChainType;
	IFXSceneGraph::EIFXPalette ePalette = IFXSceneGraph::NUMBER_OF_PALETTES;
	pBitStreamX->ReadU32X(uModChainType);
	switch (uModChainType) {
		case IFXModifierChain::NODE: ePalette = IFXSceneGraph::NODE; break;
		case IFXModifierChain::RESOURCE: ePalette = IFXSceneGraph::GENERATOR; break;
		case IFXModifierChain::TEXTURE: ePalette = IFXSceneGraph::TEXTURE; break;
		default: IFXCHECKX(IFX_E_UNSUPPORTED);
	}
	uPosition += 4;

	// Read attributes
	U32 uAttributes = 0;
	pBitStreamX->ReadU32X(uAttributes);
	uPosition += 4;
	if (uAttributes & 0x00000001) {
		// Read bounding sphere information
		F32 tmp = 0;
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		uPosition += 4*4;
	}
	if (uAttributes & 0x00000002) {
		// Read axis-aligned bounding box
		F32 tmp = 0;
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		pBitStreamX->ReadF32X(tmp);
		uPosition += 4*6;
	}

	// Read padding
	U32 uPadding = (4-((2 + length) & 3)) & 3;
	uPosition += uPadding;
	while (uPadding-- > 0) {
		U8 tmp;
		pBitStreamX->ReadU8X(tmp);
	}

	// Read count of modifer declaration blocks
	U32 uModifierCount;
	pBitStreamX->ReadU32X(uModifierCount);
	uPosition += 4;

	// Create and initialize BlockReader
	IFXDECLARELOCAL(IFXReadBufferX, pReadBufferX);
	IFXCHECKX(rDataBlockX.QueryInterface(IID_IFXReadBufferX, (void**)&pReadBufferX));
	IFXDECLARELOCAL(IFXBlockReaderX, pBlockReaderX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBlockReaderX, IID_IFXBlockReaderX,(void**)&pBlockReaderX));
	pBlockReaderX->InitializeX(*m_pCoreServices, *pReadBufferX, uPosition);

	// Read modifier declaration blocks and process them
	IFXRESULT iWarningCode = IFX_OK;
	BOOL bDone = FALSE;
	while (!bDone) {
		IFXDECLARELOCAL(IFXDataBlockX, pDataBlockX);
		pBlockReaderX->ReadBlockX(pDataBlockX, iWarningCode);
		// if ReadBlock got a datablock
		if (pDataBlockX && (IFX_OK == iWarningCode)) {
			// Process the datablock
			ProcessBlockX(*pDataBlockX, ePalette);
		} else {
			// no datablock to process, -> Done
			bDone = TRUE;
		}
	}
}

void CIFXLoadManager::ProcessFileReferenceBlockX(IFXDataBlockX& rDataBlockX)
{
	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX, pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));

	// Give the data block to the bitstream
	pBitStreamX->SetDataBlockX(rDataBlockX);

	IFXFileReferenceParams FR;
	U32 i;
	U8 tmp;

	pBitStreamX->ReadIFXStringX(FR.ScopeName);
	U32 uAttributes = 0;
	pBitStreamX->ReadU32X(uAttributes);

	if (uAttributes & 0x00000001) 
	{
		F32 x,y,z,r;
		pBitStreamX->ReadF32X(x);
		pBitStreamX->ReadF32X(y);
		pBitStreamX->ReadF32X(z);
		pBitStreamX->ReadF32X(r);
	}

	if (uAttributes & 0x00000002) 
	{
		F32 x1,y1,z1,x2,y2,z2;
		pBitStreamX->ReadF32X(x1);
		pBitStreamX->ReadF32X(y1);
		pBitStreamX->ReadF32X(z1);
		pBitStreamX->ReadF32X(x2);
		pBitStreamX->ReadF32X(y2);
		pBitStreamX->ReadF32X(z2);
	}

	U32 uNumURL = 0;
	pBitStreamX->ReadU32X(uNumURL);

	for (i = 0; i < uNumURL; i++) 
	{
		FR.FileURLs.CreateNewElement();
		pBitStreamX->ReadIFXStringX(FR.FileURLs[i]);
	}

	U32 uNumFilters = 0;
	pBitStreamX->ReadU32X(uNumFilters);

	for (i = 0; i < uNumFilters; i++) 
	{
		FR.ObjectFilters.CreateNewElement();
		pBitStreamX->ReadU8X(tmp);
		FR.ObjectFilters[i].FilterType = (IFXObjectFilterType)tmp;

		switch (FR.ObjectFilters[i].FilterType) 
		{
		case IFXOBJECTFILTER_NAME:
			pBitStreamX->ReadIFXStringX(FR.ObjectFilters[i].ObjectNameFilterValue);
			break;
		case IFXOBJECTFILTER_TYPE:
			pBitStreamX->ReadU32X(FR.ObjectFilters[i].ObjectTypeFilterValue);
			break;
		default:
			IFXASSERT(0);
			break;
		}
	}

	pBitStreamX->ReadU8X(tmp);
	FR.CollisionPolicy = (IFXCollisionPolicy)tmp;

	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	IFXCHECKX(m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap));
	pBitStreamX->ReadIFXStringX(FR.WorldAlias);
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, FR.WorldAlias));

	// create new load manager
	IFXDECLARELOCAL(IFXLoadManager, pLoadManager);
	IFXCHECKX(IFXCreateComponent(CID_IFXLoadManager, IID_IFXLoadManager, (void**)&pLoadManager));
	IFXCHECKX(pLoadManager->Initialize(m_pCoreServices));

	m_SubLoadManagers.CreateNewElement() = pLoadManager;
	pLoadManager->AddRef();

	// detect local or remote file reference
	IFXDECLARELOCAL(IFXReadBuffer, pReadBuffer);
	IFXDECLARELOCAL(IFXStdio, pStdio);
	IFXString sURL;

	// cycle through all available file URLs until successful file open
	U32 nFile;

	for (nFile = 0; nFile < uNumURL; nFile++) 
	{
		sURL = FR.FileURLs[nFile];

		// transform file://c/dir/file.u3d to c:/dir/file.u3d
		if (IFXSUCCESS(sURL.FindSubstring(L"file://", &i))) 
		{
			const IFXCHAR* pStr = sURL.Raw();
			IFXCHAR p[] = L"x:";
			p[0] = pStr[7];
			sURL = IFXString(p) + IFXString(&pStr[8]);
		}

		// because full path should contain ':', we should append relative path to base URL from Core Services
		if (IFXFAILURE(sURL.FindSubstring(L":", &i))) 
		{
			IFXString sBaseURL;
			m_pCoreServices->GetBaseURL(sBaseURL);
			sURL = sBaseURL + sURL;
		}

		// create appropriate read buffer
		if (IFXSUCCESS(sURL.FindSubstring(L"://", &i)))
			IFXCHECKX(IFXCreateComponent(CID_IFXInternetReadBuffer, IID_IFXReadBuffer, (void**)&pReadBuffer));
		else
			IFXCHECKX(IFXCreateComponent(CID_IFXStdioReadBuffer, IID_IFXReadBuffer, (void**)&pReadBuffer));
		
		// open file
		IFXCHECKX(pReadBuffer->QueryInterface(IID_IFXStdio, (void**)&pStdio));

		if (IFXSUCCESS(pStdio->Open( (IFXCHAR*)sURL.Raw() )))
			break;
		else
		{
			IFXRELEASE( pStdio );
			IFXRELEASE( pReadBuffer );
		}
	}

	// check if any URL was successful opened
	if (nFile == FR.FileURLs.GetNumberElements())
	{
			throw IFXException(
						IFX_E_RESOURCE_NOT_AVAILABLE,
						L"Cannot load file referenced block : \"%ls\"\n",sURL.Raw());
	}

	// add file reference object to scenegraph
	IFXDECLARELOCAL(IFXSceneGraph, pSceneGraph);
	IFXDECLARELOCAL(IFXFileReference, pFileReference);
	IFXDECLARELOCAL(IFXMarker, pMarker);
	IFXDECLARELOCAL(IFXPalette, pFRPalette);
	U32 uId = 0;
	// get scenegraph
	IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph, (void**)&pSceneGraph));
	// get file reference palette
	IFXCHECKX(m_pCoreServices->GetFileReferencePalette(&pFRPalette));
	// create file reference component
	IFXCHECKX(IFXCreateComponent(CID_IFXFileReference, IID_IFXFileReference, (void**)&pFileReference));
	IFXCHECKX(pFileReference->QueryInterface(IID_IFXMarker, (void**)&pMarker));
	pFileReference->SetScopeName(FR.ScopeName);
	pFileReference->SetFileURLs(FR.FileURLs);
	pFileReference->SetObjectFilters(FR.ObjectFilters);
	pFileReference->SetCollisionPolicy(FR.CollisionPolicy);
	pFileReference->SetWorldAlias(FR.WorldAlias);
	// assign scenegraph
	IFXCHECKX(pMarker->SetSceneGraph(pSceneGraph));
	IFXCHECKX(pFRPalette->Add(&sURL, &uId));
	IFXCHECKX(pFRPalette->SetResourcePtr(uId, pFileReference));

	// perform loading of file reference
	IFXCHECKX(pLoadManager->Load(pReadBuffer, m_bAtomicLoad, m_sScope + FR.ScopeName, FR.CollisionPolicy, 0, FR.ObjectFilters, FR.WorldAlias, TRUE));
}

void CIFXLoadManager::ProcessFileHeaderBlockX(IFXDataBlockX& rDataBlockX)
{
	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));

	// Give the data block to the bitstream
	pBitStreamX->SetDataBlockX(rDataBlockX);

	m_eState = IFX_LOAD_STATE_HEADER_LOADING;

	// Check File Version
	U32 uFileVersion = 0;
	pBitStreamX->ReadU32X(uFileVersion);

	pBitStreamX->ReadU32X(m_uProfile);
	IFXCHECKX(m_pNameMap->SetProfile(m_uLoadId, m_uProfile));

	pBitStreamX->ReadU32X(m_uDeclarationSize);

	pBitStreamX->ReadU64X(m_fileSize);

	pBitStreamX->ReadU32X(m_characterEncoding);
	if( MIB_UTF8 != m_characterEncoding )
		IFXCHECKX( IFX_E_UNSUPPORTED );

	if( m_uProfile & IFXPROFILE_UNITSSCALE )
	{
		F64 units;
		pBitStreamX->ReadF64X(units);
		IFXCHECKX( m_pNameMap->SetUnits( m_uLoadId, units ) );
	}

	if ((uFileVersion == FileHeader_VersionCurrent) ||
		((uFileVersion & 0xFFFFFF00) == (FileHeader_VersionCurrent & 0xFFFFFF00)) ||
		((uFileVersion & 0xFFFFFF00) == FileHeader_Version_1_2_Gold) ||
		(uFileVersion == FileHeader_VersionFirstEdition) ||
		(uFileVersion == FileHeader_VersionFirstEditionBeta)) {
			m_pBTTHash->Initialize();
	} else {
		// Version is not supported
		IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_LOADER, IFXDEBUG_MESSAGE, L"File Version Mismatch: Found version:(0x%08x): Expecting version:(0x%08x)\n", uFileVersion,FileHeader_VersionCurrent);
		m_eState = IFX_LOAD_STATE_ERR;
		UnRegisterReadX();
		UnRegisterTransferX();
		// Ignore return codes from Unregister methods
		IFXCHECKX(IFX_E_UNSUPPORTED);
	}
}

void CIFXLoadManager::ProcessPriorityUpdateBlockX(IFXDataBlockX& rDataBlockX)
{
	IFXASSERT(m_eState >= IFX_LOAD_STATE_HEADER_LOADED);

	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));

	// Give the data block to the bitstream
	pBitStreamX->SetDataBlockX(rDataBlockX);
	pBitStreamX->ReadU32X(m_uPriority);

	if (m_uPriority > 0) {
		// If the state is transitioning to media loading
		// (i.e. it was previously unloaded or header
		// loading), then indicate that we should do a
		// transfer now.
		if (IFX_LOAD_STATE_HEADER_LOADING == m_eState ||
			IFX_LOAD_STATE_UNLOADED == m_eState ||
			IFX_LOAD_STATE_ERR == m_eState)
		{
			m_bTransferNow = TRUE;
		}
		m_eState = IFX_LOAD_STATE_MEDIA_LOADING;
	}
}

void CIFXLoadManager::ProcessNewBlockTypeDeclarationBlockX(IFXDataBlockX& rDataBlockX)
{
	IFXBTTHash::IFXBTTHashEntry* pNewBTTEntries = NULL;

	try
	{
		// create a bitstream
		IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));
		pBitStreamX->SetDataBlockX(rDataBlockX);

		// create a new btt entry
		IFXBTTHash::IFXBTTHashEntry NewBTTEntry;

		// populate the new btt entry with information from the datablock
		IFXString name;
		pBitStreamX->ReadIFXStringX(name);

		// read modifier type...
		U32 modType;
		pBitStreamX->ReadU32X(modType);

		// ...and set up palette type
		switch( modType )
		{
		case 0:
			NewBTTEntry.paletteType = IFXSceneGraph::NODE;
			break;
		case 1:
			NewBTTEntry.paletteType = IFXSceneGraph::GENERATOR;
			break;
		case 2:
			NewBTTEntry.paletteType = IFXSceneGraph::TEXTURE;
			break;
		default:
			IFXCHECKX(IFX_E_INVALID_RANGE);
			break;
		}
		NewBTTEntry.flags = IFXBTTHash::CHAIN;

		// get decoder CID and put it to BTTEntry
		pBitStreamX->ReadU32X(NewBTTEntry.decoderCID.A);
		pBitStreamX->ReadU16X(NewBTTEntry.decoderCID.B);
		pBitStreamX->ReadU16X(NewBTTEntry.decoderCID.C);
		U32 i;
		for ( i = 0; i < 8; i++)
			pBitStreamX->ReadU8X(NewBTTEntry.decoderCID.D[i]);

		// assign blocktype
		pBitStreamX->ReadU32X(NewBTTEntry.blockType);

		// read continuation blocks data
		U32 contBlocksCount = 0;
		pBitStreamX->ReadU32X(contBlocksCount);
		pNewBTTEntries = new IFXBTTHash::IFXBTTHashEntry[contBlocksCount];
		if( NULL == pNewBTTEntries )
			IFXCHECKX( IFX_E_OUT_OF_MEMORY );

		for ( i = 0; i < contBlocksCount; i++)
		{
			pBitStreamX->ReadU32X(pNewBTTEntries[i].blockType);
			switch( modType )
			{
			case 0:
				pNewBTTEntries[i].paletteType = IFXSceneGraph::NODE;
				break;
			case 1:
				pNewBTTEntries[i].paletteType = IFXSceneGraph::GENERATOR;
				break;
			case 2:
				pNewBTTEntries[i].paletteType = IFXSceneGraph::TEXTURE;
				break;
			default:
				IFXCHECKX(IFX_E_INVALID_RANGE);
				break;
			}
			pNewBTTEntries[i].flags = IFXBTTHash::CONTINUATION;
		}

		/// @todo: implement usage of the following values
		// read vendor name
		IFXString vendorName;
		pBitStreamX->ReadIFXStringX(vendorName);

		CIFXLoadManager::IFXExternalDecoderCont& extDecCnt = m_extensionData.CreateNewElement();

		extDecCnt.m_contCount = contBlocksCount + 1;
		extDecCnt.m_decoderCID = NewBTTEntry.decoderCID;
		extDecCnt.m_pContBlockTypes = new U32[contBlocksCount + 1];
		if( NULL == extDecCnt.m_pContBlockTypes )
			IFXCHECKX( IFX_E_OUT_OF_MEMORY );
		extDecCnt.m_pContBlockTypes[0] = NewBTTEntry.blockType;
		for( i = 1; i < contBlocksCount + 1; i++ )
			extDecCnt.m_pContBlockTypes[i] = pNewBTTEntries[i - 1].blockType;
		extDecCnt.pNBTDataBlock = &rDataBlockX;
		rDataBlockX.AddRef();

		// read decoders URLs
		U32 decoderURLs = 0;
		IFXString decoderURL;
		pBitStreamX->ReadU32X(decoderURLs);

		for ( i = 0; i < decoderURLs; i++)
		{
			pBitStreamX->ReadIFXStringX(decoderURL);
		}

		// read extension information
		IFXString extensionInfo;
		pBitStreamX->ReadIFXStringX(extensionInfo);

		// add new record to BTT
		IFXCHECKX(m_pBTTHash->Add(&NewBTTEntry));
		for ( i = 0; i < contBlocksCount; i++) 
		{
			pNewBTTEntries[i].decoderCID = NewBTTEntry.decoderCID;
			IFXCHECKX(m_pBTTHash->Add(&pNewBTTEntries[i]));
		}

		delete [] pNewBTTEntries;
	}
	catch(...)
	{
		delete [] pNewBTTEntries;
	}
}

void CIFXLoadManager::ProcessGenericBlockX(IFXDataBlockX& rDataBlockX, IFXSceneGraph::EIFXPalette ePalette)
{
	// Create a bitstream
	IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
	{
		IFXCHECKX(IFXCreateComponent(CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX));
		pBitStreamX->SetDataBlockX(rDataBlockX);
	}

	// get the block type
	U32 uBlockType = 0;
	rDataBlockX.GetBlockTypeX(uBlockType);

	// attempt to find block type record in BTT
	IFXBTTHash::IFXBTTHashEntry BTTRecord;
	{
		IFXRESULT iResultHashFind = m_pBTTHash->Find(uBlockType, &BTTRecord);
		if(IFXFAILURE(iResultHashFind)) {
			// Prepare a notification event to report the unknown block type.
			IFXDECLARELOCAL( IFXNotificationInfo, pInfo );
			IFXCHECKX( IFXCreateComponent(
							CID_IFXNotificationInfo, 
							IID_IFXNotificationInfo, 
							( void** ) &pInfo ) );
			IFXCHECKX( pInfo->Initialize( m_pCoreServices ) );
			IFXCHECKX( pInfo->SetType(IFXNotificationType_System ) );
			IFXCHECKX( pInfo->SetId(IFXNotificationId_UnknownBlockSkipped ) );
			IFXCHECKX( pInfo->SetNotificationData( ( void* ) (uintptr_t) uBlockType ) );
			IFXCHECKX( pInfo->SetKeyFilter( m_uLoadId ) );

			// Now submit the unknown block type skipped notification.  This 
			// involves getting a temporary reference to the Scheduler and 
			// Notification Manager first.
			IFXDECLARELOCAL( IFXNotificationManager, pNotificationManager );
			IFXCHECKX( m_pCoreServices->GetNotificationManager( 
											IID_IFXNotificationManager, 
											( void** ) &pNotificationManager ) );
			IFXCHECKX( pNotificationManager->SubmitEvent( pInfo ) );

			IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_LOADER, IFXDEBUG_MESSAGE, L"Unknown block type:(0x%08x)\n", uBlockType);

			// Note:  pInfo and pNotificationManager will automatically be 
			// released.

			// Return with no error since the blocktype wasn't found in 
			// BTTHash so that processing may continue to the next block.
			return;
		}
	}

	// get the name of the block (which will correspond to the object name)
	IFXString sBlockName, sMappedBlockName;
	pBitStreamX->ReadIFXStringX(sBlockName);
	IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_LOADER, IFXDEBUG_MESSAGE, L"BlockType:(0x%08x)\tName:%ls\n", uBlockType,sBlockName.Raw());

	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	IFXCHECKX(m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap));
	sMappedBlockName = sBlockName;
	IFXCHECKX(pNameMap->Map(m_uLoadId, IFXSceneGraph::NODE, sMappedBlockName));

	// check if this block is already rejected
	U32 k, n = m_RejectedNames.GetNumberElements();
	for (k = 0; k < n; k++)
		if (m_RejectedNames[k] == sBlockName)
			return;

	// get corresponding palette
	IFXDECLARELOCAL(IFXSceneGraph,pScenegraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph, (void**)&pScenegraph ));
	IFXDECLARELOCAL(IFXPalette,pPalette);
	IFXCHECKX(pScenegraph->GetPalette((BTTRecord.flags&IFXBTTHash::CHAIN) ? ePalette : BTTRecord.paletteType, &pPalette));

	// get the appropriate decoder palette
	IFXDECLARELOCAL(IFXPalette,pDecoderPalette);
	if (BTTRecord.paletteType < IFXSceneGraph::NUMBER_OF_PALETTES) {
		pDecoderPalette = m_ppDecoderPalettes[(BTTRecord.flags&IFXBTTHash::CHAIN) ? ePalette : BTTRecord.paletteType];
		pDecoderPalette->AddRef();
	} else {
		IFXCHECKX(IFX_E_INVALID_RANGE);
	}

	U32 uResourceID = 0;
	IFXRESULT iResultPaletteFind = pDecoderPalette->Find(&sBlockName, &uResourceID);

	// attempt to find entry in the corresponding palette
	U32 uResourceId = 0;
	IFXRESULT iFindResult = pPalette->Find(sMappedBlockName, &uResourceId);

	// if object with the same name already exists, then discard it if policy tells us to do that
	if ((m_CollisionPolicy == IFXCOLLISIONPOLICY_DISCARD) && IFXSUCCESS(iFindResult) && IFXFAILURE(iResultPaletteFind))
		return;

	n = m_ObjectFilters.GetNumberElements();
	BOOL bPassedFilters = (n == 0) ? TRUE : FALSE;
	for (k = 0; (k < n) && (!bPassedFilters); k++) {
		switch (m_ObjectFilters[k].FilterType) {
			case IFXOBJECTFILTER_NAME: {
				if (sBlockName.Match(m_ObjectFilters[k].ObjectNameFilterValue, 0))
					bPassedFilters = TRUE;
				break;
			}
			case IFXOBJECTFILTER_TYPE: {
				if (uBlockType == m_ObjectFilters[k].ObjectTypeFilterValue)
					bPassedFilters = TRUE;
				break;
			}
		}
	}
	if (!bPassedFilters) {
		m_RejectedNames.CreateNewElement() = sBlockName;
		return;
	}

	// get the chain position from the datablock
	U32 uChainPosition = 0;
	if (BTTRecord.flags & IFXBTTHash::CHAIN)
		pBitStreamX->ReadU32X(uChainPosition);

	// If the block is a continuation block, then give the block to the appropriate,
	// existing decoder.
	if (BTTRecord.flags & IFXBTTHash::CONTINUATION) 
	{
		IFXDECLARELOCAL(IFXDecoderChainX,pDecoderChainX);

		// Added this to enable continuation blocks in modifier chain (except texture continuation blocks)
		if( uBlockType != BlockType_ResourceTextureContinuationU3D )
			pBitStreamX->ReadU32X(uChainPosition);

		IFXASSERT(IFXSUCCESS(iResultPaletteFind));
		if (IFXSUCCESS(iResultPaletteFind)) {
			IFXCHECKX(pDecoderPalette->GetResourcePtr(uResourceID, IID_IFXDecoderChainX, (void**)&pDecoderChainX));

			// give the data block to the decoder at the appropriate position in the chain
			IFXDECLARELOCAL(IFXDecoderX,pDecoderX);
			pDecoderChainX->GetDecoderX(uChainPosition, pDecoderX);
			if(pDecoderX)
				pDecoderX->PutNextBlockX(rDataBlockX);
		}
	}
	else // not a continuation block
	{
		// 1. create appropriate decoder
		IFXDECLARELOCAL(IFXDecoderX,pDecoderX);
		BOOL isDummy = FALSE;

		// if we can't create this particular decoder then create decoder for dummy modifier and then
		// put this modifier into modifier chain
		if( IFXFAILURE( IFXCreateComponent(BTTRecord.decoderCID, IID_IFXDecoderX, (void**)&pDecoderX)) )
		{
			IFXCHECKX( IFXCreateComponent( CID_IFXDummyModifierDecoder, IID_IFXDecoderX, (void**)&pDecoderX ) );
			isDummy = TRUE;
		}

		// 2. Initialize the decoder and give the object to the decoder
		F64 units;
		m_pNameMap->GetUnits( m_uLoadId, units );
		IFXLoadConfig lc( m_pCoreServices, m_uLoadId, m_bExternal, ePalette, units );
		pDecoderX->InitializeX( lc );

		IFXDECLARELOCAL( IFXExtensionDecoderX, pExtDec );
		if( isDummy || IFXSUCCESS( pDecoderX->QueryInterface( IID_IFXExtensionDecoderX, (void**)&pExtDec ) ) )
		{
			// try to find CID in m_externalDecoderConts. If found, call decoder's SetContBlockTypes and set up data
			U32 i, limit = m_extensionData.GetNumberElements();
			IFXCID cid = BTTRecord.decoderCID;
			BOOL isRegistered = FALSE;

			for( i = 0; i < limit && !isRegistered; i++ )
			{
				if( cid == m_extensionData.GetElement(i).m_decoderCID )
					isRegistered = TRUE;

				if( isRegistered )
				{
					if( isDummy )
					{
						pDecoderX->PutNextBlockX( *m_extensionData.GetElement(i).pNBTDataBlock );
					}
					else
						pExtDec->SetContBlockTypes( m_extensionData.GetElement(i).m_pContBlockTypes, 
							m_extensionData.GetElement(i).m_contCount );
				}
			}
		}

		// 3. give the data block to the decoder
		pDecoderX->PutNextBlockX(rDataBlockX);

		// 4. add the decoder to the decoder palette at the appropriate position in the
		//    decoder chain
		if (IFX_E_CANNOT_FIND == iResultPaletteFind) {
			IFXCHECKX(pDecoderPalette->Add(&sBlockName, &uResourceID));
		}

		IFXDECLARELOCAL(IFXDecoderChainX,pDecoderChainX);
		IFXRESULT iResultGetResource = pDecoderPalette->GetResourcePtr(uResourceID, IID_IFXDecoderChainX, (void**)&pDecoderChainX);
		if(IFXFAILURE(iResultGetResource)) {
			IFXCHECKX(IFXCreateComponent(CID_IFXDecoderChainX, IID_IFXDecoderChainX, (void**)&pDecoderChainX));
			pDecoderChainX->InitializeX();
			IFXDECLARELOCAL(IFXUnknown,pDecoderChainAsUnknown);
			IFXCHECKX(pDecoderChainX->QueryInterface(IID_IFXUnknown, (void**)&pDecoderChainAsUnknown));
			IFXCHECKX(pDecoderPalette->SetResourcePtr(uResourceID, pDecoderChainAsUnknown));
		}

		pDecoderChainX->AddDecoderX(*pDecoderX, uChainPosition);
	}
}

void CIFXLoadManager::ProcessTransferOrderX(IFXRESULT& rWarningCode)
{
	U32       uStartTime = IFXOSGetTime();
	BOOL      bPartialTransfer = FALSE;
	BOOL      bAnyPartialTransfer  = FALSE;
	U32       uCurrentTime = uStartTime;
	U32       uElapsedTime = uCurrentTime - uStartTime;
	U32       i = 0, j = 0;//, k = 0;

	// get the scenegraph
	IFXDECLARELOCAL(IFXSceneGraph,pSceneGraph);
	IFXCHECKX(m_pCoreServices->GetSceneGraph(IID_IFXSceneGraph, (void**)&pSceneGraph));

	// traverse each palette, and for each resource of that palette, call its transfer method...
	for (i = 0; i < IFXSceneGraph::NUMBER_OF_PALETTES; i++) {

		U32 uPaletteIndex = 0;
		IFXRESULT iPaletteIteratorReturnCode = IFX_OK;

		// attempt to access all palette entries
		for (iPaletteIteratorReturnCode = m_ppDecoderPalettes[i]->First(&uPaletteIndex); IFXSUCCESS(iPaletteIteratorReturnCode); iPaletteIteratorReturnCode = m_ppDecoderPalettes[i]->Next(&uPaletteIndex)) {
			// For each decoder in the component chain referenced by a palette entry,
			// transfer (i.e. decode) that decoder's content to the scenegraph.
			IFXDECLARELOCAL(IFXDecoderChainX,pDecoderChainX);
			IFXCHECKX(m_ppDecoderPalettes[i]->GetResourcePtr(uPaletteIndex, IID_IFXDecoderChainX, (void**)&pDecoderChainX));

			U32 uDecoderCount = 0;
			pDecoderChainX->GetDecoderCountX(uDecoderCount);

			// for the next decoder palette entry.
			for (j = 0; j < uDecoderCount; j++) {
				IFXDECLARELOCAL(IFXDecoderX,pDecoderX);
				try {
					pDecoderChainX->GetDecoderX(j, pDecoderX); /// @todo: need to handle cases where the decoder isn't present
				} catch (...) {
					continue;
				}

				// Perform idling activities.
				ThumpX();

				IFXRESULT iResultTransfer = IFX_OK;
				if(pDecoderX)
					pDecoderX->TransferX(iResultTransfer);

				bPartialTransfer = (IFX_W_PARTIAL_TRANSFER == iResultTransfer);

				// If a decoder has transferred all of its blocks and the read process has concluded,
				// then remove that decoder from the decoder chain
				if (FALSE == bPartialTransfer && TRUE == m_bReadDone) {
					pDecoderChainX->RemoveDecoderX(j);
				}

				bAnyPartialTransfer = bAnyPartialTransfer || bPartialTransfer;
			}

			// if the read process has concluded and all decoders have been removed from the decoder chain,
			// the remove the decoder chain from the decoder palette.
			pDecoderChainX->GetDecoderCountX(uDecoderCount);
			if (0 == uDecoderCount && TRUE == m_bReadDone) {
				IFXCHECKX(m_ppDecoderPalettes[i]->DeleteById(uPaletteIndex));
			}
		}
	}

	uCurrentTime = IFXOSGetTime();
	uElapsedTime = uCurrentTime - uStartTime;

	while (bAnyPartialTransfer && (m_bAtomicLoad || (uElapsedTime < m_uTransferTimeLimit))) {
		U32 uPaletteIndex = 0;
		IFXRESULT iPaletteIteratorReturnCode = IFX_OK;

		bAnyPartialTransfer = FALSE;

		i = IFXSceneGraph::GENERATOR;

		for( iPaletteIteratorReturnCode  = m_ppDecoderPalettes[i]->First(&uPaletteIndex); 
			 IFXSUCCESS(iPaletteIteratorReturnCode); 
			 iPaletteIteratorReturnCode = m_ppDecoderPalettes[i]->Next(&uPaletteIndex) ) 
		{
			// For each decoder in the component chain referenced by a palette entry,
			// transfer (i.e. decode) that decoder's content to the scenegraph.
			IFXDECLARELOCAL(IFXDecoderChainX,pDecoderChainX);
			IFXCHECKX(m_ppDecoderPalettes[i]->GetResourcePtr(uPaletteIndex, IID_IFXDecoderChainX, (void**)&pDecoderChainX));

			U32 uDecoderCount = 0;
			pDecoderChainX->GetDecoderCountX(uDecoderCount);

			// for the next decoder palette entry.
			for (j = 0; j < uDecoderCount ; j++) {
				IFXDECLARELOCAL(IFXDecoderX,pDecoderX);
				pDecoderChainX->GetDecoderX(j, pDecoderX); /// @todo: need to handle cases where the decoder isn't present

				// Perform idling activities.
				ThumpX();

				IFXRESULT iResultTransfer = IFX_OK;
				if (NULL != pDecoderX) {
					pDecoderX->TransferX(iResultTransfer);
				} else {
					IFXASSERT(0); /// @todo: Remove assert
				}

				bPartialTransfer = (IFX_W_PARTIAL_TRANSFER == iResultTransfer);

				// If a decoder has transferred all of its blocks and the read process has concluded,
				// then remove that decoder from the decoder chain
				if (FALSE == bPartialTransfer && TRUE == m_bReadDone) {
					pDecoderChainX->RemoveDecoderX(j);
				}

				bAnyPartialTransfer = bAnyPartialTransfer || bPartialTransfer;
			}

			// if the read process has concluded and all decoders have been removed from the decoder chain,
			// the remove the decoder chain from the decoder palette.
			pDecoderChainX->GetDecoderCountX(uDecoderCount);
			if(0 == uDecoderCount && TRUE == m_bReadDone) {
				IFXCHECKX(m_ppDecoderPalettes[i]->DeleteById(uPaletteIndex));
			}
		}

		uCurrentTime = IFXOSGetTime();
		uElapsedTime = uCurrentTime - uStartTime;
	}

	if (bAnyPartialTransfer)
		rWarningCode = IFX_W_PARTIAL_TRANSFER;
}

void CIFXLoadManager::NotifyStreamStateX()
{
	EIFXLoadState eState;
	GetAggregateLoadState(eState);
	// Check for changes
	if (m_eOldState == eState)
		return;
	m_eOldState = eState;

	IFXDECLARELOCAL(IFXScheduler,pScheduler);
	if (IFXFAILURE(m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)))
		return;

	IFXDECLARELOCAL(IFXNotificationManager,pNotificationManager);
	IFXCHECKX(pScheduler->GetNotificationManager(&pNotificationManager));

	IFXDECLARELOCAL(IFXNotificationInfo,pInfo);
	IFXCHECKX(IFXCreateComponent(CID_IFXNotificationInfo, IID_IFXNotificationInfo, (void**)&pInfo));

	IFXCHECKX(pInfo->Initialize(m_pCoreServices));
	IFXCHECKX(pInfo->SetType(IFXNotificationType_Update));
	IFXCHECKX(pInfo->SetId(IFXNotificationId_StreamState)); // subType
	IFXCHECKX(pInfo->SetKeyFilter(eState)); // state
	IFXCHECKX(pInfo->SetNotificationData(this)); // bytes
	IFXCHECKX(pInfo->SetUserData((void*)NotificationTaskID)); // bytes
	pNotificationManager->SubmitEvent(pInfo);
}

void CIFXLoadManager::ThumpX()
{
	IFXASSERT(m_pCoreServices);

	IFXDECLARELOCAL(IFXScheduler,pScheduler);
	if (IFXFAILURE(m_pCoreServices->GetScheduler(IID_IFXScheduler, (void**)&pScheduler)))
		return;

	IFXDECLARELOCAL(IFXNotificationManager,pNotificationManager);
	IFXCHECKX(pScheduler->GetNotificationManager(&pNotificationManager));

	// Submit the idle notify event to the Notification Manager
	// so that idling activities are performed such as making
	// sure that sound buffers are kept full and don't run dry.
	IFXCHECKX(pNotificationManager->SubmitEvent(IFXNotificationType_Idle, IFXNotificationId_Nil, 0, NULL));
}

void CIFXLoadManager::ValidateInputX(void* p)
{
	// Check input values
	if (NULL == p) {
		IFXCHECKX(IFX_E_INVALID_POINTER);
	} else if (NULL == m_pCoreServices) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}
}

IFXRESULT IFXAPI_CALLTYPE CIFXLoadManager_Factory(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT rc = IFX_OK;

	if (ppInterface)  {
		// Create the CIFXLoadManager component.
		CIFXLoadManager *pComponent = new CIFXLoadManager;

		if (pComponent) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface(interfaceId, ppInterface);

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}
