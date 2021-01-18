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
//
//	CIFXResultAllocator.cpp
//
//	DESCRIPTION
//		
//
//	NOTES
//      None.
//
//*****************************************************************************

//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXResultAllocator.h"

//*****************************************************************************
//	Defines
//***************************************************************************** 

//*****************************************************************************
//	Constants
//***************************************************************************** 

//*****************************************************************************
//	Enumerations
//*****************************************************************************

//*****************************************************************************
//	Global data
//*****************************************************************************

//*****************************************************************************
//	Local data
//*****************************************************************************

//*****************************************************************************
//	Classes, structures and types
//*****************************************************************************

//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::Allocate
//
//  Return a pointer to an allocated block of memory.
//-----------------------------------------------------------------------------

IFXRESULT CIFXResultAllocator::Allocate(CIFXCollisionResult **ppResult)
{
	IFXRESULT result;

	if( ppResult )
	{
		U8 *pu8 = IFXUnitAllocator::Allocate();
		*ppResult = ((CIFXCollisionResult *) pu8);
		(*ppResult)->Initialize();

		result = IFX_OK;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::CIFXAxisAlignedBBox
//
//  Constructor
//-----------------------------------------------------------------------------

CIFXResultAllocator::CIFXResultAllocator(U32 uNumInitialUnits, U32 uGrowByNumUnits)
{
	IFXUnitAllocator::Initialize(sizeof (CIFXCollisionResult), uNumInitialUnits, uGrowByNumUnits);
	m_uSizeList = 0;
}


//-----------------------------------------------------------------------------
//	CIFXAxisAlignedBBox::~CIFXAxisAlignedBBox
//
//  Destructor
//-----------------------------------------------------------------------------

CIFXResultAllocator::~CIFXResultAllocator()
{
	m_uSizeList = 0;
}
