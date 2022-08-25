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
  @file FileReferenceConverter.h

      This header defines the ... functionality.

  @note
*/


#ifndef FileReferenceConverter_H
#define FileReferenceConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "IConverter.h"

class IFXFileReferenceX;

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

class FileReference;
class SceneUtilities;

/**
This is the implementation of a class that is used to convert meta-data.
*/
class FileReferenceConverter : public IConverter
{
public:
	FileReferenceConverter( 
				SceneUtilities* pSceneUtils,
				const FileReference* pIDTFFileReferenceList );
    virtual ~FileReferenceConverter();

    /**
		Convert file reference
	*/
    virtual IFXRESULT Convert();

private:
    FileReferenceConverter();

	const FileReference* m_pFileReference;
	SceneUtilities* m_pSceneUtils;
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
