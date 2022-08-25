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
	@file  IFXRenderable.h

			The header file that defines the IFXRenderable interface.                
*/


#ifndef IFXRENDERABLE_H
#define IFXRENDERABLE_H

#include "IFXUnknown.h"
#include "IFXVector4.h"

#include "IFXShaderList.h"

// {F13F15B8-302E-47EA-9A73-97E976ABC253}
IFXDEFINE_GUID(IID_IFXRenderable,
0xF13F15B8, 0x302E, 0x47EA, 0x9A, 0x73, 0x97, 0xE9, 0x76, 0xAB, 0xC2, 0x53);

/**
	The IFXRenderable interface supplies the services needed to support
	dispatching of abstract data types to IFXShaders                      
*/
class IFXRenderable : virtual public IFXUnknown
{
public:
	/**
		Used to do a copy of a renderable so that portions of the
		renderable may be copied seamlessly.

		@param rSrcRenderable	The Renderable to copy from.

		@param isCopyElements	If this is true all of the elements are copied.
								If it is false the elements will be referenced
								from the previous data packet.

		@param isCopyShaders	If this is true all of the shaders are copied. 
								If it is false the Shaders will be referenced
								from the previous data packet.

		@return An IFXRESULT value.
	*/
	virtual IFXRESULT IFXAPI TransferData( 
								IFXRenderable& rSrcRenderable,
								BOOL isCopyElements, 
								BOOL isCopyShaders ) = 0;

	/**
		Returns a BOOL describing whether the data will be rendered.
	*/
	virtual BOOL IFXAPI GetEnabled( void )=0;

	/**
		Controls whether or not the data will be rendered.

		@param  isEnabled	The BOOL that determines whether or not the data 
							will be rendered.

		@return An IFXRESULT value.
		@retval  IFX_OK   No error.
	*/
	virtual IFXRESULT IFXAPI SetEnabled( BOOL isEnabled )=0;

	/**
		Returns a reference to the IID that the renderable elements support.

		@return An IFXRESULT value.
		@retval  IFX_OK   No error.
	*/
	virtual const IFXIID& IFXAPI GetElementType( void )=0;

	/**
		Returns the number of renderable elements in the IFXRenderable.
	*/
	virtual U32 IFXAPI GetNumElements( void )=0;


	/**
		Returns the ShaderList of a specific renderable element.

		@param  index
				The index of the renderable element for which we want the
				shader set.

		@param  ppOutShaderList
				The returned shader set associated with this renderable element.

		@return An IFXRESULT value.
		@retval  IFX_OK   No error.
		@retval  IFX_E_INVALID_RANGE  The index index is larger than the
									renderable element count
	*/
	virtual IFXRESULT IFXAPI GetElementShaderList( 
										U32  index,
										IFXShaderList** ppOutShaderList) = 0;

	/**
		Sets the shader sets for a specific renderable element.

		@param  index
				The index of the renderable element for which we will assign
				a new shader set.

		@param  pInShaderList
				The shader set to assign to this renderable element.

		@return An IFXRESULT value.
		@retval  IFX_OK   No error.
		@retval  IFX_E_INVALID_RANGE	The index index is larger than the
										renderable element count
	*/
	virtual IFXRESULT IFXAPI SetElementShaderList( 
										U32 index,
										IFXShaderList* pInShaderList) = 0;

	/**  
		Returns the bounding sphere of this renderable element 
	*/
	virtual const IFXVector4& IFXAPI CalcBoundingSphere() = 0;

	/**
		Returns the array of shader sets associated with this renderable (for example,
		if this mesh group contains three meshes, then this call will return an
		array of three shader sets).
	*/
	virtual IFXRESULT IFXAPI GetShaderDataArray(IFXShaderList*** pppShaderArray) = 0;
};


#endif
