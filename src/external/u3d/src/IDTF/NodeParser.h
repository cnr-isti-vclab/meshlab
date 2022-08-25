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
@file NodeParser.h

This header defines the ... functionality.

@note
*/

#ifndef NodeParser_H
#define NodeParser_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
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

class FileScanner;
class Node;
class ParentList;
class ParentData;
struct ViewTexture;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class NodeParser : public BlockParser
{
public:
	NodeParser( FileScanner* pScanner, Node* pNode );
	virtual ~NodeParser();

	/**
	Parse node
	*/
	IFXRESULT Parse();

protected:
	IFXRESULT ParseParentList( ParentList* pParentList );
	IFXRESULT ParseParentData( ParentData* pParentData );
	IFXRESULT ParseNodeData();
	IFXRESULT ParseLightNodeData();
	IFXRESULT ParseViewTexture( ViewTexture* pViewTexture );
	IFXRESULT ParseViewNodeData();
	IFXRESULT ParseModelNodeData();

private:
	NodeParser();

	FileScanner* m_pScanner;
	Node* m_pNode;
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
