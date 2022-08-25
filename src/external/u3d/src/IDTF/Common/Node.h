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
@file Node.h

This header defines the ... functionality.

@note
*/


#ifndef Node_H
#define Node_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "ParentList.h"
#include "MetaDataList.h"

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
	class Node : public MetaDataList
	{
	public:
		Node();
		virtual ~Node();

		/**
		Set node's type
		*/
		void SetType( const IFXString& rType );

		/**
		Get node's type
		*/
		const IFXString& GetType() const;

		/**
		Set node's name
		*/
		void SetName( const IFXString& rName );

		/**
		Get node's name
		*/
		const IFXString& GetName() const;

		/**
		Set node's parent list
		*/
		void SetParentList( const ParentList& rParentList );

		/**
		Get node's parent list
		*/
		const ParentList& GetParentList() const;

		/**
		Set name of node resource
		*/
		void SetResourceName( const IFXString& rName );

		/**
		Get name of node resource
		*/
		const IFXString& GetResourceName() const;

	protected:
		IFXString m_resourceName;

	private:
		IFXString m_type;
		IFXString m_name;
		ParentList m_parentList;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE Node::Node()
	{
	}

	IFXFORCEINLINE Node::~Node()
	{
	}

	IFXFORCEINLINE void Node::SetType( const IFXString& rType )
	{
		m_type = rType;
	}

	IFXFORCEINLINE const IFXString& Node::GetType() const
	{
		return m_type;
	}

	IFXFORCEINLINE void Node::SetName( const IFXString& rName )
	{
		m_name = rName;
	}

	IFXFORCEINLINE const IFXString& Node::GetName() const
	{
		return m_name;
	}

	IFXFORCEINLINE void Node::SetParentList( const ParentList& rParentList )
	{
		m_parentList = rParentList;
	}

	IFXFORCEINLINE const ParentList& Node::GetParentList() const
	{
		return m_parentList;
	}

		IFXFORCEINLINE void Node::SetResourceName( const IFXString& rName )
	{
		m_resourceName = rName;
	}

	IFXFORCEINLINE const IFXString& Node::GetResourceName() const
	{
		return m_resourceName;
	}

	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************
}

#endif
