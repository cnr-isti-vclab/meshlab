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
	@file	CIFXLight.h
*/

#ifndef __CIFXLIGHT_H__
#define __CIFXLIGHT_H__

#include "CIFXNode.h"
#include "IFXLight.h"

class CIFXLight : private CIFXNode, virtual public IFXLight
{
			CIFXLight();
	virtual  ~CIFXLight();
	friend
	IFXRESULT IFXAPI_CALLTYPE CIFXLight_Factory( IFXREFIID riid,
								void**    ppv );
public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid,
							void**    ppv );


	// IFXMarkerX
	void IFXAPI       GetEncoderX ( IFXEncoderX*& rpEncoderX );

	// IFXSpatial
	IFXRESULT IFXAPI GetSpatialBound(    IFXVector4&        rOutSphere, U32 WorldInstance );
	IFXSpatial::eType IFXAPI GetSpatialType();

	// IFXModifier
	IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
						U32&       rOutNumberOfOutputs,
						U32*&      rpOutOutputDepAttrs );
	IFXRESULT IFXAPI  GetDependencies ( IFXGUID*   pInOutputDID,
								IFXGUID**& rppOutInputDependencies,
								U32&       rOutNumberInputDependencies,
								IFXGUID**& rppOutOutputDependencies,
								U32&       rOutNumberOfOutputDependencies,
								U32*&      rpOutOutputDepAttrs );
	IFXRESULT IFXAPI  GenerateOutput ( U32    inOutputDataElementIndex,
							void*& rpOutData, BOOL& rNeedRelease );
	IFXRESULT IFXAPI  SetDataPacket ( IFXModifierDataPacket* pInInputDataPacket,
							IFXModifierDataPacket* pInDataPacket );
	IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage,
            void*         pMessageContext );

	// Node
	void IFXAPI Counter(EIFXNodeCounterType type, U32* puOutCount);

	// IFXLight
	void IFXAPI Disable( IFXRenderContext* pRenderLayer, U32 uInLightID);
	void IFXAPI Enable( IFXRenderContext*  pRenderLayer, U32* puInLightID, U32 lightInstance);
	IFXLightResource* IFXAPI GetLightResource( void );
	IFXRESULT IFXAPI GetLightResourceID(U32*);
	IFXRESULT IFXAPI SetLightResourceID(U32);

private:
  // IFXModifier
  static const IFXGUID* m_scpOutputDIDs[];

  // IFXLight
  U32 m_lightResourceID;
};


#endif
