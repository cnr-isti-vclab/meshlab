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
// IFXRenderLight.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRenderContext.h
#ifndef IFX_RENDER_LIGHT_H
#define IFX_RENDER_LIGHT_H

#include <float.h>
#include "IFXMatrix4x4.h"

//==============================
// IFXRenderLight
//==============================
/**
 *	Param struct for storing various light parameters.  This is used to 
 *	pass light parameters into an IFXRender object.
 */
class IFXRenderLight
{
public:
	//=============================================
	// IFXRenderLight Constructor/Destructor
	//=============================================
	IFXRenderLight();
	virtual ~IFXRenderLight();

	//==============================
	// IFXRenderLight methods
	//==============================
	/**
	 *	Sets the type of light.
	 *
	 *	@param	eType	Input IFXenum value specifying the type of light.
	 *					Possible values are:
	 *					- @b IFX_AMBIENT: Light is provided uniformly to all areas.
	 *						This only provides ambient illumination.
	 *					- @b IFX_POINT: Light comes from a point in space, 
	 *						irradiates in all directions.
	 *					- @b IFX_SPOT: Light comes from a point in space, 
	 *						irradiation clamped to conical area.
	 *					- @b IFX_DIRECTIONAL: Light comes from a specified 
	 *						direction in world space to all vertices.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified type is invalid.
	 */
	IFXRESULT IFXAPI SetType(IFXenum eType);

	/**
	 *	Sets the ambient color of the light.
	 *
	 *	@param vColor	4-component color vector.  Each (R,G,B,A) channel ranges from 0.0 - 1.0.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetAmbient(const IFXVector4& vColor);

	/**
	 *	Sets the diffuse color of the light.
	 *
	 *	@param vColor	4-component color vector.  Each (R,G,B,A) channel ranges from 0.0 - 1.0.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetDiffuse(const IFXVector4& vColor);

	/**
	 *	Sets the specular color of the light.
	 *
	 *	@param vColor	4-component color vector.  Each (R,G,B,A) channel ranges from 0.0 - 1.0.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetSpecular(const IFXVector4& vColor);
	
	/**
	 *	Sets the light intensity factor.  This is a scalar floating point value this is multiplied
	 *	by the ambient, diffuse, and specular colors before they are sent to the rendering system.
	 *
	 *	This value can be used to approximate super-saturating lights and negative lights.
	 *
	 *	@param	fIntensity	Input F32 specifying the light intensity.  Any floating point value
	 *						is valid.
	 *
	 *	@return	An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetIntensity(F32 fIntensity);

	/**
	 *	Sets the constant, linear, and quadratic attenuation factors of the light.  Attenuation
	 *	from the light to the vertex is defined by the formula: 1 / ( C + Ld + Qd^2 ), where:
	 *		- @b C - constant attenuation
	 *		- @b L - linear attenuation
	 *		- @b Q - quadratic attenuation
	 *		- @b d - distance from vertex to light
	 *	
	 *	You must not specify (0,0,0).
	 *
	 *	@param vAtten Input attenuation factor: vector(C,L,Q).
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified attenuation is invalid (all 0's).
	 */
	IFXRESULT IFXAPI SetAttenuation(const IFXVector3& vAtten);

	/**
	 *	Sets the light's world transform matrix.
	 *
	 *	@param mMatrix	Input world matrix of the light.  For directional lights, this only
	 *					specifies direction.  For point lights, only position.  For spot
	 *					lights, both direction and position.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetWorldMatrix(const IFXMatrix4x4& mMatrix);

	/**
	 *	Enables or disables spot light radial attenuation.
	 *
	 *	@param bDecay	Boolean value specifying whether or not spotlights should decay radially.
	 *					If @b TRUE then the spot light will decay from normal intensity to 0 
	 *					over the range InnerAngle - OuterAngle.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetSpotDecay(BOOL bDecay);
	
	/**
	 *	Sets the angle at which spot decay begins.
	 *
	 *	@param fAngle	Floating point angle in degrees that specifies the width of the full
	 *					intensity cone as measured from the spotlight center line.  Valid range is 
	 *					from 0 - 90.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified @a fAngle is not between 0 - 90.
	 */
	IFXRESULT IFXAPI SetSpotInnerAngle(F32 fAngle);
	
	/**
	 *	Sets the cut off angle of the spot light.
	 *
	 *	@param fAngle	Floating point angle in degrees that specifies the width of the cone of 
	 *					light provided by the spot light as measured from the spot light center
	 *					line.  Essentially, this is the cut off angle.  Valid range is from 0 - 90.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified @a fAngle is invalid.
	 */
	IFXRESULT IFXAPI SetSpotOuterAngle(F32 fAngle);
	
	/**
	 *	Sets the range of influence of the light.  The light range is valid for IFX_SPOT and IFX_POINT
	 *	lights.  The maximum value is sqrt(FLT_MAX).  If a light is used to light geometry further 
	 *	away than fRange, the light provides no contribution to the lighting process.
	 *
	 *	@param fRange	Floating point value specifying world space units for the light range.  
	 *					Must a positive value less than sqrt(FLT_MAX).
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified @ fRange is invalid.
	 */
	IFXRESULT IFXAPI SetRange(F32 fRange);
	
	
	/**
	 *	Gets the light type.
	 *
	 *	@return IFXenum representing the light type.
	 *
	 *	@retval	IFX_AMBIENT	Light is provided uniformly to all areas.
	 *						This only provides ambient illumination.
	 *	@retval	IFX_POINT	Light comes from a point in space, 
	 *						irradiates in all directions.
	 *	@retval	IFX_SPOT	Light comes from a point in space, 
	 *					irradiation clamped to conical area.
	 *	@retval	IFX_DIRECTIONAL	Light comes from a specified 
	 *							direction in world space to all vertices.
	 *	@see SetType()
	 */
	IFXenum IFXAPI GetType() const;
	
	/**
	 *	Gets the light ambient color.
	 *
	 *	@return An IFXVector4 representing the light ambient color.
	 *	@see SetAmbient()
	 */
	const IFXVector4& IFXAPI GetAmbient() const;
	
	/**
	 *	Gets the light diffuse color.
	 *
	 *	@return An IFXVector4 representing the light diffuse color.
	 *	@see SetDiffuse()
	 */
	const IFXVector4& IFXAPI GetDiffuse() const;
	
	/**
	 *	Gets the light specular color.
	 *
	 *	@return An IFXVector4 representing the light specular color.
	 *	@see SetSpecular()
	 */
	const IFXVector4& IFXAPI GetSpecular() const;
	
	/**
	 *	Gets the light intensity.
	 *
	 *	@return An F32 representing the light intensity factor.
	 *	@see SetIntensity()
	 */
	F32	IFXAPI GetIntensity() const;

	/**
	 *	Gets the light attenuation.
	 *
	 *	@return An IFXVector3 representing the light attenuation coefficients.
	 *	@see SetAttenuation()
	 */
	const IFXVector3& IFXAPI GetAttenuation() const;
	
	/**
	 *	Gets the light world transform matrix.
	 *
	 *	@return An IFXMatrix4x4 representing the light world matrix.
	 *	@see SetWorldMatrix()
	 */
	const IFXMatrix4x4& IFXAPI GetWorldMatrix() const;
	
	/**
	 *	Gets whether or not spot decay is enabled.
	 *
	 *	@return @b TRUE if spot decay is enabled, @b FALSE otherwise.
	 *	@see SetSpotDecay()
	 */
	BOOL IFXAPI GetSpotDecay() const;
	
	/**
	 *	Gets the light's spot inner angle.
	 *
	 *	@return The spot light inner angle.
	 *	@see SetSpotInnerAngle()
	 */
	F32 IFXAPI GetSpotInnerAngle() const;
	
	/**
	 *	Gets the light's spot outer angle.
	 *
	 *	@return The spot light outer (cutoff) angle.
	 *	@see SetSpotOuterAngle()
	 */
	F32 IFXAPI GetSpotOuterAngle() const;
	
	/**
	 *	Gets the light range.
	 *
	 *	@return The light's the maximum range of influence.
	 *	@see SetRange()
	 */
	F32 IFXAPI GetRange() const;
	
	/**
	 *	Initializes all data members to default values.  
	 *
	 *	Default values for IFXRenderLight are:
	 *		- @b Type: IFX_DIRECTIONAL
	 *		- @b Ambient: IFXVector4(0,0,0,0)
	 *		- @b Diffuse: IFXVector4(1,1,1,1)
	 *		- @b Specular: IFXVector4(1,1,1,1)
	 *		- @b Intensity: 1.0
	 *		- @b Attenuation: IFXVector3(1,1,1)
	 *		- @b World @b Matrix: Identity matrix
	 *		- @b Spot @b Decay: TRUE
	 *		- @b Spot @b Inner @b Angle: 0.0
	 *		- @b Spot @b Outer @b Angle: 45.0
	 *		- @b Light @b Range: sqrt(FLT_MAX)
	 */
	void IFXAPI InitData();

protected:
	IFXenum		m_eType;			///< Light Type
	IFXVector4	m_vAmbient;			///< Ambient Color
	IFXVector4	m_vDiffuse;			///< Diffuse Color
	IFXVector4	m_vSpecular;		///< Specular Color
	F32			m_fIntensity;		///< Light Intensity
	IFXVector3	m_vAttenuation;		///< Vector(const, linear, quadratic) coef.
	IFXMatrix4x4 m_mWorldMatrix;	///< Light to World transform
	BOOL		m_bSpotDecay;		///< Spot decay enabled boolean
	F32			m_fSpotInnerAngle;	///< Spot inner angle - in degrees, from center
	F32			m_fSpotOuterAngle;	///< Spot outer angle - in degrees, from center
	F32			m_fLightRange;		///< Light range - Max is sqrt(FLT_MAX)
};
//=======================================
// IFXRenderLight Implementation
//=======================================
IFXINLINE IFXRenderLight::IFXRenderLight()
{
	InitData();
}

IFXINLINE IFXRenderLight::~IFXRenderLight()
{
	// EMPTY
}

IFXINLINE void IFXRenderLight::InitData()
{
	m_eType = IFX_DIRECTIONAL;
	m_vAmbient.Set(0,0,0,0);
	m_vDiffuse.Set(1,1,1,1);
	m_vSpecular.Set(1,1,1,1);
	m_fIntensity = 1.0f;
	m_vAttenuation.Set(1,0,0);
	m_mWorldMatrix.Reset();
	m_bSpotDecay = TRUE;
	m_fSpotInnerAngle = 0;
	m_fSpotOuterAngle = 90;
	m_fLightRange = sqrtf(FLT_MAX);
}

IFXINLINE IFXRESULT IFXRenderLight::SetType(IFXenum eType)
{
	IFXRESULT rc = IFX_OK;

	if(m_eType != eType)
	{
		switch(eType)
		{
		case IFX_AMBIENT:
		case IFX_POINT:
		case IFX_SPOT:
		case IFX_DIRECTIONAL:
			m_eType = eType;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetAmbient(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vAmbient = vColor;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetDiffuse(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vDiffuse = vColor;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetSpecular(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vSpecular = vColor;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetIntensity(F32 fIntensity)
{
	m_fIntensity = fIntensity;

	return IFX_OK;
}

IFXINLINE IFXRESULT IFXRenderLight::SetAttenuation(const IFXVector3& vAtten)
{
	IFXRESULT rc = IFX_OK;

	if(! (m_vAttenuation == vAtten) )
	{
		if(vAtten == IFXVector3(0,0,0))
		{
			rc = IFX_E_INVALID_RANGE;
		}
		else
		{
			m_vAttenuation = vAtten;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetWorldMatrix(const IFXMatrix4x4& mMatrix)
{
	IFXRESULT rc = IFX_OK;

	m_mWorldMatrix = mMatrix;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetSpotDecay(BOOL bDecay)
{
	IFXRESULT rc = IFX_OK;

	m_bSpotDecay = bDecay;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetSpotInnerAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotInnerAngle = fAngle;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetSpotOuterAngle(F32 fAngle)
{
	IFXRESULT rc = IFX_OK;

	m_fSpotOuterAngle = fAngle;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderLight::SetRange(F32 fRange)
{
	IFXRESULT rc = IFX_OK;

	if(m_fLightRange != fRange)
	{
		if(fRange < sqrtf(FLT_MAX))
		{
			m_fLightRange = fRange;
		}
		else
		{
			fRange = sqrtf(FLT_MAX);
		}
	}

	return rc;
}

IFXINLINE IFXenum IFXRenderLight::GetType() const
{
	return m_eType;
}

IFXINLINE const IFXVector4& IFXRenderLight::GetAmbient() const
{
	return m_vAmbient;
}

IFXINLINE const IFXVector4& IFXRenderLight::GetDiffuse() const
{
	return m_vDiffuse;
}

IFXINLINE const IFXVector4& IFXRenderLight::GetSpecular() const
{
	return m_vSpecular;
}

IFXINLINE F32 IFXRenderLight::GetIntensity() const
{
	return m_fIntensity;
}

IFXINLINE const IFXVector3& IFXRenderLight::GetAttenuation() const
{
	return m_vAttenuation;
}

IFXINLINE const IFXMatrix4x4& IFXRenderLight::GetWorldMatrix() const
{
	return m_mWorldMatrix;
}

IFXINLINE BOOL IFXRenderLight::GetSpotDecay() const
{
	return m_bSpotDecay;
}

IFXINLINE F32 IFXRenderLight::GetSpotInnerAngle() const
{
	return m_fSpotInnerAngle;
}

IFXINLINE F32 IFXRenderLight::GetSpotOuterAngle() const
{
	return m_fSpotOuterAngle;
}

IFXINLINE F32 IFXRenderLight::GetRange() const
{
	return m_fLightRange;
}


#endif
