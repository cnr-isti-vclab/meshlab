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
  @file SceneResources.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "SceneResources.h"
#include "Tokens.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

SceneResources::SceneResources()
{
}

SceneResources::~SceneResources()
{
}

/*
IFXRESULT SceneResources::SetResourceList( const ResourceList* pResourceList )
{
	IFXRESULT result = IFX_OK;

	IFXString type;
	result = pResourceList->GetType( &type );

	if( IFXSUCCESS( result ) )
	{
		if( type == IDTF_LIGHT )
			m_lightResourceList = *static_cast<const LightResourceList*>(pResourceList);
		else if( type == IDTF_VIEW )
			m_viewResourceList = *static_cast<const ViewResourceList*>(pResourceList);
		else if( type == IDTF_MODEL )
			m_modelResourceList = *static_cast<const ModelResourceList*>(pResourceList);
		else if( type == IDTF_SHADER )
			m_shaderResourceList = *static_cast<const ShaderResourceList*>(pResourceList);
		else if( type == IDTF_MATERIAL )
			m_materialResourceList = *static_cast<const MaterialResourceList*>(pResourceList);
		else if( type == IDTF_TEXTURE )
			m_textureResourceList = *static_cast<const TextureResourceList*>(pResourceList);
		else if( type == IDTF_MOTION )
			m_motionResourceList = *static_cast<const MotionResourceList*>(pResourceList);
		else
			result = IFX_E_UNDEFINED_RESOURCE;
	}

	return result;
}
*/

ResourceList* SceneResources::GetResourceList( const IFXString& type )
{
	ResourceList* pResourceList = NULL;
	
	if( type == IDTF_LIGHT )
		pResourceList = static_cast<ResourceList*>(&m_lightResourceList);
	else if( type == IDTF_VIEW )
		pResourceList = static_cast<ResourceList*>(&m_viewResourceList);
	else if( type == IDTF_MODEL )
		pResourceList = static_cast<ResourceList*>(&m_modelResourceList);
	else if( type == IDTF_SHADER )
		pResourceList = static_cast<ResourceList*>(&m_shaderResourceList);
	else if( type == IDTF_MATERIAL )
		pResourceList = static_cast<ResourceList*>(&m_materialResourceList);
	else if( type == IDTF_TEXTURE )
		pResourceList = static_cast<ResourceList*>(&m_textureResourceList);
	else if( type == IDTF_MOTION )
		pResourceList = static_cast<ResourceList*>(&m_motionResourceList);
	else
		pResourceList = NULL;

	return pResourceList;
}


const LightResourceList& SceneResources::GetLightResourceList() const
{
	return m_lightResourceList;
}

const ViewResourceList& SceneResources::GetViewResourceList() const
{
	return m_viewResourceList;
}

const ModelResourceList& SceneResources::GetModelResourceList() const
{
	return m_modelResourceList;
}

const ShaderResourceList& SceneResources::GetShaderResourceList() const
{
	return m_shaderResourceList;
}

const TextureResourceList& SceneResources::GetTextureResourceList() const
{
	return m_textureResourceList;
}


const MaterialResourceList& SceneResources::GetMaterialResourceList() const
{
	return m_materialResourceList;
}


const MotionResourceList& SceneResources::GetMotionResourceList() const
{
	return m_motionResourceList;
}



//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
