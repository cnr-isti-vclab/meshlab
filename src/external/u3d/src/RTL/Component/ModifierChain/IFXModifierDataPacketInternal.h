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
	@file	IFXModifierDataPacketInternal.h

			The header file that defines the IFXModifierDataPacketInternal 
			interface.
*/

#ifndef __IFXMODIFIERDATAPACKETINTERNAL_INTERFACES_H__
#define __IFXMODIFIERDATAPACKETINTERNAL_INTERFACES_H__

#include "IFXModifierDataPacket.h"

class IFXModifierChain;
class IFXModifierDataPacketInternal;
class IFXModifier;

#define APPENDED_DATAPACKET_INDEX  (U32)-3
#define PROXY_DATAPACKET_INDEX  (U32)-2
#define INVALID_DATAPACKET_INDEX  (U32)-1
#define TIME_ELEMENT_INDEX  0
#define TRANSFORM_ELEMENT_INDEX  1

#define IFXDATAELEMENTSTATE_INVALID  0x00
#define IFXDATAELEMENTSTATE_VALID   0x01
#define IFXDATAELEMENTSTATE_CONSUMED 0x02
#define IFXDATAELEMENTSTATE_VALIDATIONFAILED 0x03

#define INVALID_DATAELEMENT_INDEX (U32)-1


struct IFXIntraDependencies;
#define IFXDIDINVSEQGROWSIZE 2

struct IFXDidInvElement
{
  U32         uMIndex; // Modifier index
  U32         uEIndex; // Element index

  IFXDidInvElement() {};
  IFXDidInvElement(U32 in_uM, U32 in_uEl)
  : uMIndex(in_uM),uEIndex(in_uEl)
  { };

  BOOL operator == (const IFXDidInvElement& in_rhs)
  {
    return in_rhs.uEIndex == uEIndex && in_rhs.uMIndex == uMIndex;
  };
};

struct IFXDataElementState
{
  U32         State:4;
  U32         AspectBit:5;
  U32         Pad:23;
  void*       pValue;
  BOOL        bNeedRelease;
  U32         ChangeCount;
  U32         Generator;

  // Invalidations
  /// @todo: Move Away from Here-- in to private file.
  U32         m_uInvCount;
  U32         m_uInvAllocated;
  IFXDidInvElement* m_pInvSeq;

  IFXDataElementState();
  ~IFXDataElementState();
  IFXRESULT AddInv(U32 in_ModIdx, U32 in_DEIdx);
};

struct IFXDataPacketState
{
  U32               m_NumDataElements;
  U32               m_Enabled;
  U32               m_LockedDataElement;
  IFXDidEntry*          m_pDids;
  IFXDataElementState*      m_pDataElements;
  IFXModifierDataPacketInternal*  m_pDataPacket;
  IFXModifier*          m_pModifier;

  IFXDataPacketState();
  ~IFXDataPacketState();
};



// {C3A2BC97-84C-40C6-86E2-BA9AAF5AAD3F}
IFXDEFINE_GUID(IID_IFXModifierDataPacketInternal,
0xC3A2BC97, 0x84C, 0x40C6, 0x86, 0xE2, 0xBA, 0x9A, 0xAF, 0x5A, 0xAD, 0x3F);


class IFXModifierDataPacketInternal : virtual public IFXModifierDataPacket
/**
       The IFXModifierDataPacketInternal interface supplies extended DataPacket
     services that relate to result caching within a ModifierChain. This
     Internal interface is used to allow efficient interaction between
     the IFXModifierChain and the data packet*/
{
public:


// INITIALIZATION:

  virtual IFXRESULT IFXAPI  SetModifierChain(IFXModifierChain* in_pModChain,
                    U32 in_ChainIdx,
                    IFXDataPacketState* in_pState) = 0;
  /**<
          Sets the Modifier Chain and the index of the data packet in
      the modifier chain.

  @param  in_pModChain
           The index of the DataElement.

  @param  in_ChainIdx
           The U32 refering to the index.

  @return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The ModifierDataPacket was not initialized properly. */


  virtual IFXRESULT IFXAPI  GetDataPacketState(IFXDataPacketState** out_ppState,
            IFXIntraDependencies**) = 0;

  virtual void IFXAPI  DoPostChanges(U32) = 0;

};

#endif
