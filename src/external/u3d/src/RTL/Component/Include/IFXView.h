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
	@file	IFXView.h
		
		Defines IFXView interface
*/

#ifndef __IFXVIEW_H__
#define __IFXVIEW_H__

#include "IFXNode.h"
#include "IFXModel.h"
#include "IFXSpatialSetQuery.h"
#include "IFXViewResource.h"

class IFXViewLayer;

// {7A29861B-8B01-11d3-8467-00A0C939B104}
IFXDEFINE_GUID(IID_IFXView,
0x7a29861B, 0x8b01, 0x11d3, 0x84, 0x67, 0x0, 0xa0, 0xc9, 0x39, 0xb1, 0x4);

class IFXView : virtual public IFXNode
{
public:
	virtual IFXViewResource* IFXAPI GetViewResource( void )=0;
	virtual IFXRESULT IFXAPI GetViewResourceID(U32*)=0;
	virtual IFXRESULT IFXAPI SetViewResourceID(U32)=0;

  // Rendering
  /**
  *  This method renders all scene elements seen by the camera.
  *
  *  @param pRenderContext   The pointer to the render context, which
  *     handles all aspects of actually rendering the scene data.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
    *  @retval IFX_E_INVALID_POINTER  pRenderContext was NULL
  */
  virtual IFXRESULT IFXAPI RenderView(IFXRenderContext* pRenderContext, U32 ViewInstance = 0)=0;
  virtual IFXRESULT IFXAPI RenderViewLayer(U32 uLayer, IFXRenderContext& rRenderLayer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)=0;

  /**
  *  Gets the far clipping distance.
  */
  virtual F32     IFXAPI GetFarClip()=0;
  
  /**
  *  Sets the far clipping distance.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE  The far clipping distance was not
  *      greater than zero.
  */
  virtual IFXRESULT IFXAPI SetFarClip(F32 distance)=0;

  /**
  *  Gets the near clipping distance.
  */
  virtual F32     IFXAPI GetNearClip()=0;
  
  /**
  *  Sets the near clipping distance.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE  The near clipping distance was not
  *      greater than zero.
  */
  virtual IFXRESULT IFXAPI SetNearClip(F32 distance)=0;

  /**
  *  Gets the area of the window in which this view will render
  */
  virtual IFXRESULT IFXAPI GetViewport(IFXF32Rect& rcView)=0;
  
  /**
  *  Sets the area of the window in which this view will render
  */
  virtual IFXRESULT IFXAPI SetViewport(const IFXF32Rect& rcView)=0;

  /**
  *  Gets the height of the viewport in screen space
  */
  virtual IFXRESULT IFXAPI GetOrthoHeight(F32* pOrthoHeight)=0;
  
  /**
  *  Sets the height of the viewport in screen space.
  */
  virtual IFXRESULT IFXAPI SetOrthoHeight(F32 orthoHeight)=0;

  /**
  *  Gets the camera's field of view (in degrees). For 3-point perspective projection modes.
  */
  virtual IFXRESULT IFXAPI GetProjection(F32* pProjectionDegrees)=0;
  
  /**
  *  Sets the camera's field of view (in degrees). For 3-point perspective projection modes.
  */
  virtual IFXRESULT IFXAPI SetProjection(F32 degrees)=0;

  /**
  *  Gets the camera's field of view (in degrees). For 1- and 2-point perspective projection modes.
  */
  virtual IFXRESULT IFXAPI GetProjection(IFXVector3* pProjVector)=0;
  
  /**
  *  Sets the camera's field of view (in degrees). For 1- and 2-point perspective projection modes.
  */
  virtual IFXRESULT IFXAPI SetProjection(IFXVector3 projVector)=0;

  // Overlays/Backdrops
  /**
  *  Returns the the number of Layers of this type for this view.
  *
  *  @param uLayer  The type of layer to do the count for:
  *                 IFX_VIEW_OVERLAY or IFX_VIEW_BACKDROP
  *  @param uLayerCount  The number of backdrop or overlay layers
  *                 associated with this view.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   uLayer was not IFX_VIEW_OVERLAY or
  *                                IFX_VIEW_BACKDROP
  */
  virtual IFXRESULT IFXAPI GetLayerCount(IFXenum uLayer, U32& uLayerCount)=0;

  /**
  *  Inserts the Layer at the given position in the list.
  *
  *  @param uLayer  The type of layer to we are inserting:
  *                 IFX_VIEW_OVERLAY or IFX_VIEW_BACKDROP
  *  @param uIndex  The position in the layer list to insert this new layer.
  *  @param Layer   The layer we are inserting
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   uLayer was not IFX_VIEW_OVERLAY or
  *                                IFX_VIEW_BACKDROP  or uIndex is out
  *                                of range
  */
  virtual IFXRESULT IFXAPI InsertLayer(IFXenum uLayer, U32 uIndex,
                  const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)=0;
  /**
  *  Adds the Layer at the end of the specified layer list.
  *
  *  @param uLayer  The type of layer to we are adding:
  *                 IFX_VIEW_OVERLAY or IFX_VIEW_BACKDROP
  *  @param Layer   The layer we are adding
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   uLayer was not IFX_VIEW_OVERLAY or
  *                                IFX_VIEW_BACKDROP or uIndex is out
  *                                of range
  */
  virtual IFXRESULT IFXAPI AddLayer(IFXenum uLayer, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)=0;
  
  /**
  *  Removes the Layer given by index from the specified layer list
  *
  *  @param uLayer  The type of layer to we are removing:
  *                 IFX_VIEW_OVERLAY or IFX_VIEW_BACKDROP
  *  @param uIndex  The position in the layer list of the layer to remove
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   uLayer was not IFX_VIEW_OVERLAY or
  *                                IFX_VIEW_BACKDROP or uIndex is out
  *                                of range
  */
  virtual IFXRESULT IFXAPI RemoveLayer(IFXenum uLayer, U32 uIndex)=0;

  /**
  *  Returns a reference to the requested overlay/backdrop layer.
  *
  *  @param uLayer  The type of layer to we are requesting:
  *                 IFX_VIEW_OVERLAY or IFX_VIEW_BACKDROP
  *  @param uIndex  The position in the layer list of the layer we are
  *                 requesting
  *  @param Layer   The returned layer
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   uLayer was not IFX_VIEW_OVERLAY or
  *                                IFX_VIEW_BACKDROP  or uIndex is out
  *                                of range
  */
  virtual IFXRESULT IFXAPI GetLayer(IFXenum uLayer, U32 uIndex, IFXViewLayer& Layer)=0;
  virtual CIFXViewLayer* IFXAPI GetLayer(IFXenum uLayer)=0;
  
  /**
  *  Sets the requested overlay/backdrop layer to a new value.
  *
  *  @param uLayer  The type of layer to we are setting:
  *                 IFX_VIEW_OVERLAY or IFX_VIEW_BACKDROP
  *  @param uIndex  The position in the layer list of the layer we are
  *                 setting
  *  @param Layer   The new layer to place at this position in the list.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   uLayer was not IFX_VIEW_OVERLAY or
  *                                IFX_VIEW_BACKDROP  or uIndex is out
  *                                of range
  */
  virtual IFXRESULT IFXAPI SetLayer(IFXenum uLayer, U32 uIndex, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)=0;
  virtual IFXRESULT IFXAPI RecalcAllLayerMatrices(IFXRect *pViewport, F32 fScaleX, F32 fScaleY)=0;

  // Picking
	using IFXNode::Pick;
  /**
  *  Returns the first model hit by a ray emitted from the camera
  *  and passing through the specified screen coordinate.
  *
  *  @param X  The X-coordinate of the picking location (screen space).
  *  @param Y  The Y-coordinate of the picking location (screen space).
  *  @param ppOutPickedModel   A pointer to the picked model.
  *
  *  @note  Only nodes under the camera root node are picked by this
  *      method.
  */
  virtual IFXRESULT IFXAPI Pick(F32 x, F32 y, U32 ViewInstance, IFXModel** ppOutPickedModel, U32* pInstance)=0;
  
  /**
  *  Returns the list of models hit by a ray emitted from the camera
  *  and passing through the specified screen coordinate.
  *
  *  @param X  The X-coordinate of the picking location (screen space).
  *  @param Y  The Y-coordinate of the picking location (screen space).
  *  @param ppPickedObjectList   A list of all models hit by the ray
  *
  *  @note  Only nodes under the camera root node are picked by this
  *      method.
  */
  virtual IFXRESULT IFXAPI PickMultiple(F32 x, F32 y, U32 ViewInstance, IFXSimpleList** ppPickedObjectList)=0;

  /**
  *  Sets the screen-space scale
  *
  *  @param fScaleX  The scale in the X direction
  *  @param fScaleY  The scale in the Y direction
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE   One of the scale values was zero.
  */
  virtual IFXRESULT IFXAPI SetScale(F32 fScaleX, F32 fScaleY )=0;
  
  /**
  *  Gets the screen-space scale
  *
  *  @param fScaleX  A reference to the scale in the X direction
  *  @param fScaleY  A reference to the scale in the Y direction
  */
  virtual IFXRESULT IFXAPI GetScale(F32& fScaleX, F32& fScaleY ) const=0;

  /**
  *  Returns the pixel aspect ratio.
  */
  virtual F32       IFXAPI GetAspect()=0;

  /**
  *  Sets the pixel aspect ratio.
  */
  virtual IFXRESULT       IFXAPI SetAspect( F32 a )=0;

  // Culling
  /**
  *  Returns direction vector for the specified clipping plane.
  *
  *  @param  uInClipPlaneId  The clipping plane to return.  Can be
  *       one of the following values:  IFX_FRUSTUM_BOTTOM, IFX_FRUSTUM_TOP
  *       IFX_FRUSTUM_RIGHT, IFX_FRUSTUM_LEFT, IFX_FRUSTUM_FRONT, or
  *       IFX_FRUSTUM_BACK
  */
  virtual const IFXVector3& IFXAPI ClipPlane( U32 uInClipPlaneId ) const=0;
  
  /*
  *  Returns a pointer to the culling subsystem currently being used
  *  by the view.
  */
  virtual IFXRESULT IFXAPI GetCullingSubsystem( IFXSpatialSetQuery*& rpOutCuller )=0;
  
  /**
  *  Sets the culling subsystem to be used by the view.
  */
  virtual IFXRESULT IFXAPI SetCullingSubsystem( IFXSpatialSetQuery& rInCuller )=0;

  // Helpers
  /**
  *  Returns the camera's (view's) distance from the projection plane.
  */
  virtual F32 IFXAPI GetDistanceToProjectionPlane()=0;
  
  /**
  *  Gets a point on the film plane expressed in camera space.
  */
  virtual IFXRESULT IFXAPI WindowToFilm(F32 in_x, F32 in_y,
                        IFXVector3* out_pFilmPt)=0;
  /**
  *  Gets a point in camera space expressed in window coordinates.
  *
  *  @param in_pVector  The point to transform
  *  @param out_pX      The X-window coordinate of the point
  *  @param out_pY      The Y-window coordinate of the point
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_POINT  The point is beyond the clipping planes
  *      or not inside the window
  */
  virtual IFXRESULT IFXAPI FrustumToWindow(const IFXVector3* in_pVector,
    I32* const out_pX, I32* const out_pY) const=0;

  /**
	Attributes: 
	0x00000000: default values (3-point perspective projection and screen position units in screen pixels)
	0x00000001: screen position units: percentage of screen dimension
	0x00000002: projection mode: ortho
	0x00000004: projection mode: 2-point perspective
	0x00000008: projection mode: 1-point perspective
  */
  virtual void IFXAPI SetAttributes( U32 uAttrib ) = 0;
  virtual U32 IFXAPI GetAttributes( void ) = 0;
};

#endif
