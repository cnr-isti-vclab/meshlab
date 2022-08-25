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
@file SceneConverter.h

This header defines the ... functionality.

@note
*/


#ifndef SceneConverter_H
#define SceneConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "IConverter.h"
#include "NodeList.h"
#include "SceneResources.h"
#include "ModifierList.h"

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

class FileParser;
class SceneUtilities;
struct ConverterOptions;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class SceneConverter : public IConverter
{
public:
	SceneConverter( FileParser* pParser, SceneUtilities* pSceneUtils,
		ConverterOptions* pConverterOptions );
	virtual ~SceneConverter();

	/**
	*/
	virtual IFXRESULT  Convert();

protected:
	IFXRESULT CheckFileHeader();
	IFXRESULT ConvertSceneData();
	IFXRESULT ConvertFileReference();
	IFXRESULT ConvertScene();

private:
	NodeList m_nodeList;
	SceneResources m_sceneResources;
	ModifierList m_modifierList;

	SceneUtilities* m_pSceneUtils;
	FileParser* m_pParser;
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
