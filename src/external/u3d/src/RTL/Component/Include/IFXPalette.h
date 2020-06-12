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
	@file	IFXPalette.h

	This header file defines the IFXPalette class and its functionality.
*/

#ifndef __IFXPALETTE_H__
#define __IFXPALETTE_H__


#include "IFXUnknown.h"
#include "IFXString.h"

class IFXModel;

// {D12EC0A1-ADD7-11d3-A0E9-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXPalette,
			   0xd12ec0a1, 0xadd7, 0x11d3, 0xa0, 0xe9, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

// IFXSubject Aspect Bits
#define	IFX_SUBJECT_PALETTE_ADD				1
#define IFX_SUBJECT_PALETTE_REMOVE			2
#define IFX_SUBJECT_PALETTE_ADD_HIDDEN		4
#define IFX_SUBJECT_PALETTE_REMOVE_HIDDEN	8
#define IFX_SUBJECT_PALETTE_ADD_RESOURCE	16
#define IFX_SUBJECT_PALETTE_REMOVE_RESOURCE	32

#define IFX_E_PALETTE_INVALID_ENTRY				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_PALETTE_MANAGER, 0X0001)
#define IFX_E_PALETTE_NULL_RESOURCE_POINTER		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_PALETTE_MANAGER, 0X0002)

#define IFX_NULL_RESOURCE   (U32)-1

/**
This is the main interface for IFXPalette.

@note	The associated IID is named IID_IFXPalette.
*/
class IFXPalette : public IFXUnknown
{
public:
	// IFXPalette methods

	/**
	This method initializes the palette with an initial size and a grow size.

	@param	uInitialSize	A U32 indicating the initial palette size.
	@param	uGrowthSize		A U32 indicating the size by which the palette will grow
	when the initial size has been exhausted.
	@return	IFXRESULT\n

	- @b IFX_OK						-	No error.
	- @b IFX_E_ALREADY_INITIALIZED	-	Internal palette already initialized.
	- @b IFX_E_INVALID_RANGE		-	Initial input size is zero.
	- @b IFX_E_OUT_OF_MEMORY		-	Failed memory allocation.
	*/
	virtual IFXRESULT IFXAPI  Initialize(U32 uInitialSize,U32 uGrowthSize=0)=0;

	/**
	This method adds an entry to the palette.

	@param	pPaletteEntryName	Pointer to the name for 
	the palette entry.
	@param	pPaletteEntryId		A U32 pointer indicating the index
	where the entry was added.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_CHANGE	-	Trying to add an entry at reserved index zero.
	- @b IFX_E_OUT_OF_MEMORY	-	Failed memory allocation.

	@todo	Remove this deprecated helper method.
	*/
	virtual IFXRESULT IFXAPI  Add(const IFXCHAR* pPaletteEntryName, U32* pPaletteEntryId)=0;

	/**
	This method adds an entry to the palette.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@param	pPaletteEntryId		A U32 pointer indicating the index
	where the entry was added.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_CHANGE	-	Trying to add an entry at reserved index zero.
	- @b IFX_E_OUT_OF_MEMORY	-	Failed memory allocation.
	*/
	virtual IFXRESULT IFXAPI  Add(IFXString* pPaletteEntryName, U32* pPaletteEntryId)=0;

	/**
	This function adds a new entry to the palette marked as hidden.  Hidden
	entries are not exposed to the author and not written to the file. If an
	entry with the given name already exists, a unique name is created.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@param	pPaletteEntryId		A U32 pointer indicating the index
	where the entry was added.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_CHANGE	-	Trying to add an entry at reserved index zero.
	- @b IFX_E_OUT_OF_MEMORY	-	Failed memory allocation.

	@note	If it was necessary to create a unique name for the new hidden 
	palette entry, keep track of the entry id handed back in case 
	it is needed later.  Find function will not be able to locate 
	the entry using the same name specified to this function because 
	it is not aware of the unique entry name that was actually used.
	*/
	virtual IFXRESULT IFXAPI  AddHidden(IFXString* pInPaletteEntryName, U32* pOutPaletteEntryId)=0;

	/**
	This function returns the hidden status of an entry.

	@param	uInIndex	A U32 to the specified palette index.
	@param	pbOutHidden	A BOOL pointer specifying the hidden status.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	*/
	virtual IFXRESULT IFXAPI  IsHidden(U32 uInIndex, BOOL* pbOutHidden)=0;

	/**
	This function walks through the palette and tries to locate an entry with
	the specified name.  It returns the index of the entry added, or 0 zero if
	it couldn't find it.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@param	pPaletteEntryId		A U32 pointer indicating the index
	where the entry was found; zero is
	returned if the entry is not found.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.

	@todo	Remove this deprecated helper method.
	*/
	virtual IFXRESULT IFXAPI  Find(const IFXCHAR* pPaletteEntryName, U32* pPaletteEntryId)=0;

	/**
	This function walks through the palette and tries to locate an entry with
	the specified name.  It returns the index of the entry added, or 0 zero if
	it couldn't find it.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@param	pPaletteEntryId		A U32 pointer indicating the index
	where the entry was found; zero is
	returned if the entry is not found.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.
	*/
	virtual IFXRESULT IFXAPI  Find(const IFXString* pPaletteEntryName, U32* pPaletteEntryId)=0;

	/**
	This function walks through the palette and tries to locate an entry with
	the specified IFXUnknown pointer.  It returns the index of the entry added,
	or 0 zero if it couldn't find it.

	@param	pPointer	An IFXUnknown pointer to the desired object.
	@param	pIndex		A U32 pointer indicating the index
	where the entry was found; zero is
	returned if the entry is not found.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.
	*/
	virtual IFXRESULT IFXAPI  FindByResourcePtr(IFXUnknown* pPointer, U32* pIndex)=0;

	/**
	Returns the name of a given palette entry.

	@param	uPaletteEntryId		A U32 for the desired palette entry.
	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@param	pNameLength			A U32 pointer to the length of the name.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.

	@todo	Remove this deprecated helper method.
	*/
	virtual IFXRESULT IFXAPI  GetName(U32 uPaletteEntryId, IFXCHAR* pPaletteEntryName, U32* pNameLength)=0;

	/**
	Returns the name of a given palette entry.

	@param	uPaletteEntryId		A U32 for the desired palette entry.
	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	*/
	virtual	IFXRESULT IFXAPI  GetName(U32 uPaletteEntryId, IFXString* pPaletteEntryName)=0;


	/**
	This method returns the current number of items in the palette.

	@param	pPaletteSize		A U32 for representing the number to items in the
	palette.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	*/
	virtual IFXRESULT IFXAPI  GetPaletteSize(U32* pPaletteSize)=0;

	/**
	Delete's a specific palette entry by name.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.

	@todo	Remove this deprecated helper method.
	*/
	virtual IFXRESULT IFXAPI  DeleteByName(const IFXCHAR* pPaletteEntryName)=0;

	/**
	Delete's a specific palette entry by name.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.
	*/
	virtual IFXRESULT IFXAPI  DeleteByName(IFXString* pPaletteEntryName)=0;

	/**
	Delete's a specific palette entry by index.

	@param	uPaletteEntryId	A U32 to the desired palette entry.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	*/
	virtual IFXRESULT IFXAPI  DeleteById(U32 uPaletteEntryId)=0;

	/**
	This function returns the resource pointer for an entry in the palette.

	@param	uIndex		A U32 for the desired palette entry.
	@param	ppObject	An IFXUnknown pointer to the object.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	- @b IFX_E_PALETTE_NULL_RESOURCE_POINTER	-	Resource pointer is invalid.
	*/
	virtual IFXRESULT IFXAPI  GetResourcePtr(U32 uIndex, IFXUnknown** ppObject)=0;

	/**
	This function returns the resource pointer for an entry in the palette.
	In addition it returns the object with desired interface.

	@param	uIndex		A U32 for the desired palette entry.
	@param	interfaceId	An IFXREFIID - interface ID.
	@param	ppObject	An IFXUnknown pointer to the object.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	- @b IFX_E_PALETTE_NULL_RESOURCE_POINTER	-	Resource pointer is invalid.
	*/
	virtual IFXRESULT IFXAPI  GetResourcePtr(U32 uIndex, IFXREFIID interfaceId, void** ppObject)=0;

	/**
	This function sets the resource pointer for a palette entry.

	@param	uIndex		A U32 for the desired palette entry.
	@param	pObject		An IFXUnknown pointer to the object.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	- @b IFX_E_CANNOT_CHANGE	-	Trying to add an entry at reserved index zero.
	*/
	virtual IFXRESULT IFXAPI  SetResourcePtr(U32 uIndex, IFXUnknown* pObject)=0;

	/**
	This function conditionally adds a new entry to the palette.

	@param	pPaletteEntryName	Pointer to the name for
	the palette entry.
	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_OUT_OF_MEMORY	-	Memory allocation failure.
	*/
	virtual IFXRESULT IFXAPI  SetDefault(IFXString* pPaletteEntryName)=0;

	/**
	This function sets the default resource pointer.

	@param	pObject		An IFXUnknown pointer to the object.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	- @b IFX_E_CANNOT_CHANGE	-	Trying to add an entry at reserved index zero.
	*/
	virtual IFXRESULT IFXAPI  SetDefaultResourcePtr(IFXUnknown* pObject)=0;

	/**
	Delete's the default palette entry.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_NOT_INITIALIZED	-	Internal palette has not been initialized.
	- @b IFX_E_CANNOT_FIND		-	Entry is not found in the palette.
	- @b IFX_E_INVALID_RANGE	-	Passed in index is out of range.
	*/
	virtual IFXRESULT IFXAPI  DeleteDefault()=0;

	/**
	Set the observer watching the resource in uNewIndex.

	@param	uCurrentIndex	A U32 representing the current watched palette position.
	@param	uNewIndex		A U32 representing the new palette position to watch.
	@param	pModel			An IFXModel pointer indicating the model observing the resource.

	@return	IFXRESULT\n

	- @b IFX_OK					-	No error.
	- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
	- @b IFX_E_OUT_OF_MEMORY	-	Memory allocation failure.
	*/
	virtual IFXRESULT IFXAPI  SetResourceObserver(U32 uCurrentIndex, U32 uNewIndex, IFXModel* pModel)=0;

	/**
	This method takes a palette index and returns the index of the first element
	in the palette.  It returns 0 if there is no elements in the palette

	@param	pID	A U32 indicating the index for to the first palette entry.

	@return	IFXRESULT\n

	- @b IFX_OK							-	No error. \n
	- @b IFX_E_INVALID_POINTER			-	Invalid pointer passed in.\n
	- @b IFX_E_PALETTE_INVALID_ENTRY	-	Palette entry is invalid.\n

	*/
	virtual IFXRESULT IFXAPI  First(U32* pID)=0;

	/**
	This method takes a palette index and returns the index of the last element
	in the palette.  It returns 0 if there is no elements in the palette

	@param	pID	A U32 indicating the index for to the last palette entry.

	@return	IFXRESULT\n

	- @b IFX_OK							-	No error. \n
	- @b IFX_E_INVALID_POINTER			-	Invalid pointer passed in.\n
	- @b IFX_E_PALETTE_INVALID_ENTRY	-	Palette entry is invalid.\n

	*/
	virtual IFXRESULT IFXAPI  Last(U32* pID)=0;

	/**
	This method takes a palette index and returns the index of the next element
	in the palette.  It returns 0 if there is no elements in the palette

	@param	pID	A U32 indicating the index for to the next palette entry.

	@return	IFXRESULT\n

	- @b IFX_OK							-	No error. \n
	- @b IFX_E_INVALID_POINTER			-	Invalid pointer passed in.\n
	- @b IFX_E_PALETTE_INVALID_ENTRY	-	Palette entry is invalid.\n

	*/
	virtual IFXRESULT IFXAPI  Next(U32* pID)=0;

	/**
	This method takes a palette index and returns the index of the previous element
	in the palette.  It returns 0 if there is no elements in the palette

	@param	pID	A U32 indicating the index for to the previous palette entry.

	@return	IFXRESULT\n

	- @b IFX_OK							-	No error. \n
	- @b IFX_E_INVALID_POINTER			-	Invalid pointer passed in.\n
	- @b IFX_E_PALETTE_INVALID_ENTRY	-	Palette entry is invalid.\n

	*/
	virtual IFXRESULT IFXAPI  Previous(U32* pID)=0;
};


#endif
