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
	@file	CIFXLoadManager.h

			Declaration of the CIFXLoadManager class which implements the IFXLoadManager 
			and IFXTask interfaces.
			The CIFXLoadManager is used to load a file from an IFXReadBuffer
			into the scene graph.
*/

#ifndef CIFXLOADMANAGER_H__
#define CIFXLOADMANAGER_H__

#include "IFXLoadManager.h"
#include "IFXTask.h"
#include "IFXBlockReaderX.h"
#include "IFXSceneGraph.h"
#include "IFXBTTHash.h"
#include "IFXAutoRelease.h"
#include "IFXExtensionDecoderX.h"
#include "IFXFileReference.h"
#include "IFXArray.h"
#include "IFXNameMap.h"

class CIFXLoadManager : public IFXLoadManager, public IFXTask
{
public:

	// IFXUnknown
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXLoadManager

	// Initialize - get a reference to a core services object.  The core services 
	// object will be used to get access to scene graph and scheduler
	virtual IFXRESULT IFXAPI  Initialize( IFXCoreServices* pCoreServices );
	
	// Load - start loading data from the read buffer
	virtual IFXRESULT IFXAPI  Load(
						IFXReadBuffer* pReadBuffer, 
						const BOOL bAtomicLoad = TRUE, 
						const IFXString& sScope = L"", 
						const IFXCollisionPolicy uCollisionPolicy = IFXCOLLISIONPOLICY_REPLACE, 
						const U32 uLoadId = 0);
	virtual IFXRESULT IFXAPI  Load(
						IFXReadBuffer* pReadBuffer, 
						const BOOL bAtomicLoad, const IFXString& sScope, 
						const IFXCollisionPolicy uCollisionPolicy, 
						const U32 uLoadId, 
						const IFXObjectFilters& rObjectFilters, 
						const IFXString& sWorldAlias, 
						const BOOL bExternalLoad);

	// Cancel - stop the load operation, clean up, return to initialized state
	virtual IFXRESULT IFXAPI  Cancel();
	virtual IFXRESULT IFXAPI  Reset();

	virtual IFXRESULT IFXAPI  GetAggregateLoadState(EIFXLoadState& rLoadState);
	virtual IFXRESULT IFXAPI  GetAggregateFileSize(U64& rFileSize);
	virtual IFXRESULT IFXAPI  GetAggregateFileSizeLoaded(U64& rFileSize);
	virtual IFXRESULT IFXAPI  GetFileSize(U64& rFileSize);
	virtual IFXRESULT IFXAPI  GetFileSizeLoaded(U64& rFileSize);
	virtual IFXRESULT IFXAPI  GetSubLoadManagersCount(U32& rCount);
	virtual IFXRESULT IFXAPI  GetSubLoadManager(U32 uIndex, IFXLoadManager*& rLoadManager);

	virtual IFXRESULT IFXAPI  GetReadTimeLimit(U32& rReadTimeLimit);
	virtual IFXRESULT IFXAPI  SetReadTimeLimit(const U32 rReadTimeLimit);
	virtual IFXRESULT IFXAPI  GetTransferTimeLimit(U32& rTransferTimeLimit);
	virtual IFXRESULT IFXAPI  SetTransferTimeLimit(const U32 rTransferTimeLimit);

	// IFXTask
	virtual IFXRESULT IFXAPI  Execute( IFXTaskData* pTaskData );

private:
	// Member functions.
	CIFXLoadManager();
	virtual	~CIFXLoadManager();

	void ExecuteReadX();
	void ExecuteTransferX();

	void UnRegisterReadX();
	void UnRegisterTransferX();

	// block processing methods	
	void ProcessBlockX( 
				IFXDataBlockX& rDataBlockX, 
				IFXSceneGraph::EIFXPalette ePalette = IFXSceneGraph::NUMBER_OF_PALETTES ); //  driver for block processing
	void ProcessModifierChainBlockX( IFXDataBlockX& rDataBlockX ); // modifier chain blocks
	void ProcessFileReferenceBlockX( IFXDataBlockX& rDataBlockX ); // file reference blocks
	void ProcessFileHeaderBlockX( IFXDataBlockX& rDataBlockX ); // file header blocks
	void ProcessGenericBlockX( 
				IFXDataBlockX& rDataBlockX, 
				IFXSceneGraph::EIFXPalette ePalette = IFXSceneGraph::NUMBER_OF_PALETTES ); // for objects that reside in modifier chains
	void ProcessNewBlockTypeDeclarationBlockX( IFXDataBlockX& rDataBlockX ); // new block type declaration blocks
	void ProcessPriorityUpdateBlockX( IFXDataBlockX& rDataBlockX ); // priority update blocks
	
	void ProcessTransferOrderX(IFXRESULT& rWarningCode);

	void IFXAPI  NotifyStreamStateX();
	void ThumpX();

	void ValidateInputX( void* p );

	class IFXExternalDecoderCont
	{
	public:
		IFXExternalDecoderCont():
			m_pContBlockTypes(NULL),
			m_contCount(0),
			pNBTDataBlock(NULL)
		{}

		~IFXExternalDecoderCont() { IFXDELETE_ARRAY( m_pContBlockTypes ); IFXRELEASE( pNBTDataBlock ); }

		IFXCID m_decoderCID;
		U32* m_pContBlockTypes; // order is important. Array of continuation blocktypes
		U32 m_contCount; // array's size
		IFXDataBlockX* pNBTDataBlock; // New Block Type Data Blocks. Addref it when set it up
	};

	IFXArray<IFXExternalDecoderCont> m_extensionData; // array of external decoders with cont. blocks

	// Member data.
	U32					m_uRefCount;	// Number of outstanding references to the object.
	IFXDECLAREMEMBER(IFXCoreServices, m_pCoreServices);
	IFXDECLAREMEMBER(IFXBlockReaderX, m_pBlockReaderX);
	IFXDECLAREMEMBER(IFXNameMap, m_pNameMap);
	IFXDECLAREMEMBER(IFXBTTHash,m_pBTTHash);
	IFXTaskHandle		m_ReadTaskHandle;
	IFXTaskHandle		m_TransferTaskHandle;
	IFXTaskHandle		m_NotificationTaskHandle;
	BOOL				m_bReadDone;
	BOOL				m_bTransferDone;
	BOOL				m_bExternal;
	U32					m_uLoadId;
	IFXString			m_sScope;
	IFXCollisionPolicy	m_CollisionPolicy;
	IFXObjectFilters	m_ObjectFilters;
	U32					m_uProfile;
	U32					m_uDeclarationSize;
	U32					m_uPriority;
	U64					m_fileSize;
	U32					m_characterEncoding;

	IFXArray<IFXLoadManager*> m_SubLoadManagers;
	IFXArray<IFXString> m_RejectedNames;

	static const U32	ReadTaskID;
	static const U32	TransferTaskID;
	static const U32	NotificationTaskID;
	
	EIFXLoadState		m_eState;
	EIFXLoadState		m_eOldState;

	IFXPalette**		m_ppDecoderPalettes;

	BOOL				m_bTransferNow;
	BOOL				m_bAtomicLoad;

	// Time limits for read and transfer task are in the units used by IFXOS_GetTime()
	// which is currently milliseconds.
	U32	m_uReadTimeLimit;
	U32	m_uTransferTimeLimit;

	static const U32	m_uMinDecoderPaletteSize;

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXLoadManager_Factory( IFXREFIID interfaceId, void** ppInterface );
};

#endif
