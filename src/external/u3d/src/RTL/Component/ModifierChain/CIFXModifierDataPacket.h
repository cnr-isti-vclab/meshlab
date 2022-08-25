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
@file  CIFXModifierDataPacket.h

  The header file that defines the base implementation class of
  the CIFXModifierDataPacket. 
*/

#ifndef __CIFXMODIFIERDATAPACKET_H__
#define __CIFXMODIFIERDATAPACKET_H__

#include "IFXModifierDataPacketInternal.h"
#include "IFXModifierChainInternal.h"
#include "IFXModifierDataElementIter.h"
#include "CIFXSubject.h"
#include "IFXCoreCIDs.h"

class CIFXModifierDataPacket : virtual public IFXModifierDataPacketInternal,
                public CIFXSubject
{
        CIFXModifierDataPacket();
  virtual  ~CIFXModifierDataPacket();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXModifierDataPacket_Factory(IFXREFIID iid, void** ppv);

public:
  // IFXUnknown interface
  U32 IFXAPI      AddRef ();
  U32 IFXAPI      Release ();
  IFXRESULT IFXAPI  QueryInterface (     IFXREFIID    riid,
                    void**       ppv );

  // IFXDataPacket interface

  IFXRESULT  IFXAPI 	 GetDataElementAspectBit(  IFXREFGUID     rInDataElement,
                    U32&       ruOutBitField );

  IFXRESULT  IFXAPI 	 GetDataElementIndex(    IFXREFGUID     rInDataElementGUID,
                    U32&       ruOutIndex );
  IFXRESULT  IFXAPI 	 GetDataPacketSize(    U32&       ruOutNumberOfDataElements );
  IFXRESULT  IFXAPI 	 GetDataElementGUID(   U32        inDataElementIndex,
                    IFXGUID*&    rpOutDataElementGUID );
  IFXRESULT  IFXAPI 	 GetDataElement(     U32        inDataElementIndex,
                    void**       ppOutDataElement );
  IFXRESULT  IFXAPI 	 GetDataElement(     U32        inDataElementIndex,
                    IFXREFIID    desiredInterface,
                    void**       ppOutDataElement );


  // IFXModifierDataPacket interface
  IFXRESULT  IFXAPI 	 GetDataElementChangeCount(U32       inDataElementIndex,
                    U32&      uOutChangeCount );
  IFXRESULT  IFXAPI 	 InvalidateDataElement(  U32       inInvalidDataElementIndex );
  IFXRESULT  IFXAPI 	 DataElementIsValid(   U32       inDataElementIndex,
                    BOOL&     rbOutIsValid );

  IFXRESULT  IFXAPI 	 GetModifierChain(IFXModifierChain**);
  IFXRESULT  IFXAPI 	 GetChainIndex(U32*);


  // Iterator methods
  IFXRESULT  IFXAPI 	 GetIterator(U32 in_Flags, IFXModifierDataElementIterator**);

  // IFXModifierDataPacketInternal interface
  IFXRESULT  IFXAPI 	 SetModifierChain(IFXModifierChain*, U32,
                IFXDataPacketState*);

  IFXRESULT  IFXAPI 	 GetDataPacketState(IFXDataPacketState** out_ppState,
            IFXIntraDependencies**);

  void IFXAPI   DoPostChanges(U32 in_Bit) { PostChanges(in_Bit); }

private:
  // IFXUnknown
  U32               m_uRefCount;

  // IFXModifierDataPacket
  IFXModifierChain*       m_pModChain;
  IFXModifierChainInternal*   m_pModChainInternal;

  U32               m_ModChainIndex;
  IFXDataPacketState*       m_pState; // owned by the modchain

  U32                              m_uObservedElementCount;

  void Destruct();
};

#endif
