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
    @file   IFXVersion.h

            This header file defines the version resource information for
            the build.

    @note   This file is only typically modified by the official builder to
            update the various build number defines.
*/


#if !defined( IFXVersion_h )
#define IFXVersion_h


//***************************************************************************
//  Defines that vary for each official build
//***************************************************************************


// [Major].[Minor].[Build number].[0 or Patch number]
//
// For example:
//  Major version 0, minor version 8, build number 22, no patches applied
//  would be "0.8.22.0".
//
#define IFXVERSION_PRODUCTVERSION_STRING  "1.2.851.0\0"
#define IFXVERSION_PRODUCTVERSION     1,2,851,0


//***************************************************************************
//  Defines that do not change often
//***************************************************************************


#if defined( _DEBUG )
#define IFXVERSION_COMMENTS_STRING          "Debug\0"
#else
#define IFXVERSION_COMMENTS_STRING          "Release\0"
#endif
#define IFXVERSION_COMPANYNAME_STRING       "Intel Corporation\0"
#define IFXVERSION_PRODUCTNAME_STRING       "Universal 3D\0"
#define IFXVERSION_LEGALCOPYRIGHT           "Copyright © 1999 - 2006 Intel Corporation"
#define IFXVERSION_LEGALCOPYRIGHT_STRING    IFXVERSION_LEGALCOPYRIGHT "\0"


#endif
