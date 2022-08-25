//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXSubdivModifier.h

			This header defines the IFXSubdivModifier class and its functionality.
*/

#ifndef __IFXSUBDIVMODIFIER_H__
#define __IFXSUBDIVMODIFIER_H__

#include "IFXUnknown.h"
#include "IFXModifier.h"

// {B37553B7-1C5E-11d4-94DC-00A0C9A0FBAE}
IFXDEFINE_GUID(IID_IFXSubdivModifier,
0xb37553b7, 0x1c5e, 0x11d4, 0x94, 0xdc, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

/**
	This is the main interface for IFXSubdivModifier.

	@note	The associated IID is named IID_IFXSubdivModifier.
*/
class IFXSubdivModifier : virtual public IFXModifier
{
public:

	/**
		This method returns the value of the enabled flag.

		@param	pbEnable	A BOOL pointer indicating the state of the enabled flag.

		- @b TRUE	-	The modifier is enabled.\n
		- @b FALSE	-	The modifier is disabled.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No Error.
		- @b IFX_E_NOT_INITIALIZED	-	Bad value of passed in pointer.
	*/
    virtual IFXRESULT IFXAPI  GetEnable     (BOOL* pbEnable)   = 0;

	/**
		This method sets the value of the enabled flag.

		@param	bEnable	A BOOL pointer indicating the state of the enabled flag.

		- @b TRUE	-	Enabled the modifier.\n
		- @b FALSE	-	Disable the modifier.

		@return	IFXRESULT\n

		- @b IFX_OK	-	No Error.
	*/
     virtual IFXRESULT IFXAPI  SetEnable     (BOOL  bEnable)    = 0;

 	/**
		This method returns the current subdivision depth.

		@param	pDepth	A U32 pointer indicating the depth.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No Error.
		- @b IFX_E_NOT_INITIALIZED	-	Bad value of passed in pointer.

	*/
    virtual IFXRESULT IFXAPI  GetDepth      (U32*  pDepth)     = 0;

	/**
		This method sets the subdivision depth.

		@param	depth	A U32 indicating the depth.

		@return	IFXRESULT\n

		- @b IFX_OK	-	No Error.
	*/
 	virtual IFXRESULT IFXAPI  SetDepth      (U32   depth)      = 0;

 	/**
		This method returns the current subdivision tension.

		@param	pTension	A F32 pointer indicating the tension.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No Error.
		- @b IFX_E_NOT_INITIALIZED	-	Bad value of passed in pointer.

	*/
    virtual IFXRESULT IFXAPI  GetTension    (F32*  pTension)   = 0;

 	/**
		This method sets the subdivision tension.

		@param	tension	A F32 indicating the tension.

		@return	IFXRESULT\n

		- @b IFX_OK	-	No Error.
	*/
	virtual IFXRESULT IFXAPI  SetTension    (F32   tension)    = 0;

  	/**
		This method returns the current error in the screen space metric.

		@param	pError	A F32 pointer indicating the error.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No Error.
		- @b IFX_E_NOT_INITIALIZED	-	Bad value of passed in pointer.
	*/
	virtual IFXRESULT IFXAPI  GetError      (F32*  pError)     = 0;

  	/**
		This method sets the error in the screen space metric.

		@param	error	A F32 indicating the error.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No Error.
	*/
	virtual IFXRESULT IFXAPI  SetError      (F32   error)      = 0;

 	/**
		This method returns the value of the adaptive flag.

		@param	pbAdaptive	A BOOL pointer indicating the state of the adaptive flag.

		- @b TRUE	-	Adaptive subdivision is enabled.\n
		- @b FALSE	-	Uniform subdivision is enabled.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No Error.
		- @b IFX_E_NOT_INITIALIZED	-	Bad value of passed in pointer.

	*/
    virtual IFXRESULT IFXAPI  GetAdaptive   (BOOL* pbAdaptive) = 0;

	/**
		This method sets the value of the adaptive flag.

		@param	bAdaptive	A BOOL indicating the state of the adaptive flag.

		- @b TRUE	-	Enable adaptive subdivision.\n
		- @b FALSE	-	Disable uniform subdivision.

		@return	IFXRESULT\n

		- @b IFX_OK	-	No Error.
	*/
     virtual IFXRESULT IFXAPI  SetAdaptive   (BOOL  bAdaptive)  = 0;
};


#endif
