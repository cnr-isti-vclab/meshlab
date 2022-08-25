//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// IFXRender interface class
#ifndef IFX_RENDERCONTEXT_H
#define IFX_RENDERCONTEXT_H

#include "IFXMesh.h"
#include "IFXTextureObject.h"
#include "IFXRenderServices.h"

//==============================
// Forward Declarations
//==============================
class IFXTextureObject;
class IFXRenderWindow;

// {33F57EA5-8E6E-44f5-9B82-7B18AC9D1601}
IFXDEFINE_GUID(IID_IFXRenderContext,
			   0x33f57ea5, 0x8e6e, 0x44f5, 0x9b, 0x82, 0x7b, 0x18, 0xac, 0x9d, 0x16, 0x1);

//=====================================
// New error codes
//=====================================

#define IFXRESULT_COMPONENT_RENDER (0x0666 << 16)

#define IFX_W_NEED_TEXTURE_DATA MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_RENDER, 0x0001 )
#define IFX_E_NEED_RESET    MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_RENDER, 0x0002 )
#define IFX_E_INVALID_VIEWPORT  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_RENDER, 0x0003 )

//==============================
// Constant Definitions
//==============================
#include "IFXEnums.h"

//==============================
// Helper Classes
//==============================
#include "IFXRenderHelpers.h"
#include "IFXRenderCaps.h"
#include "IFXRenderBlend.h"
#include "IFXRenderClear.h"
#include "IFXRenderFog.h"
#include "IFXRenderLight.h"
#include "IFXRenderMaterial.h"
#include "IFXRenderStencil.h"
#include "IFXRenderTexUnit.h"
#include "IFXRenderView.h"


//=================================================
// IFXRenderContext - main render device interface
//=================================================
/**
*  An IFXRenderContext object controls all aspects of 3D rendering in a portion of
*  a window.  The rendering can be done using DirectX8, DirectX7, OpenGL, or
*  a software renderer.  See IFXRenderServices for more info regarding the ways to
*  specify which renderer to use.
*
*  The IFXRenderContext can handle rendering even when the rendering area spans multiple
*  monitors.  It will internally allocate multiple rendering systems depending on what
*  the lower-level rendering API is being used.
*
*  An IFXRenderContext can also handle rendering into an offscreen location of memory.
*  See IFXRenderContext::SetWindow() and IFXRenderWindow for more information.
*
*  If multiple processors are present, IFXRenderContext may use a secondary
*  thread of execution to perform the actual rendering.  This will not affect how
*  IFXRenderContext and associated IFXRender* classes are used, but it may cause
*  the thread that does use them to block on certain methods.
*/
class IFXRenderContext : virtual public IFXUnknown
{
public:
	//=========================
	// IFXRenderContext Methods
	//=========================

	/**
	*  Initializes (or reinitializes) the IFXRenderContext instance.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*
	*  @todo What other IFXRESULT codes does it return?
	*/
	virtual IFXRESULT IFXAPI Initialize()=0;

	/**
	*  Sets the renderer ID to be used for this context.  A @a uRenderId of 0
	*  tells the context to use the requested render ID that is set in the
	*  IFXRenderServices object.  Any other ID will override that setting
	*  and use a specific renderer.  If an invalid @a uRenderId is specified,
	*  one that doesn't correspond to any renderer registered with
	*  IFXRenderServices::RegisterRenderer(), then it will behave as if 0
	*  was specified.
	*
	*  @param  uRenderId Input U32 specifying the renderer id to be used
	*            for this render context.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI SetRenderer(U32 uRenderId)=0;

	/**
	*  Retrieves the renderer ID associated with this context.
	*
	*  @returns An U32 representing the renderer ID.
	*
	*  @retval 0 The actual renderer used is based on the requested renderer of
	*        the IFXRenderServices object.
	*
	*  @retval PositiveInteger The renderer ID that is being used by this context.
	*/
	virtual U32 IFXAPI GetRenderer()=0;

	/**
	*  Retrieves an IFXRenderCaps object that contains information about the
	*  capabilities of this render context.
	*
	*  @param  rCaps Output reference to an IFXRenderCaps object that will
	*          contain capability information about this render context.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_NOT_INITIALIZED This render context has not been initialized yet.
	*/
	virtual IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps)=0;

	/**
	*  Retrieves performance data accumulated since the last call to IFXRenderContext::Initialize()
	*  or IFXRenderContext::Swap().
	*
	*  @param  eData IFXenum input specifying what data is to be returned.
	*          Valid options are:
	*          - @b IFX_NUM_FACES
	*          - @b IFX_NUM_VERTICES
	*          - @b IFX_NUM_MESHES
	*          - @b IFX_NUM_RENDER_CALLS
	*
	*  @param  ruData  Output U32 reference that will receive the requested information.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_NOT_INITIALIZED This render context has not been initialized yet.
	*  @retval IFX_E_INVALID_RANGE Invalid option specified for @a eData.
	*/
	virtual IFXRESULT IFXAPI GetPerformanceData(IFXenum eData, U32& ruData)=0;

	/**
	*  Clears all performance data saved to 0.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI ClearPerformanceData()=0;

	/**
	*  Gets an interface pointer to the IFXRenderServices object that
	*  this object is associated with.
	*
	*  @param pServices  Pointer to an IFXRenderServices pointer object.  This will
	*            be written to by GetRenderServices().
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI GetRenderServices(IFXRenderServices*& pServices)=0;

	/**
	*  Gets the color format ordering (either BGRA or RGBA) that this context uses
	*  natively.
	*
	*  @return An IFXenum value.
	*
	*  @retval IFX_RGBA  The context uses RGBA color ordering.
	*  @retval IFX_BGRA  The context uses BGRA color ordering.
	*/
	virtual IFXenum   IFXAPI GetColorFormat()=0;

	/**
	*  Enables some state on the rendering device.
	*
	*  @param eParam IFXenum value that represents what state to enable.
	*          Possible values are:
	*          - @b IFX_LIGHTING: Enables per vertex lighting.
	*          - @b IFX_LIGHTn: Enables single HW light n (0-7).
	*          - @b IFX_TEXUNITn: Enable texturing on texture unit n (0-7).
	*          - @b IFX_FOG: Enable fog according to the set IFXRenderFog.
	*          - @b IFX_DEPTH_TEST: Enable per pixel testing against the Z buffer.
	*          - @b IFX_DEPTH_WRITE: Enable writing to the Z buffer.
	*          - @b IFX_FB_BLEND: Enable blending with the frame buffer.
	*          - @b IFX_FB_ALPHA_TEST: Enable alpha test during rasterization.
	*          - @b IFX_CULL: Enable culling of front, back, or both facing triangles.
	*          - @b IFX_STENCIL: Enable testing and writing to the stencil buffer.
	*          - @b IFX_DEPTH_OFFSET: Enable "polygon offset" or an equivalant.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*  @retval IFX_E_INVALID_RANGE The @a eParam value is invalid.
	*/
	virtual IFXRESULT IFXAPI Enable(IFXenum eParam)=0;

	/**
	*  Disables some state on the rendering device.
	*
	*  @param eParam IFXenum value that represents what state to disable.
	*  @see Enable() for possible values of eParam.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error
	*  @retval IFX_E_INVALID_RANGE The @a eParam value is invalid.
	*/
	virtual IFXRESULT IFXAPI Disable(IFXenum eParam)=0;

	/**
	*  Sets the current alpha blending parameters.
	*
	*  @param rBlend Reference to an IFXRenderBlend object that contains
	*          the set of alpha blending parameters.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetBlend(IFXRenderBlend& rBlend)=0;

	/**
	*  Sets the current fog parameters.
	*
	*  @param rFog Reference to an IFXRenderFog object that contains
	*        the set of fog parameters.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetFog(IFXRenderFog& rFog)=0;

	/**
	*  Sets the light parameters for the given light.
	*
	*  @param eLightId Input IFXenum specifying which light to apply these
	*          parameters to.  Valid values are:
	*          - @b IFX_LIGHT0
	*          - @b IFX_LIGHT1
	*          - @b IFX_LIGHT2
	*          - @b ...
	*          - @b IFX_LIGHT7
	*
	*  @param rLight Reference to an IFXRenderLight object that contains
	*          the set of light object parameters.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetLight(IFXenum eLightId, IFXRenderLight& rLight)=0;

	/**
	*  Gets the global ambient color.
	*
	*  @param  vColor  Output IFXVector4 representing the global ambient color.
	*
	*  @return An IFXRESULT code.
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI GetGlobalAmbient(IFXVector4& vColor)=0;

	/**
	*  Sets the global ambient color.  This color is multiplied by the material
	*  ambient color regardless of any specific lights enabled.  This only has
	*  an effect if IR_LIGHTING is enabled.
	*
	*  @param  vColor  Input IFXVector4 representing the global ambient color.
	*          The values are clamped to be within the 0.0 - 1.0 range.
	*
	*  @return An IFXRESULT code.
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI SetGlobalAmbient(const IFXVector4& vColor)=0;

	/**
	*  Sets the current material parameters.
	*
	*  @param rMat Reference to an IFXRenderMaterial object.  Contains
	*        input parameters like diffuse and specular.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetMaterial(IFXRenderMaterial& rMat)=0;


	/**
	*  Sets the current stencil parameters.
	*
	*  @param rStencil Reference to An IFXRenderStencil object.  This contains
	*          input parameters such as stencil comparator, reference value,
	*          and pass and fail operations.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error.
	*/
	virtual IFXRESULT IFXAPI SetStencil(IFXRenderStencil& rStencil)=0;

	/**
	*  Copies the texture data to underlying rendering system
	*  if needed.  It will be needed if the underlying rendering system
	*  does not have it in memory yet, or if the texture object data
	*  has changed since the last time that it was used in SetTexture()
	*  with this render context.
	*
	*  @param  rTexture  Input IFXTextureObject reference specifying the
	*            texture to send to the underlying rendering
	*            system.  The @a rTexture object contains all
	*            texture settings related to the texture.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI SetTexture(IFXTextureObject& rTexture)=0;

	/**
	*  Sets the virtual texture unit @a eTexUnit parameters to those
	*  specified by @a rTexUnit.
	*
	*  @param  eTexUnit  An IFXenum input value that specifies which
	*            texture unit the parameters should be applied to.
	*            Valid choices are:
	*            - @b IFX_TEXUNIT0
	*            - @b IFX_TEXUNIT1
	*            - @b IFX_TEXUNIT...
	*            - @b IFX_TEXUNIT7
	*
	*  @param rTexUnit Input reference to an IFXRenderTexUnit specifying the
	*          parameters for the texture unit.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetTextureUnit(IFXenum eTexUnit, IFXRenderTexUnit& rTexUnit)=0;

	/**
	*  Sets the constant color that can be referenced as texture unit inputs with IFX_CONSTANT.
	*
	*  @param  vColor  Input IFXVector4 specifying the constant color value.  Each color channel is
	*          clamped to the range [0.0 - 1.0].
	*
	*  @return An IFXRESULT error code
	*
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI SetConstantColor(const IFXVector4& vColor)=0;

	/**
	*  Sets the current view parameters.
	*
	*  @param  rView An input reference to an IFXRenderView object specifying
	*          the parameters for the view.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetView(IFXRenderView& rView)=0;

	/**
	*  Sets the current window parameters for the current device.
	*
	*  @param  rWindow An input reference to an IFXRenderWindow object specifying the
	*          parameters for the window.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetWindow(IFXRenderWindow& rWindow)=0;

	/**
	*  Gets the current window parameters for the current device.
	*
	*  @param  rWindow An input reference to an IFXRenderWindow object specifying the
	*          parameters for the window.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI GetWindow(IFXRenderWindow& rWindow)=0;

	/**
	*  Gets the number of devices that the given IFXRenderWindow will cover.  This
	*  and SetDevice() is required per frame for proper multiple monitor support.
	*
	*  @param  rWindow An input reference to an IFXRenderWindow object specifying the
	*          parameters for the window.
	*  @param  uNumDevices Output reference to a U32 that will be filled in
	*            with the number of devices that the given @a rWindow
	*            spans.
	*
	*  @return An IFXRESULT code.
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI GetNumDevices(IFXRenderWindow& rWindow, U32& uNumDevices)=0;

	/**
	*  Sets the current device number.  This number should be in the range of 0 -
	*  the value of (uNumDevices-1) returned from the most recent call to SetWindow().
	*
	*  @param  uDevice Input U32 specifying which device the following rendering
	*      calls should be used with.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error.
	*  @retval IFX_E_INVLAID_RANGE The value for @a uDevice was greater than
	*                or equal to the uNumDevices value that was
	*                returned from the most recent SetWindow()
	*                call.
	*/
	virtual IFXRESULT IFXAPI SetDevice(U32 uDevice)=0;

	/**
	*  Clears the contents of buffers owned by the render context and specified with @a
	*  rClear.
	*
	*  @param  rClear  Input reference to an IFXRenderClear object that specifies
	*          which buffers to clear and what values to clear them to.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI Clear(const IFXRenderClear& rClear)=0;

	/**
	*  Draws lines between vertices specified via IFXMesh objects contained
	*  in @a rMeshGroup.
	*
	*  @param  rMesh Input reference to an IFXMesh object
	*          that contains the lines to be drawn.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI DrawMeshLines(IFXMesh& rMesh)=0;


	/**
	*  Draws vertices specified via IFXMesh objects contained
	*  in @a rMeshGroup.
	*
	*  @param  rMesh Input reference to an IFXMesh object
	*          that contains the points to be drawn.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI DrawMeshPoints(IFXMesh& rMesh)=0;


	/**
	*
	*
	*  @param  rMesh Input reference to an IFXMesh object to be drawn.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh)=0;

	/**
	*  Deletes the texture specified with @a uTexId.
	*
	*  @param  uTexId  Input U32 specifying the texture id of the texture to be
	*          deleted.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI DeleteTexture(U32 uTexId)=0;

	/**
	*  Sets the model to world transform matrix.
	*
	*  @param  mView Input IFXMatrix4x4 reference that represents
	*          the transform from model space to world space.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetWorldMatrix(const IFXMatrix4x4& mView)=0;

	/**
	*  Sets the world to camera transform matrix.
	*
	*  @param  mView Input IFXMatrix4x4 reference that represents
	*          the transform from world space to camera space.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetViewMatrix(const IFXMatrix4x4& mView)=0;

	/**
	*  Gets the world to camera transform matrix.
	*
	*  @return The Current camera transform.
	*
	*/
	virtual const IFXMatrix4x4& IFXAPI GetViewMatrix()=0;

	/**
	*  Returns the last set IFXRenderView object.
	*
	*  @return The current render view object.
	*/
	virtual const IFXRenderView& IFXAPI GetView() const = 0;

	/**
	*  Sets the depth buffer comparison function.  The depth buffer comparison
	*  is done between the current value of the depth buffer and the depth
	*  value calculated for the pixel about to be drawn.
	*
	*  @param  eDepthMode  Input depth comparison function.  Possible
	*            values are:
	*            - @b IFX_NEVER:  The test never passes.
	*            - @b IFX_LESS:  Passes when incoming value is less than the buffer value.
	*            - @b IFX_GREATER: Passes when the incoming value is greater than the buffer value.
	*            - @b IFX_EQUAL:  Passes when the incoming value is equal to the buffer value.
	*            - @b IFX_NOT_EQUAL:  Passes when the incoming value is not equal to the buffer value.
	*            - @b IFX_LEQUAL:  Passes when the incoming value is less than or equal to the buffer value.
	*            - @b IFX_GEQUAL:  Passes when the incoming value is greater than or equal to the buffer value.
	*            - @b IFX_ALWAYS:  The test always passes.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified for @a eDepthMode is invalid.
	*/
	virtual IFXRESULT IFXAPI SetDepthMode(IFXenum eDepthMode)=0;

	/**
	*  Sets the cull mode.  Culling can be enabled and disabled using
	*  IFXRenderContext::Enable(IFX_CULL) and IFXRenderContext::Disable(IFX_CULL).
	*
	*  @param eCullMode  Input IFXenum specifying the culling mode desired.
	*            Valid culling modes are:
	*            - @b IFX_CULL_CW: Cull triangles that are clockwise in screen space.
	*            - @b IFX_CULL_CCW: Cull trianges that are counter-clockwise
	*              in screen space.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified for @a eCullMode is invalid.
	*/
	virtual IFXRESULT IFXAPI SetCullMode(IFXenum eCullMode)=0;

	/**
	*  Sets the line thickness of lines drawn with with DrawMeshLines().  This value
	*  represents the thickness of the line in screen space pixels.
	*
	*  @param  uOffset Input F32 specifying the line thickness
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI SetLineOffset(U32 uOffset)=0;

	/**
	*  Retrieves the plane normals of the top, bottom, right, and left
	*  frustum planes in camera space.  The planes that these normals represent all
	*  run through the point (0,0,0) in camera space if the projection mode is
	*  perspective.  If using an orthographic projection, the plane normals
	*  are understood to be orthogonal to the coordinate system axis.  What is
	*  provided in this case are points in camera space that lie on these planes.
	*
	*  @param  pvPlaneNormals  Output array of 4 IFXVector3 objects.  These 4
	*              vectors will receive the plane normals of the
	*              frustum planes in this order: left, top, right,
	*              bottom.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_POINTER The pointer specified for @a pvPlaneNormals
	*                  was not valid.
	*/
	virtual IFXRESULT IFXAPI GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals)=0;

	/**
	*  Copies the contents of the back buffer to the destination
	*  surface.  If the render context was set up to render direct to screen (see
	*  IFXRenderWindow), then the destination surface is the screen.  If not, then
	*  the destination surface is the memory buffer location specified with
	*  SetWindow().
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_NOT_INITIALIZED This has been called before SetWindow() and
	*                  SetView() have been called successfully.
	*/
	virtual IFXRESULT IFXAPI Swap()=0;

	/**
	*  Gets a complete local->screen matrix.
	*  @param  rOutMatrix  Output IFXMatrix4x4 reference.
	*
	*  @return An IFXRESULT code.
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI GetTransformMatrix(IFXMatrix4x4& rMatrix)=0;

	/**
	*  Calculates the number of rendering passes required for the current device
	*  to render an object with the current texture units enabled.  Since the
	*  IFXRenderContext interface supports 8 texture units, and most current
	*  hardware does not support 8 simultaneous textures, this is needed to determine
	*  how many rendering passes the hardware requires for all textures.
	*
	*  To actually configure the hardware to render a specific pass, use
	*  ConfigureRenderPass(i) with i < the value returned from this
	*  method.
	*
	*  Changing texture unit or texture data after calling CalculateNumPasses() or
	*  ConfigureRenderPass() will invalidate the data generated in these calls
	*  and result in an error when calling any of the Draw* methods.
	*
	*  @return A U32 value representing the number of rendering passes required
	*      to render an object with currently enabled and configured
	*      texture units and textures.
	*/
	virtual U32 IFXAPI CalculateNumPasses()=0;

	/**
	*  Configures the current hardware device to render one of
	*  (possibly) many passes needed to render an object depending on
	*  the number of textures being used.  This first pass is designated
	*  with a @a uRenderPass value of 0.
	*
	*  @param  uRenderPass Input U32 specifying which pass number to
	*      configure.  This value must be less than the number
	*      returned from CalculateNumPasses().
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified with @a uRenderPass
	*      was not a valid render pass number.
	*/
	virtual IFXRESULT IFXAPI ConfigureRenderPass(U32 uRenderPass)=0;

	virtual void IFXAPI Reset()=0;

protected:
	IFXRenderContext() {}
	virtual ~IFXRenderContext() {}
};

typedef IFXSmartPtr<IFXRenderContext> IFXRenderContextPtr;

#endif
