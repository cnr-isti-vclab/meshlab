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
  @file ResourceConverter.h

      This header defines the ... functionality.

  @note
*/


#ifndef ResourceConverter_H
#define ResourceConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "IConverter.h"
#include "SceneUtilities.h"
#include "TextureResource.h"

class IFXMotionResource;
class IFXString;
class IFXShaderLitTexture;
struct IFXAuthorMaterial;

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

class SceneResources;
class SceneUtilities;
struct ConverterOptions;

class MotionTrack;
class Shader;
class TextureLayer;
class Texture;
class Material;

/**
This is the implementation of a class that is used to ResourceConverter.

It supports the following interfaces:  ResourceConverter.
*/
class ResourceConverter : public IConverter
{
public:
    ResourceConverter( 
				SceneResources* pSceneResources, 
				SceneUtilities* pSceneUtils,
				ConverterOptions* pOptions );

    virtual ~ResourceConverter();

    /**
    */
    virtual IFXRESULT Convert();

private:
    ResourceConverter();
	
	IFXRESULT ConvertMotionResources();
	IFXRESULT ConvertShaderResources();
	IFXRESULT ConvertMaterialResources();
	IFXRESULT ConvertTextureResources();
	IFXRESULT ConvertModelResources();
	IFXRESULT ConvertLightResources();
	IFXRESULT ConvertViewResources();

	/**
	Insert motion data for the specified track into an IFXMotionResource

	@param	U32 trackId					The track to populate in the motion.
	@param	IFXMotionResource* pMotion	The motion that owns this track.
	@param  const MotionTrack& rIDTFTrack   IDTF motion track

	@return	IFXRESULT					Return status of this method.
	*/
	IFXRESULT ConvertKeyFrames( 
						U32 trackId, 
						IFXMotionResource* pMotion,
						const MotionTrack& rIDTFTrack );

	/**
	Converts the data for a single material into U3D  

	@return	IFXRESULT Return status of this method.
	*/
	IFXRESULT ConvertMaterial( 
						const Material& rIDTFMaterial );

	/**
	Converts the data for a single shader into U3D  

	@return	IFXRESULT Return status of this method.
	*/
	IFXRESULT ConvertShader( 
						const Shader& rIDTFShader );


	/**
	Converts a texture layer and place it in the specified shader.
	Finish up by assigning texture to the specified texture layer
	of the shader.

	@param  const U3D_IDTF::TextureLayer& rIDTFTextureLayer
											IDTF texture layer.
	@param	IFXShaderLitTexture* pShader	The Shader to hook up to the
											loaded texture layer.
	@return	IFXRESULT						Return status of this method.
	*/
	IFXRESULT ConvertTextureLayer( 
						const TextureLayer& rIDTFTextureLayer,
						IFXShaderLitTexture* pShader );

	SceneResources* m_pResources;
	SceneUtilities* m_pSceneUtils;
	ConverterOptions* m_pOptions;
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
