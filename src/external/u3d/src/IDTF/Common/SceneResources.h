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
@file SceneResources.h

This header defines the ... functionality.

@note
*/


#ifndef SceneResources_H
#define SceneResources_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"

#include "LightResourceList.h"
#include "ViewResourceList.h"
#include "ModelResourceList.h"
#include "ShaderResourceList.h"
#include "MotionResourceList.h"
#include "MaterialResourceList.h"
#include "TextureResourceList.h"

namespace U3D_IDTF
{
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


	/**
	This is the implementation of a class that is used to @todo: usage.

	It supports the following interfaces:  @todo: interfaces.
	*/
	class SceneResources
	{
	public:
		SceneResources();
		virtual ~SceneResources();

		ResourceList* GetResourceList( const IFXString& type );

		/**
		Light resource
		*/
		const LightResourceList& GetLightResourceList() const;

		/**
		View resource
		*/
		const ViewResourceList& GetViewResourceList() const;

		/**
		Model resource
		*/
		const ModelResourceList& GetModelResourceList() const;

		/**
		Shader resource
		*/
		const ShaderResourceList& GetShaderResourceList() const;

		/**
		Texture resource
		*/
		const TextureResourceList& GetTextureResourceList() const;

		/**
		Material resource
		*/
		const MaterialResourceList& GetMaterialResourceList() const;

		/**
		Motion resource
		*/
		const MotionResourceList& GetMotionResourceList() const;

	protected:

	private:
		LightResourceList  m_lightResourceList;
		ViewResourceList   m_viewResourceList;
		ModelResourceList  m_modelResourceList;
		ShaderResourceList m_shaderResourceList;
		MotionResourceList m_motionResourceList;
		TextureResourceList m_textureResourceList;
		MaterialResourceList m_materialResourceList;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************


	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************


}

#endif
