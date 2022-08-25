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

#ifndef __IFXLIGHT_H__
#define __IFXLIGHT_H__

#include "IFXNode.h"
#include "IFXLightResource.h"

class IFXRenderContext;

/**
	The interface ID for the IFXLight class.
	4517C9F4-B31E-11d3-94B1-00A0C9A0FBAE
*/
IFXDEFINE_GUID(IID_IFXLight,
0x4517c9f4, 0xb31e, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

/**
 *	The IFXLight object encapsulates a light source within a scene.  The
 *	light, like any other IFXNode object, has a transform relative to a
 *	parent node, and can have children nodes.  The light can be one of
 *	four types:
 *		- Ambient: The light provides uniform non-directional light to
 *			all objects in the visible scene.
 *		- Point: The light has a specific light source located at the
 *			center of it's local coordinate space.
 *		- Directional: The light affects all object of the scene
 *			uniformly.  Light from this source appears to each object as
 *			coming from the same direction relative to the world.
 *		- Spot: The light acts as a spot light, having a distinct location
 *			in the world, and only affecting a conical area in the scene
 *			relative to that position.
 */
class IFXLight : virtual public IFXNode,
                 virtual public IFXSpatial
{
public:
	/**
	 *	Disables this light in the given IFXRenderContext instance.
	 *
	 *	@param	pRenderContext	Input IFXRenderContext in which to disable
	 *							this light.
	 *	@param	uInLightId	This is the light ID (0 - 7) that this light
	 *						is associated with in @a pRenderContext.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	The value of @a pRenderContext
	 *									was invalid (NULL).
	 *	@retval	IFX_E_INVALID_RANGE	The value provided for @a uInLightId
	 *								was not in the range of 0 - 7.
	 */
	virtual void IFXAPI  Disable(IFXRenderContext* pRenderContext, U32 uInLightID)=0;

	/**
	 *	Enables this light in the given IFXRenderContext instance.  All
	 *	current settings of this light will be associated with the given
	 *	light ID in @a pRenderContext, and the light will be enabled
	 *	for use in rendering.
	 *
	 *	@param	pRenderContext	Input IFXRenderContext in which to enable
	 *							this light.
	 *	@param	uInLightId	This is the light ID (0 - 7) that this light
	 *						will be associated with in @a pRenderContext.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	The value of @a pRenderContext
	 *									was invalid (NULL).
	 *	@retval	IFX_E_INVALID_RANGE	The value provided for @a uInLightId
	 *								was not in the range of 0 - 7.
	 */
	virtual void IFXAPI  Enable( IFXRenderContext* pRenderContext, U32* puInLightID, U32 lightInstance)=0;

	virtual IFXLightResource* IFXAPI GetLightResource( void )=0;
	virtual IFXRESULT IFXAPI  GetLightResourceID(U32*)=0;
	virtual IFXRESULT IFXAPI  SetLightResourceID(U32)=0;
};


#endif
