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
	@file	IFXSimpleList.h

			This header defines the IFXSimpleList class and its functionality.
*/

#ifndef __IFXSIMPLELIST_INTERFACES_H__
#define __IFXSIMPLELIST_INTERFACES_H__


#include "IFXUnknown.h"

// {139C63DA-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXSimpleList,
0x139c63da, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

/**
	This is the main interface for IFXSimpleList.

	@note	The associated IID is named IID_IFXSimpleList.
*/

class IFXSimpleList : public IFXUnknown 
{
public:

	/**
		This method initializes IFXSimpleList with the given size.

		@param	uInitialSize	U32 indicating the initial list size.

		@return	IFXRESULT\n

		- @b IFX_OK						-	No error.\n
		- @b IFX_E_ALREADY_INITIALIZED	-	A valid list already exists.\n
		- @b IFX_E_INVALID_RANGE		-	Passed is size is zero.\n
		- @b IFX_E_OUT_OF_MEMORY		-	Memory allocation failure.\n
	*/
	virtual IFXRESULT IFXAPI  Initialize(U32 uInitialSize)=0;

	/**
		This method adds an IFXUnknown object.

		@param	pObject		An IFXUnknown pointer to the object.
		@param	pIndex		A U32 pointer indicating the position at which
							the object was added.
		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.\n
		- @b IFX_E_OUT_OF_MEMORY	-	Memory allocation failure.\n
	*/
	virtual IFXRESULT IFXAPI  Add(IFXUnknown* pObject, U32* pIndex)=0;

	/**
		This method deletes the IFXUnknown object at uIndex.

		@param	uIndex		A U32 indicating the object's position.

		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_NOT_INITIALIZED	-	The list has not been initialized.
		- @b IFX_E_INVALID_RANGE	-	Index is outside valid range.\n
	*/
	virtual IFXRESULT IFXAPI  Delete(U32 uIndex)=0;

	/**
		This method returns the IFXUnknown object at uIndex without
		deleting it from the list.

		@param	ppObject	An IFXUnknown pointer to the object.
		@param	uIndex		A U32 indicating the object's position.
		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.\n
		- @b IFX_E_INVALID_RANGE	-	Index is outside valid range.\n
		- @b IFX_E_NOT_INITIALIZED	-	The internal list has not been initialized.
	*/
	virtual IFXRESULT IFXAPI  Get(U32 uIndex, IFXUnknown** ppObject)=0;

	/**
		This method sets the IFXUnknown object at uIndex.  Any object at this
		index is overwritten.

		@param	uIndex		A U32 indicating the object's position.
		@param	pObject		An IFXUnknown pointer to the object.
		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.\n
		- @b IFX_E_INVALID_RANGE	-	Index is outside valid range.\n
		- @b IFX_E_NOT_INITIALIZED	-	The internal list has not been initialized.
	*/
	virtual IFXRESULT IFXAPI  Set(U32 uIndex, IFXUnknown* pObject)=0;

	/**
		This method inserts the IFXUnknown object at uIndex.  Any object at this
		index is moved before the new object is inserted.

		@param	uIndex		A U32 indicating the object's position.
		@param	pObject		An IFXUnknown pointer to the object.
		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.\n
		- @b IFX_E_INVALID_RANGE	-	Index is outside valid range.\n
		- @b IFX_E_NOT_INITIALIZED	-	The internal list has not been initialized.
	*/
	virtual IFXRESULT IFXAPI  Insert(U32 uIndex, IFXUnknown* pObject)=0;

	/**
		This method returns the size of the list.

		@param	pCount		A U32 pointer indicating the list's size.
		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.\n
	*/
	virtual IFXRESULT IFXAPI  GetCount(U32* pCount)=0;

	/**
		This method copies the contents of the internal list to pDestList.

		@param	pDestList	An IFXSimpleList pointer indicating the destination list.
		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.\n
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.\n
		- @b IFX_E_NOT_INITIALIZED	-	The internal list has not been initialized.
	*/
	virtual IFXRESULT IFXAPI  Copy(IFXSimpleList* pDestList)=0;
};

#endif
