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
	@file	IFXSpatialSetQuery.h

	The header file that defines the IFXSpatialSetQuery interface.
*/

#ifndef __IFXSPATIALSETQUERY_H__
#define __IFXSPATIALSETQUERY_H__

#include "IFXUnknown.h"
class IFXFrustum;
class IFXCollection;

// {5B00D081-4078-496c-967D-CCE18AA1A55F}
IFXDEFINE_GUID(IID_IFXSpatialSetQuery,
0x5b00d081, 0x4078, 0x496c, 0x96, 0x7d, 0xcc, 0xe1, 0x8a, 0xa1, 0xa5, 0x5f);

class IFXSpatialSetQuery : virtual public IFXUnknown
/**
	The IFXSpatialSetQuery interface supplies the services for intersection
	testing between a convex space and a set of spatials.
*/
{
public:

	virtual IFXRESULT IFXAPI QueryForSpatialSet(
			 const IFXFrustum*	pFrustum,
			 SPATIALINSTANCE_LIST*&		rpOpaque,
			 SPATIALINSTANCE_LIST*&		rpTranslucent,
			 F32&				ruOutNearClipPlane,
			 F32&				ruOutFarClipPlane )=0;
	/**<
	        Returns the set of spatials, and their associated spatials, that
			intersect the given convex space.  Spatials that support the
			IFXVisible interface are additionally configured according to the
			given convex plane order, from least to most significant bits,
			using SetIntersectionBits().

	@param  pInConvexPlanes
	         The first of a set of IFXRay structures that define the
			 outward normals of planes that define a convex space which to use
			 for the intersection test.

	@param  uInNumberOfConvexPlanes
	         The number of IFXRay structures that exist in the pInConvexPlane
			 array.

	@param  rpOutOpaqueAssociations
	         A reference to an array of IFXSpatialAssociation structures to
			 contain the spatials with type IFXSpatial::eType::OPAQUE_MODEL.
			 If the pointer is NULL the array is allocated, additionally if
			 it is not large enough to contain the result, it is reallocated.
			 The lifetime of the structure is the responsability of the caller.

	@param  ruOutNumberOfOpaqueAssociations
	         A reference to a U32 to contain the number of opaque associations
			 in the rpOutOpaqueAssociations array.

	@param  rpOutTranslucentAssociations
	         A reference to an array of IFXSpatialAssociation structures to
			 contain the spatials with type IFXSpatial::eType::TRANSLUCENT_MODEL.
			 If the pointer is NULL the array is allocated, additionally if
			 it is not large enough to contain the result, it is reallocated.
			 The lifetime of the structure is the responsibility of the caller.

	@param  ruOutNumberOfTranslucentAssociations
	         A reference to a U32 to contain the number of opaque associations
			 in the rpOutTranslucentAssociations array.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_OUT_OF_MEMORY                                             \n
	          Not enough memory was available to initialize.                 */


	virtual const IFXCID& IFXAPI GetCollectionCID() const=0;
	/**<
	        Returns a reference to a CID that describes the component that
			should be provided to SetCollection().                           */


	virtual IFXRESULT IFXAPI SetCollection( IFXCollection* pInCollection )=0;
	/**<
	        Configures the culling system to use the given collection when
			servicing a call to QueryForSpatialSet().

	@param  pInCollection
	         The collection to use when servicing a call to QueryForSpatialSet().

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_OUT_OF_MEMORY                                             \n
	          Not enough memory was available to initialize.

	@note   A reference to the given collection is held, and the collection is
	        used directly.  It is not copied.                                */


	virtual IFXRESULT IFXAPI GetCollection( IFXCollection*& rpOutCollection ) const =0;
	/**<
	        Returns the collection that was last set using SetCollection().

	@param  rpOutCollection
	         A reference to the IFXCollection pointer that should contain the
			 address of the collection used by the culling system.

	@return One of the following IFXRESULT codes:                          \n\n
-	         IFX_OK                                                          \n
	          No error.                                                    \n\n
-	         IFX_E_OUT_OF_MEMORY                                             \n
	          Not enough memory was available to initialize.                 */
};

#endif
