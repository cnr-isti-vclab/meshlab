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
@file LineSetResourceParser.h

This header defines the ... functionality.

@note
*/


#ifndef LineSetResourceParser_H
#define LineSetResourceParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "ModelResourceParser.h"
#include "Int3.h"
#include "Point.h"

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

class LineSetResource;
class FileScanner;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class LineSetResourceParser : public ModelResourceParser
{
public:
	LineSetResourceParser( FileScanner* pScanner,
		LineSetResource* pLineSetResource );
	virtual ~LineSetResourceParser();

	IFXRESULT Parse();

protected:
	IFXRESULT ParseLineSetDescription();
	IFXRESULT ParseLineTextureCoords();

private:
	LineSetResource* m_pLineSetResource;
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
