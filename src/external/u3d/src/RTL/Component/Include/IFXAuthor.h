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
	@file	IFXAuthor.h

     Declaration of IFXAuthor structures, common for all generators
*/

#ifndef __IFXAUTHOR_H__
#define __IFXAUTHOR_H__

#include "IFXDataTypes.h"
#include "IFXEnums.h"
#include <memory.h>

const U32 IFX_BAD_GEN_INDEX32 = 0xFFFFFFFF;

/** 
	Initializes an arbitrary structure to 0.

	@note	Use memset to initialize an array of structures. It
	iterates the array in case the structures
	are aligned to arbitrary boundries. 
*/
template<class T>
inline void IFXInitStruct(T* p, U32 in_Cnt = 1)
{
	if (p)
	{
		U32 i;
		for(i = 0; i < in_Cnt; ++i)
		{
			memset(p + i, 0, sizeof(T));
		}
	}
}


/**
	Describes the properties of a material used by a mesh, line set or point set.
*/
struct IFXAuthorMaterial
{
	IFXAuthorMaterial();
	
	///The number of texture layers used by this material
	U32 m_uNumTextureLayers;

	/**
		This array gives the dimensions of the texture coordinates used for 
		each texture layer in the material.  Texture coordinates can 
		range from 1-dimensional to 4-dimensional. 
	*/
	U32 m_uTexCoordDimensions[IFX_MAX_TEXUNITS]; 

	/**
		If the array of IFXAuthorMaterials gets smaller because the scrubber 
		removed unused materials and repacked them, this field gives the
		original array index of the material record.  
	*/
	U32 m_uOriginalMaterialID;  
	
	/// True if this material specifies diffuse vertex colors.
	BOOL m_uDiffuseColors;

	/// True if this material specifies specular vertex colors.
	BOOL m_uSpecularColors;

	/**
		Set to TRUE if any of the faces in this material use normals, 
		FALSE if no faces use normals.
	*/
	BOOL m_uNormals;
};

IFXINLINE IFXAuthorMaterial::IFXAuthorMaterial()
{
	IFXInitStruct(this);
	m_uNormals = TRUE;
}

#endif
