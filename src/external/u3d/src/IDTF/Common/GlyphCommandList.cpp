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
  @file GlyphCommandList.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "GlyphCommandList.h"
#include "Tokens.h"

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

IFXRESULT GlyphCommandList::AddCommand( const GlyphCommand* pCommand )
{
	IFXRESULT result = IFX_OK;

	GlyphCommand* pTmpCommand = NULL;

	if( NULL != pCommand )
	{
		const IFXString& rType = pCommand->GetType();

		if( rType == IDTF_START_GLYPH_STRING ||
			rType == IDTF_END_GLYPH_STRING ||
			rType == IDTF_START_GLYPH ||
			rType == IDTF_START_PATH ||
			rType == IDTF_END_PATH )
		{
			GlyphCommand& command = m_commandList.CreateNewElement();
			command = *pCommand;
			pTmpCommand = &command;
		}
		else if( rType == IDTF_END_GLYPH )
		{
			EndGlyph& endGlyph = m_endGlyphList.CreateNewElement();
			endGlyph = *static_cast<const EndGlyph*>(pCommand);
			pTmpCommand = &endGlyph;
		}
		else if( rType == IDTF_LINE_TO )
		{
			LineTo& lineTo = m_lineToList.CreateNewElement();
			lineTo = *static_cast<const LineTo*>(pCommand);
			pTmpCommand = &lineTo;
		}
		else if( rType == IDTF_MOVE_TO )
		{
			MoveTo& moveTo = m_moveToList.CreateNewElement();
			moveTo = *static_cast<const MoveTo*>(pCommand);
			pTmpCommand = &moveTo;
		}
		else if( rType == IDTF_CURVE_TO )
		{
			CurveTo& curveTo = m_curveToList.CreateNewElement();
			curveTo = *static_cast<const CurveTo*>(pCommand);
			pTmpCommand = &curveTo;
		}
		else
			result = IFX_E_UNDEFINED;
	}
	else
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
	{
		// add new command pointer to the command pointer list 
		// only if it has known type
		GlyphCommand*& rpCommand = m_commandPointerList.CreateNewElement();
		rpCommand = pTmpCommand;
	}

	return result;
}

const GlyphCommand* GlyphCommandList::GetCommand( U32 index ) const
{
	return m_commandPointerList.GetElementConst( index );
}

U32 GlyphCommandList::GetCommandCount() const
{
	return m_commandPointerList.GetNumberElements();
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
