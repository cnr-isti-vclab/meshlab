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
  @file MaterialResource.h

      This header defines the ... functionality.

  @note
*/

#ifndef MaterialResource_H
#define MaterialResource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
#include "Resource.h"
#include "Color.h"


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
*/

class Material : public Resource
{
public:
	Material();
	virtual ~Material() {};

	IFXString m_ambientEnabled;
	IFXString m_diffuseEnabled;
	IFXString m_specularEnabled;
	IFXString m_emissiveEnabled;
	IFXString m_reflectivityEnabled;
	IFXString m_opacityEnabled;
	Color m_ambient;
	Color m_diffuse;
	Color m_specular;
	Color m_emissive;
	F32 m_reflectivity;
	F32 m_opacity;
};


//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE Material::Material()
:	m_ambientEnabled( L"TRUE" ),
	m_diffuseEnabled( L"TRUE" ),
	m_specularEnabled( L"TRUE" ),
	m_emissiveEnabled( L"TRUE" ),
	m_reflectivityEnabled( L"TRUE" ),
	m_opacityEnabled( L"TRUE" )
{
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
