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
  @file ParentList.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ParentList.h"
#include "ParentData.h"
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

ParentList::ParentList()
{
}

ParentList::~ParentList()
{
}

void ParentList::AddParentData( const ParentData& rParentData )
{
	ParentData& parentData = m_parentDataList.CreateNewElement();
	const IFXString& name = rParentData.GetParentName();

	parentData.SetParentName( name );
	const IFXMatrix4x4& matrix = rParentData.GetParentTM();

	parentData.SetParentTM( matrix );
}

const ParentData& ParentList::GetParentData( U32 index ) const
{
	return m_parentDataList.GetElementConst( index );
}

U32 ParentList::GetParentCount() const
{
	return m_parentDataList.GetNumberElements();
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
