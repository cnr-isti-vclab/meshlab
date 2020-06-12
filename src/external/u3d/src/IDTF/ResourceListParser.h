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
@file ResourceListParser.h

This header defines the ... functionality.

@note
*/


#ifndef ResourceListParser_H
#define ResourceListParser_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "BlockParser.h"
#include "FileScanner.h"
#include "ResourceList.h"

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
	class ResourceListParser : public BlockParser
	{
	public:
		ResourceListParser( FileScanner* pScanner, ResourceList* pResourceList );
		virtual ~ResourceListParser();

		IFXRESULT Parse();

	protected:

		/**
		*/
		IFXRESULT ParseResourceList( IFXRESULT (ResourceListParser::*ParseResource)() );

		/**
		*/
		IFXRESULT ParseLightResource();

		/**
		*/
		IFXRESULT ParseViewResource();

		/**
		*/
		IFXRESULT ParseModelResource();

		/**
		*/
		IFXRESULT ParseMeshResource();

		/**
		*/
		IFXRESULT ParsePointSetResource();

		/**
		*/
		IFXRESULT ParseLineSetResource();

		/**
		*/
		IFXRESULT ParseShaderResource();

		/**
		*/
		IFXRESULT ParseMaterialResource();

		/**
		*/
		IFXRESULT ParseTextureResource();

		/**
		*/
		IFXRESULT ParseMotionResource();

	private:
		ResourceListParser();

		FileScanner* m_pScanner;
		ResourceList* m_pResourceList;
		IFXString m_resourceName;
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


}

#endif
