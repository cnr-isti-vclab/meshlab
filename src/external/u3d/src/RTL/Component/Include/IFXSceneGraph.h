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

@file  IFXSceneGraph.h

	The header file that defines the IFXSceneGraph interface.                */

#ifndef __IFXSCENEGRAPH_H__
#define __IFXSCENEGRAPH_H__

#include "IFXUnknown.h"
#include "IFXMarker.h"
#include "IFXSubject.h"

class IFXCoreServices;
class IFXPalette;

/// The interface ID (IFXIID) for the IFXSceneGraph interface.
// {7A298619-8B01-11d3-8467-00A0C939B104}
IFXDEFINE_GUID(IID_IFXSceneGraph,
0x7a298619, 0x8b01, 0x11d3, 0x84, 0x67, 0x0, 0xa0, 0xc9, 0x39, 0xb1, 0x4);


class IFXSceneGraph : virtual public IFXUnknown
{
public:
	/**
	 *	The set of palettes managed by the IFXSceneGraph.
	 */
	enum EIFXPalette
	{
		MATERIAL,
		GENERATOR,
		SHADER,
		TEXTURE,
		MOTION,
		NODE,	// groups, models, lights, views
		MIXER,
		SIM_TASK,
		VIEW,
		LIGHT,
		NUMBER_OF_PALETTES,
		FILE_REFERENCE = 100 // this palette actually exists in core services object
	};


	/**
	 *	Retrieves the specified palette.
	 *
	 *	@param	palette	One of the palettes in the EIFXPallete enum.
	 *
	 *	@param	ppPalette	The address of an IFXPallete interface pointer to
	 *	                    contain the address of the requested palette.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of ppPalette was 0.
	 */
	virtual IFXRESULT IFXAPI  GetPalette(EIFXPalette palette, IFXPalette** ppPalette)=0;



	/**
	 *	IFXSceneGraph debugging flags.
	 */
	enum EIFXSceneGraphDebugFlags
	{
		GROUP_FRAMES  = 1<<1,
		LIGHTS        = 1<<2,
		LIGHT_BOUNDS  = 1<<3,
		LIGHT_FRAMES  = 1<<4,
		LIGHT_LINKS   = 1<<5,
		MODEL_BOUNDS  = 1<<6,
		MODEL_FRAMES  = 1<<7,
		NODE_BOUNDS   = 1<<8,
		VIEW_FRAMES   = 1<<9,
		VIEW_FRUSTUMS = 1<<10,
		POINT_CLOUD   = 1<<11,
		WIRE_FRAME    = 1<<12,
		FLAT_SHADED   = 1<<13,
		BONES		  = 1<<14,
		ALL           = 0xFFFFFFFF
	};



	/**
	 *	Retrieves the debug flags.
	 *
	 *	@param	puOutDebugFlags	The address of a U32 to contain the debug flags.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a puOutDebugFlags was 0.
	 */
	virtual IFXRESULT IFXAPI  GetDebugFlags(U32* puOutDebugFlags)=0;



	/**
	 *	Sets the debug flags.
	 *
	 *	@param	uInDebugFlags	A U32 to be used as the debug flags.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetDebugFlags(U32 uInDebugFlags)=0;



	/**
	 *	Retrieves the value to be used to mark objects in the IFXSceneGraph.
	 *
	 *	@return	A U32 value.
	 */
	virtual U32       IFXAPI  CurrentMark(void)=0;



	/**
	 *	Unmarks all objects in the IFXSceneGraph.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  UnmarkAll(void)=0;



	/**
	 *	Initializes the IFXSceneGraph.  This must be called before any other
	 *	method is used.
	 *
	 *	@param	pCS	The address of the associated IFXCoreServices.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a pCS was 0.
	 */
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *pCS)=0;



	/**
	 *	Retrieves the address of the associated IFXCoreServices.
	 *
	 *	@param	ppCS	The address of a IFXCoreServices pointer to contain the
	 *					address of the associated IFXCoreServies.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a ppCS was 0.
	 */
	virtual IFXRESULT IFXAPI  GetCoreServices(IFXCoreServices **ppCS)=0;



	/**
	 *	Retrieves the boolean state of procedural animation.
	 *
	 *	@param	pState	The address of a boolean to contain the state value.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a pState was 0.
	 */
	virtual IFXRESULT IFXAPI  GetAnimationEnabledState( BOOL* pState ) = 0;



	/**
	 *	Sets the boolean state of procedural animation.
	 *
	 *	@param	state	A boolean value to be used as the animation state.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
    virtual IFXRESULT IFXAPI  SetAnimationEnabledState( BOOL state ) = 0;

	/**
	 *	Retrieves an IFXSubject interface from the associated simulation clock.
	 *
	 *	@param	ppOutClockSubject	The address of a IFXSubject pointer
	 *								to contain a pointer to the IFXSubject
	 *								interface of the associated simulation clock.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a ppOutClockSubject was 0.
	 */
	virtual IFXRESULT IFXAPI  GetSimClockSubject(IFXSubject** ppOutClockSubject)=0;

	/** Recursively marks a subset of the database.*/
	virtual IFXRESULT IFXAPI  Mark( void )=0;

	/** Determines whether a component is marked. */
	virtual IFXRESULT IFXAPI  Marked( BOOL* pbOutMarked )=0;

	/**Set the streaming priority ( zero = initial load segment )
	The two default, optional arguments are used in the following way:
	- by default, bRecursive == TRUE, indicating that components should
	propagate the SetPriority() call to any subordinate (child)
	components. If bRecursive == FALSE, the SetPriority() only applies
	to the component upon which the method was called (i.e. local only).
	- by default, bPromotionOnly == TRUE, indicating that the specified
	priority (uInPriority) is only applied if it is a greater priority
	(lower U32) than the current priority of the component.  In other
	words, by default, you cannot lower a components priority.  If
	bPromotionOnly == FALSE, then the component accepts the new priority
	irregardless of its value relative to its old priority.*/
	virtual void IFXAPI  SetPriority( U32 uInPriority,
							  BOOL bRecursive = IFX_TRUE,
							  BOOL bPromotionOnly = IFX_TRUE )=0;
};


#endif


