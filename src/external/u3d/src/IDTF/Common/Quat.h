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
@file Quat.h

This header defines the ... functionality.

@note
*/


#ifndef Quat_H
#define Quat_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXVector4.h"

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
	class Quat
	{
	public:
		Quat() {};
		virtual ~Quat() {};

		/**
		*/
		void SetQuat( const IFXVector4& rQuat );
		const IFXVector4& GetQuat() const;

	protected:

	private:
		IFXVector4 m_quat;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void Quat::SetQuat( const IFXVector4& rQuat )
	{
		m_quat = rQuat;
	}

	IFXFORCEINLINE const IFXVector4& Quat::GetQuat() const
	{
		return m_quat;
	}

	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************


}

#endif
