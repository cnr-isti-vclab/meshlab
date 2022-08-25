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
  @file CIFXShadingModifier.h

      This module defines the CIFXShadingModifier component.
*/


#ifndef CIFXSHADINGMODIFIER_H
#define CIFXSHADINGMODIFIER_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXShadingModifier.h"
#include "CIFXModifier.h"
#include "IFXCoreCIDs.h"
#include "IFXSceneGraph.h"
#include "IFXMaterialResource.h"

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//---------------------------------------------------------------------------
/**
  This is the implementation of a component that is used to use shading as
  modifier.

  It supports the following interfaces:  IFXUnknown, IFXShadingModifier,
  IFXMarker, IFXMarkerX, IFXModifier, IFXSubject, IFXMetadataX.

  @note This component's id is named {FEA5D3C8-4BF4-4735-AD2B-A94ECB5EA44A}.

  @note This component can be instantiated multiple times.
*/
class CIFXShadingModifier : private CIFXModifier,
                   virtual public   IFXShadingModifier
{
  // Member functions:  IFXUnknown.

  U32 IFXAPI      AddRef ();
  U32 IFXAPI      Release ();
  IFXRESULT IFXAPI  QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

  // Member functions:  IFXShadingModifier.

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

  // IFXShadingModifier

  virtual IFXRESULT IFXAPI  GetEnable     (BOOL* pbEnable);
  virtual IFXRESULT IFXAPI  SetEnable     (BOOL  bEnable);

  IFXRESULT  IFXAPI 	 GetElementShaderList( U32 uInElement, IFXShaderList** out_ppShaderList );
  IFXRESULT  IFXAPI 	 SetElementShaderList( U32 uInElement, IFXShaderList* in_pShaderList, BOOL isValid = TRUE );

  IFXRESULT  IFXAPI 	 GetShaderDataArray(IFXShaderList*** pShaderArray);
  IFXRESULT  IFXAPI 	 GetAttributes(U32* pShadingAttributes);
  IFXRESULT  IFXAPI 	 SetAttributes(U32 shadingAttributes);
  IFXRESULT  IFXAPI 	 GetSize(U32* size);

    // Factory function.

    friend IFXRESULT IFXAPI_CALLTYPE CIFXShadingModifier_Factory( IFXREFIID interfaceId, void** ppInterface );

private:

  // Member functions.

  CIFXShadingModifier();
  virtual ~CIFXShadingModifier();

  IFXRESULT RobustConstructor();

  IFXRESULT AllocateShaders(IFXShaderList** in_ppData = NULL, BOOL in_bCopy = TRUE, U32 numCopy = 0);
  void Deallocate(U32 num);

  // Member data.

  U32 m_refCount; ///< Number of outstanding references to the component.

  // IFXModifier
  static const IFXDID* m_scpOutputDIDs[];
  static const IFXDID* m_scpInputDIDs[];
  U32 m_uMeshGroupDataElementIndex;

  // IFXShadingModifier
  BOOL          m_bEnabled;
  IFXShaderList**    m_ppShaders;
  U32 m_uNumElements;
  U32 m_attributes;
};

#endif
