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
	@file	IFXMaterialResource.h

			IFXMaterialResource interface.
*/

#ifndef __IFXMATERIALRESOURCE_H__
#define __IFXMATERIALRESOURCE_H__

#include "IFXUnknown.h"
#include "IFXVector4.h"
#include "IFXMarkerX.h"

/// The interface ID (IFXIID) for the IFXMaterialResource interface.
// {CA165395-B33D-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(IID_IFXMaterialResource,
0xca165395, 0xb33d, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

/**
 *	The IFXMaterialResource interface encapsulates a set of values that
 *  represent surface properties.
 */
class IFXMaterialResource : virtual public IFXMarkerX
{
public:

	/**
	*	The attribute enabled flags.
	*/
	enum Attributes
	{
		AMBIENT      = 1,
		DIFFUSE      = 1<<1,
		SPECULAR     = 1<<2,
		EMISSIVE     = 1<<3,
		REFLECTIVITY = 1<<4,
		OPACITY      = 1<<5,

		ALL          = 0xFFFFFFFF,
		VALID        = 0x0000003F
	};

	/**
	 *	Gets the enabled flags for all attributes.
	 *
	 *	@return	An U32 value that contains the attribute enabled flags.
	 */
	virtual U32 IFXAPI GetAttributes(void)=0;

	/**
	 *	Sets the enabled flags for all attributes.
	 *
	 *	@param	uFlags	A U32 value witch will be used to set the attribute
	 *	                enabled flag word.
	 */
	virtual void IFXAPI SetAttributes(U32 uFlags)=0;

	/**
	 *	Gets the ambient color value.
	 *
	 *	@param	pOutColor	The address of an IFXVector4 to contain the
	 *	                    ambient color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pOutColor
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetAmbient(IFXVector4* pOutColor)=0;

	/**
	 *	Sets the ambient color value..
	 *
	 *	@param	InColor	An IFXVector4 to be used as the ambient color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI SetAmbient(IFXVector4 InColor)=0;

	/**
	 *	Gets the diffuse color value.
	 *
	 *	@param	pOutColor	The address of an IFXVector4 to contain the
	 *	                    diffuse color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pOutColor
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetDiffuse(IFXVector4* pOutColor)=0;

	/**
	 *	Sets the diffuse color value..
	 *
	 *	@param	InColor	An IFXVector4 to be used as the diffuse color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI SetDiffuse(IFXVector4 InColor)=0;

	/**
	 *	Gets the specular color value.
	 *
	 *	@param	pOutColor	The address of an IFXVector4 to contain the
	 *	                    specular color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pOutColor
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetSpecular(IFXVector4* pOutColor)=0;

	/**
	 *	Sets the specular color value..
	 *
	 *	@param	InColor	An IFXVector4 to be used as the specular color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI SetSpecular(IFXVector4 InColor)=0;

	/**
	 *	Gets the emissive color value.
	 *
	 *	@param	pOutColor	The address of an IFXVector4 to contain the
	 *	                    emissive color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pOutColor
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetEmission(IFXVector4* pOutColor)=0;

	/**
	 *	Sets the emissive color value.
	 *
	 *	@param	InColor	An IFXVector4 to be used as the emissive color.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI SetEmission(IFXVector4 InColor)=0;

	/**
	 *	Gets the opacity factor.
	 *
	 *	@param	pfOutOpacityFactor	The address of an F32 to contain the
	 *	                            opacity factor.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pfOutOpacityFactor
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetOpacity(F32* pfOutOpacityFactor)=0;

	/**
	 *	Sets the opacity factor.
	 *
	 *	@param	fInOpacityFactor	An F32 to be used as the opacity factor.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The value specified for @a fInOpacityFactor
	 *								was not between 0 and 1.
	 */
	virtual IFXRESULT IFXAPI SetOpacity(F32 fInOpacityFactor)=0;

	/**
	 *	Gets the transparency enabled boolean value.
	 *
	 *	@param	pbOutTransparent	The address of the BOOL to contain the
	 *	                            transparency switch.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pbOutTransparent
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetTransparent(BOOL* pbOutTransparent)=0;

	/**
	 *	Sets the transparency enabled switch.
	 *
	 *	@param	bInTransparent	An BOOL to be used as the transparency switch.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI SetTransparent(BOOL bInTransparent)=0;

	/**
	 *	Gets the reflectivity factor.
	 *
	 *	@param	pfOutReflectivity	The address of an F32 to contain the
	 *	                            reflectivity factor.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a pfOutReflectivity
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI GetReflectivity(F32* pfOutReflectivity)=0;

	/**
	 *	Sets the reflectivity factor.
	 *
	 *	@param	fInReflectivityFactor	An F32 to be used as the reflectivity factor.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The value specified for @a fInReflectivityFactor
	 *								was not between 0 and 1.
	 */
	virtual IFXRESULT IFXAPI SetReflectivity(F32 fInReflectivityFactor)=0;
};

#endif
