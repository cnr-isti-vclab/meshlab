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

/**
	@file	CIFXDummyModifier.h

			The header file that defines the base implementation class of the
			CIFXDummyModifier.
*/

#ifndef __CIFXDummyModifier_H__
#define __CIFXDummyModifier_H__

#include "CIFXModifier.h"
#include "IFXCLODManagerInterface.h"
#include "IFXNeighborResControllerIntfc.h"
#include "IFXRenderable.h"
#include "IFXDummyModifier.h"

class CIFXDummyModifier : private CIFXModifier,
							virtual public IFXDummyModifier
{
public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (   
								IFXREFIID  riid,
								void**     ppv );
	// IFXMarker
	void IFXAPI		  GetEncoderX (IFXEncoderX*& rpEncoderX);

	// IFXModifier
	IFXRESULT IFXAPI  GetOutputs ( 
								IFXGUID**& rpOutOutputs,
								U32&       rOutNumberOfOutputs,
								U32*&	   rpOutOutputDepAttrs );
	IFXRESULT IFXAPI  GetDependencies (    
								IFXGUID*   pInOutputDID,
								IFXGUID**& rppOutInputDependencies,
								U32&       rOutNumberInputDependencies,
								IFXGUID**& rppOutOutputDependencies,
								U32&       rOutNumberOfOutputDependencies,
								U32*&      rpOutOutputDepAttrs );

	IFXRESULT IFXAPI  GenerateOutput ( 
								U32    inOutputDataElementIndex,
								void*& rpOutData, BOOL& rNeedRelease );

	IFXRESULT IFXAPI  SetDataPacket ( 
								IFXModifierDataPacket* pInInputDataPacket,
								IFXModifierDataPacket* pInDataPacket );
	IFXRESULT IFXAPI  Notify (	IFXModifierMessage eInMessage,
								void*             pMessageContext );

	// IFXDummyModifier
	IFXRESULT IFXAPI SetDataBlock( IFXDataBlockQueueX *pDataBlock );
	IFXDataBlockQueueX* IFXAPI GetDataBlock();

private:
			CIFXDummyModifier();
	virtual ~CIFXDummyModifier();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXDummyModifier_Factory( IFXREFIID iid, void** ppv );

	U32 m_uRefCount;

	IFXDataBlockQueueX *m_pDataBlockQueue;
};

#endif
