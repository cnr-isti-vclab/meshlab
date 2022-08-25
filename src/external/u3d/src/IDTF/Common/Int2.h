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
@file Int2.h

This header defines the ... functionality.

@note
*/


#ifndef Int2_H
#define Int2_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"

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
	class Int2
	{
	public:
		Int2() {};
		virtual ~Int2() {};

		/**
		*/
		void SetData( const I32& rA, const I32& rB );
		const I32& GetA() const;
		const I32& GetB() const;

		I32 m_a;
		I32 m_b;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void Int2::SetData( const I32& rA, const I32& rB )
	{
		m_a = rA;
		m_b = rB;
	}

	IFXFORCEINLINE const I32& Int2::GetA() const
	{
		return m_a;
	}

	IFXFORCEINLINE const I32& Int2::GetB() const
	{
		return m_b;
	}

	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************

	//***************************************************************************
	//  Failure return codes
	//***************************************************************************

	/**
	@todo:  Insert module/interface specific return code description.
	*/
	//#define IFX_E_????  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )

}

#endif
