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
//
//  CIFXSimpleObject.h
//
//  DESCRIPTION
//
//  NOTES
//
//***************************************************************************
#ifndef __CIFXSimpleObject_H__
#define __CIFXSimpleObject_H__

#include "CIFXSubject.h"
#include "IFXSubject.h"

class CIFXSimpleObject : virtual public IFXSubject,
								 public CIFXSubject

{
  CIFXSimpleObject();
  ~CIFXSimpleObject();
  friend IFXRESULT IFXAPI_CALLTYPE CIFXSimpleObject_Factory(IFXREFIID iid, void** ppv);

public:
  // IFXUnknown Interface
  U32 IFXAPI  AddRef ();
  U32 IFXAPI  Release ();
  IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);

  // IFXSubject Interface

  // attaches an observer to this subject
  IFXRESULT  IFXAPI 	 Attach( IFXObserver* pInObserver,
            U32          uInIntrestBits,
            IFXREFIID    rIType=IID_IFXUnknown,
            U32          shiftBits=0 );
  // removes an observer
  IFXRESULT  IFXAPI 	 Detach(IFXObserver* pObs);

  // the observers will get update calls when the
  // subject state the observer is interested in changes
  void IFXAPI   PostChanges(U32 changedBits);

  void IFXAPI   PreDestruct();

private:
  U32 m_uRefCount;
};


#endif
