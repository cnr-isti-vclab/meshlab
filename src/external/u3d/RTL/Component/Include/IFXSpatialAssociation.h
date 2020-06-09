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
#ifndef __IFXSPATIALASSOCIATION_H__
#define __IFXSPATIALASSOCIATION_H__

#include "IFXUnknown.h"
#include "IFXSpatial.h"

// {2CC72080-E884-492c-B8CB-827C3CC56987}
IFXDEFINE_GUID(IID_IFXSpatialAssociation,
0x2cc72080, 0xe884, 0x492c, 0xb8, 0xcb, 0x82, 0x7c, 0x3c, 0xc5, 0x69, 0x87);

/**This interface manages associated spatials in a spatial set.*/
class IFXSpatialAssociation : virtual public IFXUnknown
{
public:

	enum Type
	{
		LIGHTS
	};

	virtual U32 IFXAPI  GetAssociatedSpatialCount(Type eAssociationType = LIGHTS)=0;
	/**<
	        Returns the number of associated spatials of the specified type.

	@param  eAssociationType
			The type of spatial which to return the count.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                      */


	virtual IFXRESULT IFXAPI  GetAssociatedSpatial(U32 uInSpatialIndex,
	                                       IFXSpatial** ppOutSpatial,
	                                       Type eInAssociationType = LIGHTS)=0;
	/**<
	        Returns the associated spatial specified by the spatial type and
	        the index.

	@param  uInSpatialIndex
	        An integer that refers to the zero based index of the associated
	        spatial.

	@param  ppOutSpatial
	         The address of a spatial pointer to contain the address of the
	         specified associated spatial.

	@param  eAssociationType
	         The specified spatial type.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_INVALID_RANGE                                             \n
	          The index specified was larger than the number of currently
              associated spatials.                                           */


	virtual IFXRESULT IFXAPI  SetAssociatedSpatials(SPATIALINSTANCE_LIST* pInSpatials,
	                                        U32 uInSpatialCount,
	                                        Type eAssociationType = LIGHTS)=0;
	/**<
	        Configures the provided set of spatials as the spatial's associated
	        spatial set.

	@param  ppOutSpatials
	         Array of spatial ptrs of associated spatials.

	@param  uInSpatialCount
	        A count of the number of spatials contained in the spatial ptr array.

	@param  eAssociationType
	         The specified spatial type.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                      */


	virtual IFXRESULT IFXAPI  AppendAssociatedSpatials(SPATIALINSTANCE_LIST* pInSpatials,
	                                           U32 uInSpatialCount,
	                                           Type eAssociationType = LIGHTS)=0;

	/**<
	        Appends a provided set of spatials to this spatial's associated
	        spatial set.

	@param  ppInSpatials
	         The address of the array of spatial pointers.

	@param  uInSpatialCount
	        An integer that specifies the number of spatials in ppInspatials.

	@param  eAssociationType
	         The specified spatial type.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                      */
};

#endif
