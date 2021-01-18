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
@file GlyphCommandList.h

This header defines the ... functionality.

@note
*/


#ifndef GlyphCommandList_H
#define GlyphCommandList_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "IFXString.h"
#include "IFXArray.h"

#include "GlyphCommands.h"

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
class GlyphCommandList
{
public:
	GlyphCommandList() {};
	virtual ~GlyphCommandList() {};

	/**
	*/
	IFXRESULT AddCommand( const GlyphCommand* pCommand );

	/**
	*/
	const GlyphCommand* GetCommand( U32 index ) const;

	/**
	*/
	U32 GetCommandCount() const;

private:
	IFXArray<GlyphCommand*> m_commandPointerList;
	IFXArray<GlyphCommand> m_commandList;
	IFXArray<EndGlyph> m_endGlyphList;
	IFXArray<MoveTo> m_moveToList;
	IFXArray<LineTo> m_lineToList;
	IFXArray<CurveTo> m_curveToList;
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


//***************************************************************************
//  Failure return codes
//***************************************************************************

}

#endif
