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
@file GlyphCommands.h

This header defines the ... functionality.

@note
*/


#ifndef GlyphCommands_H
#define GlyphCommands_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXString.h"

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

class GlyphCommand
{
public:
	GlyphCommand() {};
	virtual ~GlyphCommand() {};

	void SetType( const IFXString& rType );
	const IFXString& GetType() const;

private:
	IFXString m_type;
};

class EndGlyph : public GlyphCommand
{
public:
	EndGlyph() {};
	virtual ~EndGlyph() {};

	F32 m_offset_x;
	F32 m_offset_y;
};

class MoveTo : public GlyphCommand
{
public:
	MoveTo() {};
	virtual ~MoveTo() {};

	F32 m_moveto_x;
	F32 m_moveto_y;
};

class LineTo : public GlyphCommand
{
public:
	LineTo() {};
	virtual ~LineTo() {};

	F32 m_lineto_x;
	F32 m_lineto_y;
};

class CurveTo : public GlyphCommand
{
public:
	CurveTo() {};
	virtual ~CurveTo() {};

	F32 m_control1_x;
	F32 m_control1_y;
	F32 m_control2_x;
	F32 m_control2_y;
	F32 m_endpoint_x;
	F32 m_endpoint_y;
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void GlyphCommand::SetType( const IFXString& rType )
{
	m_type = rType;
}

IFXFORCEINLINE const IFXString& GlyphCommand::GetType() const
{
	return m_type;
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
