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
	@file	IFXShaderList.h

			IFXShaderList interface.
*/

#ifndef __IFXSHADERLIST_H__
#define __IFXSHADERLIST_H__

#include "IFXUnknown.h"

// {FEF98CBD-6D18-4C7E-A6C0-B5A8137DB061}
IFXDEFINE_GUID(IID_IFXShaderList,
0xFEF98CBD, 0x6D18, 0x4C7E, 0xA6, 0xC0, 0xB5, 0xA8, 0x13, 0x7D, 0xB0, 0x61);

#define  IFXSHADERLIST_CLEAR_ENTRY   (U32)-1
#define  IFXSHADERLIST_DEFAULT_VALUE (U32)-1
#define  IFXSHADERLIST_HARD_END (U32)-2

/**
	IFXShaderList manages renderable element shaders. The ShaderList
	is basically an array of shaders indexed from 0 to N
	It has some special behavior:\n

	Setting a shader to IFXSHADERLIST_CLEAR_ENTRY will cause the shader list to
	shrink to N if the entry being cleared is at the end.\n

	Setting Shader (N, S) will cause list to grow to size N.
	Any entries added will be padded to 0 (the default shader ID).
*/

class IFXShaderList : virtual public IFXUnknown 
{
public:
	// IFXShaderList
	/**
	*  Allocates a shader set and initializes all entries to a default value.
	*
	*  @param in_Size        The size of the shader set to allocate.
	*  @param DefaultValue   The default value to assign to all entries
	*                        in the shader set.
	*/
	virtual IFXRESULT IFXAPI  Allocate(U32 in_Size, U32 DefaultValue = 0) = 0;
	
	/**
	*  Copies the specified shader set into this shader set.  The copy
	*  operation causes this shader set to be reallocated to match the
	*  input shader set, as well as mirroring all values between the two
	*  shader sets.
	*
	*  @param in_pSrc  The shader set to copy into this shader set.
	*/
	virtual IFXRESULT IFXAPI  Copy(IFXShaderList* in_pSrc) = 0;
	
	/**
	*  Overlays the source shader set onto this shader set.
	*
	*  @param in_pSource   The shader set to overlay onto this shader set.
	*/
	virtual IFXRESULT IFXAPI  Overlay(IFXShaderList* in_pSource) = 0;
	
	/**
	*  Compares this shader set to another shader set
	*
	*  @param in_pSource   The shader set to compare against this shader set.
	*
  	*  @return An IFXRESULT value.
	*  @retval IFX_OK   The two shaders sets are identical.
	*  @retval IFX_E_UNDEFINED  The shader sets are not identical.
	*/
	virtual IFXRESULT IFXAPI  Equals(IFXShaderList* in_pSource) = 0;

	/**
	*  Returns the number of shaders in the shader set.
	*/
	virtual U32 IFXAPI  GetNumShaders()  = 0;
	
	/**
	*  Sets the number of shaders in the shader set.  This operation
	*  causes a reallocation.
	*
	*  @param in_Size   The new size of the shader set
	*/
	virtual IFXRESULT IFXAPI  SetNumShaders(U32 in_Size)  = 0;
	
	/**
	*  Returns the number of shaders in the shader set that are not
	*  set to the IFXSHADERLIST_DEFAULT_VALUE value.
	*/
	virtual U32 IFXAPI  GetNumActualShaders()  = 0;
	
	/**
	*  Sets one of shader set array elements to a specific value.
	*
	*  @param in_Idx   The entry in the shader set to assign new value.
	*  @param in_Shader  The Shader ID to assign to this element of the
	*     shader set (typically an index in the shader palette).
	*
	*  @note  If in_Idx is a value greater than GetNumShaders(), the
	*     shader set will be reallocated with enough space to hold
	*     all the shader IDs in between.  If in_Idx is less than
	*     GetNumShaders(), but in_Shader equals IFXSHADERLIST_HARD_END,
	*     the shader set if resized so that in_Idx forms the end of
	*     the shader set.  If in_Idx is less than GetNumShaders(),
	*     but in_Shader equals IFXSHADERLIST_DEFAULT_VALUE, the shader set
	*     is shrunk to remove all shader set entries that contain
	*     IFXSHADERLIST_DEFAULT_VALUE from the end of the list.
	*/
	virtual IFXRESULT IFXAPI  SetShader(U32 in_Idx, U32 in_Shader) = 0;
	
	/**
	*  Sets the value stored in the shader set at a given array index.
	*
	*  @param in_Idx   The index of the shader set entry from which we
	*     wish the value.
	*  @param out_pShader  The returned shader ID stored in the shader set.
	*
  	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_INDEX	in_Idx is larger than GetNumShaders()
	*/
	virtual IFXRESULT IFXAPI  GetShader(U32 in_Idx, U32* out_pShader) = 0;
};


#endif
