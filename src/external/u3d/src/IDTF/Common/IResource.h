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
@file IResource.h

This header defines the ... functionality.

@note
*/


#ifndef IResource_H
#define IResource_H


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
	class IResource
	{
	public:
		/**
		Set node's type
		*/
		virtual void IFXAPI SetType( const IFXString& rType ) = 0;

		/**
		Get node's type
		*/
		virtual IFXRESULT IFXAPI GetType( IFXString* pType ) const  = 0;

		/**
		Set node's name
		*/
		virtual void IFXAPI SetName( const IFXString& rName ) = 0;

		/**
		Get node's name
		*/
		virtual IFXRESULT IFXAPI GetName( IFXString* pName ) const = 0;

		/**
		Set parent list
		*/
		//virtual void IFXAPI SetParentList( const ParentList& rParentList ) = 0;

		/**
		Get parent list
		*/
		//virtual IFXRESULT  IFXAPI GetParentList( ParentList* pParentList ) const = 0;
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
