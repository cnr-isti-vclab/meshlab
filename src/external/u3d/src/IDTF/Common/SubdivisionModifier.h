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
@file SubdivisionModifier.h

This header defines the ... functionality.

@note
*/


#ifndef SubdivisionModifier_H
#define SubdivisionModifier_H


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
class SubdivisionModifier : public Modifier
{
public:
	SubdivisionModifier() {};
	virtual ~SubdivisionModifier() {};

	/**
	*/
	void SetEnabled( const IFXString& rValue );
	const IFXString& GetEnabled() const;

	/**
	*/
	void SetAdaptive( const IFXString& rValue );
	const IFXString& GetAdaptive() const;

	/**
	*/
	void SetDepth( const I32& rDepth );
	const I32& GetDepth() const;

	/**
	*/
	void SetTension( const F32& rTension );
	const F32& GetTension() const;

	/**
	*/
	void SetError( const F32& rError );
	const F32& GetError() const;

	/**
	*/
	void SetSubdivisionLevel( const F32& rClodLevel );
	const F32& GetSubdivisionLevel() const;

private:
	IFXString m_enabled;
	IFXString m_adaptive;
	I32 m_depth;
	F32 m_tension;
	F32 m_error;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void SubdivisionModifier::SetEnabled( const IFXString& rValue )
{
	m_enabled = rValue;
}

IFXFORCEINLINE const IFXString& SubdivisionModifier::GetEnabled() const
{
	return m_enabled;
}

IFXFORCEINLINE void SubdivisionModifier::SetAdaptive( const IFXString& rValue )
{
	m_adaptive = rValue;
}

IFXFORCEINLINE const IFXString& SubdivisionModifier::GetAdaptive() const
{
	return m_adaptive;
}

IFXFORCEINLINE void SubdivisionModifier::SetDepth( const I32& rDepth )
{
	m_depth = rDepth;
}

IFXFORCEINLINE const I32& SubdivisionModifier::GetDepth() const
{
	return m_depth;
}

IFXFORCEINLINE void SubdivisionModifier::SetTension( const F32& rTension )
{
	m_tension = rTension;
}

IFXFORCEINLINE const F32& SubdivisionModifier::GetTension() const
{
	return m_tension;
}

IFXFORCEINLINE void SubdivisionModifier::SetError( const F32& rError )
{
	m_error = rError;
}

IFXFORCEINLINE const F32& SubdivisionModifier::GetError() const
{
	return m_error;
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
