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
@file INode.h

This header defines the ... functionality.

@note
*/


#ifndef INode_H
#define INode_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "ParentList.h"

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
	class INode
	{
	public:
		/**
		Set node's type
		*/
		virtual void IFXAPI SetType( const IFXString& rType ) = 0;

		/**
		Get node's type
		*/
		virtual const IFXString& GetType() const  = 0;

		/**
		Set node's name
		*/
		virtual void IFXAPI SetName( const IFXString& rName ) = 0;

		/**
		Get node's name
		*/
		virtual const IFXString& GetName() const = 0;

		virtual void IFXAPI SetParentList( const ParentList& rParentList ) = 0;
		virtual const ParentList& GetParentList() const = 0;

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
