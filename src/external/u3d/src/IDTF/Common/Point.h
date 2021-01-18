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
@file Point.h

This header defines the ... functionality.

@note
*/


#ifndef Point_H
#define Point_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXVector3.h"

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
	class Point
	{
	public:
		Point() {};
		virtual ~Point() {};

		/**
		@todo:  INSERT CUSTOM METHOD PROTOTYPES AND DESCRIPTIONS.
		DOCUMENT THEM IN ONLY ONE SPOT.  IT'S BEST TO DO THIS IN THE
		INTERFACE HEADER HERE TO REDUCE DUPLICATION.

		@param  inputValue    An input value.
		@param  pOutputValue  A pointer to the output value that is
		initialize upon success.

		@return Upon success, IFX_OK is returned.  Otherwise, one of the
		following failures are returned:  @todo: return codes.
		*/
		void SetPoint( const IFXVector3& rPoint );
		const IFXVector3& GetPoint() const;

	protected:

	private:
		IFXVector3 m_point;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void Point::SetPoint( const IFXVector3& rPoint )
	{
		m_point = rPoint;
	}

	IFXFORCEINLINE const IFXVector3& Point::GetPoint() const
	{
		return m_point;
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
