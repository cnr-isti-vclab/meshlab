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
@file ModelResourceParser.h

This header defines the ... functionality.

@note
*/


#ifndef ModelResourceParser_H
#define ModelResourceParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "BlockParser.h"
#include "Int2.h"
#include "Int3.h"
#include "Point.h"
#include "Color.h"

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

class ModelResource;

/**
	This is the implementation of a class that is used to parse
	model resources.

	It supports the BlockParser functionality.
*/
class ModelResourceParser : public BlockParser
{
public:
	ModelResourceParser( FileScanner* pScanner, ModelResource* pModelResource );
	virtual ~ModelResourceParser();

	/**
	*/
	IFXRESULT ParseShadingDescriptions();

	/**
	*/
	IFXRESULT ParseTextureCoords();

	/**
	*/
	IFXRESULT ParseSkeleton();

protected:

	IFXRESULT ParseInt2List( const IFXCHAR* pToken, const I32 count,
		IFXArray< Int2 >& rList);
	IFXRESULT ParseInt3List( const IFXCHAR* pToken, const I32 count,
		IFXArray< Int3 >& rList);
	IFXRESULT ParsePointList( const IFXCHAR* pToken, const I32 count,
		IFXArray< Point >& rList);
	IFXRESULT ParseColorList( const IFXCHAR* pToken, const I32 count,
		IFXArray< Color >& rList);
	IFXRESULT ParseIntList( const IFXCHAR* pToken, const I32 count,
		IFXArray< I32 >& rList);

protected:
	FileScanner* m_pScanner;
	ModelResource* m_pModelResource;
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
