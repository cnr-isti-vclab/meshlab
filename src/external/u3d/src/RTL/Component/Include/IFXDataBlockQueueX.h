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
/**	@file IFXDataBlockQueueX.h
		Declaration of IFXDataBlockQueueX exception-based interface.
		IFXDataBlockQueueX is used to hold a list of pointers to IFXDataBlockX.
**/
//*****************************************************************************
#ifndef IFXDATABLOCKQUEUEX_H__
#define IFXDATABLOCKQUEUEX_H__

#include "IFXUnknown.h"
#include "IFXDataBlockX.h"

//#define KEEP_DATA_BLOCKS

// {20DC6261-EF33-4352-9B4B-8D2313653CAA}
IFXDEFINE_GUID(IID_IFXDataBlockQueueX,
0x20dc6261, 0xef33, 0x4352, 0x9b, 0x4b, 0x8d, 0x23, 0x13, 0x65, 0x3c, 0xaa);

/**		Declaration of IFXDataBlockQueueX exception-based interface.
		IFXDataBlockQueueX is used to hold a list of pointers to IFXDataBlockX.
*/
class IFXDataBlockQueueX : public IFXUnknown
{
public:
	/// Clears out the data block list, releasing the contents.
	virtual void IFXAPI  ClearX() = 0;


	/** Gets the next data block from the list.
	@param rpDataBlockX A pointer to a data block is removed from the queue and placed in rpDataBlock.
	@param rbDone After removing the returned data block from the queue,
	rbDone is set to TRUE if the queue is empty is set to FALSE if the queue is not empty.
	@return Return is void.
	**/
	virtual void IFXAPI  GetNextBlockX(IFXDataBlockX*& rpDataBlockX, BOOL& rbDone) = 0;

	/// Peeks at the next block in the list without removing it from the list.
	virtual void IFXAPI  PeekNextBlockX(IFXDataBlockX*& rpDataBlockX) = 0;

	/// Adds a data block to the end of the list.
	virtual void IFXAPI  AppendBlockX(IFXDataBlockX& rDataBlockX) = 0;

	/// Gets a copy of the data block list.
	virtual void IFXAPI  CopyX(IFXDataBlockQueueX*& rpDataBlockQueueX) = 0;
};

//#define IFX_E_EMPTY_LIST			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_BITSTREAM, 0x0008 )

#endif // #ifndef IFXDATABLOCKQUEUEX_H__
