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
@file FileParser.h

This header defines the ... functionality.

@note
*/


#ifndef FileParser_H
#define FileParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "File.h"
#include "FileScanner.h"
#include "BlockParser.h"

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

class SceneData;
class FileReference;
class Node;
class Modifier;
class NodeList;
class SceneResources;
class ResourceList;
class ModifierList;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class FileParser : public BlockParser
{
public:
	FileParser();
	virtual ~FileParser();

	/**
	Initialize file scanner
	*/
	IFXRESULT Initialize( const IFXCHAR* pFileName );

	/**
	Parse file format name and format vertion
	*/
	IFXRESULT ParseFileHeader( IFXString* pFormatName, I32* pVersionNumber );

	/**
	Parse scene objects
	*/
	IFXRESULT ParseSceneData( SceneData* pSceneData );

	/**
	Parse file reference
	*/
	IFXRESULT ParseFileReference( FileReference* pFileReference );

	/**
	Parse scene nodes
	*/
	IFXRESULT ParseNodes( NodeList* pNodeList );

	/**
	Parse scene resources
	*/
	IFXRESULT ParseResources( SceneResources* pSceneResources );

	/**
	Parse scene modifiers
	*/
	IFXRESULT ParseModifiers( ModifierList* pModifierList );

	/**
	*/
	IFXRESULT Parse();

protected:
	/**
	Parse node
	*/
	IFXRESULT ParseNode( Node* pNode );

	/**
	Parse resources
	*/
	IFXRESULT ParseResourceList( ResourceList* pResourceList );

	/**
	Parse modifier
	*/
	IFXRESULT ParseModifier( Modifier* pModifier );

private:
	FileScanner m_scanner;
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
