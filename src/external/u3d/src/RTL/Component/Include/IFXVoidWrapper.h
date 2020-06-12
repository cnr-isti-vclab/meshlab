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
// IFXVoidWrapper.h

#ifndef IFX_VOID_WRAPPER_H
#define IFX_VOID_WRAPPER_H

#include "IFXUnknown.h"

// {9A7A3E75-0C2A-4b46-BD1C-E624BFA0BD71}
IFXDEFINE_GUID(IID_IFXVoidWrapper,
0x9a7a3e75, 0xc2a, 0x4b46, 0xbd, 0x1c, 0xe6, 0x24, 0xbf, 0xa0, 0xbd, 0x71);

/**
 *	The IFXVoidWrapper objects will wrap an arbitrary object for the
 *	purposes of reference counting.  There is no type checking, so use
 *	at your own risk!
 */
class IFXVoidWrapper : virtual public IFXUnknown
{
public:
	/**
	 *	Set data to be held by the object.
	 *
	 *	@param	pvData	Void pointer value to keep.
	 *
	 *	@return	void
	 */
	virtual void  IFXAPI SetData(void* pvData)=0;

	/**
	 *	Retreive the void pointer value held by the object.
	 *
	 *	@return	The void* pointer value held by the object.
	 */
	virtual void* IFXAPI GetData() const=0;

protected:
	IFXVoidWrapper() {}				///< Constructor - empty
	virtual ~IFXVoidWrapper() {}	///< Destructor - empty
};

#endif // IFX_VOID_WRAPPER_H

// END OF FILE



