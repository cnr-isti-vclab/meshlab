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
@file TextureResourceList.h

This header defines the ... functionality.

@note
*/


#ifndef TextureResourceList_H
#define TextureResourceList_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "ResourceList.h"
#include "TextureResource.h"

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
	class TextureResourceList : public ResourceList
	{
	public:
		TextureResourceList() {};
		virtual ~TextureResourceList() {};

		/**
		*/
		void AddResource( const Texture& rResource );

		/**
		*/
		const Texture& GetResource( U32 index ) const;
		U32 GetResourceCount() const;

	private:
		IFXArray< Texture > m_resourceList;
	};

	//***************************************************************************
	//  Inline functions
	//***************************************************************************

	IFXFORCEINLINE void TextureResourceList::AddResource( const Texture& rResource )
	{
		Texture& resource = m_resourceList.CreateNewElement();
		resource = rResource;
	}

	IFXFORCEINLINE const Texture& TextureResourceList::GetResource( U32 index ) const
	{
		return m_resourceList.GetElementConst( index );
	}

	IFXFORCEINLINE U32 TextureResourceList::GetResourceCount() const
	{
		return m_resourceList.GetNumberElements();
	}


	//***************************************************************************
	//  Global function prototypes
	//***************************************************************************


	//***************************************************************************
	//  Global data
	//***************************************************************************
}

#endif
