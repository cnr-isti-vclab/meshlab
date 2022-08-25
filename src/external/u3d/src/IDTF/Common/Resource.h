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
  @file Resource.h

      This header defines the ... functionality.

  @note
*/

#ifndef Resource_H
#define Resource_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXString.h"
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
*/
class Resource : public MetaDataList
{
public:
	void SetName( const IFXString& rName );
	const IFXString& GetName() const;

protected:
	IFXString m_name;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void Resource::SetName( const IFXString& rName )
{
	m_name = rName;
}

IFXFORCEINLINE const IFXString& Resource::GetName() const 
{
	return m_name;
}

//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************
}


#endif
