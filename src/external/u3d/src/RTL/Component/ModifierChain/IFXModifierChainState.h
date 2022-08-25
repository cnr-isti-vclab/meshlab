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
/*
@file  IFXModifierChainState.h

*/

#ifndef __IFXMODIFIERCHAINSTATE_H__
#define __IFXMODIFIERCHAINSTATE_H__

#include "IFXArray.h"
#include "IFXMatrix4x4.h"

#include "IFXModifierDataPacketInternal.h"
#include "IFXDidRegistry.h"

class IFXModifierChain;
class IFXModifierChainInternal;
class IFXModifier;
class IFXModifierDataPacket;



#define IFXDIDDEPSEQGROWSIZE 2
struct sElementDependency
{
  sElementDependency() {};
  sElementDependency(U32 in_uEl, U32 in_uDA)
    : uEIndex(in_uEl), uDepAttr(in_uDA)
    { };
  BOOL operator == (const sElementDependency& in_rhs)
    { return in_rhs.uEIndex == uEIndex && in_rhs.uDepAttr == uDepAttr; };

  U32            uEIndex; // Element index
  U32            uDepAttr; // Attributes the Element is dependent on.

};

struct IFXIntraDependencies
{
  IFXIntraDependencies();
  ~IFXIntraDependencies();

  IFXRESULT AddDependentElement(U32 in_DepEl, U32 in_Attr);
  IFXRESULT CopyFrom(IFXIntraDependencies*);
  void CopyTo(IFXIntraDependencies*);

  U32             Size;
  U32             AllocatedSize;
  sElementDependency*  pDepElementsList;

};

//-----------------------------------------------------------------------------
// -- IFXModifierChainState
// -- Helper class to hold a modifier chain state.
//-----------------------------------------------------------------------------

class IFXModifierChainState
{
public:
  IFXModifierChainState();
  ~IFXModifierChainState();

  IFXRESULT Initialize( IFXModifierChainInternal* in_pChain,
              IFXModifierChainInternal* in_pPrevChain,
              IFXModifierDataPacketInternal *in_pOverrideDP,
              U32,
              IFXDidRegistry* pDidRegistry);

  U32 NumModifiers()
  {
    return m_NumModifiers - 1;
  };


  IFXRESULT SetModifier(U32, IFXModifier*, BOOL);
  IFXRESULT GetModifier(U32, IFXModifier**); // addrefs
  IFXRESULT GetModifierDataPacket(U32, IFXModifierDataPacket**); // addrefs

  // the following are non addreffing calls that do index adjustments.
  IFXModifier* GetModifierNR(U32 in_Idx)
  {
    return m_pDataPacketState[in_Idx].m_pModifier;
  };
  BOOL GetModifierEnabled(U32 in_Idx)
  {
    return m_pDataPacketState[in_Idx].m_Enabled;
  };
  IFXModifierDataPacketInternal* GetDataPacketNR(U32 in_Idx)
  {
    if(in_Idx == PROXY_DATAPACKET_INDEX)
      return m_pDataPacketState[0].m_pDataPacket;
    else
      return m_pDataPacketState[in_Idx].m_pDataPacket;
  };

  // the following are non addreffing that do NOT do index adjustments.
  IFXModifierDataPacketInternal* GetOutputDataPacketNR()
  {
    return m_pDataPacketState[m_NumModifiers-1].m_pDataPacket;
  };
  IFXModifierDataPacketInternal* GetBaseDataPacketNR()
  {
    return m_pBaseDataPacket!=NULL?m_pBaseDataPacket:m_pDataPacketState->m_pDataPacket;
  };

  IFXModifierDataPacketInternal* GetProxyDataPacketNR()
  {
    return m_pDataPacketState[0].m_pDataPacket;
  };

  // previous chain
  IFXModifierChainInternal* GetPrevChainNR()
  {
    return m_pPreviousModifierChain;
  };

  void AttachToPrevChain();
  void DetachFromPrevChain();

  IFXRESULT Build(BOOL in_bReqValidation);

  IFXRESULT Invalidate( U32 inDataElementIndex,
                                         U32 uInModifierIndex );

  IFXRESULT ProcessDependencies( U32 inDataElementIndex,
                                         U32 uInModifierIndex );

  IFXRESULT SetActive();
  IFXRESULT IFXAPI  NotifyActive();

  void SetNeedTime()
  {
    m_bNeedTime = FALSE;
  };
  BOOL GetNeedTime()
  {
    return m_bNeedTime;
  };

  IFXIntraDependencies* GetIntraDeps()
  {
    return m_pDepSeq;
  };


private:
  IFXRESULT BuildProxyDataPacket();
  IFXRESULT Destruct();


  IFXRESULT BuildModifierDataPacket(U32 in_ModIdx, BOOL in_bReqValidation);

  IFXRESULT AddAppendedChainInvSeq();


  // the following BMDP* functions are helpers for BuildModifierDataPacket
  IFXRESULT BMDPAddOutputs(U32 in_ModIdx,
                IFXDID** ppOutputs,
                U32 uOutputCount,
                U32* pOutputIndices);

  IFXRESULT BMDPPopulateDataElements(U32 in_ModIdx);

  IFXRESULT BMDPConfigureOutputs(U32 in_ModIdx,
                U32 uOutputCount,
                U32* pOutputIndices);


  IFXRESULT BMDPVerifyInputs(U32 ModIdx,
                IFXModifier* pMod,
                IFXDID** ppOutputs,
                U32 NumOutputs);

  /** Adds Outputs to a datapackets.
  */
  IFXRESULT BMDPConsumeElements(U32 in_ModIdx,
                U32 uOutputIdx,
                U32 in_UnChangedAttrs );

  IFXRESULT BMDPScheduleInvalidations( U32 in_ModIdx,
                U32 uOutputIdx,
                IFXDID* pOutputDid,
                IFXDID** ppInputs,
                U32 uInputCount);

  IFXRESULT BMDPSetOutputDeps(U32 in_ModIdx,
                U32 uOutputIdx,
                IFXDID* pOutputDid,
                IFXDID** ppOutputDependencies,
                U32 uOutputDependencyCount,
                U32* upOutputDependencyAttrs);

  IFXRESULT BMDPScheduleDefaultInvalidations(U32 in_ModIdx);

  // Did List Builders and utilities
  U32 GetDidIndex(const IFXDID&, U32);
  U32 AppendDid(const IFXDID&, U32);
  BOOL GrowDids(U32 in_Size);

  // DepElements
  IFXRESULT AddDependentElement(U32, U32);


public: // Public data for use by the trusted CIFXModifierChain


  U32 m_NumModifiers; // Number of stages in use -- always one greater then

  // the modifier data packet Information
  IFXDataPacketState* m_pDataPacketState;

  IFXModifierDataPacketInternal*     m_pBaseDataPacket;
  /**< this is the output DataPacket
  from the prepended chain.
    is only valid if there is a prepended chain, other wise the
    proxy data packet is used.
  */

  IFXArray<IFXMatrix4x4>* m_pTransform; // the default transform for the data packet.
private: // private Data

  BOOL m_bNeedTime; // Is this mod chain time dependent

  IFXModifierChainInternal* m_pModChain;
  IFXModifierChainInternal* m_pPreviousModifierChain; // the prepended Modifier Chain

  U32 m_NumDataElements; // the Number of DataElements of the Last DataPacket
  U32 m_NumAllocatedDataElements; // the Size fo the Did Array
  IFXDidEntry* m_pDids;   // The master Did List
  IFXIntraDependencies* m_pDepSeq; // the DepSequence For the Last modifier in the chain

  IFXDidRegistry* m_pDidRegistry;
  U32* m_pTime;

  /** the is a proxy data packet
  at the begining of the chain to handle input deps on prepended chains.
  and Intra Deps from previous Chains. This DataPacket Never actually
  holds any data it just holds the dependency links. since the chain indices
  are irrelevent to each other.
  */



};

#endif
