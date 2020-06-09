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
	@file	IFXProgressCallback.h

			This header file defines the IFXProgressCallback interface and its functionality.
*/

#ifndef IFXProgressCallback_H
#define IFXProgressCallback_H

#include "IFXDataTypes.h"

/**
	This is the main interface for IFXProgressCallback.
*/
class IFXProgressCallback
{
public:
	virtual ~IFXProgressCallback() {}
	/**
		Informs the calling app of the maximum value of the update it will be called with.

		@param	max	The maximum value.\n
	*/
	virtual void IFXAPI  InitializeProgress( F32 max ) = 0;

	/**
		Informs how much progress has been made. Objects implementing this
		interface need to return TRUE to continue processing or FALSE to cancel processing.

		@param	progress	The current progress.\n

		@return	Returns a boolean result.\n
		- @b IFX_TRUE to continue processing.\n
		- @b IFX_FALSE to cancel processing.\n
	*/
	virtual BOOL IFXAPI  UpdateProgress( F32 progress ) = 0;
};


#endif
