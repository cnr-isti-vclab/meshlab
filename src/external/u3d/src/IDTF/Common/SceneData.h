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
@file SceneData.h

This header defines the ... functionality.

@note
*/


#ifndef SceneData_H
#define SceneData_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "Color.h"
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
	class SceneData : public MetaDataList
	{
	public:
		SceneData() {};
		virtual ~SceneData() {};

		/**
		@todo: SetAmbientLight
		*/
		void SetAmbientLight( const Color& rColor );

		/**
		@todo: GetAmbientLight
		*/
		const Color* GetAmbientLight() const;

	protected:

	private:
		Color m_ambientLight;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void SceneData::SetAmbientLight( const Color& rColor )
	{
		m_ambientLight = rColor;
	}

	IFXFORCEINLINE const Color* SceneData::GetAmbientLight() const
	{
		return &m_ambientLight;
	}

	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************
}

#endif
