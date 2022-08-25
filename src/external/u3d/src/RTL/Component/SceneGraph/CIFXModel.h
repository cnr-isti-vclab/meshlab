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
@file  CIFXModel.h                               
*/

#ifndef __CIFXMODEL_H__
#define __CIFXMODEL_H__

#include "CIFXNode.h"
#include "IFXModel.h"
#include "CIFXResourceClient.h"
#include "IFXFrustum.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXRenderable.h"
#include "IFXLightSet.h"
#include "IFXShaderList.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXAutoRelease.h"

class CIFXModel : private CIFXNode,
				  private CIFXResourceClient,
		  virtual public  IFXModel
{
public:
  // IFXUnknown
  U32 IFXAPI      AddRef ();
  U32 IFXAPI      Release ();
  IFXRESULT IFXAPI  QueryInterface (   IFXREFIID       riid,
                  void**        ppv );

  // IFXResourceClient
  U32   IFXAPI     GetResourcePalette() { return (U32)IFXSceneGraph::GENERATOR; }


  // IFXMarker
  IFXRESULT		IFXAPI	SetSceneGraph( IFXSceneGraph* pInSceneGraph );
  IFXRESULT		IFXAPI   Mark();
  void			IFXAPI   SetPriority(   U32         uInPriority,
									BOOL        bRecursive,
									BOOL        bPromotionOnly );

  // IFXMarkerX
  void IFXAPI     GetEncoderX (IFXEncoderX*& rpEncoderX);

  // IFXNode
  void  IFXAPI     Counter(EIFXNodeCounterType type, U32* puOutCount);

  // IFXSpatial
  IFXRESULT IFXAPI  GetSpatialBound(    IFXVector4&    rOutSphere, U32 WorldInstance );
  IFXSpatial::eType IFXAPI GetSpatialType();

  // IFXModifier
  IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
              U32&     rOutNumberOfOutputs,
              U32*&    rpOutOutputDepAttrs );
  IFXRESULT IFXAPI  GetDependencies (    IFXGUID*   pInOutputDID,
                  IFXGUID**& rppOutInputDependencies,
                  U32&     rOutNumberInputDependencies,
                  IFXGUID**& rppOutOutputDependencies,
                  U32&     rOutNumberOfOutputDependencies,
                  U32*&    rpOutOutputDepAttrs );
  IFXRESULT IFXAPI  GenerateOutput ( U32  inOutputDataElementIndex,
                void*& rpOutData, BOOL& rNeedRelease );
  IFXRESULT IFXAPI  SetDataPacket ( IFXModifierDataPacket* pInInputDataPacket,
              IFXModifierDataPacket* pInDataPacket );
  IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage,
            void*         pMessageContext );

  // IFXModel
  BOOL IFXAPI   GetDebug() { return m_bDebug; }
  void IFXAPI   SetDebug( BOOL bDebug );
  U32  IFXAPI   GetVisibility() { return m_uVisible; }
  void IFXAPI   SetVisibility( U32 uInVisible ) { m_uVisible = uInVisible; }
  const IFXLightSet& IFXAPI GetLightSet();
  const IFXFrustum&	 IFXAPI GetFrustum() const { return *m_pFrustum; }
  IFXRESULT IFXAPI   SetFrustum( const IFXFrustum& rInFrustum );
  U32     IFXAPI     GetViewSize( void ) { return m_uViewSize; }
  IFXRESULT IFXAPI   SetViewSize( U32 uInViewSize );
  U32     IFXAPI     GetPickable() { return m_uPickable; }
  void    IFXAPI     SetPickable( U32 uPickable ) { m_uPickable = uPickable; }

  // IFXSpatialAssociation
  U32 IFXAPI   GetAssociatedSpatialCount( IFXSpatialAssociation::Type eInAssociationType );
  IFXRESULT IFXAPI   GetAssociatedSpatial( U32      uInSpatialIndex,
											IFXSpatial** ppOutSpatial,
											IFXSpatialAssociation::Type eInAssociationType );
  IFXRESULT IFXAPI   SetAssociatedSpatials( SPATIALINSTANCE_LIST* pInSpatials,
											U32      uInSpatialCount,
											IFXSpatialAssociation::Type eInAssociationType );
  IFXRESULT IFXAPI   AppendAssociatedSpatials( SPATIALINSTANCE_LIST* pInSpatials,
												U32      uInSpatialCount,
												IFXSpatialAssociation::Type eInAssociationType );
  // IFXObserver
  virtual IFXRESULT IFXAPI   Update( IFXSubject* pInSubject, U32 uInChangeBits,IFXREFIID rIType );

  // For Mapping Style Names
  IFXRESULT IFXAPI   SetResourceIndex( U32 uResourceIndex );

  static IFXRESULT IFXAPI Shutdown();

private:
			CIFXModel();
  virtual  ~CIFXModel();
  friend IFXRESULT IFXAPI_CALLTYPE CIFXModel_Factory(  
										IFXREFIID riid,
										void**    ppv );

  // IFXModifier
  U32 m_uLightSetDataElementIndex;
  U32 m_uFrustumDataElementIndex;
  U32 m_uViewSizeDataElementIndex;
  U32 m_uBoundFrameDataElementIndex;
  U32 m_uBoundFrameBoundsDataElementIndex;

  // IFXModel
  BOOL        m_bDebug;
  U32         m_uPickable;  // 0:none, 1:front, 2:back, 3:both
  U32         m_uVisible;   // 0:none, 1:front, 2:back, 3:both
  IFXDECLAREMEMBER(IFXLightSet,m_pLightSet);
  static IFXFrustum frustum;
  IFXFrustum* m_pFrustum;
  IFXSubject* m_pResourceSubjectNR;
  U32         m_uViewSize;

  // BoundFrame
  IFXDECLAREMEMBER(IFXBoundSphereDataElement,m_pBoundFrameBounds);
  BOOL        m_bProcessingBoundFrameBounds;
  IFXMeshGroupPtr   m_BoundFrameMeshGroup;
  IFXDECLAREMEMBER(IFXRenderable,m_BoundFrameMeshGroupRndr);
  IFXMeshPtr  m_AxisMesh;
  IFXInterleavedDataPtr m_AxisLineSet;
  IFXRESULT   _AllocateBoundFrame();
  U32         m_uBoundFrameShader;
  U32         m_uBoundFrameMaterial;
  IFXMeshPtr  m_RenderedSphereMesh;
  BOOL        m_bBoundFrameAllocated;
  static IFXMeshPtr m_sUnitSphereMesh;
  static U32  m_suBoundFaceCount;

  // world Sphere cache via mod chain observation
  BOOL        m_bAttachedToModChain;
  BOOL        m_bWorldSphereDirty;

  IFXRESULT AttachToModChainForWorldPositionChange();
};


#endif
