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
	@file	IFXShadingModifier.h

			This header defines the IFXShadingModifier class and its functionality.
*/

#ifndef IFXSHADINGMODIFIER_H
#define IFXSHADINGMODIFIER_H

#include "IFXUnknown.h"
#include "IFXModifier.h"
#include "IFXShaderList.h"

// {FEA5D3C8-4BF4-4735-AD2B-A94ECB5EA44A}
IFXDEFINE_GUID(IID_IFXShadingModifier,
			   0xfea5d3c8, 0x4bf4, 0x4735, 0xad, 0x2b, 0xa9, 0x4e, 0xcb, 0x5e, 0xa4, 0x4a);


/**
	This is the main interface for IFXShadingModifier.

	@note	The associated IID is named IID_IFXShadingModifier.
*/
class IFXShadingModifier : virtual public IFXModifier
{
public:

	enum EIFXAttributes
	{
		MESH = 1,
		LINE = 1 << 1,
		POINT = 1 << 2,
		GLYPH = 1 << 3
	};
	/**
	This method returns the value of the enabled flag.

	@param  pbEnable  A BOOL pointer indicating the state of the enabled flag.

	- @b TRUE - The modifier is enabled.\n
	- @b FALSE  - The modifier is disabled.

	@return IFXRESULT\n

	@retval IFX_OK         - No Error.
	@retval IFX_E_NOT_INITIALIZED  - Bad value of passed in pointer.
	*/
	virtual IFXRESULT IFXAPI  GetEnable     (BOOL* pbEnable)   = 0;

	/**
	This method sets the value of the enabled flag.

	@param  bEnable A BOOL pointer indicating the state of the enabled flag.

	- @b TRUE - Enabled the modifier.\n
	- @b FALSE  - Disable the modifier.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  SetEnable     (BOOL  bEnable)    = 0;

	/**
	This method gets shader set for particular renderable lement.

	@param  uInElement Index of element in renderable group.
	@param  out_ppShaderList A pointer to the needed shader set.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  GetElementShaderList( U32 uInElement, IFXShaderList** out_ppShaderList )    = 0;

	/**
	This method sets shader set for particular renderable lement.

	@param  uInElement Index of element in renderable group.
	@param  in_ppShaderList A pointer to the shader set.
	@param  isValid If TRUE and in_pShaderList is NULL then it means that
			shading modifier is a pass-through modifier for this
			renderable element. If FALSE and in_pShaderList is NULL then
			shading data will be cut at uInElement. If TRUE and
			in_pShaderList is not NULL then shader list is applied to the
			selected renderable element. If FALSE and in_pShaderList
			is not NULL then shader list is applied to all renderable elements
			from 0 to uInElement.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	@retval  IFX_E_INVALID_RANGE  The uInElement index is larger than the
	renderable element count
	*/
	virtual IFXRESULT IFXAPI  SetElementShaderList( U32 uInElement, IFXShaderList* in_pShaderList, BOOL isValid = TRUE )    = 0;

	/**
	This method gets shader sets for entire renderable group.

	@param  pShaderArray A pointer to shading group.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	@retval  IFX_E_INVALID_RANGE  The uInElement index is larger than the
	renderable element count
	*/
	virtual IFXRESULT IFXAPI GetShaderDataArray(IFXShaderList*** pShaderArray)    = 0;

	/**
	This method gets shading attributes of modifier.

	@param  shadingAttributes desired value of the attributes.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  GetAttributes(U32* pShadingAttributes)    = 0;

	/**
	This method sets shading attributes of modifier.

	@param  shadingAttributes desired value of the attributes.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  SetAttributes(U32 shadingAttributes)    = 0;

	/**
	This method gets size of shading group.

	@return IFXRESULT\n

	@retval IFX_OK - No Error.
	*/
	virtual IFXRESULT IFXAPI  GetSize(U32* size)    = 0;
};

#endif
