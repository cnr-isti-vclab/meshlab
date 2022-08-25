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

@file  IFXObserver.h

       The header file that defines the IFXObserver interface.

@note  Related interfaces: \n IFXSubject

@note  This interface follows the pattern commonly know as the \n Observer
       pattern, the Publish/Subscribe pattern, or the Dependents pattern.

@note  The Observer pattern is documented in "Design Patterns," written by
       Erich Gamma et. al., published by Addison-Wesley in 1995.             
*/

#ifndef __IFXOBSERVER_H__
#define __IFXOBSERVER_H__

#include "IFXUnknown.h"
class IFXSubject;

// {0985328C-ED0E-473e-B911-234FD2AEC1A7}
IFXDEFINE_GUID(IID_IFXObserver,
0x985328c, 0xed0e, 0x473e, 0xb9, 0x11, 0x23, 0x4f, 0xd2, 0xae, 0xc1, 0xa7);

class IFXObserver : virtual public IFXUnknown
/**
       The IFXObserver interface supplies the services for callback
	   processing.                                                           
*/
{
public:
	virtual IFXRESULT IFXAPI  Update(IFXSubject* pInSubject, U32 uInChangeBits, IFXREFIID rIType=IID_IFXUnknown)=0;
	/**<
	        Notification that the provided subject has changed.

	@param  pInSubject
	         A pointer to the IFXSubject interface of the component that
			 changed.

	@param  uInChangeBits
             The aspects of interest that changed as defined by the supplied
			 IFXSubject's published interest bits.  if uInChangeBits are 0,
			 then the subject is shutting down, and should be released if
			 AddRef'ed, or local references should be cleared in the weak
			 reference case.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_INVALID_POINTER                                           \n
              pInSubject was NULL.                                         \n\n
-            IFX_E_OUT_OF_MEMORY                                             \n
              Not enough memory is available to resolve the change.          \n

    @note   This method is typically called from @e IFXSubject::PostChanges().

    @note   This method with uInChangeBits = 0 allows for the circular
			reference between the Observer and the Subject to
			be broken.            
	*/
};


struct SIFXObserverRequest
{
	IFXObserver * pObserver;
	U32	uObserverInterests;
	IFXREFIID rInterfaceType;
	U32 shift;
	U32 timeStamp; // to avoid duplicate posts
	SIFXObserverRequest( IFXObserver* pObs,
	                     U32           interests,
						 IFXREFIID rIType,
						 U32 shiftBits=0,
						 U32 ts = (U32)-1 ): pObserver(pObs),
	                                      uObserverInterests(interests),
										  rInterfaceType(rIType),shift(shiftBits),timeStamp(ts){};
};


#endif
