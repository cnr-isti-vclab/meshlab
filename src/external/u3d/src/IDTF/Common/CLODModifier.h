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
@file CLODModifier.h

This header defines the ... functionality.

@note
*/


#ifndef CLODModifier_H
#define CLODModifier_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
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

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class CLODModifier : public Modifier
{
public:
	CLODModifier() {};
	virtual ~CLODModifier() {};

	/**
	*/
	void SetAutoLODControl( const IFXString& rValue );
	const IFXString& GetAutoLODControl() const;

	/**
	*/
	void SetLODBias( const F32& rLodBias );
	const F32& GetLODBias() const;

	/**
	*/
	void SetCLODLevel( const F32& rClodLevel );
	const F32& GetCLODLevel() const;

private:
	IFXString m_autoLODControl;
	F32 m_lodBias;
	F32 m_clodLevel;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void CLODModifier::SetLODBias( const F32& rLodBias )
{
	m_lodBias = rLodBias;
}

IFXFORCEINLINE const F32& CLODModifier::GetLODBias() const
{
	return m_lodBias;
}

IFXFORCEINLINE void CLODModifier::SetCLODLevel( const F32& rClodLevel )
{
	m_clodLevel = rClodLevel;
}

IFXFORCEINLINE const F32& CLODModifier::GetCLODLevel() const
{
	return m_clodLevel;
}

IFXFORCEINLINE void CLODModifier::SetAutoLODControl( const IFXString& rValue )
{
	m_autoLODControl = rValue;
}

IFXFORCEINLINE const IFXString& CLODModifier::GetAutoLODControl() const
{
	return m_autoLODControl;
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

/**
@todo:  Insert module/interface specific return code description.
*/
//#define IFX_E_????  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )

}

#endif
