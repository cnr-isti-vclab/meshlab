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
@file NodeConverter.h

This header defines the ... functionality.

@note
*/


#ifndef NodeConverter_H
#define NodeConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "IConverter.h"
#include "ParentList.h"

class IFXNode;

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

class SceneUtilities;
class NodeList;
class Node;
class LightNode;
class ViewNode;
class ModelNode;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class NodeConverter : public IConverter
{
public:
	NodeConverter(
		NodeList* pNodeList,
		SceneUtilities* pSceneUtils );
	virtual ~NodeConverter();

	/**
	*/
	virtual IFXRESULT Convert();

private:
	NodeConverter();

	IFXRESULT ConvertModel( const ModelNode* pIDTFModel );
	IFXRESULT ConvertLight( const LightNode* pIDTFLight );
	IFXRESULT ConvertView( const ViewNode* pIDTFView );
	IFXRESULT ConvertGroup( const Node* pNode );
	IFXRESULT ConvertParentList( 
					IFXNode* pIFXNode, const ParentList& rParentList );

	NodeList* m_pNodeList;
	SceneUtilities* m_pSceneUtils;
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
