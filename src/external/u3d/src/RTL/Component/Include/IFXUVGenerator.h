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
	@file	IFXUVGenerator.h

			Interface header file for the texture coordinate generator.
*/

#ifndef __IFXUVGENERATOR_H__
#define __IFXUVGENERATOR_H__

#include "IFXUnknown.h"
#include "IFXMatrix4x4.h"
#include "IFXEnums.h"

class IFXLightSet;
class IFXMesh;

// {E1969931-B25D-11d3-A0EB-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXUVGenerator,
0xe1969931, 0xb25d, 0x11d3, 0xa0, 0xeb, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);


/**
	Contains parameters to an IFXUVGenerator implementation.  These are
	used to control various forms of texture coordinate generation.
 */
class IFXUVMapParameters
{
public:
	IFXenum				eWrapMode;			///< the wrap mode
	IFXenum				eOrientation;		///< use the specified vertex or normals for orientation
	IFXenum				uTextureLayer;		///< the texture layer to build
	IFXMatrix4x4		mWrapTransformMatrix; ///< Pre texcoordgen model transform.
	F32					fShininess;			///< Specular power

	/// Constructor
	IFXUVMapParameters()
	{
		Reset();
	}

	/// Set data members to default values.
	void Reset()
	{
		eWrapMode = 0xffffffff;
		eOrientation = 0xffffffff;
		uTextureLayer = IFX_MAX_TEXUNITS;
		mWrapTransformMatrix.Reset();
		fShininess = 0;
	}
};

/**
 *	This object will generate texture coordinates for one texture layer
 *	of an IFXMesh object.  The algorithm for generating the texture
 *	coordinates is dependent on the implementation of this interface.
 */
class IFXUVGenerator : public IFXUnknown
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
	virtual IFXRESULT IFXAPI  Generate(	IFXMesh& pMesh,
								IFXUVMapParameters* pMapParams,
								IFXMatrix4x4* pModelMatrix,
								IFXMatrix4x4* pViewMatrix,
								const IFXLightSet* pLightSet )=0;
};

#endif
