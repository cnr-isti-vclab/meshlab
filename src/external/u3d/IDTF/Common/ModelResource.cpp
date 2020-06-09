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
  @file ModelResource.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ModelResource.h"
using namespace U3D_IDTF;

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


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

ModelResource::ModelResource()
{
	ClearModelDescription();
	m_type = L"";
	m_name = L"";
}

ModelResource::~ModelResource()
{
	ClearModelDescription();
}

void ModelResource::ClearModelDescription()
{
	m_modelDescription.positionCount = 0;
	m_modelDescription.normalCount = 0;
	m_modelDescription.diffuseColorCount = 0;
	m_modelDescription.specularColorCount = 0;
	m_modelDescription.textureCoordCount = 0;
	m_modelDescription.shadingCount = 0;
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
