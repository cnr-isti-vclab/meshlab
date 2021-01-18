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
@file ShadingModifier.h

This header defines the ... functionality.

@note
*/


#ifndef ShadingModifier_H
#define ShadingModifier_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXArray.h"
#include "Modifier.h"

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

class ShaderList
{
public:
	void AddShaderName( const IFXString& rName );
	const IFXString& GetShaderName( U32 index ) const ;
	U32 GetShaderCount() const;

private:
	IFXArray< IFXString > m_shaderNames;
};

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class ShadingModifier : public Modifier
{
public:
	ShadingModifier() {};
	virtual ~ShadingModifier() {};

	void AddShaderList( const ShaderList& rShaderList );
	const ShaderList& GetShaderList( U32 index ) const;
	U32 GetShaderListCount() const;
	U32 GetAttributes() const;
	void SetAttributes( U32 attr );

private:
	IFXArray< ShaderList > m_shaderLists;
	U32 m_attributes;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void ShaderList::AddShaderName( const IFXString& rName )
{
	IFXString& name = m_shaderNames.CreateNewElement();
	name = rName;
}

IFXFORCEINLINE const IFXString& ShaderList::GetShaderName( U32 index ) const
{
	return m_shaderNames.GetElementConst( index );
}

IFXFORCEINLINE U32 ShaderList::GetShaderCount() const
{
	return m_shaderNames.GetNumberElements();
}

IFXFORCEINLINE void ShadingModifier::AddShaderList( const ShaderList& rShaderList )
{
	ShaderList& shaderList = m_shaderLists.CreateNewElement();
	shaderList = rShaderList;
}

IFXFORCEINLINE const ShaderList& ShadingModifier::GetShaderList( U32 index ) const
{
	return m_shaderLists.GetElementConst( index );
}

IFXFORCEINLINE U32 ShadingModifier::GetShaderListCount() const
{
	return m_shaderLists.GetNumberElements();
}

IFXFORCEINLINE U32 ShadingModifier::GetAttributes() const
{
	return m_attributes;
}

IFXFORCEINLINE void ShadingModifier::SetAttributes( U32 attr )
{
	m_attributes = attr;
}


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

}

#endif
