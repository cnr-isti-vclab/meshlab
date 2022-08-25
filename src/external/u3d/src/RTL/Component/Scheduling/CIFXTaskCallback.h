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
	@file	CIFXTaskCallback.h

			This module defines the CIFXTaskCallback component.
*/


#ifndef CIFXTaskCallback_H
#define CIFXTaskCallback_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXTaskCallback.h"


//***************************************************************************
//	Classes, structures and types
//***************************************************************************

/**
	This is the implementation of a component that can be registered as a 
	task which will call a client defined callback function whenever the 
	task is executed.

	It supports the following interfaces:  IFXTaskCallback, IFXTask and 
	IFXUnknown.

	@note	This component's id is named CID_IFXTaskCallback.

	@note	This component can be instantiated multiple times.
*/
class CIFXTaskCallback : public IFXTaskCallback
{
public:

	// Member functions:  IFXUnknown.
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID	interfaceId,
											void**		ppInterface );
	// Member functions:  IFXTask.
	IFXRESULT	IFXAPI	Execute( IFXTaskData*	pTaskData );

	// Member functions:  IFXTaskCallback.
	IFXTaskCallbackFunction* IFXAPI GetFunction() const;
	void	IFXAPI	SetFunction( IFXTaskCallbackFunction*	pCallbackFunction );

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXTaskCallback_Factory( 
												IFXREFIID	interfaceId,
												void**		ppInterface );
private:

	// Member functions.
			CIFXTaskCallback();
	virtual	~CIFXTaskCallback();

	// Member data.
	U32						 m_refCount;			///< Number of outstanding references to the component.
	IFXTaskCallbackFunction* m_pCallbackFunction;	///< Task callback function pointer.  If non-NULL when the task is executed the function will be called.
};


#endif
