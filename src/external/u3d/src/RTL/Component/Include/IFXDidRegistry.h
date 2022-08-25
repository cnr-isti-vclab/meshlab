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
/*
@file  IFXDidRegistry.h

  The header file that defines the IFXDidRegistry interface. */


#ifndef __IFXDIDREGISTRY_H__
#define __IFXDIDREGISTRY_H__

#include "IFXUnknown.h"
#include "IFXDids.h"

// {ACB792D5-401B-4eb3-B658-D746D12740D4}
IFXDEFINE_GUID(IID_IFXDidRegistry,
0xacb792d5, 0x401b, 0x4eb3, 0xb6, 0x58, 0xd7, 0x46, 0xd1, 0x27, 0x40, 0xd4);

class IFXDidRegistry : virtual public IFXUnknown
/**
       The DidRegistry interface supplies the service to identify active DIDs
     that support the IFXUnknown interface.                                */
{
public:

  virtual IFXRESULT IFXAPI  CopyDID( IFXREFDID rInDataElement, IFXREFDID rInTemplate) = 0;
  /**<
        Adds the supplied DID to the Registry as supporting the
        supplied interface


    @param  rInDataElement
        The IFXREFGUID identifying the DataElement.

    @param  rInFlags
        The Flags supported by this Did

    @return One of the following IFXRESULT codes:                          \n\n
  -            IFX_OK                                                          \n
          No error.                                                    \n\n
  -            IFX_E_OUT_OF_MEMORY                                             \n
          Not enough memory was available to initialize.                 */

  virtual IFXRESULT IFXAPI  AddDID( IFXREFDID rInDataElement, U32 rInFlags ) = 0;
  /**<
        Adds the supplied DID to the Registry as supporting the
        supplied interface


    @param  rInDataElement
        The IFXREFGUID identifying the DataElement.

    @param  rInFlags
        The Flags supported by this Did

    @return One of the following IFXRESULT codes:                          \n\n
  -            IFX_OK                                                          \n
          No error.                                                    \n\n
  -            IFX_E_OUT_OF_MEMORY                                             \n
          Not enough memory was available to initialize.                 */


  virtual U32 IFXAPI  GetDidFlags( IFXREFDID rInDataElement) = 0;
  /**<
        Returns TRUE if the specified DID has been registered as supporting
        the requested interface, otherwise returns FALSE.

    @param  rInDataElement
        The IFXREFGUID identifying the DataElement.

    @return  rInIID
        The
  */
};

#endif // __IFXDIDREGISTRY_H__
