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
@file ParentData.h

This header defines the ... functionality.

@note
*/


#ifndef ParentData_H
#define ParentData_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "IFXMatrix4x4.h"

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
	class ParentData
	{
	public:
		ParentData() {};
		virtual ~ParentData() {};

		/**
		Set parent name
		*/
		void SetParentName( const IFXString& rParentName );
		const IFXString& GetParentName() const;
		void SetParentTM( const IFXMatrix4x4& rMatrix );
		const IFXMatrix4x4& GetParentTM() const;

	protected:

	private:
		IFXString m_parentName;
		IFXMatrix4x4 m_parentTM;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void ParentData::SetParentName( const IFXString& rParentName )
	{
		m_parentName = rParentName;
	}

	IFXFORCEINLINE const IFXString& ParentData::GetParentName() const
	{
		return m_parentName;
	}

	IFXFORCEINLINE void ParentData::SetParentTM( const IFXMatrix4x4& rMatrix )
	{
		m_parentTM = rMatrix;
	}

	IFXFORCEINLINE const IFXMatrix4x4& ParentData::GetParentTM() const
	{
		return m_parentTM;
	}

	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************
}

#endif
