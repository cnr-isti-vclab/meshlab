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
// IFXRenderFog.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_FOG_H
#define IFX_RENDER_FOG_H

//==============================
// IFXRenderFog
//==============================
/**
 *	This is the interface encapsulating an object used to pass values relating to 
 *	fog into an IFXRender object.  All fog related states are contained in this
 *	interface.  To enable fog, use IFXRenderContext::Enable(IFX_FOG).
 *
 *	@see IFXRenderContext::SetFogParams(), IFXRenderContext::Enable(), IFXRenderContext::Disable()
 */
class IFXRenderFog
{
public:
	//=============================================
	// IFXRenderFog Constructor/Destructor
	//=============================================
	IFXRenderFog();
	virtual ~IFXRenderFog();

	//==============================
	// IFXRenderFog methods
	//==============================
	/**
	 *	Sets the method for calculating fog density.
	 *
	 *	@param eMode	Input IFXenum specifying the method for how fog is calculated. 
	 *					Valid options are:
	 *					- @b IFX_FOG_LINEAR: Fog density has linear falloff from the LinearNear
	 *						distance to the Far distance.
	 *					- @b IFX_FOG_EXP: Fog density has exponential falloff from the camera to
	 *						the Far distance.
	 *					- @b IFX_FOG_EXP2: Fog density has exponential squared falloff from the
	 *						camera to the Far distance.
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified mode is invalid.
	 */
	IFXRESULT IFXAPI SetMode(IFXenum eMode);
	
	/**
	 *	Sets the fog color.
	 *
	 *	@param vColor	Input IFXVector4 specifying the fog color.  Color component
	 *					values are clamped within the range (0.0 - 1.0).
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetColor(const IFXVector4& vColor);
	
	/**
	 *	Sets the distance at which linear fog starts to have an effect.
	 *
	 *	@param fNear	Input F32 specifying the linear fog start distance.  This 
	 *					value is clamped to a minimum of 0.
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetLinearNear(F32 fNear);
	
	/**
	 *	Sets the distance at which fog density reaches maximum.  Note, density for fog
	 *	when using the @b IFX_FOG_EXP or @b IFX_FOG_EXP2 modes will never reach full 1.0.  This
	 *	distance specifies where the fog density hits 0.99 in these modes.  When the fog mode
	 *	is IFX_FOG_LINEAR, this distance does specify the full 1.0 fog density distance.
	 *
	 *	@param fFar	Input F32 specifying the fog saturation distance.  This value is clamped 
	 *				to a minumum of 0.
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetFar(F32 fFar);	
	
	/**
	 *	Retrieves the currently set fog mode.
	 *
	 *	@return The currently set fog mode.
	 *
	 *	@retval	IFX_FOG_LINEAR	Fog density has linear falloff from the LinearNear
	 *							distance to the Far distance.
	 *	@retval	IFX_FOG_EXP	Fog density has exponential falloff from the camera to
	 *						the Far distance.
	 *	@retval	IFX_FOG_EXP2	Fog density has exponential squared falloff from the
	 *						camera to the Far distance.
	 *	@see SetMode()
	 */
	IFXenum IFXAPI GetMode() const;
	
	/**
	 *	Retrieves the currently set fog color.
	 *
	 *	@return The fog color.
	 *	@see SetFogColor()
	 */
	const IFXVector4& IFXAPI GetColor() const;
	
	/**
	 *	Retrieves the currently set linear fog near distance.
	 *
	 *	@return The linear fog near distance.
	 *	@see SetFogLinearNear()
	 */
	F32 IFXAPI GetLinearNear() const;

	/**
	 *	Retrieves the currently set fog saturation distance.
	 *
	 *	@return The fog far distance.
	 *	@see SetFogFar()
	 */
	F32 IFXAPI GetFar() const;

	/**
	 *	Initializes data to default values.  Default values are:
	 *
	 *	- <b>Fog Mode</b>: IFX_FOG_EXP
	 *	- <b>Fog Color</b>: IFXVector4(0,0,0,0), Black
	 *	- <b>Fog Linear Near</b>: 0.0
	 *	- <b>Fog Far</b>: 1000.0
	 */
	void IFXAPI InitData();

protected:
	IFXenum		m_eMode;
	IFXVector4	m_vColor;
	F32			m_fLinearNear;
	F32			m_fFar;
};
//=======================================
// IFXRenderFog Implementation
//=======================================
IFXINLINE IFXRenderFog::IFXRenderFog()
{
	InitData();
}

IFXINLINE IFXRenderFog::~IFXRenderFog()
{
	// EMPTY
}

IFXINLINE void IFXRenderFog::InitData()
{
	m_eMode = IFX_FOG_EXP;
	m_vColor.Set(0,0,0,0);
	
	m_fLinearNear = 0.0f;
	m_fFar = 1000.0f;
}

IFXINLINE IFXRESULT IFXRenderFog::SetMode(IFXenum eMode)
{
	IFXRESULT rc = IFX_OK;
	
	if(m_eMode != eMode)
	{
		switch(eMode)
		{
		case IFX_FOG_LINEAR:
		case IFX_FOG_EXP:
		case IFX_FOG_EXP2:
			m_eMode = eMode;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
			break;
		}
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderFog::SetColor(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;
	
	m_vColor = vColor;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderFog::SetLinearNear(F32 fNear)
{
	IFXRESULT rc = IFX_OK;
	
	m_fLinearNear = fNear;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderFog::SetFar(F32 fFar)
{
	IFXRESULT rc = IFX_OK;
	
	m_fFar = fFar;
	
	return rc;
}

IFXINLINE IFXenum IFXRenderFog::GetMode() const
{
	return m_eMode;
}

IFXINLINE const IFXVector4& IFXRenderFog::GetColor() const
{
	return m_vColor;
}

IFXINLINE F32 IFXRenderFog::GetLinearNear() const
{
	return m_fLinearNear;
}

IFXINLINE F32 IFXRenderFog::GetFar() const
{
	return m_fFar;
}


#endif // IFX_RENDER_FOG_H

