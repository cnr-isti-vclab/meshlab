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

@file  IFXSubject.h

       The header file that defines the IFXSubject interface.

@note  Related interfaces: \n IFXObserver              \n\n

       This interface follows the pattern commonly know as the \n Observer
       pattern, the Publish/Subscribe pattern, or the Dependents pattern.  \n\n

       The Observer pattern is documented in "Design Patterns," written by
       Erich Gamma et. al., published by Addison-Wesley in 1995.             */

#ifndef __IFXSUBJECT_H__
#define __IFXSUBJECT_H__
#include "IFXUnknown.h"
class IFXObserver;

IFXDEFINE_GUID(IID_IFXSubject,
0xa88bfe00, 0xd1e5, 0x4f01, 0xa2, 0x4a, 0x44, 0xbf, 0x8c, 0x46, 0xc6, 0x2f);

class IFXSubject : virtual public IFXUnknown
/**
       The IFXSubject interface supplies the services for distributing change
       notifications to attached dependents.                                 */
{
public:
	virtual IFXRESULT IFXAPI  Attach(IFXObserver* pInObserver, U32 uInIntrestBits,
							 IFXREFIID rIType=IID_IFXUnknown, U32 shiftBits =0)=0;
	/**<
	        Associates the provided IFXObserver for callback when the specified
			aspects of interest change.

	@param  pInObserver
	         A pointer to the IFXObserver to become associated.

	@param  uInIntrestBits
             The U32 bit field that describes the aspects of interest whose
			 change will trigger a @e IFXObserver::Resolve() call.

	@param  rIType
             x.

	@param  shiftBits
             Used for components supporting multiply inherited interfaces each
			 with subject interfaces.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_INVALID_POINTER                                           \n
              pInObserver was NULL.                                        \n\n
-            IFX_E_OUT_OF_MEMORY                                             \n
              Not enough memory is available to resolve the change.          \n

    @note   This method is typically called from the IFXObserver.             
	*/


	virtual IFXRESULT IFXAPI  Detach(IFXObserver* pInObserver)=0;
	/**<
	        Disassociates the provided IFXObserver.

	@param  pInObserver
	         A pointer to the IFXObserver that should be disassociated.

    @pre    The provided IFXObserver is associated.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_INVALID_POINTER                                           \n
              pInObserver was NULL.                                        \n\n
-            IFX_E_SUBJECT_INVALID_OBSERVER                                  \n
              The provided observer was not associated.                    \n\n

    @note   This method is typically called from the IFXObserver.             
			*/


	virtual void IFXAPI  PostChanges(U32 uInChangedBits)=0;
	/**<
	        Publishes to attached Observers and ChanageManager that changes have
			occured.

	@param  uInChangedBits
             The U32 bit field that describes the conceptual change with
			 respect to the published interests.

    @note   Should be called by the IFXSubject on itself when properties
            change that correlate to the published interests.  When @e
			PostChanges() is called, all attached Observer @e
            Notify() methods are called.                                     */


	// To  give the inheriting classes a chance to work with the
	// message being posted by overloading this function.

	virtual void IFXAPI  PreDestruct()=0;
};

#endif
