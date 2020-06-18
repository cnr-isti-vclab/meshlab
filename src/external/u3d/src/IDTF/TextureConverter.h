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
  @file TextureConverter.h

      This header defines the ... functionality.

  @note
*/


#ifndef TextureConverter_H
#define TextureConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "IConverter.h"

class IFXTextureObject;

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

class Texture;
class SceneUtilities;

/**
This is the implementation of a class that is used to load the named TGA file,
and build a U3D texture from this image if we haven't already seen a texture 
with this name before.
*/
/**
 convert texture.
*/
class TextureConverter : public IConverter
{
public:
	TextureConverter( const Texture* pTexture, SceneUtilities* pSceneUtils );
    virtual ~TextureConverter();

    virtual IFXRESULT Convert();

	/**
	*/
	void SetQuality( U32 quality, U32 limit );

private:
    TextureConverter();

	IFXRESULT SetImageProperties( IFXTextureObject* pTextureObject );

	const Texture* m_pTexture;
	SceneUtilities* m_pSceneUtils;
	U32	m_quality;
	U32	m_limit;
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
