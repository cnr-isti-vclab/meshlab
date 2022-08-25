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
@file ModifierParser.h

This header defines the ... functionality.

@note
*/


#ifndef ModifierParser_H
#define ModifierParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "Modifier.h"
#include "BlockParser.h"

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

class ShaderList;
class FileScanner;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class ModifierParser : public BlockParser
{
public:
	ModifierParser( FileScanner* pScanner, Modifier* pModifier );
	virtual ~ModifierParser();

	/**
	Parse modifier
	*/
	IFXRESULT Parse();

protected:
	IFXRESULT ParseCommonModifierData();
	IFXRESULT ParseAnimationModifier();
	IFXRESULT ParseBoneWeightModifier();
	IFXRESULT ParseShadingModifier();
	IFXRESULT ParseCLODModifier();
	IFXRESULT ParseSubdivisionModifier();
	IFXRESULT ParseGlyphModifier();

private:
	ModifierParser();
	IFXRESULT ParseShaderList( ShaderList* pShaderList );

	FileScanner* m_pScanner;
	Modifier* m_pModifier;
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
