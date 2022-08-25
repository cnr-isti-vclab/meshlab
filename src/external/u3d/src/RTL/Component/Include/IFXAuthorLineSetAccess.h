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
	@file	IFXAuthorLineSetAccess.h

			Declaration of IFXAuthorLinesetAccess interface
*/

#ifndef __IFXAuthorLineSetAccess_H__
#define __IFXAuthorLineSetAccess_H__

// {94D3034F-81B7-4071-A2AE-6C6C9942AFC6}
IFXDEFINE_GUID(IID_IFXAuthorLineSetAccess, 
0x94d3034f, 0x81b7, 0x4071, 0xa2, 0xae, 0x6c, 0x6c, 0x99, 0x42, 0xaf, 0xc6);

class IFXAuthorLineSetAccess
{
public:
	virtual ~IFXAuthorLineSetAccess() {}
	/**
		Sets a pointer for array of positions and size of array.

		@param	pPositions	Array of positions.
		@param	size		Size of array.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
					following failures are returned:  IFX_E_INVALID_POINTER.
	*/
	virtual	IFXRESULT IFXAPI SetPositionArray(IFXVector3* pPositions, U32 size) = 0;

	/**
		Sets a pointer for array of normals and size of array.

		@param	pNormals	Array of normals.
		@param	size		Size of array.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
					following failures are returned:  IFX_E_INVALID_POINTER.
	*/
	virtual	IFXRESULT IFXAPI SetNormalArray(IFXVector3* pNormals, U32 size) = 0;

	/**
		Sets a pointer for array of diffuse colors and size of array.

		@param	pDiffuseColors	Array of diffuse colors.
		@param	size		Size of array.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
					following failures are returned:  IFX_E_INVALID_POINTER.
	*/
	virtual	IFXRESULT IFXAPI SetDiffuseColorArray(IFXVector4* pDiffuseColors, U32 size) = 0;

	/**
		Sets a pointer for array of specular colors and size of array.

		@param	pSpecularColors	Array of specular colors.
		@param	size			Size of array.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
					following failures are returned:  IFX_E_INVALID_POINTER.
	*/
	virtual	IFXRESULT IFXAPI SetSpecularColorArray(IFXVector4* pSpecularColors, U32 size) = 0;

	/**
		Sets a pointer for array of texture coordinates and size of array.

		@param	pTexCoors	Array of texture coordinates.
		@param	size		Size of array.

		@return	Upon success, IFX_OK is returned.  Otherwise, one of the
					following failures are returned:  IFX_E_INVALID_POINTER.
	*/
	virtual	IFXRESULT IFXAPI SetTexCoordArray(IFXVector4* pTexCoors, U32 size) = 0;
};
#endif
