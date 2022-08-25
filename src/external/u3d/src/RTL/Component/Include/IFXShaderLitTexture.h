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
	@file	IFXShaderLitTexture.h

			Contains the IFXShaderLitTexture interface.
*/


#ifndef __IFXSHADERLITTEXTURE_H__
#define __IFXSHADERLITTEXTURE_H__


#include "IFXUnknown.h"
#include "IFXShader.h"
class IFXSceneGraph;

// IFX_SHADERLITTEXTURE_PRIORITY_FACTOR is used to decrease the priority
// of data blocks which contain texture image data
#define IFX_SHADERLITTEXTURE_PRIORITY_FACTOR 4

#define IFX_TEXTURE_REPEAT_1 0x01 // U
#define IFX_TEXTURE_REPEAT_2 0x02 // V

// {90656A62-0366-11d4-8905-444553540000}
IFXDEFINE_GUID(IID_IFXShaderLitTexture,
0x90656a62, 0x366, 0x11d4, 0x89, 0x5, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

class IFXShaderLitTexture : virtual public IFXShader
{
public:
	enum Flags
	{
		MATERIAL    = (1<<0),
		USEDIFFUSE  = (1<<1),
		FLAT        = (1<<2),
		WIRE        = (1<<3),
		POINT       = (1<<4)
	};

	enum BlendFunction
	{
		MULTIPLY = IFX_MODULATE,
		ADD = IFX_ADD,
		REPLACE = IFX_SELECT_ARG0,
		BLEND = IFX_INTERPOLATE
	};

	enum BlendSource
	{
		ALPHA = 0,
		CONSTANT
	};

	enum TextureMode
	{
		TM_NONE,
		TM_PLANAR,
		TM_CYLINDRICAL,
		TM_SPHERICAL,
		TM_REFLECTION,

#ifdef SPEC_COVERS_CUBE_MAPS
		TM_LOCALPOSITION,
		TM_LOCALNORMAL,
		TM_LOCALREFLECTION,
		TM_WORLDPOSITION,
		TM_WORLDNORMAL,
		TM_WORLDREFLECTION,
		TM_VIEWPOSITION,
		TM_VIEWNORMAL,
		TM_VIEWREFLECTION,
#endif

		TM_NUM_MODES
	};

	/**
	*  Returns a bitmask of the enabled channels.
	*/
	virtual       U32       IFXAPI GetChannels(                                 )=0;

	/**
	*  Sets all enabled shader channels in a single call.
	*/
	virtual        IFXRESULT IFXAPI SetChannels(            U32  uInEnabledFlags )=0;

	/**
	*  Returns a bitmask of the IFXShaderLitTexture::Flags debugging flags.
	*
	*  @note	These flags are not a part of the U3D specification and are 
	*			therefore not decoded or encoded as part of the Lit Texture 
	*			Shader block (9.8.3).
	*
	*  @note	The default flags set are IFXShaderLitTexture::MATERIAL and 
	*			IFXShaderLitTexture::USEDIFFUSE.  All other flags are 
	*			clear by default.
	*/
	virtual U32 IFXAPI GetFlags() = 0;

	/**
	*  Sets all IFXShaderLitTexture::Flags debugging flags in a single call.
	*
	*  @note	These flags are not a part of the U3D specification and are 
	*			therefore not decoded or encoded as part of the Lit Texture 
	*			Shader block (9.8.3).
	*
	*  @note	The default flags set are IFXShaderLitTexture::MATERIAL and 
	*			IFXShaderLitTexture::USEDIFFUSE.  All other flags are 
	*			clear by default.
	*/
	virtual IFXRESULT IFXAPI SetFlags(U32 uInEnabledFlags) = 0;

	/**
	*  Returns shader channels that use a texture containing an alpha channel.
	*/
	virtual       U32       IFXAPI GetAlphaTextureChannels(                     )=0;
	/**
	*  Sets which shader channels use a texture containing an alpha channel.
	*/
	virtual        IFXRESULT IFXAPI SetAlphaTextureChannels( U32  uInEnabledFlags )=0;

	/**
	*  Returns the material palette ID of the IFXMaterialResource used
	*  by this shader.
	*/
	virtual       U32       IFXAPI GetMaterialID(                               )=0;

	/**
	*  Sets the material palette ID of the IFXMaterialResource used
	*  by this shader.
	*/
	virtual       IFXRESULT IFXAPI SetMaterialID(          U32  uInMaterialID   )=0;

	/**
	*  Returns a reference to the interface to an object that provides frame
	*  buffer blend and alpha test parameters to an IFXRenderContext
	*  interface.  This interface encapsulates all state settings for alpha
	*  blending and alpha test.
	*/
	virtual IFXRenderBlend&	 IFXAPI GetRenderBlend(							  )=0;

	/**
	*  Returns the flag indicating whether or not alpha testing is enabled
	*/
	virtual		   BOOL		 IFXAPI GetAlphaTestEnabled(						  )=0;

	/**
	*  Turns alpha testing on or off.  Alpha testing determines if an
	*  incoming fragment is drawn by comparing the fragment's alpha value
	*  to a reference value.
	*/
	virtual		   IFXRESULT IFXAPI SetAlphaTestEnabled(    BOOL bInEnabled      )=0;

	/**
	*  Reports whether or not lighting calculations are done for surfaces
	*  using this shader.
	*/
	virtual		   BOOL		 IFXAPI GetLightingEnabled(						  )=0;

	/**
	*  Enables or disables lighting calculations for surfaces using this
	*  shader.
	*/
	virtual		   IFXRESULT IFXAPI SetLightingEnabled(	 BOOL bInEnabled	  )=0;

	/**
	*  Returns a reference to the interface to an object that encapsulates
	*  all material settings needed for vertex lighting.  All material
	*  settings for vertex lighting must be set through this interface, and
	*  then submitted to an IFXRender object with IFXRenderContext::SetMaterial().
	*/
	virtual	IFXRenderMaterial& IFXAPI GetRenderMaterial(						  )=0;

	/**
	*  Returns the texture palette ID of the texture being used in the
	*  specified texture layer of this shader (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI GetTextureID(           U32  uInLayer,
	                                                 U32* puOutID         )=0;

	/**
	*  Sets the texture palette ID for a specific texture layer of the shader
	*  (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI SetTextureID(           U32  uInLayer,
	                                                 U32  uInID           )=0;

	/**
	*  Returns the intensity (weighting factor) used when determining how
	*  strongly a texture layer contributes to the overall effect of a
	*  shader (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI GetTextureIntensity(    U32  uInLayer,
	                                                 F32* pfOutIntensity  )=0;

	/**
	*  Sets the intensity (weighting factor) used when determining how
	*  strongly a texture layer contributes to the overall effect of a
	*  shader (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI SetTextureIntensity(    U32  uInLayer,
	                                                 F32  fInIntensity    )=0;

	/**
	*  Returns the manner in which the current texture layer is combined
	*  with the result coming from the previous layers (specified by
	*  layer number).
	*/
	virtual       IFXRESULT IFXAPI GetBlendFunction(       U32 uInLayer,
	                                                 BlendFunction* peOut )=0;

	/**
	*  Sets the manner in which the current texture layer is combined
	*  with the result coming from the previous layers (specified by
	*  layer number).  Valid values are MULTIPLY, ADD, REPLACE, and BLEND.
	*/
	virtual       IFXRESULT IFXAPI SetBlendFunction(       U32 uInLayer,
	                                                 BlendFunction eInFn  )=0;

	/**
	*  Returns whether the blending operation combines the current layer
	*  with the result coming from previous layers using a blending
	*  constant, or the alpha value of each pixel (specified by
	*  layer number).
	*/
	virtual       IFXRESULT IFXAPI GetBlendSource(         U32 uInLayer,
	                                                 BlendSource* peOut   )=0;

	/**
	*  Sets whether the blending operation combines the current layer
	*  with the result coming from previous layers using a blending
	*  constant, or the alpha value of each pixel(specified by
	*  layer number).  Valid values are ALPHA or CONSTANT.
	*/
	virtual       IFXRESULT IFXAPI SetBlendSource(         U32 uInLayer,
	                                                 BlendSource eIn      )=0;

	/**
	*  Gets the blending ratio that is used when
	*  SetBlendFunction is set to BLEND and SetBlendSource is set
	*  to CONSTANT for the specific channel (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI GetBlendConstant(       U32 uInLayer,
	                                                 F32* pfOutConstant )=0;

	/**
	*  Sets the blending ratio that is used when
	*  SetBlendFunction is set to BLEND and SetBlendSource is set
	*  to CONSTANT for the specific channel (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI SetBlendConstant(       U32 uInLayer,
	                                                 F32 fInConstant )=0;

	/**
	*  Returns the source of texture coordinates used to map the
	*  texture in the specified texture layer (specified by a layer
	*  number) onto the model.
	*/
	virtual       IFXRESULT IFXAPI GetTextureMode(         U32 uInLayer,
	                                                 TextureMode* peOut   )=0;

	/**
	*  Sets the source of texture coordinates used to map the
	*  texture in the specified texture layer (specified by a layer
	*  number) onto the model.  Valid values include TM_NONE, TM_PLANAR,
	*  TM_CYLINDRICAL, TM_SPHERICAL, TM_REFLECTION, TM_DIFFUSE, and
	*  TM_SPECULAR.  If the mode is set to TM_NONE, the texture is
	*  mapped using texture coordinates present within the model.
	*  In all cases but TM_NONE, texture coordinates are generated by the
	*  shader automatically, on a frame-by-frame basis in the case of
	*  TM_REFLECTION, TM_DIFFUSE, and TM_SPECULAR.
	*/
	virtual       IFXRESULT IFXAPI SetTextureMode(         U32 uInLayer,
	                                                 TextureMode  eInMode )=0;

	/**
	*  Returns the texture-space transform that operates on the
	*  texture coordinates after they have been applied to the model
	*  for the given texture layer (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI GetTextureTransform(    U32 uInLayer,
	                                         IFXMatrix4x4** ppmOutTextureTransform )=0;

	/**
	*  Sets the texture-space transform that operates on the
	*  texture coordinates after they have been applied to the model
	*  for the given texture layer (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI SetTextureTransform(    U32 uInLayer,
	                                         IFXMatrix4x4* pmInTextureTransform  )=0;

	/**
	*  Returns the model-space transform that operates on the
	*  texture coordinates before they are applied to the model
	*  for the given texture layer (specified by layer number).
	*/
	virtual       IFXRESULT IFXAPI GetWrapTransform(       U32 uInLayer,
	                                         IFXMatrix4x4** ppmOutWrapTransform )=0;

	/**
	*  Sets the model-space transform that operates on the
	*  texture coordinates before they are applied to the model
	*  for the given texture layer (specified by layer number).
	*  This transform only has an effect if the value returned by
	*  GetTextureMode() is TM_PLANAR, TM_CYLINDRICAL, or TM_SPHERICAL.
	*/
	virtual       IFXRESULT IFXAPI SetWrapTransform(       U32 uInLayer,
	                                         IFXMatrix4x4* pmInWrapTransform  )=0;

	/**
	*  Returns whether or not the texture in the specified texture layer
	*  should be tiled (repeated) beyond the texture coordinate range
	*  generated on the model (texture layer specified by layer number)
	*/
	virtual       IFXRESULT IFXAPI GetTextureRepeat(         U32 uInLayer,
	                                         U8* puOutTextureRepeat   )=0;

	/**
	*  Sets whether or not the texture in the specified texture layer
	*  should be tiled (repeated) beyond the texture coordinate range
	*  generated on the model (texture layer specified by layer number)
	*/
	virtual       IFXRESULT IFXAPI SetTextureRepeat(         U32 uInLayer,
	                                         U8  uInTextureRepeat )=0;

	/*
	*  Returns whether the diffuse material color is used to modulate the
	*  texture used for the BASE texture layer.
	*
	*  @note	This setting is not a part of the U3D specification and is 
	*			therefore not decoded or encoded as part of the Lit Texture 
	*			Shader block (9.8.3).
	*
	*  @note	This is simply a helper function that obtains the setting of 
	*			the IFXShaderLitTexture::USEDIFFUSE flag which is generally 
	*			obtained by the IFXShaderLitTexture::GetFlags function.  It 
	*			defaults to TRUE and being set.
	*/
	virtual       BOOL IFXAPI GetUseDiffuse( void )=0;

	/**
	*  Sets whether the diffuse material color is used to modulate the
	*  texture used for the BASE texture layer. When GetBlendFunction()
	*  is set to BLEND and this flag is set to true, the diffuse color
	*  will be weighed with the texture color when determining final
	*  color.  For example, if GetBlendFunction() returns BLEND and
	*  GetBlendConstant() returns 100.0, only the pure texture color
	*  will show. If GetBlendConstant() returns 0.0, only the diffuse
	*  color will show. If GetBlendConstant() returns 10.0, the final
	*  color will be 10% texture color, 90% diffuse color.
	*
	*  @note	This setting is not a part of the U3D specification and is 
	*			therefore not decoded or encoded as part of the Lit Texture 
	*			Shader block (9.8.3).
	*
	*  @note	This is simply a helper function that controls if the 
	*			IFXShaderLitTexture::USEDIFFUSE flag is set or not that is 
	*			generally controlled by the IFXShaderLitTexture::SetFlags 
	*			function.  It defaults to TRUE and being set.
	*/
	virtual void IFXAPI SetUseDiffuse( BOOL bUseDiffuse )=0;

	/**
	*  Sets whether the texture transform (not the texture-coordinate
	*  transform) is inverted or not (it is inverted by default).  Inverting
	*  the transform makes things like scaling textures more natural,
	*  since to enlarging an image means that the texture transform
	*  needs to actually contain an inverse of the the scale, and this
	*  does it for you automatically.
	*/
	virtual IFXRESULT IFXAPI SetInvertTexTransforms(BOOL bInvert)=0;

	/**
	*  Returns whether the texture transform (not the texture-coordinate
	*  transform) is inverted or not (it is inverted by default).
	*/
	virtual BOOL      IFXAPI GetInvertTexTransforms() const=0;


	// These are for the #standard shader only.


	/**
	*  Enables or disables the drawing of any mesh faces that pass through
	*  this shader (default = TRUE).  Applies only to the standard shader.
	*/
	virtual IFXRESULT IFXAPI SetDrawFaces(BOOL bEnable)	=0;

	/**
	*  Returns whether or not the faces rendered using this shader are
	*  drawn or not. Applies only to the standard shader.
	*/
	virtual BOOL	  IFXAPI GetDrawFaces()				=0;

	/**
	*  Enables or disables the drawing of any line primitives present
	*  in the mesh being rendered using this shader (default = FALSE).
	*  Allies only to the standard shader.
	*/

	virtual IFXRESULT IFXAPI SetDrawLines(BOOL bEnable)	=0;

	/**
	*  Returns whether or not any line primitives present in the mesh
	*  passing through this shader should be rendered.
	*/
	virtual BOOL	  IFXAPI GetDrawLines()				=0;
};


#endif
