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
	@file	ShadingDescription.h

			This header defines the shading description container.
*/


#ifndef ShadingDescription_H
#define ShadingDescription_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"

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
	This is the implementation of a class that is used to handle shading 
	description.
*/
class ShadingDescription
{
public:
	ShadingDescription() {};
	~ShadingDescription() {};

	void AddTextureCoordDimension( const U32& rDimension );
	const U32& GetTextureCoordDimention( U32 index ) const;
	U32 GetTextureLayerCount() const;

	U32 m_shaderId;
	U32 m_textureLayerCount;

private:
	IFXArray< U32 > m_textureCoordDimensions; // number of texture layers
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void
	ShadingDescription::AddTextureCoordDimension( const U32& rDimension )
{
	U32& dimension = m_textureCoordDimensions.CreateNewElement();
	dimension = rDimension;
}

IFXFORCEINLINE const U32&
	ShadingDescription::GetTextureCoordDimention( U32 index ) const
{
	return m_textureCoordDimensions.GetElementConst( index );
}

IFXFORCEINLINE U32
	ShadingDescription::GetTextureLayerCount() const
{
	return m_textureCoordDimensions.GetNumberElements();
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

}

#endif
