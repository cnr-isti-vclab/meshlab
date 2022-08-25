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
// IFXRenderTexUnit.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRenderContext.h
#ifndef IFX_RENDER_TEXUNIT_H
#define IFX_RENDER_TEXUNIT_H

//==============================
// IFXRenderTexUnit
//==============================
/**
*  This is the interface to an object that encapsulates all settings of a texture unit
*  (or texture stage).  It is used to pass in these settings to an IFXRender object
*  using IFXRenderContext::SetTextureUnit().  Specific texture units can be enabled or disabled
*  with IFXRenderContext::Enable(IFX_TEXUNITn) or IFXRenderContext::Disable(IFX_TEXUNITn) where n is
*  between 0 and IFX_MAX_TEXUNITS.  IFX_TEXUNITn is guaranteed to be equivalant to
*  (IFX_TEXUNIT0 + n).
*
*  Each texture unit can be thought of a two separate blending stages, one for
*  RGB and one for Alpha.  Each blending stage can have two inputs, one output.
*
*  @see IFXRenderContext::SetTextureUnit(), IFXRenderContext::Enable(), IFXRenderContext::Disable()
*/
class IFXRenderTexUnit
{
public:
	//=============================================
	// IFXRenderTexUnit Constructor/Destructor
	//=============================================
	IFXRenderTexUnit();
	virtual ~IFXRenderTexUnit();

	//================================
	// IFXRenderTexUnit methods
	//================================
	/**
	*  Specifies input 0 for the RGB blending stage.  Note: not all video cards will work with
	*  every available input.  In those cases, the closest match is used.
	*
	*  @param eInput Input IFXenum specifying the input to be used.  Valid values are:
	*          - @b IFX_INCOMING: Use the output of the previous texture unit.  This is
	*            the same is IFX_DIFFUSE for IFX_TEXUNIT0.
	*          - @b IFX_DIFFUSE: Use the lit diffuse material color.
	*          - @b IFX_TEXTURE: Use the value retrieved from the associated texture.
	*          - @b IFX_CONSTANT: Use the value specified with IFXRenderContext::SetConstantColor().
	*
	*          All of these options can be combined with @b IFX_INVERSE.
	*          For example (IFX_DIFFUSE | IFX_INVERSE) will use the result of
	*          (White - DiffuseColor) as the actual input.
	*
	*          All of these options can also be combined with @b IFX_ALPHA.  If
	*          IFX_ALPHA is specified, than the alpha value for the specified
	*          source is replicated across the rgb channels.  For example,
	*          (IFX_TEXTURE | IFX_ALPHA | IFX_INVERSE) will use
	*          (White - TextureAlpha) as a grayscale input into the color channels.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetRGBInput0(IFXenum eInput);

	/**
	*  Specifies input 1 for the RGB blending stage.  Note: not all video cards will work with
	*  every available input.  In those cases, the closest match is used.
	*
	*  @todo Make a reference to a see also link describing the caps settings.
	*
	*  @param eInput Input IFXenum specifying the input to be used.  Valid values are:
	*          - @b IFX_INCOMING: Use the output of the previous texture unit.  This is
	*            the same is IFX_DIFFUSE for IFX_TEXUNIT0.
	*          - @b IFX_DIFFUSE: Use the lit diffuse material color.
	*          - @b IFX_CONSTANT: Use the value specified with IFXRenderContext::SetConstantColor().
	*
	*          All of these options can be combined with @b IFX_INVERSE.
	*          For example (IFX_DIFFUSE | IFX_INVERSE) will use the result of
	*          (White - DiffuseColor) as the actual input.
	*
	*          All of these options can also be combined with @b IFX_ALPHA.  If
	*          IFX_ALPHA is specified, than the alpha value for the specified
	*          source is replicated across the rgb channels.  For example,
	*          (IFX_CONSTANT | IFX_ALPHA | IFX_INVERSE) will use
	*          (White - ConstantAlpha) as a grayscale input into the color channels.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetRGBInput1(IFXenum eInput);

	/**
	*  Sets the RGB interpolating source.  This is only used when the RGBBlendFunc is set to
	*  IFX_INTERPOLATE.  The interpolating value is the alpha channel value from the specified
	*  source.
	*
	*  @param  eInput  Input IFXenum specifying the source of the interpolating value.
	*          Valid options are:
	*          - @b IFX_INCOMING:  Use the output alpha value of the previous
	*            texture unit.  This is the same as IFX_DIFFUSE for IFX_TEXUNIT0.
	*          - @b IFX_DIFFUSE: Use the lit diffuse material alpha value.
	*          - @b IFX_CONSTANT: Use the value specified with IFXRenderContext::SetConstantColor().
	*          - @b IFX_TEXTURE: Use the alpha value from the texture bound to
	*            this texture unit.
	*
	*          All of these options are implicitly combined with IFX_ALPHA.  It
	*          is not possible to use color values as the interpolation value.
	*
	*          It is not possible to use IFX_INVERSE to invert the interpolating
	*          value.  If IFX_INVERSE is included, it will be ignored.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetRGBInterpolatorSource(IFXenum eInput);

	/**
	*  Specifies the blending function between RGBInput0 and RGBInpu1.
	*
	*  Note: Not all video cards support all blending functions.  In this case, the closest
	*  approximate match will be used.
	*
	*
	*
	*  @param eFunc  Input IFXenum specifying which blending function to use.  Color or alpha
	*          values represented below are in the range (0,1).  Valid options are:
	*          - @b  IFX_SELECT_ARG0: out = Input0
	*          - @b  IFX_MODULATE: out = Input0 * Input1
	*          - @b  IFX_MODULATE2X: out = Input0 * Input1 * 2
	*          - @b  IFX_MODULATE4X: out = Input0 * Input1 * 4
	*          - @b  IFX_ADD: out = Input0 + Input1
	*          - @b  IFX_ADDSIGNED: out = Input0 + Input1 - 0.5
	*          - @b  IFX_ADDSIGNED2X: out = (Input0 + Input1 - 0.5) * 2
	*          - @b  IFX_INTERPOLATE: out = Input0*Incoming.Alpha + (Input1*(1-Interpolator.Alpha))
	*          - @b  IFX_DOTPRODUCT3: out = Input0.R*Input1.R + Input0.B*Input1.B + Input0.G*Input1.G
	*            If IFX_DOTPRODUCT3 is not supported, then IFX_SELECT_ARG0 is used instead.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetRGBBlendFunc(IFXenum eFunc);

	/**
	*  Specifies input 0 for the Alpha blending stage.  Note: not all video cards will work with
	*  every available input.  In those cases, the closest match is used.
	*
	*
	*
	*  @param eInput Input IFXenum specifying the input to be used.  Valid values are:
	*          - @b IFX_INCOMING: Use the output of the previous texture unit.  This is
	*            the same is IFX_DIFFUSE for IFX_TEXUNIT0.
	*          - @b IFX_DIFFUSE: Use the lit diffuse material alpha.
	*          - @b IFX_TEXTURE: Use the alpha value retrieved from the associated texture.
	*          - @b IFX_CONSTANT: Use the alpha value specified with IFXRenderContext::SetConstantColor().
	*
	*          All of these options can be combined with @b IFX_INVERSE.
	*          For example (IFX_DIFFUSE | IFX_INVERSE) will use the result of
	*          (White - DiffuseAlpha) as the actual input.
	*
	*          All of these options are implicitly combined with IFX_ALPHA.  It
	*          is not possible to use color values as input into the Alpha combiner.
	*
	*  @return An IFXRESULT error code
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetAlphaInput0(IFXenum eInput);

	/**
	*  Specifies input 1 for the Alpha blending stage.  Note: not all video cards will work with
	*  every available input.  In those cases, the closest match is silently used.
	*
	*
	*
	*  @param eInput Input IFXenum specifying the input to be used.  Valid values are:
	*          - @b IFX_INCOMING: Use the output of the previous texture unit.  This is
	*            the same is IFX_DIFFUSE for IFX_TEXUNIT0.
	*          - @b IFX_DIFFUSE: Use the lit diffuse material alpha.
	*          - @b IFX_CONSTANT: Use the alpha value specified with IFXRenderContext::SetConstantColor().
	*
	*          All of these options can be combined with @b IFX_INVERSE.
	*          For example (IFX_DIFFUSE | IFX_INVERSE) will use the result of
	*          (White - DiffuseAlpha) as the actual input.
	*
	*          All of these options are implicitly combined with IFX_ALPHA.  It
	*          is not possible to use color values as input into the Alpha combiner.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetAlphaInput1(IFXenum eInput);

	/**
	*  Sets the Alpha interpolating source.  This is only used when the AlphaBlendFunc is set to
	*  IFX_INTERPOLATE.  The interpolating value is the alpha channel value from the specified
	*  source.
	*
	*  @param  eInput  Input IFXenum specifying the source of the interpolating value.
	*          Valid options are:
	*          - @b IFX_INCOMING:  Use the output alpha value of the previous
	*            texture unit.  This is the same as IFX_DIFFUSE for IFX_TEXUNIT0.
	*          - @b IFX_DIFFUSE: Use the lit diffuse material alpha value.
	*          - @b IFX_CONSTANT: Use the value specified with IFXRenderContext::SetConstantColor().
	*          - @b IFX_TEXTURE: Use the alpha value from the texture bound to
	*            this texture unit.
	*
	*          All of these options are implicitly combined with IFX_ALPHA.  It
	*          is not possible to use color values as the interpolation value.
	*
	*          It is not possible to use IFX_INVERSE to invert the interpolating
	*          value.  If IFX_INVERSE is included, it will be ignored.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetAlphaInterpolatorSource(IFXenum eInput);

	/**
	*  Specifies the blending function between AlphaInput0 and AlphaInpu1.
	*
	*  Note: Not all video cards support all blending functions.  In this case, the closest
	*  approximate match will be used.
	*
	*
	*
	*  @param eFunc  Input IFXenum specifying which blending function to use.  Color or alpha
	*          values represented below are in the range (0,1).  Valid options are:
	*          - @b  IFX_SELECT_ARG0: out = Input0
	*          - @b  IFX_MODULATE: out = Input0 * Input1
	*          - @b  IFX_MODULATE2X: out = Input0 * Input1 * 2
	*          - @b  IFX_MODULATE4X: out = Input0 * Input1 * 4
	*          - @b  IFX_ADD: out = Input0 + Input1
	*          - @b  IFX_ADDSIGNED: out = Input0 + Input1 - 0.5
	*          - @b  IFX_ADDSIGNED2X: out = (Input0 + Input1 - 0.5) * 2
	*          - @b  IFX_INTERPOLATE: out = Input0*Diffuse.Alpha + (Input1*(1-Interpolator.Alpha))
	*          - @b  IFX_DOTPRODUCT3: out = Input0.R*Input1.R + Input0.B*Input1.B + Input0.G*Input1.G.
	*            When this is specified, the RGBBlendFunc must be @b IFX_DOTPRODUCT3.  If
	*            IFX_DOTPRODUCT3 is not supported (or the RGBBlendFunc is not IFX_DOTPRODUCT3),
	*            then IFX_SELECT_ARG0 is used instead.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetAlphaBlendFunc(IFXenum eFunc);

	/**
	*
	*/
	IFXRESULT IFXAPI SetConstantColor(const IFXVector4& vColor);

	/**
	*  Specifies the texture coordinate generation function for this texture unit.
	*
	*  @param eGenMode Input IFXenum specifying the texture coordinate generation function.
	*          Valid values are:
	*          - @b IFX_NONE: The texture transform is applied to incoming texture coordinates.
	*          - @b IFX_TEXGEN_VIEWPOSITION: The vertex position transformed into eyespace
	*            is used as the texture coordinate, then the texture transform is applied.
	*          - @b IFX_TEXGEN_VIEWNORMAL: The vertex normal transformed into eyespace is used
	*            as the texture coordinate, then the texture transform is applied.
	*          - @b IFX_TEXGEN_WORLDPOSITION: The vertex position transformed into worldspace
	*            is used as the texture coordinate, then the texture transform is applied.
	*          - @b IFX_TEXGEN_WORLDNORMAL: The vertex normal transformed into worldspace is used
	*            as the texture coordinate, then the texture transform is applied.
	*          - @b IFX_TEXGEN_REFLECTION_CUBE: The reflection vector in eye space is used as
	*            the texture coordinate, then the texture transform is applied.  This is
	*            meant to be used as input into a cube map.
	*          - @b IFX_TEXGEN_REFLECTION_SPHERE: The eye space position and normal is used
	*            to calculate the u,v coordinates into a 2D spherical reflection map, then
	*            the texture transform is applied.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The value specified is invalid.
	*/
	IFXRESULT IFXAPI SetTexCoordGen(IFXenum eGenMode);

	/**
	*  Sets the texture to use for this texture unit.  Since the texture validity is not known
	*  until render time, this method does not check to see if the texture ID is valid.
	*
	*  @param uTexId Input U32 specifying the registered texture ID for the texture to use.
	*          Specifying 0 will not associate a texture with this texture unit.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*/
	IFXRESULT IFXAPI SetTextureId(U32 uTexId);

	/**
	*  Sets the texture repeat state for the U texture coordinate.
	*
	*  @param bEnabled Input BOOL specifying whether texture repeating in the U texture coordinate
	*          is enabled.
	*
	*  @return An IFXRESULT error code.  Always @b IFX_OK.
	*/
	IFXRESULT IFXAPI SetTexRepeatU(BOOL bEnabled);

	/**
	*  Sets the texture repeat state for the V texture coordinate.
	*
	*  @param bEnabled Input BOOL specifying whether texture repeating in the V texture coordinate
	*          is enabled.
	*
	*  @return An IFXRESULT error code.  Always @b IFX_OK.
	*/
	IFXRESULT IFXAPI SetTexRepeatV(BOOL bEnabled);

	/**
	*  Sets the texture repeat state for the W texture coordinate.
	*
	*  @param bEnabled Input BOOL specifying whether texture repeating in the W texture coordinate
	*          is enabled.
	*
	*  @return An IFXRESULT error code.  Always @b IFX_OK.
	*/
	IFXRESULT IFXAPI SetTexRepeatW(BOOL bEnabled);

	/**
	*  Specifies the texture transform matrix applied to the texture coordinates for this stage
	*  before rendering.
	*
	*  @param mTexture Input IFXMatrix4x4 specifying the texture transform matrix.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*/
	IFXRESULT IFXAPI SetTextureTransform(const IFXMatrix4x4& mTexture);

	/**
	*  Sets the texture coordinate set to use for applying textures.  If the mesh does
	*  not contain texture coordinates for this index, then the first texture coordinate
	*  set (index 0) is used.  If -1 is specified, then the texture coordinate set used
	*  is dependent on the tex unit number of this texture unit.  For example, IFX_TEXUNIT7
	*  would try to use the 8th set of texture coordinates (index 7).
	*
	*  @param  iTexCoordSet  Input index of the texture coordinate set to use for
	*              this texture unit.
	*
	*  @return An IFXRESULT error code.
	*
	*  @retval IFX_OK  No error.
	*  @retval IFX_E_INVALID_RANGE The @a iTexCoordSet parameters was not in the range
	*                [-1, 7].
	*/
	IFXRESULT IFXAPI SetTextureCoordinateSet(I32 iTexCoordSet);

	/**
	*  Gets the RGBInput0 source.
	*  @return The source for RGBInput0
	*  @see SetRGBInput0()
	*/
	IFXenum IFXAPI GetRGBInput0() const;

	/**
	*  Gets the RGB blending function.
	*  @return The RGB blending funciton.
	*  @see SetRGBBlendFunc()
	*/
	IFXenum IFXAPI GetRGBInput1() const;

	/**
	*  Gets the RGB Interpolating source.
	*  @return the RGB interpolating source.
	*  @see SetRGBInterpolatorSource()
	*/
	IFXenum IFXAPI GetRGBInterpolatorSource() const;

	/**
	*  Gets the RGB blending function.
	*  @return The RGB blending function.
	*  @see SetRGBBlendFunc()
	*/
	IFXenum IFXAPI GetRGBBlendFunc() const;

	/**
	*  Gets the AlphaInput0 source.
	*  @return The source for AlphaInput0.
	*  @see SetAlphaInput0()
	*/
	IFXenum IFXAPI GetAlphaInput0() const;

	/**
	*  Gets the AlphaInput1 source.
	*  @return The source for AlphaInput1.
	*  @see SetAlphaInput1()
	*/
	IFXenum IFXAPI GetAlphaInput1() const;

	/**
	*  Gets the Alpha Interpolating source.
	*  @return the Alpha interpolating source.
	*  @see SetAlphaInterpolatorSource()
	*/
	IFXenum IFXAPI GetAlphaInterpolatorSource() const;

	/**
	*  Gets the Alpha blending function.
	*  @return The Alpha blending function.
	*  @see SetAlphaBlendFunc()
	*/
	IFXenum IFXAPI GetAlphaBlendFunc() const;

	/**
	*  Gets the constant color.
	*  @return The constant color.
	*  @see SetConstantColor()
	*/
	const IFXVector4& IFXAPI GetConstantColor() const;

	/**
	*  Gets the texture coordinate generation mode.
	*  @return The texture coordinate generation mode.
	*  @see SetTexCoordGen()
	*/
	IFXenum IFXAPI GetTexCoordGen() const;

	/**
	*  Gets the texture ID of the texture bound to this texture unit.
	*  @return The texture ID of the set texture.
	*  @see SetTextureId()
	*/
	U32 IFXAPI GetTextureId() const;

	/**
	*  Gets the texture repeat state in the U tex coord dimension.
	*
	*  @param bEnabled Output BOOL to receive the texture repeat state.
	*  @see SetTexRepeatU()
	*/
	BOOL IFXAPI GetTexRepeatU() const;

	/**
	*  Gets the texture repeat state in the V tex coord dimension.
	*
	*  @param bEnabled Output BOOL to receive the texture repeat state.
	*  @see SetTexRepeatV()
	*/
	BOOL IFXAPI GetTexRepeatV() const;

	/**
	*  Gets the texture repeat state in the W tex coord dimension.
	*
	*  @param bEnabled Output BOOL to receive the texture repeat state.
	*  @see SetTexRepeatW()
	*/
	BOOL IFXAPI GetTexRepeatW() const;

	/**
	*  Get the texture transform for this texture unit.
	*  @return The texture transform matrix.
	*  @see SetTextureTransform()
	*/
	const IFXMatrix4x4& IFXAPI GetTextureTransform() const;

	/**
	*  Gets the currently set texture coordinate set.
	*  @return The texture coordinate set index (or -1).
	*  @see SetTextureCoordinateSet()
	*/
	I32 IFXAPI GetTextureCoordinateSet() const;

	/**
	*  Initializes all state settings to default values.  Default values are:
	*
	*  - <b>RGBInput0</b>: IFX_TEXTURE
	*  - <b>RGBInput1</b>: IFX_INCOMING
	*  - <b>RGBInterpolator</b>: IFX_CONSTANT
	*  - <b>RGB Blend Func</b>: IFX_MODULATE
	*  - <b>Alpha Input0</b>: IFX_TEXTURE
	*  - <b>Alpha Input1</b>: IFX_INCOMING
	*  - <b>Alpha Interpolator</b>: IFX_CONSTANT
	*  - <b>Alpha Blend Func</b>: IFX_MODULATE
	*  - <b>Tex Coord Generation</b>: IFX_NONE
	*  - <b>Texure ID</b>: 1 (Default white texture)
	*  - <b>Texture Transform</b>: Identity matrix
	*  - <b>Texture Coordinate Set</b>: -1
	*/
	void IFXAPI InitData();

protected:
	IFXenum m_eRGBInput0;
	IFXenum m_eRGBInput1;
	IFXenum m_eRGBInterpSrc;
	IFXenum m_eRGBBlendFunc;
	IFXenum m_eAInput0;
	IFXenum m_eAInput1;
	IFXenum m_eAInterpSrc;
	IFXenum m_eABlendFunc;
	BOOL  m_bTexRepeatU;
	BOOL  m_bTexRepeatV;
	BOOL  m_bTexRepeatW;

	IFXVector4 m_vConstantColor;

	IFXenum m_eTexCoordGen;

	U32   m_uTexId;

	I32   m_iTexCoordSet;

	IFXMatrix4x4  m_mTexMatrix;
};

//=======================================
// IFXRenderTexUnit Implementation
//=======================================
IFXINLINE IFXRenderTexUnit::IFXRenderTexUnit()
{
	InitData();
}

IFXINLINE IFXRenderTexUnit::~IFXRenderTexUnit()
{
	// EMPTY
}

IFXINLINE void IFXRenderTexUnit::InitData()
{
	m_eRGBInput0 = IFX_TEXTURE;
	m_eRGBInput1 = IFX_INCOMING;
	m_eRGBInterpSrc = IFX_CONSTANT;
	m_eRGBBlendFunc = IFX_MODULATE;
	m_eAInput0 = IFX_TEXTURE;
	m_eAInput1 = IFX_INCOMING;
	m_eAInterpSrc = IFX_CONSTANT;
	m_eABlendFunc = IFX_MODULATE;
	m_eTexCoordGen = IFX_NONE;
	m_bTexRepeatU = TRUE;
	m_bTexRepeatV = TRUE;
	m_bTexRepeatW = FALSE;

	m_vConstantColor.Set(1,1,1,1);

	m_uTexId = 1;

	m_iTexCoordSet = -1;

	m_mTexMatrix.Reset();
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetRGBInput0(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	if(m_eRGBInput0 != eInput)
	{
		IFXenum eModifiers = eInput & (IFX_INVERSE | IFX_ALPHA);
		eInput = eInput & ~(IFX_INVERSE | IFX_ALPHA);
		switch (eInput)
		{
		case IFX_INCOMING:
		case IFX_DIFFUSE:
		case IFX_TEXTURE:
		case IFX_CONSTANT:
			m_eRGBInput0 = eInput | eModifiers;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetRGBInput1(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	if(m_eRGBInput1 != eInput)
	{
		IFXenum eModifiers = eInput & (IFX_INVERSE | IFX_ALPHA);
		eInput = eInput & ~(IFX_INVERSE | IFX_ALPHA);
		switch (eInput)
		{
		case IFX_INCOMING:
		case IFX_DIFFUSE:
		case IFX_CONSTANT:
			m_eRGBInput1 = eInput | eModifiers;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetRGBInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	if(m_eRGBInterpSrc != eInput)
	{
		eInput &= ~(IFX_INVERSE | IFX_ALPHA);
		switch(eInput)
		{
		case IFX_INCOMING:
		case IFX_DIFFUSE:
		case IFX_CONSTANT:
		case IFX_TEXTURE:
			m_eRGBInterpSrc = eInput;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetRGBBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	if(m_eRGBBlendFunc != eFunc)
	{
		switch(eFunc)
		{
		case IFX_SELECT_ARG0:
		case IFX_MODULATE:
		case IFX_MODULATE2X:
		case IFX_MODULATE4X:
		case IFX_ADD:
		case IFX_ADDSIGNED:
		case IFX_ADDSIGNED2X:
		case IFX_INTERPOLATE:
		case IFX_DOTPRODUCT3:
			m_eRGBBlendFunc = eFunc;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetAlphaInput0(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	if(m_eAInput0 != eInput)
	{
		IFXenum eModifiers = eInput & IFX_INVERSE;
		eInput = eInput & ~(IFX_INVERSE | IFX_ALPHA);
		switch (eInput)
		{
		case IFX_INCOMING:
		case IFX_DIFFUSE:
		case IFX_TEXTURE:
		case IFX_CONSTANT:
			m_eAInput0 = eInput | eModifiers;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetAlphaInput1(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	if(m_eAInput1 != eInput)
	{
		IFXenum eModifiers = eInput & IFX_INVERSE;
		eInput = eInput & ~(IFX_INVERSE | IFX_ALPHA);
		switch (eInput)
		{
		case IFX_INCOMING:
		case IFX_DIFFUSE:
		case IFX_TEXTURE:
		case IFX_CONSTANT:
			m_eAInput1 = eInput | eModifiers;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetAlphaInterpolatorSource(IFXenum eInput)
{
	IFXRESULT rc = IFX_OK;

	if(m_eAInterpSrc != eInput)
	{
		eInput &= ~(IFX_INVERSE | IFX_ALPHA);
		switch(eInput)
		{
		case IFX_INCOMING:
		case IFX_DIFFUSE:
		case IFX_CONSTANT:
		case IFX_TEXTURE:
			m_eAInterpSrc = eInput;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetAlphaBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;

	if(m_eABlendFunc != eFunc)
	{
		switch(eFunc)
		{
		case IFX_SELECT_ARG0:
		case IFX_MODULATE:
		case IFX_MODULATE2X:
		case IFX_MODULATE4X:
		case IFX_ADD:
		case IFX_ADDSIGNED:
		case IFX_ADDSIGNED2X:
		case IFX_INTERPOLATE:
		case IFX_DOTPRODUCT3:
			m_eABlendFunc = eFunc;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetConstantColor(const IFXVector4& vColor)
{
	m_vConstantColor = vColor;

	return IFX_OK;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTexCoordGen(IFXenum eGenMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_eTexCoordGen != eGenMode)
	{
		switch(eGenMode)
		{
		case IFX_NONE:
		case IFX_TEXGEN_REFLECTION_SPHERE:
		case IFX_TEXGEN_VIEWPOSITION:
		case IFX_TEXGEN_VIEWNORMAL:
		case IFX_TEXGEN_VIEWREFLECTION:
		case IFX_TEXGEN_WORLDPOSITION:
		case IFX_TEXGEN_WORLDNORMAL:
		case IFX_TEXGEN_WORLDREFLECTION:
		case IFX_TEXGEN_LOCALPOSITION:
		case IFX_TEXGEN_LOCALNORMAL:
		case IFX_TEXGEN_LOCALREFLECTION:
			m_eTexCoordGen = eGenMode;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTextureId(U32 uTexId)
{
	IFXRESULT rc = IFX_OK;

	m_uTexId = uTexId;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTextureTransform(const IFXMatrix4x4& mTexture)
{
	IFXRESULT rc = IFX_OK;

	m_mTexMatrix = mTexture;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTexRepeatU(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatU = bEnabled;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTexRepeatV(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatV = bEnabled;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTexRepeatW(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;

	m_bTexRepeatW = bEnabled;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderTexUnit::SetTextureCoordinateSet(I32 iTexCoordSet)
{
	IFXRESULT rc = IFX_OK;

	if(iTexCoordSet < -1 || iTexCoordSet >= IFX_MAX_TEXUNITS)
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if(IFXSUCCESS(rc))
	{
		if(m_iTexCoordSet != iTexCoordSet)
		{
			m_iTexCoordSet = iTexCoordSet;
		}
	}

	return rc;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetRGBInput0() const
{
	return m_eRGBInput0;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetRGBInput1() const
{
	return m_eRGBInput1;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetRGBInterpolatorSource() const
{
	return m_eRGBInterpSrc;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetRGBBlendFunc() const
{
	return m_eRGBBlendFunc;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetAlphaInput0() const
{
	return m_eAInput0;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetAlphaInput1() const
{
	return m_eAInput1;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetAlphaInterpolatorSource() const
{
	return m_eAInterpSrc;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetAlphaBlendFunc() const
{
	return m_eABlendFunc;
}

IFXINLINE const IFXVector4& IFXRenderTexUnit::GetConstantColor() const
{
	return m_vConstantColor;
}

IFXINLINE IFXenum IFXRenderTexUnit::GetTexCoordGen() const
{
	return m_eTexCoordGen;
}

IFXINLINE U32 IFXRenderTexUnit::GetTextureId() const
{
	return m_uTexId;
}

IFXINLINE BOOL IFXRenderTexUnit::GetTexRepeatU() const
{
	return m_bTexRepeatU;
}

IFXINLINE BOOL IFXRenderTexUnit::GetTexRepeatV() const
{
	return m_bTexRepeatV;
}

IFXINLINE BOOL IFXRenderTexUnit::GetTexRepeatW() const
{
	return m_bTexRepeatW;
}

IFXINLINE const IFXMatrix4x4& IFXRenderTexUnit::GetTextureTransform() const
{
	return m_mTexMatrix;
}

IFXINLINE I32 IFXRenderTexUnit::GetTextureCoordinateSet() const
{
	return m_iTexCoordSet;
}


#endif
