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

@file  IFXDummyModifier.h

	The header file that defines the IFXDummyModifier interface. */

#ifndef __IFXDUMMYMODIFIER_H__
#define __IFXDUMMYMODIFIER_H__

#include "IFXUnknown.h"
#include "IFXModifier.h"
#include "IFXDataBlockQueueX.h"

// {8EF3BB86-7573-4f32-91A6-57427FCCCE2C}
IFXDEFINE_GUID(IID_IFXDummyModifier,
0x8ef3bb86, 0x7573, 0x4f32, 0x91, 0xa6, 0x57, 0x42, 0x7f, 0xcc, 0xce, 0x2c);

/**
	This interface describes Dummy Modifier which is used as placeholder inside Modifier
	Chain in case if decoder isn't registered for extension modifier. In this case 
	Dummy Modifier is created inside M.C. and data block with data associated with this 
	modifier is stored here. Then - during encoding process - this data is written back 
	into new file as it is.
*/
class IFXDummyModifier : virtual public IFXModifier
{
public:
	/** 
		Sets Data Blocks with data which wasn't properly decoded. It will be written
		to file during encoding process without any changes.
	*/
	virtual IFXRESULT IFXAPI SetDataBlock( IFXDataBlockQueueX *pDataBlock ) = 0;

	/**
		Returns pointer to Data Block Queue
	*/
	virtual IFXDataBlockQueueX* IFXAPI GetDataBlock() = 0;
};

#endif

