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
@file GlyphModifier.h

This header defines the ... functionality.

@note
*/


#ifndef GlyphModifier_H
#define GlyphModifier_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "Modifier.h"
#include "GlyphCommandList.h"

#include "IFXMatrix4x4.h"

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
class GlyphModifier : public Modifier
{
public:
	GlyphModifier() {};
	virtual ~GlyphModifier() {};

	/**
	*/
	void SetBillboard( const IFXString& rValue );
	void SetSingleShader( const IFXString& rValue );
	const IFXString& GetBillboard() const;
	const IFXString& GetSingleShader() const;

	void SetTM( const IFXMatrix4x4& rMatrix );
	const IFXMatrix4x4& GetTM() const;

	/**
	*/
	IFXRESULT AddCommand( const GlyphCommand* pCommand );

	/**
	*/
	const GlyphCommand* GetCommand( U32 index ) const;

	/**
	*/
	U32 GetCommandCount() const;

	GlyphModifier& operator= (const GlyphModifier& rModifier);

private:
	IFXString m_billboard;
	IFXString m_singleShader;
	GlyphCommandList m_commandList;
	IFXMatrix4x4 m_tm;
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
