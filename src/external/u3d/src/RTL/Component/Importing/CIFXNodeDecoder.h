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
//
//	CIFXNodeBaseDecoder.h
//
//	DESCRIPTION:
//		Declaration of the CIFXNodeBaseDecoder.
//		The CIFXNodeBaseDecoder contains common node decoding functionality that
//		is used by the individual specific node decoders (e.g. CIFXLightDecoder).
//
//	NOTES: This class is intended to be used as an abstract base class
//		   for various types of node decoders. As such, both the
//		   constructor and destructor are declared as protected members.
//		   This does, in itself, suffice in keeping a stand-alone instance
//		   of this class from being created. Note that the destructor is
//		   also declared as pure virtual to further enforce the abstract
//		   nature of this class - but the destructor does still have an
//		   implementation.
//
//*****************************************************************************
#ifndef CIFXNODEDECODER_H__
#define CIFXNODEDECODER_H__

#include "IFXNode.h"
#include "IFXDataBlockQueueX.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXBitStreamX.h"
#include "IFXAutoRelease.h"
#include "IFXNameMap.h"
#include "IFXDecoderX.h"

class CIFXNodeBaseDecoder
{
protected:
	// Member functions.
	CIFXNodeBaseDecoder();
	virtual ~CIFXNodeBaseDecoder() = 0;

	BOOL CommonNodeReadU3DX(IFXDataBlockX &rDataBlockX);
	void CheckInitializedX();
	void InitializeX(const IFXLoadConfig &lc);
	void CreateObjectX(IFXDataBlockX &rDataBlockX, IFXREFCID cid);
	void AddParentNodePlaceholderX( IFXDataBlockX &rDataBlockX );
	void ReplaceNodeX( U32 uResourceId, IFXUnknown &rUnknownNode );

	// Member data.
	U32 m_uRefCount;	// Number of outstanding references to the object.
	U32 m_uLoadId;
	BOOL m_bExternal;
	IFXDECLAREMEMBER(IFXBitStreamX,m_pBitStreamX);
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXDataBlockQueueX,m_pDataBlockQueueX);
	IFXDECLAREMEMBER(IFXNode,m_pNode);  // the node
	IFXDECLAREMEMBER(IFXSceneGraph,m_pSceneGraph);

	F64 m_unitScale;
};

#endif // #ifndef CIFXNodeBaseDecoder_H__
