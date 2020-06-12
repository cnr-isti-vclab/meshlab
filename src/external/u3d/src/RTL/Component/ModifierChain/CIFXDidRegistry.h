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
@file  CIFXDidRegistry.h

  The header file that defines the base implementation class of the
  CIFXDidRegistry.                                                 
*/


#ifndef __CIFXDIDREGISTRY_H__
#define __CIFXDIDREGISTRY_H__

#include "IFXDidRegistry.h"
#include "CIFXSubject.h"
#include "IFXHash.h"

/**
  Class to hash guids for IFXHash lookups
*/
class IFXGUIDHasher
{
public:
  U32 operator()(IFXREFGUID in_rGuid)
  {
    const U32* gd = (U32*)&in_rGuid;
    return gd[0] + gd[1] + gd[2] + gd[3];

  };
};


class CIFXDidRegistry : private CIFXSubject,
                virtual public   IFXDidRegistry
{
            CIFXDidRegistry();
  virtual  ~CIFXDidRegistry();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXDidRegistry_Factory( IFXREFIID iid, void** ppv );

public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface (   IFXREFIID  riid,
                              void**     ppv );

  // IFXDidRegistry
  IFXRESULT IFXAPI   CopyDID( IFXREFDID rInDataElement, IFXREFDID rInTemplate);
  IFXRESULT IFXAPI   AddDID( IFXREFDID rInDataElement, U32 InDidFlags );

  U32 IFXAPI   GetDidFlags( IFXREFDID rInDataElement );
private:
  // IFXUnknown
  U32                     m_uRefCount;

  // IFXDidRegistry
  IFXHash< IFXDID, U32, IFXGUIDHasher > m_Didhash;
};

#endif
