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

@file  IFXResourceClient.h

       The header file that defines the IFXResourceClient interface.         */

#ifndef __IFXRESOURCECLIENT_INTERFACE_H__
#define __IFXRESOURCECLIENT_INTERFACE_H__

#include "IFXUnknown.h"

// {39F641BC-EBDE-4e1f-BC6E-2D2D2FF167D4}
IFXDEFINE_GUID(IID_IFXResourceClient,
0x39f641bc, 0xebde, 0x4e1f, 0xbc, 0x6e, 0x2d, 0x2d, 0x2f, 0xf1, 0x67, 0xd4);


class IFXResourceClient : virtual public IFXUnknown
/**This interface is used to point palette entries to resources.
*/
{
public:
	virtual U32       IFXAPI  GetResourcePalette( void )=0;
	/**<
	        Returns the IFXSceneGraph::EIFXPalette index that contains the
			resource in use.                                                 */


	virtual U32       IFXAPI  GetResourceIndex( void )=0;
	/**<
	        Returns the palette entry index of the resource in use.          */


	virtual IFXRESULT IFXAPI  SetResourceIndex( U32 uInEntryIndex )=0;
	/**<
	        Changes the resource to the one at the given palette index.

	@param uInEntryIndex
	        The new resource's palette entry index.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_INVALID_RANGE                                             \n
              The index exceeded the size of the palette.                  \n*/
};


#endif //#define __IFXRESOURCECLIENT_INTERFACE_H__

