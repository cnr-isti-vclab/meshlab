
//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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

#ifndef __CIFXSubdivModifier_h__
#define __CIFXSubdivModifier_h__

#include "IFXSubdivModifier.h"
#include "CIFXModifier.h"
#include "IFXMeshGroup.h"
#include "IFXNeighborMesh.h"
#include "IFXCoreCIDs.h"

#define IFX_SDS_MAX_ALLOWED_SUBDIVISION_DEPTH 5

// Temporary struct to cache parameters that are set prior to
// initalization:
typedef struct
{
  bool    bActive;
  U32     uDepth;
  F32     fError;
  F32     fTension;
  BOOL    bAdaptive;

} IFXSubdivInitData;

// Forward declaration:
class IFXSubdivisionManager;
class IFXScreenSpaceMetric;


class CIFXSubdivModifier : private CIFXModifier,
                   virtual public  IFXSubdivModifier
{
            CIFXSubdivModifier();
  virtual  ~CIFXSubdivModifier();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXSubdivModifier_Factory(IFXREFIID iid, void** ppv);

public:
  // IFXUnknown
  U32 IFXAPI            AddRef ();
  U32 IFXAPI            Release ();
  IFXRESULT IFXAPI      QueryInterface (IFXREFIID riid, void **ppv);

  // IFXMarkerX
  void IFXAPI  GetEncoderX (IFXEncoderX*& rpEncoderX) ;


  // IFXModifier
  IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
                        U32&       rOutNumberOfOutputs,
              U32*&    rpOutOutputDepAttrs );

  IFXRESULT IFXAPI  GetDependencies (    IFXGUID*   pInOutputDID,
                                IFXGUID**& rppOutInputDependencies,
                                  U32&       rOutNumberInputDependencies,
                                  IFXGUID**& rppOutOutputDependencies,
                                U32&       rOutNumberOfOutputDependencies,
                  U32*&    rpOutOutputDepAttrs );

  IFXRESULT IFXAPI  GenerateOutput ( U32    inOutputDataElementIndex,
                            void*& rpOutData, BOOL& rNeedRelease );

  IFXRESULT IFXAPI  SetDataPacket ( IFXModifierDataPacket* pInInputDataPacket,
               IFXModifierDataPacket* pInDataPacket );

  IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage,
                    void*               pMessageContext );

  // IFXSubdivModifier
  virtual IFXRESULT IFXAPI  GetEnable   (BOOL* pbEnable);
  virtual IFXRESULT IFXAPI  SetEnable   (BOOL bEnable);
    virtual IFXRESULT IFXAPI  GetDepth    (U32* pDepth);
    virtual IFXRESULT IFXAPI  SetDepth    (U32 depth);
    virtual IFXRESULT IFXAPI  GetTension  (F32* pTension);
    virtual IFXRESULT IFXAPI  SetTension  (F32 tension);
  virtual IFXRESULT IFXAPI  GetError    (F32* pError);
    virtual IFXRESULT IFXAPI  SetError    (F32 error);
  virtual IFXRESULT IFXAPI  GetAdaptive (BOOL* pbAdaptive);
  virtual IFXRESULT IFXAPI  SetAdaptive (BOOL bAdaptive);

private:
  // IFXUnknown attributes...
  U32 m_uRefCount;

  // IFXModifier
  static const IFXDID* m_scpOutputDIDs[];
  static const IFXDID* m_scpInputDIDs[];
  U32 m_uMeshGroupDataElementIndex;
  U32 m_uNeighborMeshDataElementIndex;
  U32 m_uFrustumDataElementIndex;
  U32 m_uTransformDataElementIndex;

  // IFXSubdivModifier
  IFXSubdivisionManager*  m_pSubdivMgr;
  IFXScreenSpaceMetric* m_pScreenSpaceMetric;
  IFXSubdivInitData   m_InitData;
  BOOL          m_bEnabled;
  BOOL                    m_bFaceDataChanged;
  BOOL                    m_bOtherDataChanged;
  U32*                    m_puOtherChangeCounts;
  U32*                    m_puFaceChangeCounts;

  IFXRESULT InitializeSubdiv(IFXMeshGroup* pMeshGroup, IFXNeighborMesh* pNeighborMesh );
  IFXRESULT   DataChanged(IFXMeshGroup* pMeshGroup);
  U32         CalculateSafeDepth();
  void      ResetInitData();
};

#endif
