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
	@file	IFXBoundVolume.h

			Bounding volume base interface class
*/

#ifndef IFXBOUNDVOLUME_DOT_H
#define IFXBOUNDVOLUME_DOT_H

#define IFXRESULT_BOUNDINGVOLUME     11

#define IFX_BV_SPLIT_BOTH            MAKE_IFXRESULT_PASS( IFXRESULT_BOUNDINGVOLUME, 0x0001 )
#define IFX_BV_SPLIT_RIGHT           MAKE_IFXRESULT_PASS( IFXRESULT_BOUNDINGVOLUME, 0x0002 )
#define IFX_BV_SPLIT_LEFT            MAKE_IFXRESULT_PASS( IFXRESULT_BOUNDINGVOLUME, 0x0003 )
#define IFX_BV_SPLIT_NONE            MAKE_IFXRESULT_PASS( IFXRESULT_BOUNDINGVOLUME, 0x0004 )
#define IFX_BV_SINGLE_TRI            MAKE_IFXRESULT_PASS( IFXRESULT_BOUNDINGVOLUME, 0x0005 )
#define IFX_BV_MULTI_TRI             MAKE_IFXRESULT_PASS( IFXRESULT_BOUNDINGVOLUME, 0x0006 )

#include "IFXVector3.h"

/**
	This is the main interface for IFXBoundVolume - the interface from
	which all bounding volume objects used by the bounding hierarchy
	are derived.
*/
class IFXBoundVolume
{
public:
	virtual ~IFXBoundVolume() {}
	/**
		This method takes as input another bounding volume and tests it and
		the current one for overlap.  In the current implementation of the
		bounding hierarchy the bounding volumes are oriented bounding boxes.

		@param	pBound		A pointer an IFXBoundVolume.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		  
		- @b IFX_E_INVALID_POINTER is returned if an invalid pointer is passed for
		  pBoundVolume.\n
	*/
	virtual IFXRESULT IFXAPI  IntersectBound(IFXBoundVolume *pBound) = 0;


	/**
		This method takes as input another bounding volume and tests it and
		the current one for overlap using box/sphere overlap tests.  The 
		pBound passed in always refers to the sphere object.

		@param	pBound					A pointer an IFXBoundVolume.
		@param	fSphereRadiusSquared	An F32 representing the squared radius 
										of the sphere.
		@param	vSphereCentroid			An IFXVector3 reference to the sphere's 
										centroid.
		@param	vContactPoint			An IFXVector3 reference to the computed 
										point of contact.
		@param	vContactNormal			An IFXVector3 array pointing to the normal
										vectors computed at the point of contact.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		  
		- @b IFX_E_INVALID_POINTER is returned if an invalid pointer is passed for
		  pBoundVolume.\n
	*/
	virtual IFXRESULT IFXAPI  IntersectBoxSphere(IFXBoundVolume* pBound, 
										 F32             fSphereRadiusSquared,	
										 IFXVector3&     vSphereCentroid,
										 IFXVector3&     vContactPoint, 
										 IFXVector3      vContactNormal[2]) = 0;

	/**
		This method tests for an intersection of a three dimensional ray and 
		an IFXBoundVolume object.

		@param	vOrigin			An IFXVector3 reference to the ray's origin.
		@param	vDirection		An IFXVector3 reference to the ray's direction.
										point of contact.
		@param	uPickType		A U32 representing the picking mode.

				NONE - do not test this object
				FRONT - test for front facing triangle intersections.
				BACK  - test for back facing triangle intersections.
				BOTH  - test for front and back facing intersection.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		  
		- @b .\n
	*/
	virtual IFXRESULT IFXAPI  IntersectRay( IFXVector3 vOrigin, 
							  		IFXVector3 vDirection,
									U32        uPickType) = 0;

	/**
		This method tests for an intersection of a three dimensional ray and 
		and the triangles contained within the IFXBoundVolume.

		@param	vOrigin			An IFXVector3 reference to the ray's origin.
		@param	vDirection		An IFXVector3 reference to the ray's direction.
										point of contact.
		@param	uPickType		A U32 representing the picking mode.

				NONE - do not test this object
				FRONT - test for front facing triangle intersections.
				BACK  - test for back facing triangle intersections.
				BOTH  - test for front and back facing intersection.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		  
		- @b .\n
	*/	
	virtual IFXRESULT IFXAPI  IntersectRayTriangle(IFXVector3 vOrigin,
										   IFXVector3 vDirection,
										   U32        uPickType) = 0;

	/**
		This method takes as input another bounding volume and tests it and
		the current one for overlap.  In the current implementation of the
		bounding hierarchy the bounding volumes are oriented bounding boxes.

		@param	pBound		A pointer an IFXBoundVolume.

		@return	Returns an IFXRESULT code.\n
		
		- @b IFX_OK upon success.\n
		  
		- @b IFX_E_INVALID_POINTER is returned if an invalid pointer is passed for
		  pBoundVolume.\n
	*/	
	virtual	IFXRESULT IFXAPI  IntersectTriangle(IFXBoundVolume *pBound) = 0;
};

#endif
