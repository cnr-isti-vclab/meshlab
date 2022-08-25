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
  @file IFXUnknown.h

      This header defines the IFXUnknown interface and
      the associated interface identifier.

  @note Interface and component templates are available for use when
      creating new ones.  Also available is a guidelines document that
      describes both create and usage rules.
*/


#ifndef IFXUnknown_H
#define IFXUnknown_H

// This is to remove "inherits via dominance" warnings from MSVC.
// MSVC is being a little too paranoid.
#if defined (_MSC_VER)
#pragma warning (disable : 4250)
#endif /* _MSC_VER */

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "IFXResult.h"
#include "IFXGUID.h"
#include "IFXSmartPtr.h"

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Interfaces and interface identifiers
//***************************************************************************

/**
  This is the main interface supported by every IFXCOM component.

  @note Every interface supported by an IFXCOM component must either
      derive from this interface or derive from another interface that
      eventually terminates in a derivation from this interface.

  @note The associated IID is named IID_IFXUnknown.
*/
class IFXUnknown
{
  public:
    virtual ~IFXUnknown() {}
    /**
      This method increments the reference count for an interface on a
      component.

      @note It should be called for every new copy of a pointer to an
          interface on a given component.

      @return It returns a U32 that contains a value from 1 to 2^32 - 1
          that defines the new reference count.  The return value
          should only be used for debugging purposes.
    */
    virtual U32 IFXAPI  AddRef( void ) = 0;

    /**
      This method decrements the reference count for the calling
      interface on a component.

      @note If the reference count on a component falls to zero, the
          component is destroyed.

      @return It returns a U32 that contains a value from 1 to 2^32 - 1
          that defines the new reference count.  The return value
          should only be used for debugging purposes.
    */
    virtual U32 IFXAPI  Release( void ) = 0;

    /**
      This method provides access to the various interfaces supported by
      a component.  Upon success, it increments the component's
      reference count and hands back a pointer to the specified
      interface.

      @note A number of rules must be adhered to by all
          implementations of IFXAPI  QueryInterface .  For a list of such
          rules, refer to the IFXCOM usage guidelines.

      @return Upon success, IFX_OK is returned.  Otherwise, either
          IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED are returned.
    */
    virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId,
                      void**  ppInterface ) = 0;
};

/**
  This IID identifies the IFXUnknown interface.

  @note The GUID string is {DEC1B7A0-3DC7-11d3-81F4-0004AC2EDBCD}.
*/
// {DEC1B7A0-3DC7-11d3-81F4-0004AC2EDBCD}
IFXDEFINE_GUID(IID_IFXUnknown,
    0xDEC1B7A0, 0x3DC7, 0x11d3, 0x81, 0xF4, 0x00, 0x04, 0xAC, 0x2E, 0xDB, 0xCD );

typedef IFXSmartPtr<IFXUnknown> IFXUnknownPtr;

#endif
