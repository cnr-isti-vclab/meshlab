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

@file  IFXLightSet.h

	The header file that defines the IFXLightSet interface.                  */

#ifndef __IFXLIGHTSET_H__
#define __IFXLIGHTSET_H__

#include "IFXUnknown.h"
#include "IFXSpatial.h"
#include "IFXLight.h"

/// The interface ID (IFXIID) for the IFXLightSet interface.
// {09ACD7E3-5816-42b6-98E4-20292BE4F86C}
IFXDEFINE_GUID(IID_IFXLightSet,
0x9acd7e3, 0x5816, 0x42b6, 0x98, 0xe4, 0x20, 0x29, 0x2b, 0xe4, 0xf8, 0x6c);

/**
 *	The IFXLightSet interface exposes a number of methods to manage a set of
 *	semantically similar lights.  Typically, this is a set of lights that
 *  contribute to the illumination of a set of surfaces.
 */
class IFXLightSet : virtual public IFXUnknown
{
public:
	/**
	 *	Sets the lights in the set.
	 *
	 *	@param	uInCount	The number of light pointers in the ppInLight list.
	 *
	 *	@param	ppInLights	The address of the first of @a uInCount IFXLight
	 *	                    interface pointers.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a ppInLights
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI  Set( U32 uInCount, SPATIALINSTANCE_LIST** ppInLights )          =0;



	/**
	 *	Adds a number of lights to the light set.
	 *
	 *	@param	uInCount	The number of light pointers in the ppInLight list.
	 *
	 *	@param	ppInLights	The address of the first of @a uInCount IFXLight
	 *	                    interface pointers.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The value specified for @a ppInLights
	 *									was 0.
	 */
	virtual IFXRESULT IFXAPI  Append( U32 uInCount, SPATIALINSTANCE_LIST** ppInLights )       =0;



	/**
	 *	Gets the number of lights in the light set.
	 *
	 *	@return	A U32 value that is the number of lights in the light set.
	 */
	virtual U32       IFXAPI  GetNumLights( void ) const                            =0;



	/**
	 *	Gets a light from the light set.
	 *
	 *	@param	uLightIndex	The number of light to return.
	 *
	 *	@param	rpOutLight	A reference to an IFXLight interface pointer to
	 *	                    contain a pointer to the specified light in the
	 *	                    light set.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The value specified for @a uLightIndex
	 *								was greater than the number of lights
	 *								in the light set.
	 */
	virtual IFXRESULT IFXAPI  GetLight( U32 uLightIndex, IFXLight*& rpOutLight, U32& lightInstance ) const =0;
};


#endif
