//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// IFXSimpleHash.h
#ifndef IFX_SIMPLEHASH_H
#define IFX_SIMPLEHASH_H

#include "IFXUnknown.h"

// {E8151F85-ED11-4b5e-A299-567967AD8169}
IFXDEFINE_GUID(IID_IFXSimpleHash,
0xe8151f85, 0xed11, 0x4b5e, 0xa2, 0x99, 0x56, 0x79, 0x67, 0xad, 0x81, 0x69);

/**
 *  This is the interface to CIFXSimpleHash.  The simple hash class is a light-weight
 *	hash table containing references to IFXUnknown (or IFXUnknown derived) objects.
 *	Objects are hashed via a simple U32 key.  The internal table size is always a size
 *	of the form: (2^n)-1.  This allows a very simple hashing function.
 */
class IFXSimpleHash : virtual public IFXUnknown
{
public:
	//==============================
	// IFXSimpleHash methods
	//==============================
	/**
	 *  Initialze the hash to the specified table size.  The actual implemented table
	 *	size will be the closest size to @a uTableSize that is of the form: (2^n)-1.
	 *	Calling initialize after data is stored in the table will release all stored
	 *	data.
	 *
	 *	@param	uTableSize	Input U32 specifying the desired table size.
	 *
	 *	@return	An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_OUT_OF_MEMORY	There was an error in allocating space
	 *								for the table.
	 */
	virtual IFXRESULT IFXAPI Initialize(U32 uTableSize)=0;

	/**
	 *  Removes all stored data from the table.  This is accomplished by deleting the
	 *	existing table and allocating a new table.
	 *
	 *	@return	An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_OUT_OF_MEMORY	There was an error in allocating space
	 *								for the table.
	 */
	virtual IFXRESULT IFXAPI Clear()=0;

	/**
	 *  Add a new IFXUnknown derived object to the hash.  If the specified key @a uId
	 *	has already been used, the old data associated with that key is released.
	 *
	 *	@param	uId		Input U32 specifying the ID number to be
	 *					used as the key for hashing.
	 *	@param	pUnk	Input smart pointer reference to an IFXUnknown object
	 *					to be added to the table.  This object will be AddRef()'ed.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_OUT_OF_MEMORY	The object could not be added to the table
	 *								because there was a memory allocation error.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI AddData(U32 uId, IFXUnknownPtr& spUnk)=0;

	/**
	 *  Add a new IFXUnknown derived object to the hash.  If the specified key @a uId
	 *	has already been used, the old data associated with that key is released.
	 *
	 *	@param	uId		Input U32 specifying the ID number to be
	 *					used as the key for hashing.
	 *	@param	pUnk	Input pointer to an IFXUnknown object
	 *					to be added to the table.  This object will be AddRef()'ed.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_OUT_OF_MEMORY	The object could not be added to the table
	 *								because there was a memory allocation error.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 *	@retval	IFX_E_INVALID_POINTER	The specified @a pUnk is NULL.
	 */
	virtual IFXRESULT IFXAPI AddData(U32 uId, IFXUnknown* pUnk)=0;

	/**
	 *	Retrieve an object associated with key @a uId.  The hash will
	 *	still hold a reference to the object after this method returns.
	 *	The smart pointer, @a spUnk, will also hold a reference to this
	 *	object when the method returns.
	 *
	 *	@param	uId		Input U32 Id number associated with the object
	 *					to retrieve.
	 *
	 *	@param	spUnk	Output smart pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object held in the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	The specified @a uId does
	 *								not have an object associated with it.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI GetData(U32 uId, IFXUnknownPtr& spUnk)=0;

	/**
	 *	Retrieve an object associated with key @a uId.  The hash will
	 *	still hold a reference to the object after this method returns.
	 *	The pointer, @a pUnk, will also hold a reference to this
	 *	object when the method returns.
	 *
	 *	@param	uId		Input U32 Id number associated with the object
	 *					to retrieve.
	 *
	 *	@param	pUnk	Output pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object held in the hash.  You must call Release()
	 *					on this object when you are done with it!
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	The specified @a uId does
	 *								not have an object associated with it.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI GetData(U32 uId, IFXUnknown*& pUnk)=0;

	/**
	 *	Retrieve the object stored in the hash that has the lowest id.
	 *	This is useful for doing insertion sort operations on a set of
	 *	data.
	 *
	 *	@param uId	Output U32 reference to hold the Id of the object
	 *				retrieved.
	 *
	 *	@param	spUnk	Output smart pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object held in the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	There are no objects in the hash.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI GetLowestId(U32& uId, IFXUnknownPtr& spUnk)=0;

	/**
	 *	Retrieve the object stored in the hash that has the lowest id.
	 *	This is useful for doing insertion sort operations on a set of
	 *	data.
	 *
	 *	@param uId	Output U32 reference to hold the Id of the object
	 *				retrieved.
	 *
	 *	@param	pUnk	Output pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object held in the hash.  You must call Release()
	 *					on this object when you are done with it!
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	There are no objects in the hash.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI GetLowestId(U32& uId, IFXUnknown*& pUnk)=0;

	/**
	 *	Retrieves an object specified with @a uId and places it into the
	 *	smart pointer reference @a spUnk.  The object is then removed
	 *	from the hash and the reference to the object held by the hash
	 *	is Release()'ed.
	 *
	 *	@param	uId		Input U32 Id number associated with the object
	 *					to retrieve.
	 *
	 *	@param	spUnk	Output smart pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object that was held in the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	The specified @a uId does
	 *								not have an object associated with it.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI ExtractData(U32 uId, IFXUnknownPtr& spUnk)=0;

	/**
	 *	Retrieves an object specified with @a uId and places it into the
	 *	smart pointer reference @a spUnk.  The object is then removed
	 *	from the hash and the reference to the object held by the hash
	 *	is Release()'ed.
	 *
	 *	@param	uId		Input U32 Id number associated with the object
	 *					to retrieve.
	 *
	 *	@param	pUnk	Output pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object that was held in the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	The specified @a uId does
	 *								not have an object associated with it.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI ExtractData(U32 uId, IFXUnknown*& pUnk)=0;

	/**
	 *	Retrieve the object stored in the hash that has the lowest id.
	 *	This is useful for doing insertion sort operations on a set of
	 *	data.  When this method is called, the object will be removed
	 *	from the hash, and the reference to the object held by the hash
	 *	will be Release()'ed.
	 *
	 *	@param uId	Output U32 reference to hold the Id of the object
	 *				retrieved.
	 *
	 *	@param	spUnk	Output smart pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object that was held in the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	There are no objects in the hash.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI ExtractLowestId(U32& uId, IFXUnknownPtr& spUnk)=0;

	/**
	 *	Retrieve the object stored in the hash that has the lowest id.
	 *	This is useful for doing insertion sort operations on a set of
	 *	data.  When this method is called, the object will be removed
	 *	from the hash, and the reference to the object held by the hash
	 *	will be Release()'ed.
	 *
	 *	@param uId	Output U32 reference to hold the Id of the object
	 *				retrieved.
	 *
	 *	@param	pUnk	Output pointer reference to and IFXUnknown object.
	 *					This will receive the pointer to the IFXUnknown
	 *					object that was held in the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	There are no objects in the hash.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI ExtractLowestId(U32& uId, IFXUnknown*& pUnk)=0;

	/**
	 *	This will cause the hash to release the object specified with @a uId.
	 *
	 *	@param uId	Input U32 specifying the Id of the object to be released
	 *				from the hash.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_CANNOT_FIND	The specified @a uId does
	 *								not have an object associated with it.
	 *	@retval	IFX_E_NOT_INITIALIZED	The Initialize() method has
	 *									not been called successfully yet.
	 */
	virtual IFXRESULT IFXAPI RemoveData(U32 uId)=0;

protected:
};
typedef IFXSmartPtr<IFXSimpleHash> IFXSimpleHashPtr;

#endif // IFX_SIMPLEHASH_H

// END OF FILE

