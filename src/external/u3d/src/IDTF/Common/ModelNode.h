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
@file ModelNode.h

This header defines the ... functionality.

@note
*/


#ifndef ModelNode_H
#define ModelNode_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "Node.h"
#include "IFXString.h"

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
	class ModelNode : public Node
	{
	public:
		ModelNode();
		virtual ~ModelNode() {};

		void SetVisibility( const IFXString& rVisibility );
		const IFXString& GetVisibility() const;

	private:
		IFXString m_visibility;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE ModelNode::ModelNode()
		: m_visibility( L"FRONT" )
	{
	}

	IFXFORCEINLINE void ModelNode::SetVisibility( const IFXString& rVisibility )
	{
		m_visibility = rVisibility;
	}

	IFXFORCEINLINE const IFXString& ModelNode::GetVisibility() const
	{
		return m_visibility;
	}

	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************

}

#endif
