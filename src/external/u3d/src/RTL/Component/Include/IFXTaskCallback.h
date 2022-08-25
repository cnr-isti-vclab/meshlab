//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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
	@file	IFXTaskCallback.h

			This header defines the IFXTaskCallback interface and
			the associated interface identifier.

	@note	The IFXTaskCallback interface is supported by the 
			CIFXTaskCallback component.  Its component id is named 
			CID_IFXTaskCallback.
*/


#ifndef IFXTaskCallback_H
#define IFXTaskCallback_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXTask.h"


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


/**
	This is the prototype for the function that is called by a task 
	component that supports the IFXTaskCallback interface when it is 
	executed by the Scheduler.

	@note	The prototype signature should match the IFXTask::Execute 
			function.
*/
typedef IFXRESULT	( IFXTaskCallbackFunction )( 
						IFXTaskData*	pTaskData );


//***************************************************************************
//	Interfaces and interface identifiers
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This interface is used to define a callback function that will be 
	called whenever a task is executed by the Scheduler that supports this 
	interface.

	@note	The associated IID is named IID_IFXTaskCallback.
*/
class IFXTaskCallback : public IFXTask
{
	public:

		/**
			Function used to get the callback function pointer that will be 
			used when the IFXTask::Execute function is called.

			@return	Always returns the callback function pointer that is 
					stored.  It may be either a valid function pointer or 
					NULL.
		*/
		virtual	IFXTaskCallbackFunction* IFXAPI	GetFunction() const = 0;

		/**
			Function used to set the callback function pointer that will be 
			used when the IFXTask::Execute function is called.
			
			@param	pCallbackFunction	Callback function pointer to be used or NULL to 
										indicate that no function should be used.

			@return	Always succeeds so there is no return value.
		*/
		virtual	void IFXAPI SetFunction(
							IFXTaskCallbackFunction*	pCallbackFunction ) = 0;
};

/**
	This IID identifies the IFXTaskCallback interface.

	@note	The GUID string is {C2375BC3-54E1-4f4c-87E3-6B983FBA3791}.
*/
IFXDEFINE_GUID(IID_IFXTaskCallback,
0xc2375bc3, 0x54e1, 0x4f4c, 0x87, 0xe3, 0x6b, 0x98, 0x3f, 0xba, 0x37, 0x91);


#endif
