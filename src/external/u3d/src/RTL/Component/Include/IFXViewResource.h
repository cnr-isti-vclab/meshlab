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
	@file  IFXViewResource.h                                                             
*/

#ifndef __IFXViewResource_H__
#define __IFXViewResource_H__

#include "IFXRenderPass.h"
#include "IFXModel.h"
#include "IFXSpatialSetQuery.h"
#include "IFXMarkerX.h"
#include "IFXRenderContext.h"

class IFXViewLayer;

// {3D6515EF-13E1-46fb-ADCA-53D5F18D64FF}
IFXDEFINE_GUID(IID_IFXViewResource,
0x3d6515ef, 0x13e1, 0x46fb, 0xad, 0xca, 0x53, 0xd5, 0xf1, 0x8d, 0x64, 0xff);

class CIFXViewLayer;

class IFXViewResource : virtual public IFXMarkerX
{
public:
  /**
  *  Returns a pointer to the camera root node.  The camera root node is not
  *  the parent of the camera in the scenegraph hierarchy, but the
  *  potion of the scenegraph hierarchy to be rendered by the camera.  The
  *  camera can "see" all nodes under the camera root node in the
  *  scenegraph hierarchy.
  *
  *  @param pNodeIndex  The returned index of the camera root node
  *  @param pNodeInstance  The returned instance of the camera root node
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_POINTER  ppNode was NULL
  */
  virtual IFXRESULT IFXAPI  GetRootNode(U32* pNodeIndex, U32* pNodeInstance)=0;

  /**
  *  Sets a pointer to the camera root node.  The camera root node is not
  *  the parent of the camera in the scenegraph hierarchy, but the
  *  potion of the scenegraph hierarchy to be rendered by the camera.  The
  *  camera can "see" all nodes under the camera root node in the
  *  scenegraph hierarchy.
  *
  *  @param pRootNode  The pointer to the new camera root node
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_POINTER  ppNode was NULL
  */
  virtual IFXRESULT IFXAPI  SetRootNode(U32 nodeIndex, U32 nodeInstance)=0;

  /**
  *  Clears the Root Node that was specified.
  *  Sets m_nodeSet to FALSE.
  */
  virtual void IFXAPI ClearRootNode() = 0;

  /**
  *  Returns a reference to the interface that encapsulates all necessary
  *  state settings relating to clearing the back/depth/stencil buffers
  *  associated with an IFXRender object, in this case the current render
  *  pass.
  */
  virtual IFXRenderClear& IFXAPI GetRenderClear()=0;

  // Fog
  /**
  *  Gets the current fog value.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_POINTER   pbEnable was NULL
  */
  virtual IFXRESULT IFXAPI  GetFogEnableValue( BOOL* pbEnable )=0;

  /**
  *  Sets the fog value.
  */
  virtual IFXRESULT IFXAPI  FogEnable( BOOL bEnable )=0;

  /**
  *  Returns a reference to the interface which encapsulates the various
  *  for parameters.
  */
  virtual IFXRenderFog& IFXAPI GetRenderFog()=0;

  // Color
  /**
  *  Gets the value of the color buffer flag for the current render pass.
  */
  virtual IFXRESULT IFXAPI  GetColorBufferEnabled( BOOL& bEnabled )=0;

  /**
  *  Sets the value of the color buffer flag for the current render pass.
  */
  virtual IFXRESULT IFXAPI  SetColorBufferEnabled( BOOL  bEnabled )=0;

  // Depth
  /**
  *  Gets the value of the depth testing flag for the current render pass.
  */
  virtual IFXRESULT IFXAPI  GetDepthTestEnabled( BOOL& bEnabled )=0;

  /**
  *  Sets the value of the depth testing flag for the current render pass.
  *  If TRUE, the Z value of the incoming pixel is compared using the
  *  depth function against the Z value already in the depth buffer.
  *  It replaces the pixel in the depth buffer if it passes the test.
  */
  virtual IFXRESULT IFXAPI  SetDepthTestEnabled( BOOL  bEnabled )=0;

  /**
  *  Returns the value of the depth writing flag for the current render
  *  pass.  If TRUE, the Z values from new pixels are written into the
  *  depth buffer.
  */
  virtual IFXRESULT IFXAPI  GetDepthWriteEnabled( BOOL& bEnabled )=0;

  /**
  *  Sets the value of the depth write flag for the current render pass.
  */
  virtual IFXRESULT IFXAPI  SetDepthWriteEnabled( BOOL  bEnabled )=0;

  /**
  *  Gets the function used to compare the incoming pixel's Z value against
  *  the Z-value already stored in the depth buffer for the current render
  *  pass.
  */
  virtual IFXRESULT IFXAPI  GetDepthFunc(IFXenum& eDepthFunc )=0;

  /**
  *  Sets the function used to compare the incoming pixel's Z value against
  *  the Z-value already stored in the depth buffer for the current render
  *  pass.  Legal values are IFX_ALWAYS, IFX_LESS, IFX_LEQUAL, IFX_GREATER,
  *  IFX_GEQUAL, IFX_EQUAL, IFX_NOT_EQUAL, and IFX_NEVER.
  */
  virtual IFXRESULT IFXAPI  SetDepthFunc(IFXenum  eDepthFunc )=0;

  // Stencil
  /**
  *  Returns a reference to the flag indicating whether or not stencil
  *  buffering is enabled for the current render pass.
  */
  virtual IFXRESULT IFXAPI  GetStencilEnabled( BOOL& bEnabled )=0;

  /**
  *  Enables or disabled stencil mode for the current render pass.
  */
  virtual IFXRESULT IFXAPI  SetStencilEnabled( BOOL bEnabled )=0;

  /**
  *  Returns a reference the IFXRenderStencil object used for the current
  *  render pass.  This contains input parameters such as stencil
  *  comparator, reference value, and pass and fail operations.
  */
  virtual IFXRenderStencil& IFXAPI GetRenderStencil()=0;

  // Multipass renderin support
  /**
  *  Sets the number of render passes to be done by this camera.
  *
  *  @param uNumPasses  The number of render passes to be made by
  *      the camera
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE  More than 31 render passes were requested
  */
  virtual IFXRESULT IFXAPI  SetNumRenderPasses(U32 uNumPasses)=0;

  /**
  *  Gets the number of render passes currently done by this camera.
  */
  virtual U32 IFXAPI  GetNumRenderPasses(void)=0;

  /**
  *  Forces the view to switch to the specified render pass.
  *
  *  @param uPassNum  The pass number to set the camera to.
  *
  *  @return An IFXRESULT value.
  *  @retval IFX_OK No error.
  *  @retval IFX_E_INVALID_RANGE  An attempt was made to set the render
  *      pass to a value beyond GetNumRenderPasses().
  */
  virtual IFXRESULT IFXAPI  SetCurrentRenderPass(U32 uPassNum)=0;

  /**
  *  Gets the camera's current render pass.
  */
  virtual U32       IFXAPI  GetCurrentRenderPass()=0;

  virtual IFXRenderPass* IFXAPI GetRenderPass(void)=0;
  virtual IFXRenderPass* IFXAPI GetRenderPass(U32 uIndex)=0;
};


/**
*
*  @note  The layer location is a a point, in pixels, measured from the
*         top left corner of the window's rect to the overlay's regPoint.
*  @note  The "regPoint" is the point at the center of the overlay/backdrop
*/
class IFXViewLayer
{
public:
  IFXViewLayer()
    {
      m_fRotation = 0;
      m_vScale.Set(1,1);
      m_iRegX = 0;
      m_iRegY = 0;
      m_iLocX = 0;
      m_iLocY = 0;
      m_fBlend = 1;
      m_uTextureId = 0;
    }

  virtual ~IFXViewLayer()
    {
    }

  IFXVector2  m_vScale; ///< The scale of the layer (default = 1)
  F32     m_iLocX;  ///< The X screen-space coordinate of the layer
  F32     m_iLocY;  ///< The Y screen-space coordinate of the layer
  I32     m_iRegX;  ///< The X coordinate of the regPoint
  I32     m_iRegY;  ///< The Y coordinate of the regPoint

  F32     m_fRotation;  ///< The layer's rotation value
  F32     m_fBlend;     ///< The layer's blend value

  U32     m_uTextureId;  ///< The texture palette ID of the texture used for this layer
};

#endif
