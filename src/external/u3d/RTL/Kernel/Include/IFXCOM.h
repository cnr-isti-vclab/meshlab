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
@file IFXCOM.h

This module declares the core IFXCOM functionality.

@note Interface and component templates are available for use when
creating new ones.  Also available is a guidelines document
that describes both create and usage rules.
*/


#ifndef IFXCOM_H
#define IFXCOM_H


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXAPI.h"
#include "IFXDataTypes.h"
#include "IFXGUID.h"
#include "IFXResult.h"
#include "IFXArray.h"

struct IFXComponentDescriptor; 


//***************************************************************************
//  Global function prototypes
//***************************************************************************


/**
This is a function that all clients should use to initializes the IFXCOM
library. Applications must initialize the IFXCOM library before they can
call any IFXCOM library functions.

@return Upon success, it returns the value IFX_OK. Otherwise, it will
return the following value:
IFX_E_OUT_OF_MEMORY - not enough memory to initialize component
system
*/
extern "C"
IFXRESULT IFXAPI IFXCOMInitialize();

/**
This is a function that all clients should use to uninitializes the IFXCOM
library. Applications must uninitialize the IFXCOM library at the end of
its execution to release all active components and unloads all plug-ins.

@return Upon success, it returns the value IFX_OK. Otherwise, it will
return one the following values:
IFX_E_NOT_INITIALIZED - component system was not initialized
IFX_W_CANNOT_UNLOAD   - component system detected active components
and cannot upload plug-in.
*/
extern "C"
IFXRESULT IFXAPI IFXCOMUninitialize();

/**
This is a function that all clients of IFXCOM should use to
create an instance of a component and get a pointer to one of its
interfaces.  It handles @e both styles of components, non-singleton and
singleton.

@param  rComponentId  Reference to the identifier of the component to be
created.
@param  rInterfaceId  Reference to the interface identifier supported by
the component, that a pointer to will be handed back
if this method is successful.
@param  ppInterface Pointer to an interface pointer, that upon success
will be initialized with a pointer to the requested
interface supported by the newly created component.

@return Upon success, it returns the value IFX_OK.  Otherwise, it will
return one of the following values:  IFX_E_COMPONENT,
IFX_E_INVALID_POINTER, IFX_E_OUT_OF_MEMORY or IFX_E_UNSUPPORTED.
It's also possible for component specific construction values to
be returned.
*/
extern "C"
IFXRESULT IFXAPI IFXCreateComponent(
									IFXREFCID rComponentId,
									IFXREFIID rInterfaceId,
									void**    ppInterface );

/**
This is a function that all clients of IFXCOM should use to
register components at run-time after IFXCOM has been successfully
initialized with IFXCOMInitialize.

@note In the case of component being registered already registered the
component database registers new component only if it has newer
version

@param  pComponentDescriptor Pointer to a component descriptor which is
registered.

@return Upon success, it returns the value IFX_OK.  Otherwise, it will
return one of the following values:  IFX_E_INVALID_POINTER,
IFX_E_OUT_OF_MEMORY or IFX_E_UNDEFINED.
*/
extern "C"
IFXRESULT IFXAPI IFXRegisterComponent(
									  IFXComponentDescriptor* pComponentDescriptor );

/**
	This function is used for registering DIDs from plugins.
*/
extern "C"
IFXRESULT IFXAPI IFXGetPluginsDids( IFXArray<IFXDID*> *&pDidsList );

//***************************************************************************
//  Failure return codes
//***************************************************************************

/**
Component identifier specified is not supported.
*/
#define IFX_E_COMPONENT MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x2000 )

#endif
