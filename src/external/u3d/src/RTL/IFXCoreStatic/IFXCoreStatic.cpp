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
  @file IFXCoreStatic.cpp

      This is a helper module for IFXCOM clients to use that contains
      wrapper implementations of the functions exported by the
      dynamically loaded IFXCore DL.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXCOM.h"
#include "IFXOSLoader.h"
#include "IFXMemory.h"
#include "IFXPlugin.h"
#include <stdlib.h>

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

class IFXCoreLib
{
public:
    IFXCoreLib() :
        m_handle(0)
    {
    };

    ~IFXCoreLib()
    {
        Unload();
    };

    // Load dynamic library
    IFXRESULT Load()
    {
        IFXRESULT result = IFX_OK;

    if( NULL == m_handle )
    {
      m_handle = IFXLoadCoreLibrary();

      if( NULL == m_handle )
      {
        result = IFX_E_INVALID_HANDLE;
      }
    }
    else
      result = IFX_E_ALREADY_INITIALIZED;


        return result;
    };

    // Unload dynamic library
    IFXRESULT Unload()
    {
        IFXRESULT result = IFX_OK;

        if( NULL != m_handle )
        {
            //Release the DLL if we get out of scope
            result = IFXReleaseLibrary(m_handle);
            m_handle = NULL;
        }
    else
      result = IFX_E_INVALID_HANDLE;

        return result;
    };

    //Get the functions address
    IFXOSFUNC GetFuncPtr( const char* pFuncName ) const
    {
        return IFXGetAddress( m_handle, pFuncName );
    };

private:
    IFXHANDLE m_handle;
};

extern "C"
{
    typedef IFXRESULT ( IFXAPI IFXCOMInitializeFunction )();
    typedef IFXRESULT ( IFXAPI IFXCOMUninitializeFunction )();
    typedef IFXRESULT ( IFXAPI IFXCreateComponentFunction )(
                            IFXREFCID componentId,
                            IFXREFIID interfaceId,
                            void**    ppInterface );
    typedef IFXRESULT ( IFXAPI IFXRegisterComponentFunction )(
                            IFXComponentDescriptor* pComponentDescriptor );
    typedef IFXRESULT ( IFXAPI IFXGetMemoryFunctionsFunction )(
              IFXAllocateFunction**   ppAllocateFunction,
                            IFXDeallocateFunction** ppDeallocateFunction,
                            IFXReallocateFunction** ppReallocateFunction );
    typedef IFXRESULT ( IFXAPI IFXSetMemoryFunctionsFunction )(
                            IFXAllocateFunction*    pAllocateFunction,
                            IFXDeallocateFunction*  pDeallocateFunction,
                            IFXReallocateFunction*  pReallocateFunction );
    typedef void*     ( IFXAPI JFXAllocateFunction )( size_t byteCount );
    typedef void      ( IFXAPI JFXDeallocateFunction )( void* pMemory );
    typedef void*     ( IFXAPI JFXReallocateFunction )( void* pMemory, size_t byteCount );
}


//***************************************************************************
//    Local data
//***************************************************************************

static IFXCoreLib s_coreLib;

// functions from IFXCore, which can be used by client
static IFXCOMInitializeFunction      *gs_pIFXCOMInitializeFunction      = NULL;
static IFXCOMUninitializeFunction    *gs_pIFXCOMUninitializeFunction    = NULL;
static IFXCreateComponentFunction    *gs_pIFXCreateComponentFunction    = NULL;
static JFXAllocateFunction           *gs_pIFXAllocateFunction           = NULL;
static JFXDeallocateFunction         *gs_pIFXDeallocateFunction         = NULL;
static JFXReallocateFunction         *gs_pIFXReallocateFunction         = NULL;
static IFXRegisterComponentFunction  *gs_pIFXRegisterComponentFunction  = NULL;
static IFXGetMemoryFunctionsFunction *gs_pIFXGetMemoryFunctionsFunction = NULL;
static IFXSetMemoryFunctionsFunction *gs_pIFXSetMemoryFunctionsFunction = NULL;


//***************************************************************************
//    Global functions
//***************************************************************************

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXCOMInitialize()
{
    IFXRESULT result = IFX_OK;

    // load IFXCore
    result = s_coreLib.Load();

    if( IFXSUCCESS( result ) )
    {
      // setup pointers to functions from IFXCore

        gs_pIFXCOMInitializeFunction =
            ( IFXCOMInitializeFunction* ) s_coreLib.GetFuncPtr(
                      "IFXCOMInitialize" );
        if ( NULL == gs_pIFXCOMInitializeFunction )
            result = IFX_E_INVALID_POINTER;

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXCOMUninitializeFunction =
        ( IFXCOMUninitializeFunction* ) s_coreLib.GetFuncPtr(
                          "IFXCOMUninitialize" );
      if ( NULL == gs_pIFXCOMUninitializeFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXCreateComponentFunction =
        ( IFXCreateComponentFunction* ) s_coreLib.GetFuncPtr(
                          "IFXCreateComponent" );
      if ( NULL == gs_pIFXCreateComponentFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXAllocateFunction =
        ( JFXAllocateFunction* ) s_coreLib.GetFuncPtr( "IFXAllocate" );
      if ( NULL == gs_pIFXAllocateFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXDeallocateFunction =
        ( JFXDeallocateFunction* ) s_coreLib.GetFuncPtr( "IFXDeallocate" );
      if ( NULL == gs_pIFXDeallocateFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXReallocateFunction =
        ( JFXReallocateFunction* ) s_coreLib.GetFuncPtr( "IFXReallocate" );
      if ( NULL == gs_pIFXReallocateFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXRegisterComponentFunction =
        ( IFXRegisterComponentFunction* ) s_coreLib.GetFuncPtr(
                          "IFXRegisterComponent" );
      if ( NULL == gs_pIFXRegisterComponentFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXGetMemoryFunctionsFunction =
        ( IFXGetMemoryFunctionsFunction* ) s_coreLib.GetFuncPtr(
                          "IFXGetMemoryFunctions" );
      if ( NULL == gs_pIFXGetMemoryFunctionsFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXSUCCESS( result ) )
    {
      gs_pIFXSetMemoryFunctionsFunction =
        ( IFXSetMemoryFunctionsFunction* ) s_coreLib.GetFuncPtr(
                          "IFXSetMemoryFunctions" );
      if ( NULL == gs_pIFXSetMemoryFunctionsFunction )
        result = IFX_E_INVALID_POINTER;
    }

    if( IFXFAILURE( result ) )
    {
      IFXASSERTBOX(0,"IFXCOMInitialize() failed. Cannot map functions from IFXCore.");
    }
    }
  else
  {
    result = IFX_E_INVALID_FILE;
    IFXASSERTBOX(0,"IFXCOMInitialize() failed. Cannot load IFXCore.");
  }

    if( IFXSUCCESS( result ) )
    {
        // call actual initialization function from IFXCore
        result = gs_pIFXCOMInitializeFunction();
    }
    else
    {
        // in case of any failure this function should
        // undo the work previously performed

        gs_pIFXCOMInitializeFunction      = NULL;
        gs_pIFXCOMUninitializeFunction    = NULL;
        gs_pIFXCreateComponentFunction    = NULL;
        gs_pIFXAllocateFunction           = NULL;
        gs_pIFXDeallocateFunction         = NULL;
        gs_pIFXReallocateFunction         = NULL;
        gs_pIFXRegisterComponentFunction  = NULL;
        gs_pIFXGetMemoryFunctionsFunction = NULL;
        gs_pIFXSetMemoryFunctionsFunction = NULL;

        s_coreLib.Unload();
    }

    return result;
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXCOMUninitialize()
{
    IFXRESULT result = IFX_OK;

    if( NULL != gs_pIFXCOMUninitializeFunction )
    {
        // call actual IFXCOMUninitialize function from IFXCore
        result = gs_pIFXCOMUninitializeFunction();
    }
    else
    {
        // IFXCOMInitialize should be called before
        result = IFX_E_NOT_INITIALIZED;
    }

    // if real IFXCOMUninitialize function returns IFX_W_CANNOT_UNLOAD this
    // function cannot revert to the initial state
    if( IFXSUCCESS( result ) && result != IFX_W_CANNOT_UNLOAD )
    {
        gs_pIFXCOMInitializeFunction      = NULL;
        gs_pIFXCOMUninitializeFunction    = NULL;
        gs_pIFXCreateComponentFunction    = NULL;
        gs_pIFXAllocateFunction           = NULL;
        gs_pIFXDeallocateFunction         = NULL;
        gs_pIFXReallocateFunction         = NULL;
        gs_pIFXRegisterComponentFunction  = NULL;
        gs_pIFXGetMemoryFunctionsFunction = NULL;
        gs_pIFXSetMemoryFunctionsFunction = NULL;

        s_coreLib.Unload();
    }

    return result;
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXCreateComponent(
                    IFXREFCID componentId,
                    IFXREFIID interfaceId,
                    void**        ppInterface )
{
    IFXRESULT result = IFX_OK;

    IFXASSERT( gs_pIFXCreateComponentFunction );
    if( NULL != gs_pIFXCreateComponentFunction )
    {
    // call actual IFXCreateComponent function from IFXCore
    result = gs_pIFXCreateComponentFunction(
                componentId,
                interfaceId,
                ppInterface );
  }
  else
    result = IFX_E_NOT_INITIALIZED;

  return result;
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXRegisterComponent(
                    IFXComponentDescriptor* pComponentDescriptor )
{
    IFXRESULT result = IFX_OK;

    IFXASSERT( gs_pIFXRegisterComponentFunction );
  if( NULL != gs_pIFXRegisterComponentFunction )
  {
    // call actual IFXCreateComponent function from IFXCore
    result = gs_pIFXRegisterComponentFunction( pComponentDescriptor );
  }
  else
    result = IFX_E_NOT_INITIALIZED;

  return result;
}

//---------------------------------------------------------------------------
extern "C"
void* IFXAPI IFXAllocate( size_t byteCount )
{
  void* result = NULL;

    IFXASSERT( gs_pIFXAllocateFunction );
  if( NULL != gs_pIFXAllocateFunction )
  {
    // Call actual IFXAllocate function from IFXCore.
    result = gs_pIFXAllocateFunction( byteCount );
  }

  return result;
}

//---------------------------------------------------------------------------
extern "C"
void IFXAPI IFXDeallocate( void* pMemory )
{
    IFXASSERT( gs_pIFXDeallocateFunction );
  if( NULL != gs_pIFXDeallocateFunction )
  {
    // Call actual IFXDeallocate function from IFXCore.
    gs_pIFXDeallocateFunction( pMemory );
  }
}

//---------------------------------------------------------------------------
extern "C"
void* IFXAPI IFXReallocate( void* pMemory, size_t byteCount )
{
  void* result = NULL;

    IFXASSERT( gs_pIFXReallocateFunction );
  if( NULL != gs_pIFXReallocateFunction )
  {
    // Call actual IFXReallocate function from IFXCore.
    result = gs_pIFXReallocateFunction( pMemory, byteCount );
  }

  return result;
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXGetMemoryFunctions(
                    IFXAllocateFunction** ppAllocateFunction,
                    IFXDeallocateFunction** ppDeallocateFunction,
                    IFXReallocateFunction** ppReallocateFunction )
{
    IFXRESULT result = IFX_OK;

    IFXASSERT( gs_pIFXGetMemoryFunctionsFunction );
  if( NULL != gs_pIFXGetMemoryFunctionsFunction )
  {
    // call actual IFXGetMemoryFunctions function from IFXCore
    result = gs_pIFXGetMemoryFunctionsFunction(
          ppAllocateFunction,
          ppDeallocateFunction,
          ppReallocateFunction );
  }
  else
    result = IFX_E_NOT_INITIALIZED;

  return result;
}

//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI IFXSetMemoryFunctions(
                    IFXAllocateFunction*    pAllocateFunction,
                    IFXDeallocateFunction*    pDeallocateFunction,
                    IFXReallocateFunction*    pReallocateFunction )
{
    IFXRESULT result = IFX_OK;

    IFXASSERT( gs_pIFXSetMemoryFunctionsFunction );
  if( NULL != gs_pIFXSetMemoryFunctionsFunction )
  {
    // call actual IFXSetMemoryFunctions function from IFXCore
    result = gs_pIFXSetMemoryFunctionsFunction(
          pAllocateFunction,
          pDeallocateFunction,
          pReallocateFunction );
  }
  else
    result = IFX_E_NOT_INITIALIZED;

  return result;
}
