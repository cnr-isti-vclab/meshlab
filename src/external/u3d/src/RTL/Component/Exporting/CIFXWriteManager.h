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
//  CIFXWriteManager.h
//
//  DESCRIPTION
//    This module defines the CIFXWriteManager component.  It is used to...
//
//  NOTES
//
//***************************************************************************
///@todo: usage
#ifndef __CIFXWRITEMANAGER_H__
#define __CIFXWRITEMANAGER_H__

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXWriteManager.h"
#include "IFXBlockWriterX.h"
#include "IFXCoreServices.h"
#include "IFXSceneGraph.h"
#include "IFXDataBlockQueueX.h"
#include "IFXAutoRelease.h"
#include "IFXArray.h"
#include "IFXExtensionEncoderX.h"

class CIFXWriteManager : public IFXWriteManager
{
	CIFXWriteManager();
	virtual ~CIFXWriteManager();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXWriteManager_Factory( IFXREFIID interfaceId, void** ppInterface );

public:
	// Member functions: IFXUnknown.
	U32 IFXAPI      AddRef ();
	U32 IFXAPI      Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// Member functions: IFXWriteManager.
	virtual IFXRESULT IFXAPI  Initialize( IFXCoreServices *pCoreServices );
	virtual IFXRESULT IFXAPI  Write( IFXWriteBuffer *pWriteBuffer, IFXExportOptions eExportOptions );
	virtual IFXRESULT IFXAPI  SetQualityFactor( U32 uQualityFactor, IFXExportOptions ePalettes );

private:

	// this internal structure is used to store data about extension blocktypes
	struct IFXExtBlocktype
	{
		IFXCID m_cid;
		U32 m_blocksCount;
		U32	*m_blockTypes;
		IFXExtBlocktype()  { m_blocksCount = 0; m_blockTypes = NULL; };
		~IFXExtBlocktype() { if( m_blockTypes ) delete m_blockTypes; };
	};

	// this indicates whether a given component is to be written
	void ComponentIsMarkedForWrite( IFXUnknown* pInUnknown, BOOL* pbOutMarked );

	// retrieve data blocks from resources contained by a given palette
	void PutPalette( IFXSceneGraph::EIFXPalette ePalette );

	// write out all the blocks in the priority queue to the write buffer passed in via Write()
	void WriteQueueX();

	// set the quality on all resources contained by a given palette
	void SetQualityOnPalette( U32 qualityFactor, IFXSceneGraph::EIFXPalette ePalette );

	/// Checks if given encoder is an extension one. If yes then process it.
	void HandleExtensionsX( IFXEncoderX *pEncoder, IFXDataBlockQueueX *pDBQueue );
	/// Puts New Object declaration block (0xFFFFFF16) into file with given data. 
	void CreateNewObjectDeclarationBlockX( U32 *blockTypes, IFXExtensionData *pExtData, 
		IFXDataBlockQueueX& rDataBlockQueue );

	// IFXUnknown attributes...
	U32 m_uRefCount;

	// IFXWriteManager attributes...
	IFXDECLAREMEMBER(IFXSceneGraph,m_pScenegraph);
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXDataBlockQueueX,m_pPriorityQueue);
	IFXDECLAREMEMBER(IFXBlockWriterX,m_pBlockWriter);
	IFXExportOptions m_ExportOptions;

	/// Current new object block type value
	U32 m_newObjectBlockType;
	/** Array to store CIDs of extension encoders which already wrote their New Object declaration block (NODB).
		We need to keep track of this to prevent existance of multiple NODB for the same extension object.*/
	IFXArray<IFXExtBlocktype> m_extCIDs;
};

#endif
