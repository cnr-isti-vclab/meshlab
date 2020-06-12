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
	@file	CIFXView.h

	@todo	Add support for different types of projections
			and screen position units to View node
*/

#ifndef __CIFXVIEW_H__
#define __CIFXVIEW_H__

#include "CIFXNode.h"
#include "IFXView.h"
#include "IFXFrustum.h"
#include "IFXShader.h"
#include "InsertionSort.h"
#include "IFXCoreCIDs.h"
#include "IFXAutoRelease.h"

class IFXMaterialResource;
class IFXSpatialSetQuery;

// The Fog density calculations use a saturation value of 1/100 (0.01).
// constant = ln(1/saturation value)
#define EXPONENTIAL_FOG_CONSTANT \
                            ((F32)(4.6051701859880913680359829093687))
// constant = sqrt(ln(1/saturation value))
#define EXPONENTIAL_SQUARED_FOG_CONSTANT \
                            ((F32)(2.14596602628934723963618357029))

// Layer Mirror is used to store extra data for the layer that can't be
// included in the model
class CIFXViewLayer : public IFXViewLayer
{
public:
  CIFXViewLayer();
  virtual ~CIFXViewLayer();

  const CIFXViewLayer& operator=(const IFXViewLayer& Layer);

  F32            m_fTexSizeScaleX;
  F32            m_fTexSizeScaleY;

  IFXMatrix4x4   m_Transform;

  BOOL           m_bTextureDirty;

  CIFXViewLayer* m_pNext;
  CIFXViewLayer* m_pPrev;
};

class CIFXView : private CIFXNode,
         virtual public   IFXView
{
            CIFXView();
  virtual  ~CIFXView();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXView_Factory(   IFXREFIID           riid,
                                void**              ppv );
public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface (     IFXREFIID           riid,
                                void**              ppv );

  // IFXMarker
  IFXRESULT IFXAPI SetSceneGraph( IFXSceneGraph* pInSceneGraph );

  // IFXMarkerX
  void IFXAPI       GetEncoderX (IFXEncoderX*& rpEncoderX);

  // Node
  void    IFXAPI   Counter(EIFXNodeCounterType type, U32* puOutCount);

  // IFXObserver
  IFXRESULT IFXAPI Update( IFXSubject* pInSubject, U32 uInChangeBits,IFXREFIID rIType );

  // IFXModifier
  IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
                        U32&       rOutNumberOfOutputs,
                        U32*&      rpOutOutputDepAttrs );
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

  // IFXView
  IFXViewResource* IFXAPI GetViewResource( void );
  IFXRESULT  IFXAPI 	 GetViewResourceID(U32*);
  IFXRESULT  IFXAPI 	 SetViewResourceID(U32);

  const IFXVector3& IFXAPI ClipPlane( U32 uInClipPlaneId ) const;
  IFXINLINE F32 IFXAPI GetAspect() { return m_View.GetPixelAspect(); }
  IFXINLINE IFXRESULT  IFXAPI 	 SetAspect( F32 a ) { return m_View.SetPixelAspect( a ); }

  F32		 IFXAPI    GetDistanceToProjectionPlane();
  IFXINLINE   F32 IFXAPI		GetFarClip()        { return m_farClip; }
  IFXRESULT  IFXAPI 	 SetFarClip(         F32                 fInDistance );
  IFXINLINE  F32 IFXAPI  GetNearClip()       { return m_nearClip; }
  IFXRESULT  IFXAPI 	 SetNearClip(        F32                 fInDistance );
  IFXRESULT  IFXAPI 	 GetViewport(IFXF32Rect& rcView);
  IFXRESULT  IFXAPI 	 SetViewport(const IFXF32Rect& rcView);
  IFXRESULT  IFXAPI 	 GetOrthoHeight(     F32*                pfOutOrthoHeight );
  IFXRESULT  IFXAPI 	 SetOrthoHeight(     F32                 fInOrthoHeight );
  IFXRESULT  IFXAPI 	 GetProjection(      F32*                pfOutProjectionDegrees );
  IFXRESULT  IFXAPI 	 SetProjection(      F32                 fInDegrees );
  IFXRESULT  IFXAPI 	 GetProjection(		IFXVector3*			pProjVector );
  IFXRESULT  IFXAPI 	 SetProjection(		IFXVector3			projVector );

	using CIFXNode::Pick;
  IFXRESULT  IFXAPI 	 Pick(               F32                 x,
                                F32                 y,
								U32 ViewInstance,
                                IFXModel**          ppOutPickedModel,
								U32*				pInstance);
  IFXRESULT  IFXAPI 	 PickMultiple(       F32                 x,
                                F32                 y,
								U32 ViewInstance,
                                IFXSimpleList**     ppOutPickedObjectList );
  IFXRESULT  IFXAPI 	 WindowToFilm(       F32                 x,
                                F32                 y,
                                IFXVector3*         pvOutFilmPt );
  IFXRESULT  IFXAPI 	 FrustumToWindow(    const IFXVector3*  pvcVector,
                                I32* const          picX,
                                I32* const          picY ) const;
  IFXRESULT  IFXAPI 	 GetScale( F32& fScaleX, F32& fScaleY ) const;
  IFXRESULT  IFXAPI 	 SetScale( F32 fScaleX, F32 fScaleY );

  IFXRESULT  IFXAPI 	 GetCullingSubsystem( IFXSpatialSetQuery*& rpOutCuller );
  IFXRESULT  IFXAPI 	 SetCullingSubsystem( IFXSpatialSetQuery& rInCuller );

  IFXRESULT  IFXAPI 	 ServiceWeakRefDetach( IFXNode *pNode );

  // Overlays & Backdrops
  IFXRESULT  IFXAPI 	 GetLayerCount(IFXenum uLayer, U32& uLayerCount);

  IFXRESULT  IFXAPI 	 InsertLayer(IFXenum uLayer, U32 uIndex, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY);
  IFXRESULT  IFXAPI 	 AddLayer(IFXenum uLayer, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY);
  IFXRESULT  IFXAPI 	 RemoveLayer(IFXenum uLayer, U32 uIndex);

  IFXRESULT  IFXAPI 	 GetLayer(IFXenum uLayer, U32 uIndex, IFXViewLayer& Layer);
  CIFXViewLayer* IFXAPI GetLayer(IFXenum uLayer) { return m_pLayer[uLayer]; };
  IFXRESULT  IFXAPI 	 SetLayer(IFXenum uLayer, U32 uIndex, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY);

  // Layer helper functions (not available through the view interface)
  IFXRESULT  IFXAPI 	 FindLayerByIndex(U32 uLayer, U32 uIndex, CIFXViewLayer*& pLayer);
  IFXRESULT  IFXAPI 	 UpdateLayerMesh(CIFXViewLayer* pLayer);
  IFXRESULT  IFXAPI 	 RecalcAllLayerMatrices(IFXRect *pViewport, F32 fScaleX, F32 fScaleY);

  IFXRESULT  IFXAPI 	 RenderViewLayer(U32 uLayer, IFXRenderContext& rRenderLayer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY);

  void IFXAPI SetAttributes( U32 uAttrib );
  U32 IFXAPI GetAttributes( void ) { return m_attributes; };

protected:

	// IFXModifier
	static const IFXGUID* m_scpOutputDIDs[];

	// IFXView
	IFXRESULT UpdateViewport();
	IFXRESULT GenerateRay( const F32   x, const F32   y, const U32 ViewInstance, IFXVector3& pos, IFXVector3& dir );
	IFXRESULT IFXAPI RenderView(IFXRenderContext* pRenderLayer, U32 ViewInstance = 0);
	IFXRESULT ComputeFrustumPlanes(IFXMatrix4x4& mUnscaledWorld);
	IFXRESULT RecalcLayerMatrix(U32 uLayer, U32 uIndex, IFXRect *pViewport, F32 fScaleX, F32 fScaleY);

	// must be handled thru attributes
	IFXRESULT GetProjectionMode(IFXenum* pProjectionMode);
	IFXRESULT SetProjectionMode(IFXenum projectionMode);

	U32					m_uQualityFactor;

	IFXRenderContext*	pCurrentRenderContext;

	IFXSpatialSetQuery	*m_pCullingSubsystem;

	IFXFrustum          m_frustum;

	U32					m_layer;
	IFXRenderView		m_View;
	IFXRenderView		m_LayerView;
	F32					m_distanceToProjectionPlane;
	F32					m_firstDistanceToProjectionPlane;

	IFXRect				m_rcScreenSize;
	IFXF32Rect			m_rcViewport;
	IFXRect				m_rcRenderViewport;
	F32					m_fScaleX;
	F32					m_fScaleY;
	BOOL				m_bViewOrScaleChanged;

	F32					m_nearClip;
	F32					m_farClip;

	U32					m_viewResourceID;

	U32					m_attributes;

	CIFXViewLayer*           m_pLayer[IFX_VIEW_NUM_LAYERS];
	static IFXMeshPtr        ms_spLayerMesh;
	static IFXRenderTexUnit  ms_LayerTexUnit;
	static IFXRenderBlend    ms_LayerBlend;
	static IFXRenderMaterial ms_LayerMaterial;

	struct IFXTranslucentElement
	{
		IFXShader::IFXShadedElement se;
		IFXShader*                  pShader;
		F32                         fDepth;
	};
	class CSpatialDistance
	{
	public:
		BOOL operator() (IFXTranslucentElement* a, IFXTranslucentElement* b)
		{
			return a->fDepth > b->fDepth;
		}
	} compFunc;

	IFXList<IFXTranslucentElement*> m_translucents;
};


#endif
