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
	@file	IFXMarker.h

			The IFXMarker interface provides i/o control of components within the
			scenegraph database.

	@note	If Mark() or Marked() is called on a component that has not been
			initialized(), an IFX_E_NOT_INITIALIZED error will be returned.

*/

#ifndef __IFXMARKER_H__
#define __IFXMARKER_H__

#include "IFXUnknown.h"
#include "IFXMetaDataX.h"

class IFXSceneGraph;

IFXDEFINE_GUID(IID_IFXMarker,
0xa67dbd21, 0x622, 0x11d4, 0x89, 0x5, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

/**
	The IFXMarker interface provides i/o control of components within the 
	scenegraph database.

	@note	If Mark() or Marked() is called on a component that has not been 
			initialized(), an IFX_E_NOT_INITIALIZED error will be returned.
*/

class IFXMarker : virtual public IFXMetaDataX
{
public:

	/** Declares database ownership. */
	virtual IFXRESULT IFXAPI SetSceneGraph( IFXSceneGraph* pInSceneGraph )=0;

	/** Returns the scenegraph that was last passed into Initialize(). */
	virtual IFXRESULT IFXAPI GetSceneGraph( IFXSceneGraph** ppOutSceneGraph)=0;

	/** Recursively marks a subset of the database.*/
	virtual IFXRESULT IFXAPI  Mark( void )=0;

	/** Determines whether a component is marked. */
	virtual IFXRESULT IFXAPI  Marked( BOOL* pbOutMarked )=0;

	/** Gets the streaming sequence relative to other components. */
	virtual U32       IFXAPI  GetPriority( void )=0;

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
	virtual void      IFXAPI  SetPriority( U32 uInPriority,
								   BOOL bRecursive = IFX_TRUE,
								   BOOL bPromotionOnly = IFX_TRUE )=0;

	/** Sets flag to indicate if object was loaded from external
	referenced file or not. */
	virtual void IFXAPI  SetExternalFlag(BOOL bExternalFlag) = 0;

	/** Returns flag indicating if object was loaded from external
	refernced file. */
	virtual void IFXAPI  GetExternalFlag(BOOL* bExternalFlag) = 0;
};


#endif
