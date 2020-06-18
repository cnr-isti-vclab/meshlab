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
// CIFXRenderContext.h
#ifndef CIFX_RENDER_CONTEXT_H
#define CIFX_RENDER_CONTEXT_H

#include "CIFXRenderServices.h"
#include "IFXRenderWindow.h"

class CIFXRenderContext : virtual public IFXRenderContext, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXRenderContextFactory(IFXREFIID intId, void** ppUnk);

	//=========================
	// IFXRenderContext Methods
	//=========================
	/**
	*  Initializes (or reinitializes) the IFXRenderContext instance.
	*
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI Initialize();

	/**
	*  Sets the renderer id to be used for this context.  A @a uRenderId of 0
	*  tells the context to use the requested render id that is set in the
	*  IFXRenderServices object.  Any other id will override that setting
	*  and use a specific renderer.  If an invalid @a uRenderId is specified,
	*  (i.e. it doesn't correspond to any renderer registered with
	*  IFXRenderServices::RegisterRenderer()), then it will behave as if 0
	*  was specified.
	*
	*  @param  uRenderId Input U32 specifying the renderer id to be used
	*            for this render context.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	IFXRESULT IFXAPI SetRenderer(U32 uRenderId);

	/**
	*  Retreives the renderer id associated with this context.
	*
	*  @returns An U32 representing the renderer id.
	*
	*  @retval 0 The actual renderer used is based on the requested renderer of
	*        the IFXRenderServices object.
	*
	*  @retval PositiveInteger The renderer id that is being used by this context.
	*/
	U32 IFXAPI GetRenderer() { return m_uRenderId; }

	/**
	*  Retreive an IFXRenderCaps object that contains information about the
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
	IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps);

	/**
	*  Retreive performance data accumulated since the last call to IFXRenderContext::Initialize()
	*  or IFXRenderContext::ClearPerformanceData().
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
	IFXRESULT IFXAPI GetPerformanceData(IFXenum eData, U32& ruData);

	/**
	*  Clears all performance data saved to 0.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	IFXRESULT IFXAPI ClearPerformanceData();

	/**
	*  Gets an interface pointer to the IFXRenderServices object that
	*  this object is associated with.
	*
	*  @param pServices  Pointer to an IFXRenderServices pointer object.  This will
	*            be written to by GetRenderServices().
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI GetRenderServices(IFXRenderServices*& pServices);

	/**
	*  Gets the color format ordering (either BGRA or RGBA) that this context uses
	*  natively.
	*
	*  @return An IFXenum value.
	*
	*  @retval IFX_RGBA  The context uses RGBA color ordering.
	*  @retval IFX_BGRA  The context uses BGRA color ordering.
	*/
	IFXenum   IFXAPI GetColorFormat();

	/**
	*  Enable some state on the rendering device.
	*
	*  @param eParam IFXenum value that represents what state to enable.
	*          Possible values are:
	*          - @b IFX_LIGHTING: Enables per vertex lighting.
	*          - @b IFX_LIGHTn: Enables single HW light n (0-7).
	*          - @b IFX_TEXn: Enable textureing on texture unit n (0-7).
	*          - @b IFX_FOG: Enable fog according to the set IFXRenderFog.
	*          - @b IFX_DEPTH_TEST: Enable per pixel testing against the Z buffer.
	*          - @b IFX_DEPTH_WRITE: Enable writing to the Z buffer.
	*          - @b IFX_FB_BLEND: Enable blending with the frame buffer.
	*          - @b IFX_FB_ALPHA_TEST: Enable alpha test during rasterazation.
	*          - @b IFX_CULL: Enable culling of front, back, or both facing trianges.
	*          - @b IFX_STENCIL: Enable testing and writing to the stencil buffer.
	*          - @b IFX_DEPTH_OFFSET: Enable "polygon offset" or an equivilant.
	*
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI Enable(IFXenum eParam);

	/**
	*  Disable some state on the rendering device.
	*
	*  @param eParam IFXenum value that represents what state to disable.
	*  @see Enable() for possible values for eParam.
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI Disable(IFXenum eParam);

	/**
	*  Sets the current alpha blending parameters.
	*
	*  @param r  Reference to an IFXRenderBlend object that contains
	*          the set of alpha blending parameters.
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI SetBlend(IFXRenderBlend& rBlend);

	/**
	*  Sets the current fog parameters.
	*
	*  @param r  Reference to an IFXRenderFog object that contains
	*          the set of fog parameters.
	*
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI SetFog(IFXRenderFog& rFog);

	/**
	*  Sets the light parameters for the given light.
	*
	*  @param uLightId Input U32 id specifying which light to apply these parameters to.
	*  @param r  Reference to an IFXRenderLight object that contains
	*          the set of light object parameters.
	*
	*  @todo Document the ID thing a little more.
	*
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI SetLight(U32 uLightId, IFXRenderLight& rLight);

	/// @todo Implement these!
	IFXRESULT IFXAPI GetGlobalAmbient(IFXVector4& vColor);

	IFXRESULT IFXAPI SetGlobalAmbient(const IFXVector4& vColor);

	/**
	*  Sets the current material parameters.
	*
	*  @param rMat Reference to an IFXRenderMaterial object.  Contains
	*        input parameters such as diffuse, specular, etc.
	*
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI SetMaterial(IFXRenderMaterial& rMat);


	/**
	*  Sets the current stencil parameters.
	*
	*  @param r  Reference to An IFXRenderStencil object.  This contains
	*          input parameters such as stencil comparator, reference value,
	*          and pass and fail operations.
	*
	*  @return An IFXRESULT error code.
	*/
	IFXRESULT IFXAPI SetStencil(IFXRenderStencil& rStencil);

	/**
	*
	*/
	IFXRESULT IFXAPI SetTexture(IFXTextureObject& rTexture);

	/**
	*
	*/
	IFXRESULT IFXAPI SetTextureUnit(IFXenum eTexUnit, IFXRenderTexUnit& rTexUnit);

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
	virtual IFXRESULT IFXAPI SetConstantColor(const IFXVector4& vColor);

	/**
	*
	*/
	IFXRESULT IFXAPI SetView(IFXRenderView& rView);

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
	IFXRESULT IFXAPI SetWindow(IFXRenderWindow& rWindow);
	IFXRESULT IFXAPI GetWindow(IFXRenderWindow& rWindow);

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
	IFXRESULT IFXAPI GetNumDevices(IFXRenderWindow& rWindow, U32& uNumDevices);

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
	virtual IFXRESULT IFXAPI SetDevice(U32 uDevice);

	/**
	*
	*/
	IFXRESULT IFXAPI Clear(const IFXRenderClear& rClear);

	/**
	*  Draws lines between vertices specified via IFXMesh objects contained
	*  in @a rMeshGroup.
	*
	*  @todo Fix description and parameter info
	*
	*  @param  rMeshGroup  Input reference to an IFXRenderMeshGroup object
	*            that contains IFXMesh objects which contain
	*            the lines to be drawn.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	IFXRESULT IFXAPI DrawMeshLines(IFXMesh& rMesh);

	/**
	*  Draws vertices specified via IFXMesh objects contained
	*  in @a rMeshGroup.
	*
	*  @todo Fix description and parameter info
	*
	*  @param  rMeshGroup  Input reference to an IFXRenderMeshGroup object
	*            that contains IFXMesh objects which contain
	*            the vesrtices to be drawn.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	IFXRESULT IFXAPI DrawMeshPoints(IFXMesh& rMesh);

	/**
	*  Draws meshes specified with @a rMeshGroup.
	*
	*  @todo Fix description and parameter info
	*
	*  @param  rMeshGroup  Input reference to an IFXRenderMeshGroup object
	*            that contains the IFXMesh objects to be drawn
	*            along with local->world transforms and lights to
	*            be used when drawing each IFXMesh.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	IFXRESULT IFXAPI DrawMesh(IFXMesh& rMesh);

	/**
	*
	*/
	IFXRESULT IFXAPI DeleteTexture(U32 uTexId);

	/**
	*
	*/
	IFXRESULT IFXAPI SetViewMatrix(const IFXMatrix4x4& mView);

	/**
	*
	*/
	const IFXMatrix4x4& IFXAPI GetViewMatrix();


	/**
	*  This will return the last set IFXRenderView object
	*
	*  @return The current render view object.
	*/
	const IFXRenderView& IFXAPI GetView() const { return m_View; }

	/**
	*
	*/
	IFXRESULT IFXAPI SetWorldMatrix(const IFXMatrix4x4& mView);

	/**
	*
	*/
	IFXRESULT IFXAPI SetDepthMode(IFXenum eDepthMode);

	/**
	*
	*/
	IFXRESULT IFXAPI SetCullMode(IFXenum eCullMode);

	/**
	*  Sets the amount of line offset in minimal Z buffer units.  This is only used
	*  during the method DrawMeshLines().
	*
	*  @param  uOffset Input U32 specifying the line offset to be used during
	*          DrawMeshLines.  This value represents the number of
	*          minimal Z buffer units to add to the computed Z value
	*          used for hidden surface removal.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	IFXRESULT IFXAPI SetLineOffset(U32 uOffset);

	/**
	*
	*/
	//  IFXRESULT IFXAPI SetDepthOffset(F32 fFactor, F32 fUnits);

	/**
	*
	*/
	IFXRESULT IFXAPI GetFrustumPlaneNormals(IFXVector3* pvPlaneNormals);

	/**
	*  Get a complete local->screen matrix
	*  @param  rOutMatrix  Output IFXMatrix4x4 reference.
	*
	*  @return An IFXRESULT code.
	*  @retval IFX_OK  No error.
	*/
	IFXRESULT IFXAPI GetTransformMatrix(IFXMatrix4x4& rMatrix);

	/**
	*  Copy the contents of the backbuffer to the screen.
	*/
	IFXRESULT IFXAPI Swap();

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
	virtual U32 IFXAPI CalculateNumPasses();

	/**
	*  This will configure the current hardware device to render one of
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
	virtual IFXRESULT IFXAPI ConfigureRenderPass(U32 uRenderPass);

	void IFXAPI Reset();

protected:
	CIFXRenderContext();
	virtual ~CIFXRenderContext();

	virtual IFXRESULT IFXAPI Destroy();

	void IFXAPI InitData();

	IFXRESULT IFXAPI Construct();

	virtual IFXRESULT IFXAPI CreateRender(U32 uMonitorNum, IFXRenderWindow& rWindow);

	IFXRESULT IFXAPI ConfigureTexturePass(U32 uRenderPass);
	IFXRESULT IFXAPI ConfigureSpecularPass();

	CIFXRenderServicesPtr m_pRS;

	IFXRenderPtr* m_ppRender;
	IFXMonitor* m_pMonitors;
	U32 m_uNumMonitors;
	IFXRect m_rcWindowVC;

	U32 m_uCurrentDevice;
	U32 m_uRenderId;

	IFXRenderCaps m_Caps;

	IFXRenderTexUnit  m_CurrTexUnit;
	IFXRenderTexUnit  m_pTexUnits[IFX_MAX_TEXUNITS];
	IFXRenderMaterial m_Material;
	IFXRenderMaterial m_CurrMaterial;
	IFXRenderBlend    m_Blend;
	IFXRenderBlend    m_CurrBlend;
	IFXVector4      m_pvConstantColors[IFX_MAX_TEXUNITS];
	IFXRenderView   m_View;
	BOOL        m_bCurrLighting;
	BOOL        m_bLighting;
	BOOL        m_pbLightEnabled[IFX_MAX_LIGHTS];
	BOOL        m_bCurrBlendEnabled;
	BOOL        m_bBlendEnabled;
	BOOL        m_bCurrAlphaTestEnabled;
	BOOL        m_bAlphaTestEnabled;
	BOOL        m_pbTexUnitEnabled[IFX_MAX_TEXUNITS];
	I32         m_ppiTexUnitMap[IFX_MAX_TEXUNITS][IFX_MAX_TEXUNITS];
	U32         m_uNumRenderPasses;
	BOOL        m_bSeparateSpecularPass;

	IFXRenderWindow   m_Window;
};
typedef IFXSmartPtr<CIFXRenderContext> CIFXRenderContextPtr;

#endif
