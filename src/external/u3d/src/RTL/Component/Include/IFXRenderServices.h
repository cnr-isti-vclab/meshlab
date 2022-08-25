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
// IFXRenderServices interface class
#ifndef IFX_RENDERSERVICES_H
#define IFX_RENDERSERVICES_H

#include "IFXUnknown.h"
#include "IFXRenderHelpers.h"
#include "IFXEnums.h"

/**
 *  Interface ID for the IFXRenderServices interface.
 */
// {44969B67-4A5F-4242-B775-5467F302978B}
IFXDEFINE_GUID(IID_IFXRenderServices,
0x44969b67, 0x4a5f, 0x4242, 0xb7, 0x75, 0x54, 0x67, 0xf3, 0x2, 0x97, 0x8b);

/**
 *  The IFXMonitor object details a physical monitor attached to the
 *  system.  It contains an IFXRect containing the set resolution of the
 *  monitor, and a device id that identifies which video device the
 *  monitor is attached to.
 */
struct IFXMonitor
{
	IFXRect m_rcScreen; ///< Resolution of monitor.
	U32 m_uDeviceNum; ///< Video adapter device id.
};

/**
 *  The IFXRenderServices object is a singleton rendering manager.  It keeps
 *  track of registered renderer types and allocated render device objects.
 *  It also holds default values for various rendering states (i.e. internal
 *  texture pixel format).  The IFXRenderServices object must be created before
 *  any IFXRenderContext objects, and must be destroyed after all IFXRenderContext
 *  objects have been destroyed.
 */
class IFXRenderServices : virtual public IFXUnknown
{
public:
  /**
   *  Registers a renderer type with the system.  Each renderer type
   *  (DirectX8, OpenGL, Software, etc.) is described by a renderer ID,
   *  along with class ID's (CID's) for the type specific implementation
   *  of IFXRender and IFXRenderDevice interfaces (these implementations
   *  should be derived from CIFXRender and CIFXRenderDevice respectively).
   *
   *  @param  uRenderId Input render ID that will be associated with
   *            this renderer type.  This value must not
   *            be 0.
   *
   *  @param  cidRender Input IFXREFCID (class ID) that identifies
   *            the renderer specific implementation of
   *            IFXRender.  This class should be derived from
   *            CIFXRender.
   *
   *  @param  cidRenderDevice Input IFXREFCID (class ID) that identifies
   *              the renderer specific implementation of
   *              IFXRenderDevice.  This class should be
   *              derived from CIFXRenderDevice.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE Either the value of @a uRenderId is
   *                0, or the given @a uRenderId has already
   *                been registered.
   */
  virtual IFXRESULT IFXAPI RegisterRenderer(  U32 uRenderId,
                        IFXREFCID cidRender,
                        IFXREFCID cidRenderDevice )=0;

  /**
   *  Resets all information relating to what renderer types are supported
   *  to default values.  This will force the system to re-run the query
   *  to determine which renderers are supported when needed.  This is
   *  appropriate to do, for example, when the color depth of the desktop
   *  changes, potentially making some renderers unable to function
   *  properly.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   */
  virtual IFXRESULT IFXAPI ResetSupportedInfo()=0;

  /**
   *  Inform the render services object that the given (pre-registered) renderer
   *  type is not supported, and shouldn't even be queried to see if it is.
   *  Some renderer types and driver combinations can cause a system crash
   *  when querying for device support.  This will prevent that from happening
   *  on known bad video drivers.
   *
   *  @param  uRenderId The renderer type ID corresponding to a registered
   *            renderer type.  This renderer type will be listed
   *            as unsupported and will not be queried for support.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The value specified for @a uRenderId was invalid.
   */
  virtual IFXRESULT IFXAPI SetNotSupported(U32 uRenderId)=0;

  /**
   *  Query whether a given renderer type is supported on the current system
   *  configuration.  If this has not been determined previously, the system
   *  will query the renderer type for support (usually doing a test
   *  initialization using that renderer type).
   *
   *  @param  uRenderId The renderer type ID corresponding to a registered
   *            renderer type.
   *
   *  @return A BOOL value.
   *
   *  @retval TRUE  The renderer type is supported on the current system
   *          configuration.
   *  @retval FALSE The renderer type is not supported on the current system
   *          configuration.  This could be due to lack of driver or
   *          hardware support, or the user could be using an
   *          incompatible color depth.  This could also be caused
   *          by an invalid @a uRenderId value.
   */
  virtual BOOL    IFXAPI GetSupported(U32 uRenderId)=0;

  /**
   *  Retrieve the user-defined "recommended" flag for the specified
   *  renderer type.  All renderers have this flag default to TRUE
   *  (recommended) upon registration.
   *
   *  @param  uRenderId The renderer type id corresponding to a registered
   *            renderer type.
   *
   *  @return A BOOL value.
   *
   *  @retval TRUE  The renderer type is recommended.
   *  @retval FALSE The renderer type is not recommended.  This is due to
   *          previous call to SetRecommended().  This could also
   *          be caused by an invalid @a uRenderId value.
   */
  virtual BOOL    IFXAPI GetRecommended(U32 uRenderId)=0;

  /**
   *  Sets the recommended flag for the specified renderer type.  This flag
   *  is primarily used to denote bad renderer type / video driver combinations.
   *
   *  @param  uRenderId The renderer type ID corresponding to a registered
   *            renderer type.
   *  @param  bRecommended  Input BOOL containing the new recommended flag state.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a uRenderId value was invalid.
   */
  virtual IFXRESULT IFXAPI SetRecommended(U32 uRenderId, BOOL bRecommended)=0;

  /**
   *  Gets the user-defined requested renderer type ID.
   *
   *  @return A U32 value containing the currently set requested
   *      renderer type id.
   */
  virtual U32     IFXAPI GetRequestedRenderId()=0;

  /**
   *  Sets the requested renderer type.  When an IFXRenderContext instance
   *  has a renderer ID of 0, it will use this renderer type instead.
   *  Changing this value will result in all instances of IFXRenderContext
   *  that contain a renderer ID of 0 to switch to the new renderer type
   *  during the next frame of rendering.
   *
   *  @param  uRenderId The renderer type ID corresponding to a registered
   *            renderer type.  This must be a supported renderer
   *            type.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a uRenderId value was invalid.
   *  @retval IFX_E_UNSUPPORTED The renderer type specified by @a uRenderId
   *                is not supported.
   */
  virtual IFXRESULT IFXAPI SetRequestedRenderId(U32 uRenderId)=0;

  /**
   *  Gets the user-defined "safe" renderer type ID.  This value
   *  currently not used for anything.
   *
   *  @return A U32 value containing the "safe" renderer type id.
   */
  virtual U32     IFXAPI GetSafeRenderId()=0;

  /**
   *  Set the "safe" renderer type ID.  This value is currently unused.
   *
   *  @param  uRenderId The renderer type ID corresponding to a registered
   *            renderer type.  This must be a supported renderer
   *            type.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a uRenderId value was invalid.
   *  @retval IFX_E_UNSUPPORTED The renderer type specified by @a uRenderId
   *                is not supported.
   */
  virtual IFXRESULT IFXAPI SetSafeRenderId(U32 uRenderId)=0;

  /**
   *  Gets the user-defined default anti-aliasing enabled state.
   *
   *  @return A BOOL value.
   *
   *  @retval TRUE  Anti-aliasing is enabled by default.
   *  @retval FALSE Anti-aliasing is disabled by default.
   */
  virtual BOOL    IFXAPI GetDefaultAAEnalbed()=0;

  /**
   *  Sets the default anti-aliasing enabled state.  Any IFXRenderContext
   *  instance that has @b IFX_AA_DEFAULT set for it's anti-aliasing enabled
   *  value will use this setting.  Changing this setting will automatically
   *  change all instances of IFXRenderContext that are configured to
   *  use @b IFX_AA_DEFAULT.
   *
   *  @param  bEnabled  Input BOOL specifying the anti-aliasing enabled state.
   *            @b TRUE will enable anti-aliasing, @b FALSE will
   *            disable anti-aliasing.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   */
  virtual IFXRESULT IFXAPI SetDefaultAAEnabled(BOOL bEnabled)=0;

  /**
   *  Gets the user-defined default anti-aliasing mode.
   *
   *  @return An IFXenum value.
   *  @retval IFX_AA_2X 2X anti-aliasing is used.
   *  @retval IFX_AA_3X 3X anti-aliasing is used.  On many cards this is Quincunx* anti-aliasing.
   *  @retval IFX_AA_4X 4X anti-aliasing is used.
   *  @retval IFX_AA_4X_SW  4X anti-aliasing using software emulation is used.
   *              this is very slow.
   */
  virtual IFXenum   IFXAPI GetDefaultAAMode()=0;

  /**
   *  Sets the default anti-aliasing mode.  Any IFXRenderContext instances that have
   *  an anti-aliasing mode of @b IFX_AA_DEFAULT will use this setting.
   *
   *  @param  eAAMode Input IFXenum value denoting the anti-aliasing mode.  The
   *          possible values are:
   *            - @b IFX_AA_2X: 2X anti-aliasing is used.
   *            - @b IFX_AA_3X: 3X anti-aliasing is used.
   *              On many cards this is Quincunx* anti-aliasing.
   *            - @b IFX_AA_4X: 4X anti-aliasing is used.
   *            - @b IFX_AA_4X_SW: 4X anti-aliasing is used with
   *              software emulation.  This is very slow.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a eAAMode value was invalid.
   */
  virtual IFXRESULT IFXAPI SetDefaultAAMode(IFXenum eAAMode)=0;

  /**
   *  Sets the default texture render format.  Textures that specify
   *  a format of @b IFX_FMT_DEFAULT will use this format instead.
   *  The initial value for this setting is IFX_RGBA_5551.
   *
   *  @param  eTexFormat  Input IFXenum specifying the default texture render
   *            format.  Possible values are:
   *              - @b IFX_RGBA_8888
   *              - @b IFX_RGBA_8880
   *              - @b IFX_RGBA_5650
   *              - @b IFX_RGBA_5551
   *              - @b IFX_RGBA_5550
   *              - @b IFX_RGBA_4444
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a eTexFormat value was invalid.
   */
  virtual IFXRESULT IFXAPI SetTextureRenderFormat(IFXenum eTexFormat)=0;

  /**
   *  Sets the pixel aspect of the device.  The pixel aspect can be changed when
   *  the display device used has a pixel aspect other than 1.0.  For instance, you
   *  are using a standard 4:3 resolution (1600x1200) on a widescreen 16:9
   *  monitor.  In that case, you would use a pixel aspect ratio of 1.333333.
   *  The initial value is 1.0.
   *
   *  @param  fRatio  The new pixel aspect ratio to use.  Must be a positive
   *          non-zero number.
   *
   *  @return An IFXRESULT value.
   *
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The value specified with @a fRatio was either
   *                0.0 or negative.
   */
  virtual IFXRESULT IFXAPI SetPixelAspectRatio(F32 fRatio)=0;

  /**
   *  Sets the depth buffer format to be used by all IFXRenderContext instances.
   *  The default value is IFX_DEPTH_D32S0.  If the rendering system
   *  cannot support the specified format, it will fall back to a format
   *  that will work.
   *
   *  @param  eDepthFormat  Input IFXenum value specifying the depth buffer
   *              format to use.  Possible values are:
   *              - @b IFX_DEPTH_D16S0: 16-bit depth, no stencil
   *              - @b IFX_DEPTH_D32S0: 24-bit depth, no stencil
   *              - @b IFX_DEPTH_D24S8: 24-bit depth, 8-bit stencil
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   *  @retval IFX_E_INVALID_RANGE The @a eDepthFormat value was invalid.
   */
  virtual IFXRESULT IFXAPI SetDepthBufferFormat(IFXenum eDepthFormat)=0;

  /**
   *  Sets whether or not vsync is enabled.  If vsync is enabled, renderer
   *  types that support it will cause IFXRenderContext::Swap() to block
   *  until the next vertical retrace of the monitor before completing.
   *  This affects all instances of IFXRenderContext.  The initial value
   *  is FALSE.
   *
   *  @param  bEnabled  Input BOOL.  If @b TRUE, vsync is enabled,
   *            otherwise, vsync is disabled.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   */
  virtual IFXRESULT IFXAPI SetVSyncEnabled(BOOL bEnabled)=0;

  /**
   *  Retrieves the default texture render format.  See SetTextureRenderFormat()
   *  for more information.
   *
   *  @return An IFXenum value.
   *  @retval IFX_RGBA_8888
   *  @retval IFX_RGBA_8880
   *  @retval IFX_RGBA_5650
   *  @retval IFX_RGBA_5551
   *  @retval IFX_RGBA_5550
   *  @retval IFX_RGBA_4444
   */
  virtual IFXenum   IFXAPI GetTextureRenderFormat()=0;

  /**
   *  Retrieves the currently set pixel aspect ratio.  See SetPixelAspectRatio()
   *  for more information.
   *
   *  @return An F32 value containing the current pixel aspect ratio.
   */
  virtual F32     IFXAPI GetPixelAspectRatio()=0;

  /**
   *  Retrieves the currently set depth buffer format.
   *
   *  @return An IFXenum value containing the current depth buffer format.
   *  @retval IFX_DEPTH_D16S0 16-bit depth, no stencil
   *  @retval IFX_DEPTH_D32S0 24-bit depth, no stencil
   *  @retval IFX_DEPTH_D24S8 24-bit depth, 8-bit stencil
   */
  virtual IFXenum   IFXAPI GetDepthBufferFormat()=0;

  /**
   *  Gets the current vsync enabled state.
   *
   *  @return A BOOL value.
   *  @retval TRUE  Vsync is currently enabled.
   *  @retval FALSE VSync is currently disabled.
   */
  virtual BOOL    IFXAPI GetVSyncEnabled()=0;

  /**
   *  Gets the current bit depth of the color buffer.  This is identical
   *  to the current color depth of the primary monitor.
   *
   *  @return A U32 value containing the number of bits per pixel in the
   *      color buffer (8/16/32).
   */
  virtual U32     IFXAPI GetColorBufferDepth()=0;

  /**
   *  Gets the number of video cards attached to the system.  This may be
   *  different than the number of display devices (multiple monitors
   *  could be attached to a single video card).
   *
   *  @return A U32 value containing the number of video cards present
   *      in the system that are usable for rendering.
   */
  virtual U32     IFXAPI GetNumDevices()=0;

  /**
   *  Retrieves information about all monitors attached to the system and
   *  usable for displaying rendered content.
   *
   *  @param  uNumMonitors  Reference to a U32 that will receive the number
   *              of usable monitors attached to the system.
   *
   *  @return A pointer to an IFXMonitor array (of length @a uNumMonitors)
   *      that contains information about each monitor in the system.
   */
  virtual const IFXMonitor* IFXAPI GetMonitors(U32& uNumMonitors) const=0;

  /**
   *  Deletes a texture from all instances of IFXRenderContext.
   *
   *  @param  uTextureId  Input U32 containing the id of the texture to
   *            be deleted.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   */
  virtual IFXRESULT IFXAPI DeleteTexture(U32 uTextureId)=0;

  /**
   *  Deletes a cached static mesh from all instances of IFXRenderContext.
   *
   *  @param  uId Input U32 containing the id of the static mesh to
   *        be deleted.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   */
  virtual IFXRESULT IFXAPI DeleteStaticMesh(U32 uId)=0;

  /**
   *  Normally, IFXRenderServices shares IFXRenderDevice instances of the
   *  same render type between multiple instances of IFXRenderContext.  It
   *  will also hold these IFXRenderDevice instances for later use.
   *  This will force instances of IFXRenderDevice that are not currently
   *  being used by any IFXRenderContext instances to be deleted.
   *
   *  @return An IFXRESULT value.
   *  @retval IFX_OK  No error.
   */
  virtual IFXRESULT IFXAPI PurgeDevices()=0;
};

/**
 *  Smart pointer specific to IFXRenderServices.
 */
typedef IFXSmartPtr<IFXRenderServices> IFXRenderServicesPtr;

#endif
