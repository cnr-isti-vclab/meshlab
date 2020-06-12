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

/**
	@file	IFXModifierBaseDecoder.h

			Declaration of the IFXModifierBaseDecoder.
			The IFXModifierBaseDecoder contains common modifier decoding
			functionality that is used by the individual specific modifier
			(and generator) decoders (e.g. CIFXGlyphGeneratorDecoder).

	@note	This class is intended to be used as an abstract base class
			for various types of modifier decoders. As such, both the
			constructor and destructor are declared as protected members.
			This does, in itself, suffice in keeping a stand-alone instance
			of this class from being created. Note that the destructor is
			also declared as pure virtual to further enforce the abstract
			nature of this class - but the destructor does still have an
			implementation.
*/

#ifndef IFXModifierBaseDecoder_H
#define IFXModifierBaseDecoder_H

#include "IFXCoreServices.h"
#include "IFXDataBlockQueueX.h"
#include "IFXAutoRelease.h"
#include "IFXSceneGraph.h"
#include "IFXDecoderX.h"

class IFXModifierBaseDecoder
{
protected:
	// Member functions.
	IFXModifierBaseDecoder();
	virtual ~IFXModifierBaseDecoder() = 0;

	void InitializeX(const IFXLoadConfig &rLoadConfig);
	void CreateObjectX(IFXDataBlockX &rDataBlockX, IFXREFCID cid);
	void ProcessChainX( IFXDataBlockX &rDataBlockX );

	// Member data.
	U32 m_uRefCount;
	U32 m_uLoadId;
	U32 m_uChainPosition;
	IFXSceneGraph::EIFXPalette m_ePalette;
	BOOL m_bExternal;
	IFXString m_stringObjectName;
	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXDataBlockQueueX,m_pDataBlockQueueX);
	IFXDECLAREMEMBER(IFXUnknown,m_pObject);
};

#endif
