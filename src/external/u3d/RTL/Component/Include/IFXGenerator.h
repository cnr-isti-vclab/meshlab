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
	@file	IFXGenerator.h

			This header defines the IFXGenerator interface, its functionality
			and the associated interface identifier.
*/

#ifndef __IFXGenerator_H__
#define __IFXGenerator_H__

#include "IFXUnknown.h"
#include "IFXMatrix4x4.h"
#include "IFXMarker.h"
#include "IFXModifier.h"


IFXDEFINE_GUID(IID_IFXGenerator,
0xB23105F5, 0x760B, 0x4DBB, 0x83, 0x75, 0x71, 0x08, 0x77, 0x63, 0x19, 0xF7);

/**
	This is the main interface for IFXGenerator.

	@note	The associated IID is named IID_IFXGenerator.
*/
class IFXGenerator : virtual public IFXModifier,
					 virtual public IFXMarker,
					 virtual public IFXUnknown
{
public:

	/**
		This method returns the generators transform.

		@return	IFXMatrix4x4 A reference to the transform.
	*/
	virtual const IFXMatrix4x4& IFXAPI GetTransform()=0;

	/**
		This method returns the generators transform.

		@param	IFXMatrix4x4 A reference to the transform.

		@return	IFXRESULT\n

		- @b IFX_OK	-	No problems.\n
	*/
	virtual IFXRESULT IFXAPI SetTransform(const IFXMatrix4x4& tInTransform)=0;

	/**
		This method invalidates the generators transform in the modifier chain.

		@return	IFXRESULT\n

		- @b IFX_OK	-	No problems.\n
	*/
	virtual IFXRESULT IFXAPI InvalidateTransform()=0;
};


#endif
