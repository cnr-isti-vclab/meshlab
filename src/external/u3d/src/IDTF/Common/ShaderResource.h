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
  @file ShaderResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef ShaderResource_H
#define ShaderResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "Resource.h"
#include "IFXString.h"
#include "IFXArray.h"

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************

// default texture layer parameters
static const F32 DEFAULT_INTENSITY = 1.0f;
static const F32 DEFAULT_BLEND_CONSTANT = 0.5f;
static const IFXCHAR DEFAULT_BLEND_FUNCTION[] = L"MULTIPLY";
static const IFXCHAR DEFAULT_BLEND_SOURCE[] = L"CONSTANT";
static const IFXCHAR DEFAULT_TEXTURE_MODE[] = L"TM_NONE";
static const IFXCHAR DEFAULT_TEXTURE_REPEAT[] = L"UV";

// default shader parameters
static const IFXCHAR DEFAULT_COLOR_BLEND_FUNCTION[] = L"ALPHA_BLEND";
static const IFXCHAR DEFAULT_COLOR_ALPHA_TEST_FUNCTION[] = L"ALWAYS";
static const F32 DEFAULT_ALPHA_TEST_REFERENCE = 0.0f;

//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

/**
*/
class TextureLayer
{
public:
	TextureLayer();
	virtual ~TextureLayer() {};

	I32 m_channel;
	F32 m_intensity;
	IFXString m_blendFunction;
	IFXString m_blendSource;
	F32 m_blendConstant;
	IFXString m_mode;
	IFXString m_alphaEnabled;
	IFXString m_repeat;
	IFXString m_textureName; // name of texture resource
};

/**
*/
class Shader : public Resource
{
public:
	Shader();
	~Shader() {};

	void AddTextureLayer( const TextureLayer& rTextureLayer );
	const TextureLayer& GetTextureLayer( U32 index ) const ;

	/**
	Returns the number of active texture layers
	*/
	U32 GetTextureLayerCount() const;

	IFXString m_materialName; // name of material resource

	IFXString m_lightingEnabled;
	IFXString m_alphaTestEnabled;
	IFXString m_useVertexColor;

	F32 m_alphaTestReference;
	IFXString m_alphaTestFunction;
	IFXString m_colorBlendFunction;

private:
	IFXArray< TextureLayer > m_textureLayerList;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE TextureLayer::TextureLayer() :
	m_intensity( DEFAULT_INTENSITY ),
	m_blendFunction( DEFAULT_BLEND_FUNCTION ),
	m_blendSource( DEFAULT_BLEND_SOURCE ),
	m_blendConstant( DEFAULT_BLEND_CONSTANT ),
	m_mode( DEFAULT_TEXTURE_MODE ),
	m_alphaEnabled( L"FALSE" ),
	m_repeat( DEFAULT_TEXTURE_REPEAT )
{
}

IFXFORCEINLINE Shader::Shader()
:	m_lightingEnabled( L"TRUE" ),
	m_alphaTestEnabled( L"FALSE" ),
	m_useVertexColor( L"FALSE" ),
	m_alphaTestReference( DEFAULT_ALPHA_TEST_REFERENCE ),
	m_alphaTestFunction( DEFAULT_COLOR_ALPHA_TEST_FUNCTION ),
	m_colorBlendFunction( DEFAULT_COLOR_BLEND_FUNCTION )
{
}

IFXFORCEINLINE void Shader::AddTextureLayer( const TextureLayer& rTextureLayer )
{
	TextureLayer& textureLayer = m_textureLayerList.CreateNewElement();
	textureLayer = rTextureLayer;
}

IFXFORCEINLINE const TextureLayer& Shader::GetTextureLayer( U32 index ) const
{
	return m_textureLayerList.GetElementConst( index );
}

IFXFORCEINLINE U32 Shader::GetTextureLayerCount() const
{
	return m_textureLayerList.GetNumberElements();
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}


#endif
