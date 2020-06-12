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
  @file IFXOSDebug.h

      This module defines some useful debug macros for performing
      asserts and outputting debug messages according to a user
      selected debug level.

      Usage of the IFXDEBUG_STARTUP, IFXDEBUG_SHUTDOWN, IFXASSERT,
      IFXASSERTBOXIFXTRACE_GENERIC and IFXTRACE_CUSTOM macros are
      guaranteed to compile out of the release build.  The debug
      related aspects of the IFXRETURN macro also compile out of the
      release build, just leaving a standard return statement.

      Usage of all macros should terminate with a semicolon.  The
      semicolon doesn't compile out of the release build so it is
      possible to use these macros as a single statement in an
      if/else block that contains no parenthesis.

  @note Prior to using any of the macros make sure to setup the module
      by using the IFXDEBUG_STARTUP macro once (like in the
      DLL_PROCESS_ATTACH block in DllMain on Win32).  When finished,
      use the IFXDEBUG_SHUTDOWN macro once (like in the
      DLL_PROCESS_DETACH block in DllMain on Win32).

  @note The output debug messages will be nicely formated if all of the
      format strings have a newline character "\n" at the end.

  @par  Debug Level Control
      The manner in which the debug level is set will vary from one
      platform to the next.  For example, on Win32 the default level
      can be overriden by the HKEY_LOCAL_MACHINE/SOFTWARE/3DIF/IFX key
      and DWORD value called DebugLevel if they exist.  For Win32, the
      full debug level is defined by the included IFX.reg file.

  @par  Example Usage
      @verbatim
      IFXRESULT ExampleFunction( void )
      {
        IFXRESULT result  = IFX_OK;

        IFXASSERT( ThisFunctionShouldReturnTrueToSucceedTheAssert() );

        IFXTRACE_GENERIC( L"Something meaningful happened\n" );

        IFXTRACE_GENERIC( L"All hell broke loose:  0x08lX\n", result );

        IFXTRACE_CUSTOM( IFXRESULT_COMPONENT_SCALABILITY_SERVICES,
                  IFXDEBUG_MESSAGE,
                  L"A good state was reached\n" );

        IFXTRACE_CUSTOM( IFXRESULT_COMPONENT_SCALABILITY_SERVICES,
                  IFXDEBUG_MESSAGE,
                  L"Only %d days till Micky!\n",
                  2 );

        IFXRETURN( result );
      }
      @endverbatim

  @note Don't use the IFXRETURN macro for functions or methods that ARE
      expected to return failures regularly or as a part of normal
      execution.  When used in such a manner, it pollutes the output
      debug message window with useless data.  IFXRETURN was
      originally intended to be most useful for client API entry
      points.
*/


#ifndef IFXOSDebug_h
#define IFXOSDebug_h


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXAPI.h"
#include "IFXResult.h"


//***************************************************************************
//  Enumerations (common to both debug and release build)
//***************************************************************************


enum IFXDebugLevel
{
  IFXDEBUG_NOTHING,           // No debug messages are displayed.

  // The following three values are for use in macros and denote the type
  // of problem they are checking for or displaying information about.

  IFXDEBUG_ERROR,             // Error debug messages displayed.
  IFXDEBUG_WARNING,           // Warning debug messages displayed plus previous ones.
  IFXDEBUG_MESSAGE,           // Message debug messages displayed plus previous ones.
#ifdef _DEBUG
  IFXDEBUG_DEFAULT  = IFXDEBUG_MESSAGE  // Default debug level without a registry entry.
#else
  IFXDEBUG_DEFAULT  = IFXDEBUG_NOTHING  // Default debug level without a registry entry.
#endif
};


//***************************************************************************
//  Defines (common to both debug and release build)
//***************************************************************************

#define _stringer(n) L ## #n
#define _stringexpand(n) _stringer(n)
#define __THISLINE__ _stringexpand(__LINE__)


#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)


#ifdef _DEBUG


//***************************************************************************
//  Global function prototypes and inlines (debug build)
//***************************************************************************


// These should only be used by the macros defined in this header file.

extern "C"
{
    void IFXAPI IFXDebugAssertBox(  const char *exp,  const char* file, unsigned int line,
                                    const char *msg,  int box,    int log             );

void    IFXAPI IFXDebugStartUp ( void );
void    IFXAPI IFXDebugShutDown( void );

I32     IFXAPI IFXDebugTraceCustom(    const U32       component,
                      const IFXDebugLevel debugLevel,
                      const IFXCHAR*       pFormatString,
                    ... );

I32     IFXAPI IFXDebugTraceGeneric(   const IFXCHAR*             pFormatString, ... );
}


//***************************************************************************
//  Defines (debug build)
//***************************************************************************


//---------------------------------------------------------------------------
/**
  This macro is used to perform any required startup tasks prior to using
  the debug macros or functions in this module.
*/
#define IFXDEBUG_STARTUP() IFXDebugStartUp()

//---------------------------------------------------------------------------
/**
  This macro is used to perform any required shutdown tasks after having
  used the IFXDEBUG_STARTUP macro.
*/
#define IFXDEBUG_SHUTDOWN() IFXDebugShutDown()

//---------------------------------------------------------------------------
/**
  This macro is used to display to the output debug window a
  IFXDEBUG_MESSAGE level message.  It's considered to be a generic message
  not specific to any component or subsystem when output.  The parameters
  to IFXTRACE_GENERIC are exactly the same as the standard printf function
  (i.e. a format string followed by a variable argument list).

  @note Make sure the final message doesn't have more than 1024
      characters.
*/
#define IFXTRACE_GENERIC ::IFXDebugTraceGeneric

//---------------------------------------------------------------------------
/**
  This macro is used to display to the output debug window a message for an
  arbitrary component and debug level.  The parameters are the same as the
  standard printf function, except they are prefixed FIRST by two extra
  parameters.  The first parameter specifies a system component identifier.
  The second parameter specifies an IFXDebug_Level enumerated value.
  Afterwards the standard printf parameters follow (i.e. a format string
  followed by a variable argument list).

  @note Make sure the final message doesn't have more than 1024
      characters.
*/
#define IFXTRACE_CUSTOM ::IFXDebugTraceCustom

/**
  This macro should only be used by other macros defined in this header
  file.
*/
#define IFXASSERTBOXEX( exp, message, box, log )        \
{                                                       \
    if( !(exp) )                                        \
        IFXDebugAssertBox( #exp, __FILE__, __LINE__, message, box, log );   \
}


#else // ! _DEBUG


//***************************************************************************
//  Inline global functions (release build)
//***************************************************************************


inline void IFXDebugTraceCustom( ... )  { }
inline void IFXDebugTraceGeneric( ... ) { }


//***************************************************************************
//  Defines (release build)
//***************************************************************************


#define IFXASSERTBOXEX( exp, message, box, log )

#define IFXDEBUG_STARTUP()
#define IFXDEBUG_SHUTDOWN()
#define IFXTRACE_GENERIC 1 ? ( ( void ) 0 ) : ::IFXDebugTraceGeneric
#define IFXTRACE_CUSTOM 1 ? ( ( void ) 0 ) : ::IFXDebugTraceCustom


#endif // end of _DEBUG


//***************************************************************************
//  More Defines (common to both debug and release build)
//***************************************************************************


#define IFXRETURN( result )     return result
#define IFXASSERTBOX( a, b )    IFXASSERTBOXEX( a, b, TRUE, TRUE )
#define IFXASSERT( condition )  IFXASSERTBOXEX( condition, "Assertion Failed!", TRUE, TRUE )


#endif
