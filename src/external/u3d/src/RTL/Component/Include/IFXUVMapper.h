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
	@file	IFXUVMapper.h

			Interface header file for the UV texture coordinate mapper classes.
*/
#ifndef __IFXUVMAPPER_INTERFACE_H__
#define __IFXUVMAPPER_INTERFACE_H__

#include "IFXUnknown.h"
#include "IFXUVGenerator.h"		// for IFXUVMapParameters parameter list

// {A5F0E2C4-188F-11d4-A121-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXUVMapper,
0xa5f0e2c4, 0x188f, 0x11d4, 0xa1, 0x21, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

/**
 *	The IFXUVMapper object is responsible for actually generating texture
 *	coordinates on a mesh.  These should not be used directly, rather
 *	they are owned by implementations of IFXUVGenerator.
 */
class IFXUVMapper : public IFXUnknown 
{
public:
	/**
	 *	Generate texture coordinates in @a pMesh.
	 *
	 *	@param	pMesh	The mesh that will contain the generated
	 *					texture coordinates.
	 *	@param	pMapParams	Pointer to an IFXUVMapParameters object
	 *						that contains more detailed parameters
	 *						to the texture coordinate generation function.
	 *	@param	pModelMatrix	The local to world matrix of @a pMesh.
	 *	@param	pViewMatrix		The world to local matrix of the current
	 *							IFXView object (camera).
	 *	@param	pLightSet	Pointer to an IFXLightSet that contains all
	 *						of the lights in the scene that affect @a pMesh.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	One or more of the required
	 *									parameters were NULL.
	 *	@retval	IFX_E_INVALID_RANGE	One of the values contained in
	 *								@a pMapParams was invalid.
	 */
	virtual IFXRESULT IFXAPI  Apply(	IFXMesh& pMesh,
								IFXUVMapParameters* pMapParams,
								IFXMatrix4x4* pModelMatrix,
								IFXMatrix4x4* pViewMatrix,
								const IFXLightSet* pLightSet)=0;
};

#endif
