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

@file  IFXCollection.h

	The header file that defines the IFXCollection interface.                */

#ifndef __IFXCOLLECTION_H__
#define __IFXCOLLECTION_H__

#include "IFXUnknown.h"
#include "IFXSpatial.h"

// {E7AEC75E-590C-4b30-888E-A97B47D59D80}
IFXDEFINE_GUID(IID_IFXCollection,
0xe7aec75e, 0x590c, 0x4b30, 0x88, 0x8e, 0xa9, 0x7b, 0x47, 0xd5, 0x9d, 0x80);

class IFXCollection : virtual public IFXUnknown
/**
	The IFXCollection interface supplies the services for managing a set of
	IFXSpatial references. */
{
public:
	virtual const IFXGUID& GetCID() = 0;

	virtual IFXRESULT IFXAPI  InitializeCollection(
	                               IFXSpatial**      pInSpatials = NULL,
	                               U32               uInNumberOfSpatials = 0,
								   IFXSpatial::eType eInType =
	                                               IFXSpatial::UNSPECIFIED )=0;
	virtual IFXRESULT IFXAPI  InitializeCollection( IFXCollection * pInCollection )=0;
	/**<
	        Initializes the Collection.

	@pre    None; re-initialization is supported.

	@post   If a spatial set is provided, the collection will contain these
	        spatials, if not, it will be empty.

	@param  rpInSpatials
	         A reference to the address of the array of spatials to add to
			 the collection.

	@param  uInNumberOfSpatials
	         The number of spatials in the rpInSpatial array.

	@param  eInType
	         If this parameter is provided, the type of ALL of the provided
			 spatials is assumed to be eInType.  If not specified, each spatial
			 is queryied for it's type separately.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_OUT_OF_MEMORY                                             \n
	          Not enough memory was available to initialize.                 */


	virtual IFXRESULT IFXAPI  AddSpatials( IFXSpatial**      pInSpatials,
	                               U32               uInNumberOfSpatials,
								   IFXSpatial::eType eInType =
	                                               IFXSpatial::UNSPECIFIED )=0;
	/**<
	        Adds the given spatials from the collection.

	@param  pInSpatials
	         The list of spatials to add to the collection.

	@param  uInNumberOfSpatials
	         The number of spatials in the list.

	@param  eInType
	         If this parameter is provided, the type of ALL of the provided
			 spatials is assumed to be eInType.  If not specified, each spatial
			 is queryied for it's type separately.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_OUT_OF_MEMORY                                             \n
	          Not enough memory was available to initialize.                 */


	virtual IFXRESULT IFXAPI  RemoveSpatials(
	                               IFXSpatial**      pInSpatials,
	                               U32               uInNumberOfSpatials,
								   IFXSpatial::eType eInType =
	                                               IFXSpatial::UNSPECIFIED )=0;
	/**<
	        Removes the given spatials from the collection.

	@param  pInSpatials
	         An array of IFXSpatial pointers that refer to the spatials to
			 remove from the collection.

	@param  uInNumberOfSpatials
	         The number of spatials in the list.

	@param  eInType
	         If this parameter is provided, the type of ALL of the provided
			 spatials is assumed to be eInType.  If not specified, each spatial
			 is queryied for it's type separately.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_NOT_FOUND                                                 \n
	          One or more of the provided spatials was not found in the
	          collection.*/


	virtual IFXRESULT IFXAPI  GetSpatials( IFXSpatial**&     rpOutSpatials,
	                               U32&              ruOutNumberOfSpatials,
	                               IFXSpatial::eType eInType               )=0;
	/**<
	        Returns a reference to the spatials, of the given type, within the
			collection.

	@param  eInType
	         The type of spatials to be returned from the collection.

	@param  rpOutSpatials
	         The address of the list of resultant spatials.

	@param  ruOutNumberOfSpatials
	         The number of resultant spatials.

	@param  eInType
	         If this parameter is provided, rpOutSpatials will contain only
			 spatials of type specified.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_INVALID_RANGE                                             \n
	          The value of eInType was greater or equal to
	          IFXSpatial::TYPE_COUNT.                                        */
};


#endif
