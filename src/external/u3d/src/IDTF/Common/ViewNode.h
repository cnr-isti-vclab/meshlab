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
@file ViewNode.h

This header defines the ... functionality.

@note
*/


#ifndef ViewNode_H
#define ViewNode_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "Node.h"
#include "ViewNodeData.h"

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
	class ViewNode : public Node
	{
	public:
		ViewNode() {};
		virtual ~ViewNode() {};

		void SetViewData( const ViewNodeData& rViewData );
		const ViewNodeData& GetViewData() const;

	protected:

	private:
		ViewNodeData m_viewData;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void ViewNode::SetViewData( const ViewNodeData& rViewData )
	{
		m_viewData = rViewData;
	}

	IFXFORCEINLINE const ViewNodeData& ViewNode::GetViewData() const
	{
		return m_viewData;
	}


	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************
}


#endif
