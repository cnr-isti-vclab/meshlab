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
// IFXRenderMaterial.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_MATERIAL_H
#define IFX_RENDER_MATERIAL_H

//==============================
// IFXRenderMaterial
//==============================
/**
 *	This interface encapsulates all material settings needed for vertex lighting.
 *	All material settings for vertex lighting must be set through this interface,
 *	and then submitted to an IFXRender object with IFXRenderContext::SetMaterial().
 *
 *	@see IFXRenderContext::SetMaterial()
 */
class IFXRenderMaterial
{
public:
	//=============================================
	// IFXRenderMaterial Constructor/Destructor
	//=============================================
	IFXRenderMaterial();
	virtual ~IFXRenderMaterial();

	//=================================
	// IFXRenderMaterial methods
	//=================================
	/**
	 *	Sets the emissive material color.  The emissive color is the color used as a base
	 *	before any lighting occurs.  This will show through even if there are no lights
	 *	in the scene.
	 *
	 *	If the mesh contains per vertex diffuse, and the UseLights property is disabled,
	 *	the diffuse vertex colors will be used instead of this value.
	 *
	 *	@param vColor	Input IFXVector4 representing the material's emissive color.  
	 *					Color component values are clamped between (0.0 - 1.0).
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetEmissive(const IFXVector4& vColor);

	/**
	 *	Sets the ambient material color.  The ambient color is multiplied by the
	 *	global ambient color and added to the emissive color before any lights
	 *	in the scene are used for lighting.  This is also multiplied by each light's
	 *	ambient light color and added into the final result.
	 *
	 *	@param vColor	Input IFXVector4 representing the material's ambient color.  
	 *					Color component values are clamped between (0.0 - 1.0).
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetAmbient(const IFXVector4& vColor);

	/**
	 *	Sets the diffuse material color.  The diffuse color is multiplied by each 
	 *	light's diffuse light color and then attenuated according to the light
	 *	attenuation property and the direction of the vertex normal.  This attenuated
	 *	value is then added into the final lit color of the vertex.
	 *
	 *	If the mesh that is lit contains per vertex diffuse, this value is ignored.
	 *
	 *	@param vColor	Input IFXVector4 representing the material's diffuse color.  
	 *					Color component values are clamped between (0.0 - 1.0).
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetDiffuse(const IFXVector4& vColor);

	/**
	 *	Sets the specular material color.  The specular color is multiplied by
	 *	each light's specular light color and then attenuated according to the light
	 *	attenuation properties, the direction of the vertex normal, the direction
	 *	of the viewer, and the material shininess value.  This attenuated value
	 *	is then added to the final result color AFTER texture maps are applied.
	 *
	 *	If the mesh being lit contains per vertex specular, this value is ignored.
	 *
	 *	@param vColor	Input IFXVector4 representing the material's specular color.  
	 *					Color component values are clamped between (0.0 - 1.0).
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetSpecular(const IFXVector4& vColor);
	
	/**
	 *	Sets the material's specular reflectivity exponent.  Specular reflections 
	 *	are calculated based on the light position, the vertex normal, and the camera
	 *	position.  This value is then raised to an exponent to control specular light
	 *	falloff.  A Shininess setting of 1.0 corresponds to an exponent of 128.  A 
	 *	shininess of 0 will disable specular lighting.
	 *
	 *	@param fShininess	Input F32 specifying the material shininess. Values are
	 *						clamped between (0,1)
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetShininess(F32 fShininess);
	
	/**
	 *	Specifies whether or not to use a fast specular calculation.  If this is
	 *	@b FALSE, then a vertex to camera vector is calculated for every vertex in
	 *	the model being lit.  If @b TRUE, then the camera direction vector is 
	 *	calculated once per model and used for every vertex.  This has the effect
	 *	of assuming the camera is an infinite distance away from the model.
	 *
	 *	@param bEnabled	Input BOOL specifying to use fast specular calculation.
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetFastSpecular(BOOL bEnabled);
	
	/**
	 *	Specifies the rendering style to be used (either filled, wireframe, or points).
	 *
	 *	@param	eStyle	Input IFXenum specifying the rendering style to be used.  
	 *					Valid options are:
	 *					- @b IFX_FILLED: Renders normal filled polygons.
	 *					- @b IFX_WIREFRAME: Renders only the edges of polygons.
	 *					- @b IFX_POINTS: Renders a point for every vertex.
	 *
	 *	@return An IFXRESULT error code
	 *
	 *	@retval IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The @a eStyle parameter is invalid.
	 */
	IFXRESULT IFXAPI SetRenderStyle(IFXenum eStyle);

	/**
	 *	Specifies the shading model for polygons and lines (either gouraud shading 
	 *	or flat shading).  Flat shading uses a single color for filling the entire
	 *	polygon or line.  Smooth shading evaluates colors at each vertex, and 
	 *	interpolates between these colors across the polygon or line.
	 *
	 *	@param	eShadeModel	Input IFXenum specifying the shading model to be used.
	 *						Valid options are:
	 *						- @b IFX_SMOOTH: Normal gouraud shading.
	 *						- @b IFX_FLAT: Flat shading.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The @a eShadeModel parameter is invalid.
	 */
	IFXRESULT IFXAPI SetShadeModel(IFXenum eShadeModel);

	/**
	 *	Specifies whether or not to use colors that are attached to the vertex.
	 *	This is useful when the mesh contains per vertex color info, but you do
	 *	not want to use them.
	 *
	 *	@param	bUseVertexColor	Input BOOL specifying whether or not to use any 
	 *							per vertex colors associated with the mesh.
	 *
	 *	@return	An IFXRESULT error code.
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetUseVertexColors(BOOL bUserVertexColor);
	
	/**
	 *	Gets the emissive material color.
	 *	@return The emissive material color.
	 *	@see SetEmissive()
	 */
	const IFXVector4& IFXAPI GetEmissive() const;
	
	/**
	 *	Gets the ambient material color.
	 *	@return The ambient material color.
	 *	@see SetAmbient()
	 */
	const IFXVector4& IFXAPI GetAmbient() const;
	
	/**
	 *	Gets the diffuse material color.
	 *	@return The diffuse material color.
	 *	@see SetDiffuse()
	 */
	const IFXVector4& IFXAPI GetDiffuse() const;
	
	/**
	 *	Gets the specular material color.
	 *	@return The specular material color.
	 *	@see SetSpecular()
	 */
	const IFXVector4& IFXAPI GetSpecular() const;
	
	/**
	 *	Gets the currently set shininess value.
	 *	@return The shininess value.
	 *	@see SetShininess()
	 */
	F32		IFXAPI GetShininess() const;
	
	/**
	 *	Gets whether or not a fast specular calculation is being used.
	 *	@return Boolean value.
	 *
	 *	@retval	TRUE	The fast specular calculation is being used
	 *	@retval	FALSE	The	"correct" specular calculation is being used.
	 *	@see SetFastSpecular()
	 */
	BOOL	IFXAPI GetFastSpecular() const;
	
	/**
	 *	Gets the currently set render style.
	 *
	 *	@return	An IFXenum value.
	 *
	 *	@retval	IFX_FILLED	Polygons are rendered filled (normal).
	 *	@retval	IFX_WIREFRAME	Polygons have their edges rendered only.
	 *	@retval	IFX_POINT	Polygons only have their vertices rendered as points.
	 *	@see SetRenderStyle().
	 */
	IFXenum IFXAPI GetRenderStyle() const;

	/**
	 *	Gets the currently set shading model.
	 *
	 *	@return An IFXenum value.
	 *
	 *	@retval	IFX_SMOOTH	Polygons and lines are shaded normally (gouraud shading).
	 *	@retval	IFX_FLAT	Polygons and lines use a single color to fill the interior
	 *						of the primitive.
	 *	@see SetShadeModel().
	 */
	IFXenum IFXAPI GetShadeModel() const;

	/**
	 *	Gets whether or not the lighting system is using vertex colors
	 *	that are supplied with the mesh.
	 *
	 *	@return Boolean value.
	 *
	 *	@retval	TRUE	The lighting system uses vertex colors.
	 *	@retval	FALSE	The lighting system does not use vertex colors.
	 *	@see SetUseVertexColors()
	 */
	BOOL	IFXAPI	GetUseVertexColors() const;
	
	/**
	 *	Resets all data to default values.  Default values are:
	 *
	 *	- <b>Emissive Color</b>: IFXVector4(0,0,0,1)
	 *	- <b>Ambient Color</b>: IFXVector4(0.25,0.25,0.25,1)
	 *	- <b>Diffuse Color</b>: IFXVector4(0.5,0.5,0.5,1)
	 *	- <b>Specular Color</b>: IFXVector4(1,1,1,1)
	 *	- <b>Shininess</b>: 0.3
	 *	- <b>Fast Specular</b>: TRUE
	 *	- <b>Render Style</b>: IFX_FILLED
	 *	- <b>Shade Model</b>: IFX_SMOOTH
	 *	- <b>Use Vertex Colors</b>: TRUE
	 */
	void IFXAPI InitData();

protected:
	IFXVector4	m_vEmissive;
	IFXVector4	m_vAmbient;
	IFXVector4	m_vDiffuse;
	IFXVector4	m_vSpecular;
	F32			m_fShininess;
	BOOL		m_bFastSpecular;
	
	BOOL m_bUseVertexColors;

	IFXenum m_eRenderStyle;
	IFXenum m_eShadeModel;
};
//=======================================
// IFXRenderMaterial Implementation
//=======================================
IFXINLINE IFXRenderMaterial::IFXRenderMaterial()
{
	InitData();
}

IFXINLINE IFXRenderMaterial::~IFXRenderMaterial()
{
	// EMPTY
}

IFXINLINE void IFXRenderMaterial::InitData()
{
	m_vEmissive.Set(0,0,0,1);
	m_vAmbient.Set(0.75f,0.75f,0.75f,1);
	m_vDiffuse.Set(0,0,0,1);
	m_vSpecular.Set(0,0,0,1);
	m_fShininess = 0;
	m_bFastSpecular = FALSE;
	m_eRenderStyle = IFX_FILLED;
	m_eShadeModel = IFX_SMOOTH;

	m_bUseVertexColors = FALSE;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetEmissive(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vEmissive = vColor;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetAmbient(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vAmbient = vColor;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetDiffuse(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vDiffuse = vColor;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetSpecular(const IFXVector4& vColor)
{
	IFXRESULT rc = IFX_OK;

	m_vSpecular = vColor;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetShininess(F32 fShininess)
{
	IFXRESULT rc = IFX_OK;

	m_fShininess= fShininess;

	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetFastSpecular(BOOL bEnabled)
{
	IFXRESULT rc = IFX_OK;
	
	m_bFastSpecular = bEnabled;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetRenderStyle(IFXenum eStyle)
{
	IFXRESULT rc = IFX_OK;

	switch(eStyle)
	{
	case IFX_FILLED:
	case IFX_WIREFRAME:
	case IFX_POINTS:
		m_eRenderStyle = eStyle;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
		break;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetShadeModel(IFXenum eShadeModel)
{
	IFXRESULT rc = IFX_OK;

	switch(eShadeModel)
	{
	case IFX_SMOOTH:
	case IFX_FLAT:
		m_eShadeModel = eShadeModel;
		break;
	default:
		rc = IFX_E_INVALID_RANGE;
		break;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderMaterial::SetUseVertexColors(BOOL bUseVertexColor)
{
	IFXRESULT rc = IFX_OK;

	m_bUseVertexColors = bUseVertexColor;

	return rc;
}

IFXINLINE const IFXVector4& IFXRenderMaterial::GetEmissive() const
{
	return m_vEmissive;
}

IFXINLINE const IFXVector4& IFXRenderMaterial::GetAmbient() const
{
	return m_vAmbient;
}

IFXINLINE const IFXVector4& IFXRenderMaterial::GetDiffuse() const
{
	return m_vDiffuse;
}

IFXINLINE const IFXVector4& IFXRenderMaterial::GetSpecular() const
{
	return m_vSpecular;
}

IFXINLINE F32 IFXRenderMaterial::GetShininess() const
{
	return m_fShininess;
}

IFXINLINE BOOL IFXRenderMaterial::GetFastSpecular() const
{
	return m_bFastSpecular;
}

IFXINLINE IFXenum IFXRenderMaterial::GetRenderStyle() const
{
	return m_eRenderStyle;
}

IFXINLINE IFXenum IFXRenderMaterial::GetShadeModel() const
{
	return m_eShadeModel;
}

IFXINLINE BOOL IFXRenderMaterial::GetUseVertexColors() const
{
	return m_bUseVertexColors;
}


#endif // IFX_RENDER_MATERIAL_H

// END OF FILE

