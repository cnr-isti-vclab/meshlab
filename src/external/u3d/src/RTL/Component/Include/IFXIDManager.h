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
// IFXIDManager.h
#ifndef IFX_IDMANAGER_H
#define IFX_IDMANAGER_H

#include "IFXUnknown.h"

// {3240F1E7-3756-47f1-A2C2-11B1B8FB13F5}
IFXDEFINE_GUID(IID_IFXIDManager,
0x3240f1e7, 0x3756, 0x47f1, 0xa2, 0xc2, 0x11, 0xb1, 0xb8, 0xfb, 0x13, 0xf5);

/** 
    Interface to the CIFXIDManager component.  This class distributes and recycles ID numbers for any purpose.
*/
class IFXIDManager : virtual public IFXUnknown
{
public:
	//==============================
	// IFXIDManager methods
	//==============================
	/**
	 *	Resets the ID manger to its initial state.  This will remove all knowledge of
	 *	previously registered ID numbers.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval IFX_OK No error.
	 */
	virtual IFXRESULT IFXAPI Clear()=0;

	/**
	 *  Retrieves a new ID number.  This will register the number returned in @a uOutId
	 *	as being used.  It will not be returned from GetId() again until it has been
	 *	released with ReleaseId().  GetId() will never return the ID number 0.
	 *
	 *	@param uOutId	Output U32 to receive the new ID number.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval IFX_OK No error.
	 */
	virtual IFXRESULT IFXAPI GetId(U32& uOutId)=0;

	/**
	 *  Releases a previously registered ID number (registered with GetID()).
	 *
	 *	@param	uInId	Input U32 containing the ID number to release.
	 *
	 *	@return	An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	@a uInId held value 0 or a number
	 *								that had not been allocated yet.
	 */
	virtual IFXRESULT IFXAPI ReleaseId(U32 uInId)=0;


	/**
	 *	Output debug information on allocated IDs.
	 */
	virtual void IFXAPI OutputAllocatedIds()=0;

//protected:
//	IFXIDManager() {}
//	~IFXIDManager() {}
};
typedef IFXSmartPtr<IFXIDManager> IFXIDManagerPtr;


#endif // #ifndef IFX_IDMANAGER_H
