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
@file NodeList.h

This header defines the ... functionality.

@note
*/


#ifndef NodeList_H
#define NodeList_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXArray.h"
#include "Node.h"
#include "LightNode.h"
#include "ViewNode.h"
#include "ModelNode.h"

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
class NodeList
{
public:
	NodeList();
	virtual ~NodeList();

	/**
	*/
	IFXRESULT AddNode( const Node* pNode );
	const Node* GetNode( U32 index ) const;
	U32   GetNodeCount() const;

protected:

private:
	IFXArray< Node* > m_nodePointerList;
	IFXArray< LightNode > m_lightNodeList;
	IFXArray< ViewNode > m_viewNodeList;
	IFXArray< ModelNode > m_modelNodeList;
	IFXArray< Node > m_groupNodeList;
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
