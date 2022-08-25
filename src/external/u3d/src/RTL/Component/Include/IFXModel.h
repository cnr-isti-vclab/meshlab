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
	@file	IFXModel.h

			This header defines the IFXModel class and its functionality.
*/

#ifndef __IFXMODEL_H__
#define __IFXMODEL_H__

#include "IFXNode.h"
#include "IFXResourceClient.h"
#include "IFXSpatialAssociation.h"
#include "IFXFrustum.h"
#include "IFXShaderList.h"

// {4517C9F5-B31E-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(IID_IFXModel,
0x4517c9f5, 0xb31e, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

#define FRONT_FACE_VISIBILITY 0
#define BACK_FACE_VISIBILITY  1

class IFXLightSet;

/**
	This is the main interface for IFXModel.

	@note	The associated IID is named IID_IFXModel.
*/
class IFXModel : virtual public IFXNode,
				 virtual public IFXResourceClient,
				 virtual public IFXSpatialAssociation,
                 virtual public IFXSpatial
{
public:

	/**
		Returns the value of the debug flag.

		@return BOOL\n

 		- @b TRUE	-	Debug geometry is visible.\n
		- @b FALSE	-	Debug geometry is not visible.
	*/
	virtual BOOL      IFXAPI GetDebug( void )=0;

	/**
		Set the value of the debug flag.

		@param	bVisible	TRUE - turn debug on, FALSE - turn debug off
		@return void\n
	*/
	virtual void      IFXAPI SetDebug( BOOL bVisible )=0;

	/**
		Returns the value of the visibility flag.

		@return U32\n

 		- @b 0	-	Not visible.\n
		- @b 1	-	Front visible.\n
		- @b 2	-	Back visible.\n
		- @b 3	-	Front and back visible.\n
	*/
	virtual U32       IFXAPI GetVisibility( void )=0;

	/**
		Sets the value of the visibility flag.

		@param	uInVisible	\n

		- @b 0	-	Not visible.\n
		- @b 1	-	Front visible.\n
		- @b 2	-	Back visible.\n
		- @b 3	-	Front and back visible.\n

		@return void\n
 	*/
	virtual void      IFXAPI SetVisibility( U32 uInVisible )=0;

	/**
		Returns the value of the pickable flag.

		@return U32\n

 		- @b 0	-	Not pickable.\n
		- @b 1	-	Front pickable.\n
		- @b 2	-	Back pickable.\n
		- @b 3	-	Front and back pickable.\n
	*/
	virtual U32       IFXAPI GetPickable()=0;

	/**
		Sets the value of the pickable flag.

		@param	uPickable	\n

		- @b 0	-	Not pickable.\n
		- @b 1	-	Front pickable.\n
		- @b 2	-	Back pickable.\n
		- @b 3	-	Front and back pickable.\n

		@return void\n
 	*/
	virtual void      IFXAPI SetPickable(U32 uPickable)=0;

	/**
		Returns the current view frustum.

		@return IFXFrustum	A reference to the frustum.\n
	*/
	virtual const IFXFrustum& IFXAPI GetFrustum( void ) const=0;

	/**
		Sets the current view frustum.

		@param IFXFrustum	A reference to the frustum.\n

		@return void
	*/
	virtual IFXRESULT IFXAPI SetFrustum( const IFXFrustum& rInFrustum )=0;

	/**
		Returns the current light set.

		@return IFXLightSet	A reference to the light set.\n
	*/
	virtual const IFXLightSet& IFXAPI GetLightSet()=0;

	/**
		Returns the current view size - computed as (width<<16) | height.

		@return U32	An unsigned integer representing the current view size.\n
	*/
	virtual U32       IFXAPI GetViewSize( void )=0; //

	/**
		Sets the current view size.

		@param uInViewSize	A U32 representing the view size.\n

		@return void
	*/
	virtual IFXRESULT IFXAPI SetViewSize( U32 uInViewSize )=0;

	/**
		Sets the styleMap index.

		@param	uResourceIndex		U32 indicating the styleMap index.
		@return IFXRESULT\n

		- @b IFX_OK	-	No problems.\n
	*/
	virtual IFXRESULT IFXAPI SetResourceIndex( U32 uResourceIndex ) = 0;

	virtual IFXRESULT IFXAPI GetSpatialBound(    IFXVector4&        rOutSphere, U32 WorldInstance )=0;
};

#endif
