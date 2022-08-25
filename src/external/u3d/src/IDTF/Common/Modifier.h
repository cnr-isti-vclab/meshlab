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
@file Modifier.h

This header defines the ... functionality.

@note
*/


#ifndef Modifier_H
#define Modifier_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
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
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class Modifier : public MetaDataList
{
public:
	Modifier() {};
	virtual ~Modifier() {};

	/**
	Set modifier type
	*/
	void SetType( const IFXString& rType );
	const IFXString& GetType() const;

	/**
	Set modifier chain type
	*/
	void SetChainType( const IFXString& rType );
	const IFXString& GetChainType() const;

	/**
	*/
	void SetName( const IFXString& rName );
	const IFXString& GetName() const;

protected:

private:
	IFXString m_name;
	IFXString m_type;
	IFXString m_chainType;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void Modifier::SetType( const IFXString& rType )
{
	m_type = rType;
}

IFXFORCEINLINE const IFXString& Modifier::GetType() const
{
	return m_type;
}

IFXFORCEINLINE void Modifier::SetChainType( const IFXString& rChainType )
{
	m_chainType = rChainType;
}

IFXFORCEINLINE const IFXString& Modifier::GetChainType() const
{
	return m_chainType;
}

IFXFORCEINLINE void Modifier::SetName( const IFXString& rName )
{
	m_name = rName;
}

IFXFORCEINLINE const IFXString& Modifier::GetName() const
{
	return m_name;
}


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

//***************************************************************************
//  Failure return codes
//***************************************************************************

}

#endif
