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

#ifndef __IFXLightResource_H__
#define __IFXLightResource_H__

#include "IFXMarkerX.h"

class IFXRenderLight;

/**
	The interface ID for the IFXLightResource class.
	4517C9F4-B31E-11d3-94B1-00A0C9A0FBAE
*/
IFXDEFINE_GUID(IID_IFXLightResource,
0x4517c9f4, 0xb31e, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

/**
 *	The IFXLightResource object encapsulates a light properties. 
 *	The light can be one of
 *	four types:
 *		- Ambient: The light provides uniform non-directional light to
 *			all objects in the visible scene.
 *		- Point: The light has a specific light source located at the
 *			center of it's local coordinate space.
 *		- Directional: The light affects all object of the scene
 *			uniformly.  Light from this source appears to each object as
 *			coming from the same direction relative to the world.
 *		- Spot: The light acts as a spot light, having a distinct location
 *			in the world, and only affecting a conical area in the scene
 *			relative to that position.
 */
class IFXLightResource : virtual public IFXMarkerX
{
public:
	/**
	 *	Light type enumerant for the four light types.
	 */
	enum LightType
	{
		AMBIENT,		///< Light provides uniform non-directional light to the scene.
		DIRECTIONAL,	///< Light provides uniform directional light to the scene.
		POINT,			///< Light is emitted from a specific point in the scene.
		SPOT			///< Like point light, but constrained to specific directions.
	};

	/**
	 *	Enumerant for various light attributes.
	 */
	enum LightAttributes
	{
		ENABLED   = 1,		///< Whether or not this light is enabled (can be used).
		SPECULAR  = 1<<1,	///< Whether or not this light provides specular highlights.
		SPOTDECAY = 1<<2	///< Whether or not spotlights have smooth or hard edge cutoffs.
	};

	/**
	 *	Enumerant for the various distance attenuation factors.
	 */
	enum LightAttenuationFactors
	{
		CONSTANT,	///< Constant attenuation factor (unaffected by distance)
		LINEAR,		///< Attenuation factor relative to distance.
		QUADRADIC	///< Attenuation factor relative to distance squared.
	};

	/**
	 *	Retrieves the light type for this light.
	 *
	 *	@return	A LightType value.
	 *
	 *	@retval	IFXLightResource::AMBIENT	This light provides uniform
	 *								non-directional light to all
	 *								objects in the scene.
	 *	@retval	IFXLightResource::DIRECTIONAL	This light provides uniform
	 *									directional light to all
	 *									objects in the scene.
	 *	@retval	IFXLightResource::POINT	This light provides light from a specific
	 *							point in space.
	 *	@retval	IFXLightResource::SPOT	This light provides light from a specific
	 *							point in space, but unlike @b POINT lights,
	 *							the light direction is constrained.
	 *
	 *	@return void
	 */
	virtual LightType IFXAPI GetType(                 void           )=0;

	/**
	 *	Sets the light type for this light.
	 *
	 *	@param	u8In	Input LightType value specifying the light type.  This
	 *					can be one of the following values:
	 *						- @b IFXLightResource::AMBIENT
	 *						- @b IFXLightResource::DIRECTIONAL
	 *						- @b IFXLightResource::POINT
	 *						- @b IFXLightResource::SPOT
	 *
	 *	@return void
	 */
	virtual void        IFXAPI SetType(                 LightType u8In )=0;

	/**
	 *	Gets the light attributes for this light.
	 *
	 *	@return A U8 value.  This can be a combination of LightAttributes
	 *			enumerants.
	 */
	virtual U8     IFXAPI     GetAttributes(           void           )=0;

	/**
	 *	Sets the light attributes for this light.
	 *
	 *	@param	uuIn	Input U8 that is composed of a number of
	 *					LightAttributes flags.
	 *
	 *	@return void
	 */
	virtual void        IFXAPI SetAttributes(           U8        uuIn )=0;

	/**
	 *	Gets the color of this light.
	 *
	 *	@return An IFXVector4 that contains the RGBA values of the
	 *			light color, each specified in the range 0.0 - 1.0.
	 */
	virtual const IFXVector4&  IFXAPI GetColor(         void           )=0;

	/**
	 *	Sets the color of this light.
	 *
	 *	@param	IFXVector4	Input IFXVector4 specifying the color
	 *						of the light.  Each channel of the
	 *						light color is clamped to the range
	 *						0.0 - 1.0.
	 *
	 *	@return void
	 */
	virtual void        IFXAPI SetColor(                IFXVector4 vIn )=0;

	/**
	 *	Gets the spot angle for this light.  The spot angle is the
	 *	angle of the cone that emanates from the light position
	 *	and defines what portions of the scene is affected by this
	 *	light (assuming it is of type @b IFXLightResource::SPOT).
	 *
	 *	@return An F32 value representing the spot angle for this light.
	 */
	virtual F32         IFXAPI GetSpotAngle(            void           )=0;

	/**
	 *	Sets the spot angle for this light.  The spot angle is the
	 *	angle of the cone that emanates from the light position
	 *	and defines what portions of the scene is affected by this
	 *	light (assuming it is of type @b IFXLightResource::SPOT).
	 *
	 *	@param	fIn	Input F32 specifying the spot angle.  This should
	 *				be the full spot angle (from one side of the cone
	 *				to the other).  The range is 0.0 - 180.0, non-inclusive.
	 *
	 *	@return void
	 */
	virtual void        IFXAPI SetSpotAngle(            F32       fIn  )=0;

	/**
	 *	Retrieves the attenuation vector.  This vector contains the
	 *	Constant, Linear, and Quadratic attenuation factors.  Lights
	 *	that are of type IFXLightResource::POINT or IFXLightResource::SPOT, will
	 *	light objects based on distance from the object's vertices to
	 *	the light position.  The formula for this attenuation is:
	 *
	 *			1/(C + Ld + Q(d*d))
	 *
	 *	where:
	 *		- @b C: Constant attenuation factor
	 *		- @b L: Linear attenuation factor
	 *		- @b Q: Quadratic attenuation factor
	 *
	 *	The result of this calculation determines the percentage of
	 *	influence that this light will have on each vertex.
	 *
	 *	@param	pOut	An F32 pointer to an array of at least 3
	 *					F32 values to be filled in with the
	 *					Constant, Linear, and Quadratic attenuation
	 *					factors.
	 *
	 *	@return An IFXRESULT value.
	 *
	 *	@retval IFX_OK No error.
	 *	@retval IFX_E_INVALID_POINTER	The value of @a pOut is invalid (NULL).
	 */
	virtual IFXRESULT   IFXAPI GetAttenuation(          F32*      pOut )=0;

	/**
	 *	Sets the attenuation vector.  This vector contains the
	 *	Constant, Linear, and Quadratic attenuation factors.  Lights
	 *	that are of type IFXLightResource::POINT or IFXLightResource::SPOT, will
	 *	light objects based on distance from the object's vertices to
	 *	the light position.  The formula for this attenuation is:
	 *
	 *			1/(C + Ld + Q(d*d))
	 *
	 *	where:
	 *		- @b C: Constant attenuation factor
	 *		- @b L: Linear attenuation factor
	 *		- @b Q: Quadratic attenuation factor
	 *
	 *	The result of this calculation determines the percentage of
	 *	influence that this light will have on each vertex.
	 *
	 *	The default value is (C,L,Q): (1,0,0)
	 *
	 *	@param	pIn	An F32 pointer to an array of at least 3
	 *				F32 values that specify what the new
	 *				Constant, Linear, and Quadratic attenuation
	 *				factors are.
	 *
	 *	@return An IFXRESULT value.
	 *
	 *	@retval IFX_OK No error.
	 *	@retval IFX_E_INVALID_POINTER	The value of @a pIn is invalid (NULL).
	 *	@retval	IFX_E_INVALID_RANGE	The attenuation factors specified are all 0.
	 *								This would result in a divide by 0 error
	 *								in the lighting equation.
	 */
	virtual IFXRESULT   IFXAPI SetAttenuation(          F32*      pIn  )=0;

	/**
	 *	Retrieves the intensity of the light.
	 *
	 *	@note see SetIntensity for more information.
	 *
	 *	@return An F32 value representing the intensity of the light.
	 */
	virtual F32         IFXAPI GetIntensity(                           )=0;

	/**
	 *	Sets the intensity of the light.  This intensity is basically
	 *	multiplied into the affect that this light has on a scene.
	 *	It is similar in practice to 1/ConstantAttenuationFactor,
	 *	but works on IFXLightResource::DIRECTIONAL lights in addition to
	 *	point and spot lights (this does not affect lights with type
	 *	IFXLightResource::AMBIENT).  Keep in mind that this value can
	 *	have any value (including negative and 0), resulting in the
	 *	ability to produce some strange effects.
	 *
	 *	@param	fIn	Input F32 value specifying the intensity of this
	 *				light.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK No error.
	 */
	virtual IFXRESULT   IFXAPI SetIntensity(            F32       fIn  )=0;

	/**
	 *	Gets the extent of this light.  The extent is the distance from
	 *	position of the light that it will have an effect in rendering.
	 *	For IFXLightResource::POINT, and IFXLightResource::SPOT, this distance is
	 *	determined by the attenuation and intensity factors.  For
	 *	IFXLightResource::DIRECTIONAL and IFXLightResource::AMBIENT, this value is always
	 *	MAX_FLOAT (approx. 1E+38).
	 *
	 *	@return An F32 value specifying the extent (range) of influence
	 *			for this light.
	 */
	virtual F32         IFXAPI GetExtent(               void           )=0;

	/**
	 *	Retrieves the underlying IFXRenderLight object for this light.
	 *	This is the object that is sent to IFXRenderContext instances
	 *	to pass along light parameters.  It is recommended to use other
	 *	methods to change light parameters.
	 *
	 *	@return A reference to an IFXRenderLight object.
	 */
	virtual IFXRenderLight& GetRenderLight()=0;
};


#endif
