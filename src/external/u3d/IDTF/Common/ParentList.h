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
@file ParentList.h

This header defines the ... functionality.

@note
*/


#ifndef ParentList_H
#define ParentList_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXArray.h"
#include "ParentData.h"

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
class ParentList
{
public:
	ParentList();
	virtual ~ParentList();

	/**
	Add parent data no the node's parent list
	*/
	void AddParentData( const ParentData& rParentData );
	const ParentData& GetParentData( U32 index ) const ;
	U32 GetParentCount() const;

protected:

private:
	IFXArray< ParentData > m_parentDataList;
};

//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}

#endif
