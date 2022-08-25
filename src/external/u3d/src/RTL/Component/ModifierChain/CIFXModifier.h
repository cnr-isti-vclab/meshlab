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
	@file	CIFXModifier.h

			The header file that defines the base implementation class of the
			CIFXModifier. 
*/

#ifndef CIFXModifier_H
#define CIFXModifier_H

#include "IFXModifier.h"
#include "CIFXSubject.h"
#include "CIFXMarker.h"


class CIFXModifier : virtual public    CIFXMarker,
							 protected CIFXSubject,
					 virtual protected IFXObserver,
					 virtual public    IFXModifier
{
protected:
			CIFXModifier();
  virtual  ~CIFXModifier();

  // IFXModifier
  virtual IFXRESULT IFXAPI  SetDataPacket( 
								IFXModifierDataPacket* pInInputDataPacket,
								IFXModifierDataPacket* pInDataPacket ) = 0;

  IFXRESULT  IFXAPI 	GetDataPacket( 
								IFXModifierDataPacket*& rpOutDataPacket );

  virtual IFXRESULT IFXAPI  GetOutputs( 
								IFXGUID**& rpOutOutputs,
								U32&       rOutNumberOfOutputs,
								U32*&      rpOutUnchangedOutputAttrs ) = 0;

  virtual IFXRESULT IFXAPI  GetDependencies(
								IFXGUID*   pInOutputDID,
								IFXGUID**& rppOutInputDependencies,
								U32&       rOutNumberInputDependencies,
								IFXGUID**& rppOutOutputDependencies,
								U32&       rOutNumberOfOutputDependencies,
								U32*&      rpOutOutputDepAttrs )=0;

  virtual IFXRESULT IFXAPI  GenerateOutput( 
								U32  inOutputDataElementIndex,
								void*& rpOutData, BOOL& rNeedRelease ) = 0;

  virtual IFXRESULT IFXAPI  Notify( 
								IFXModifierMessage eInMessage,
								void*         pMessageContext );

  IFXModifierChain*   m_pModChainNR;
  IFXSubject*       m_pModChainSubNR;

  IFXModifierDataPacket*  m_pModifierDataPacket;
  IFXModifierDataPacket*  m_pInputDataPacket;

  U32           m_uModifierChainIndex;

  IFXRESULT  IFXAPI 	SetModifierChain( IFXModifierChain* pInModifierChain, U32 inModifierChainIndex );
  IFXRESULT  IFXAPI 	GetModifierChain( IFXModifierChain** ppOutModifierChain );
  IFXRESULT  IFXAPI 	GetModifierChainIndex( U32& rOutModifierChainIndex );

  // IFXObserver
  virtual IFXRESULT IFXAPI  Update(IFXSubject* pInSubject, U32 uInChangeBits,IFXREFIID rIType = IID_IFXUnknown);

  void  IFXAPI 	PreDestruct();
};

#endif
