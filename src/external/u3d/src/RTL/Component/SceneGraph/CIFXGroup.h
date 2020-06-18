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
	@file	CIFXGroup.h

			The header file that defines the base implementation class of the
			CIFXGroup.
*/

#ifndef CIFXGROUP_H
#define CIFXGROUP_H

#include "CIFXNode.h"
#include "IFXCoreCIDs.h"

class CIFXGroup : private CIFXNode,
          virtual public   IFXNode
{
            CIFXGroup();
  virtual  ~CIFXGroup();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXGroup_Factory(IFXREFIID iid, void** ppv);

public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);


  // Node
  void  IFXAPI  Counter(EIFXNodeCounterType type, U32* puOutCount);

  // IFXModifier
  IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
                        U32&       rOutNumberOfOutputs,
              U32*&    rpOutOutputDepAttrs );
  IFXRESULT IFXAPI  GetDependencies (    IFXGUID*   pInOutputDID,
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

  // IFXMarkerX
  void IFXAPI       GetEncoderX (IFXEncoderX*& rpEncoderX);

private:
  // IFXModifier
  static const IFXGUID* m_scpOutputDIDs[];
};

#endif
