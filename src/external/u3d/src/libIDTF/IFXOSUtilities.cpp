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
//  IFXOSWin32Utilities.cpp
//
//  DESCRIPTION
//    This module defines our MacOSX specific utilities.
//
//  NOTES
//    To port to another operating system, rewrite the all of the IFXOS*.h
//  services.
//
//***************************************************************************

#ifdef WIN32

/**
  @file IFXOSUtilities.cpp

      This module defines our Win32 specific utilities.

  @note
      To port to another operating system, rewrite the all of the
      IFXOS*.h services.
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXOSUtilities.h"
#include <windows.h>
#include <mmsystem.h>
#include <float.h>
#include <stdlib.h>
#include <locale.h>

//***************************************************************************
//  Defines
//***************************************************************************

#ifndef STRICT
    #define STRICT
#endif

#define _DEBUG_REG_KEY_PROJECT      L"SOFTWARE\\3DIF\\IFX"
#define _DEBUG_REG_VALUE_DEBUGLEVEL   L"DebugLevel"
#define _MESSAGE_LENGTH_MAX                 1024

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if __MINGW32__
#define swprintf _snwprintf
#endif

//***************************************************************************
//  Global data
//***************************************************************************

//***************************************************************************
//  Local data
//***************************************************************************

BOOL      g_bUseQPC = FALSE;
LARGE_INTEGER g_QPCFrequency;

#ifdef _DEBUG
  BOOL g_bInitialized = FALSE;
#endif

U32 g_eax=0, g_ebx=0, g_ecx=0, g_edx=0;

//***************************************************************************
//  Local functions
//***************************************************************************

#ifdef _MSC_VER
static void cpuid(U32 op, U32& eax, U32& ebx, U32& ecx, U32& edx)
{
#ifndef _WIN64
  U32 A, B, C, D;
  __asm {
    mov eax, op
    cpuid
    mov A, eax
    mov B, ebx
    mov C, ecx
    mov D, edx
  }
  eax = A;
  ebx = B;
  ecx = C;
  edx = D;
#endif
}
#endif
#ifdef __MINGW32__
static void cpuid(U32 op)
{
  asm(
        "movl %%ebx, %%edi \n\t"
        "cpuid             \n\t"
        "movl %%edi, %%ebx \n\t"
        : "=a" (g_eax),
      "=c" (g_ecx),
      "=d" (g_edx)
    : "a" (op));
}
#endif

//***************************************************************************
//  Global functions
//***************************************************************************

extern "C"
U16 IFXAPI_CALLTYPE IFXGetSystemDefaultLangID(void)
{
  return GetSystemDefaultLangID();
}

extern "C"
BOOL IFXAPI_CALLTYPE IFXOSCheckCPUFeature(EIFXCPUFeature feature)
{
  BOOL supported = FALSE;
  switch (feature) {
    case IFXCPUFeature_MMX: supported = (g_edx>>23)&1; break;
    case IFXCPUFeature_SSE: supported = (g_edx>>25)&1; break;
    case IFXCPUFeature_SSE2: supported = (g_edx>>26)&1; break;
    case IFXCPUFeature_SSE3: supported = g_ecx&1; break;
  }
  return supported;
}

//---------------------------------------------------------------------------
//  IFXOSInitialize
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSInitialize( void )
{
  // Determine is the QPC ( QueryPerformanceCounter ) timing service is available.
  QueryPerformanceFrequency( &g_QPCFrequency );
  g_bUseQPC = ( 0 != g_QPCFrequency.QuadPart ) ? TRUE : FALSE;
  if( !g_bUseQPC )
    timeBeginPeriod(1);

#ifdef _MSC_VER
  cpuid(1, g_eax, g_ebx, g_ecx, g_edx);
#endif
#ifdef __MINGW32__
  cpuid(1);
#endif

#ifdef _DEBUG
  g_bInitialized = TRUE;
#endif
}

//---------------------------------------------------------------------------
//  IFXOSUninitialize
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSUninitialize( void )
{
#ifdef _DEBUG
  g_bInitialized = FALSE;
#endif

  if( !g_bUseQPC )
    timeEndPeriod(1);
}

//---------------------------------------------------------------------------
//  IFXOSGetTime
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSGetTime( void )
{
#ifdef _DEBUG
  if (!g_bInitialized) IFXOSOutputDebugString(L"IFXOSUtilities not g_bInitialized!\n");
#endif

  LARGE_INTEGER qwCounter;
  if ( g_bUseQPC )
    {
    QueryPerformanceCounter( &qwCounter);
    return ( DWORD ) ((( qwCounter.QuadPart)*1000) / g_QPCFrequency.QuadPart );
  }
    else
    return timeGetTime();
}

//---------------------------------------------------------------------------
//  IFXOSSleep
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSSleep( U32 milliseconds )
{
#ifdef _DEBUG
  if (!g_bInitialized) IFXOSOutputDebugString(L"IFXOSUtilities not g_bInitialized!\n");
#endif
  Sleep( milliseconds );
}

//---------------------------------------------------------------------------
//  IFXOSControlFP
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSControlFP( U32 value, U32 mask )
{
  return _controlfp( value, mask );
}

#ifdef _DEBUG
//---------------------------------------------------------------------------
//  IFXOSGetDebugLevel
//---------------------------------------------------------------------------
extern "C"
IFXDebugLevel IFXAPI_CALLTYPE IFXOSGetDebugLevel( void )
{
  IFXDebugLevel debugLevel  = IFXDEBUG_DEFAULT;
  HKEY      hKey;

  if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
              _DEBUG_REG_KEY_PROJECT,
              0,
              KEY_QUERY_VALUE,
              &hKey ) == ERROR_SUCCESS )
  {
    DWORD value,
        valueSize = sizeof( value ),
        valueType;

    if ( RegQueryValueEx( hKey,
                  _DEBUG_REG_VALUE_DEBUGLEVEL,
                  NULL,
                  &valueType,
                  ( LPBYTE ) &value,
                  &valueSize ) == ERROR_SUCCESS )
    {
      if ( valueType == REG_DWORD )
        debugLevel = ( IFXDebugLevel ) value;
    }

    RegCloseKey( hKey );
  }

  wchar_t debugLevelValue[2];
  if(GetEnvironmentVariable(L"U3D_DEBUGLEVEL", debugLevelValue, 2))
  {
    debugLevelValue[1] = '\0';
	debugLevel = (IFXDebugLevel)_wtoi( debugLevelValue );
  }

  return debugLevel;
}

//---------------------------------------------------------------------------
//  IFXOSOutputDebugString
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSOutputDebugString( const IFXCHAR* pMessage )
{
  OutputDebugString( pMessage );
}

#endif

//---------------------------------------------------------------------------
//  IFXOSGetUtf8StrSize
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSGetUtf8StrSize( const IFXCHAR* pWideCharString, U32* pUtf8StringSize )
{
  IFXRESULT result = IFX_OK;
  U32 len;

  if( (NULL == pWideCharString) || (NULL == pUtf8StringSize) )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    len = WideCharToMultiByte( CP_UTF8, 0, pWideCharString, -1, NULL, 0, NULL, NULL );

    if( 0 != len )
      *pUtf8StringSize = len - 1; // exclude null terminator
    else
    {
      *pUtf8StringSize = 0;
      result = IFX_E_UNDEFINED;
    }
  }

  return result;
}

//---------------------------------------------------------------------------
//  IFXOSGetWideCharStrSize
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSGetWideCharStrSize( const U8* pUtf8String, U32* pWideCharStringSize )
{
  IFXRESULT result = IFX_OK;
  U32 len;

  if( (NULL == pUtf8String) || (NULL == pWideCharStringSize) )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    len = MultiByteToWideChar( CP_UTF8, 0, (const char*)pUtf8String, -1, NULL, 0 );

    if( 0 != len )
      *pWideCharStringSize = len - 1; // exclude null terminator
    else
    {
      *pWideCharStringSize = 0;
      result = IFX_E_UNDEFINED;
    }
  }

  return result;
}


//---------------------------------------------------------------------------
//  IFXOSConvertWideCharStrToUtf8
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSConvertWideCharStrToUtf8(
                const IFXCHAR* pWideCharString,
                U8* pUtf8String,
                U32 utf8StringSize )
{
  IFXRESULT result = IFX_OK;

  if( 0 == utf8StringSize )
    result = IFX_E_BAD_PARAM;

  if( NULL == pWideCharString )
    result = IFX_E_INVALID_POINTER;

  if( NULL == pUtf8String )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    if( 0 == WideCharToMultiByte(
            CP_UTF8, 0, pWideCharString, -1,
            (char*)pUtf8String, (int)utf8StringSize, NULL, NULL ) )
      result = IFX_E_UNDEFINED;
  }

  return result;
}

//---------------------------------------------------------------------------
//  IFXOSConvertUtf8StrToWideChar
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSConvertUtf8StrToWideChar(
                const U8* pUtf8String,
                IFXCHAR* pWideCharString,
                U32 wideCharStringSize )
{
  IFXRESULT result = IFX_OK;

  if( 0 == wideCharStringSize )
    result = IFX_E_BAD_PARAM;

  if( NULL == pWideCharString )
    result = IFX_E_INVALID_POINTER;

  if( NULL == pUtf8String )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    if( 0 == MultiByteToWideChar(
                CP_UTF8, 0, (char*)pUtf8String, -1,
                pWideCharString, (int)wideCharStringSize ) )
      result = IFX_E_UNDEFINED;
  }

  return result;
}

//---------------------------------------------------------------------------
//  IFXOSDebugOutput
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSDebugOutput(
                  IFXCHAR* pMessage,
                  IFXCHAR* pFile,
                  U32 lineNum,
                  IFXCHAR* pExpression )
{
  U32 rVal;
  static IFXCHAR szBoxMessage[_MESSAGE_LENGTH_MAX];

  swprintf(szBoxMessage, _MESSAGE_LENGTH_MAX,
    L"%ls\n\nFile:\t\t%ls\nLine:\t\t%d\nExpression:\t%ls\n\nAbort = Stop Debugging\nRetry = Debug\nIgnore = Continue Running\n",
    pMessage, pFile, lineNum, pExpression);

  int boxRet = MessageBox(NULL, szBoxMessage, L"Assertion Failed!", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION );

  if(boxRet == IDIGNORE)
  {
    if(IDYES == MessageBox(NULL, L"Ignore all future instances of this assertion?", L"IFXASSERT", MB_YESNO | MB_ICONQUESTION ))
    {
      rVal = 0;
    }
  }
  else if(boxRet == IDRETRY)
  {
    rVal |= 2;
  }
  else
  {
    exit(-1);
  }
  return rVal;
}

//---------------------------------------------------------------------------
//  IFXSetDefaultLocale
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXSetDefaultLocale( void )
{
  IFXRESULT result = IFX_OK;
  char*     loc    = NULL;

  loc = setlocale(LC_CTYPE, "English");

  if (NULL == loc)
  {
    result = IFX_E_UNDEFINED;
  }

  return result;
}


#else
//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXOSUtilities.h"
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <sys/time.h>
#include <locale.h>

//***************************************************************************
//  Defines
//***************************************************************************

//***************************************************************************
//  Global data
//***************************************************************************

//***************************************************************************
//  Local data
//***************************************************************************
#define _MESSAGE_LENGTH_MAX               1024
#ifdef _DEBUG
BOOL g_bInitialized = FALSE;
#endif

#if defined( __GNUC__ ) && defined( __i386__ ) && !defined( MAC32 ) && !defined( __APPLE__ )
U32 g_eax=0, g_ecx=0, g_edx=0;
#endif

//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************

static inline void cpuid(U32 op)
{
#if defined( __GNUC__ ) && defined( __i386__ ) && !defined( MAC32 ) && !defined( __APPLE__ )
  asm(
    "movl %%ebx, %%edi  \n\t"
    "cpuid        \n\t"
    "movl %%edi, %%ebx  \n\t"
    : "=a" (g_eax),
          "=c" (g_ecx),
          "=d" (g_edx)
        : "a" (op));
#endif
}

//***************************************************************************
//  Global functions
//***************************************************************************

extern "C"
U16 IFXAPI_CALLTYPE IFXOSGetSystemDefaultLangID( void )
{
  return 0x0800; // system default language
}

extern "C"
BOOL IFXAPI_CALLTYPE IFXOSCheckCPUFeature(EIFXCPUFeature feature)
{
#if defined( __GNUC__ ) && defined( __i386__ ) && !defined( MAC32 ) && !defined( __APPLE__ )
  BOOL supported = FALSE;
  switch (feature)
  {
    case IFXCPUFeature_MMX: supported = (g_edx>>23)&1; break;
    case IFXCPUFeature_SSE: supported = (g_edx>>25)&1; break;
    case IFXCPUFeature_SSE2: supported = (g_edx>>26)&1; break;
    case IFXCPUFeature_SSE3: supported = g_ecx&1; break;
  }
  return supported;
#endif
#if defined( __GNUC__ ) && defined( __i386__ ) && ( defined( MAC32 ) || defined( __APPLE__ ) )
  return TRUE;
#else
  return FALSE;
#endif
}

//---------------------------------------------------------------------------
//  IFXOSInitialize
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSInitialize( void )
{
  cpuid(1);

#ifdef _DEBUG
  g_bInitialized = TRUE;
#endif
}

//---------------------------------------------------------------------------
//  IFXOSUninitialize
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSUninitialize( void )
{
#ifdef _DEBUG
  g_bInitialized = FALSE;
#endif
}

//---------------------------------------------------------------------------
//  IFXOSGetTime
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSGetTime( void )
{
#ifdef _DEBUG
  if (!g_bInitialized) IFXOSOutputDebugString(L"IFXOSUtilities not g_bInitialized!\n");
#endif

  struct timeval time;

  gettimeofday( &time, NULL );

  return ((time.tv_sec * 1000000) + time.tv_usec) / 1000;
}

//---------------------------------------------------------------------------
//  IFXOSSleep
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSSleep( U32 milliseconds )
{
#ifdef _DEBUG
  if (!g_bInitialized) IFXOSOutputDebugString(L"IFXOSUtilities not g_bInitialized!\n");
#endif

  struct timespec time;

  time.tv_sec = milliseconds / 1000;
  time.tv_nsec = ( milliseconds % 1000 ) * 1000000;

  nanosleep( &time, NULL );
}

//---------------------------------------------------------------------------
//  IFXOSControlFP
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSControlFP( U32 value, U32 mask )
{
  return 0;
}

#ifdef _DEBUG
//---------------------------------------------------------------------------
//  IFXOSGetDebugLevel
//---------------------------------------------------------------------------
extern "C"
IFXDebugLevel IFXAPI_CALLTYPE IFXOSGetDebugLevel( void )
{
    IFXDebugLevel debugLevel = IFXDEBUG_DEFAULT;

  char* debugLevelValue = getenv( "U3D_DEBUGLEVEL" );

  if( NULL != debugLevelValue )
    debugLevel = (IFXDebugLevel)atoi( debugLevelValue );

    return debugLevel;
}

//---------------------------------------------------------------------------
//  IFXOSOutputDebugString
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSOutputDebugString( const IFXCHAR* pMessage )
{
  char message[_MESSAGE_LENGTH_MAX];
  U32 size = wcstombs(NULL, pMessage, 0);
  wcstombs(message, pMessage, size);
  message[size] = 0;

  fprintf(stderr, "%s", message);
}

#endif

//---------------------------------------------------------------------------
//  IFXOSGetUtf8StrSize
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSGetUtf8StrSize(
                  const IFXCHAR* pWideCharString,
                  U32* pUtf8StringSize )
{
  IFXRESULT result = IFX_OK;
  size_t len;

  if( (NULL == pWideCharString) || (NULL == pUtf8StringSize) )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    len = wcstombs( NULL, pWideCharString, 0);

    if( len != (size_t)-1 )
      *pUtf8StringSize = (U32)len;
    else
    {
      *pUtf8StringSize = 0;
      result = IFX_E_UNDEFINED;
    }
  }

  return result;
}

//---------------------------------------------------------------------------
//  IFXOSGetWideCharStrSize
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSGetWideCharStrSize(
                  const U8* pUtf8String,
                  U32* pWideCharStringSize )
{
  IFXRESULT result = IFX_OK;
  size_t len;

  if( (NULL == pUtf8String) || (NULL == pWideCharStringSize) )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    len = mbstowcs( NULL, (const char*)pUtf8String, 0 );

    if( len != (size_t)-1 )
      *pWideCharStringSize = (U32)len;
    else
    {
      *pWideCharStringSize = 0;
      result = IFX_E_UNDEFINED;
    }
  }

  return result;
}


//---------------------------------------------------------------------------
//  IFXOSConvertWideCharStrToUtf8
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSConvertWideCharStrToUtf8(
                const IFXCHAR* pWideCharString,
                U8* pUtf8String,
                U32 utf8StringSize )
{
  IFXRESULT result = IFX_OK;

  if( 0 == utf8StringSize )
    result = IFX_E_BAD_PARAM;

  if( NULL == pWideCharString )
    result = IFX_E_INVALID_POINTER;

  if( NULL == pUtf8String )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    if( (size_t)-1 == wcstombs( (char*)pUtf8String, pWideCharString, utf8StringSize ) )
      result = IFX_E_UNDEFINED;
  }

  return result;
}

//---------------------------------------------------------------------------
//  IFXOSConvertUtf8StrToWideChar
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXOSConvertUtf8StrToWideChar(
                const U8* pUtf8String,
                IFXCHAR* pWideCharString,
                U32 wideCharStringSize )
{
  IFXRESULT result = IFX_OK;

  if( 0 == wideCharStringSize )
    result = IFX_E_BAD_PARAM;

  if( NULL == pWideCharString )
    result = IFX_E_INVALID_POINTER;

  if( NULL == pUtf8String )
    result = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( result ) )
  {
    if( (size_t)-1 == mbstowcs( pWideCharString, (char*)pUtf8String, wideCharStringSize) )
      result = IFX_E_UNDEFINED;
  }

  return result;
}

//---------------------------------------------------------------------------
//  IFXOSDebugOutput
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSDebugOutput(
                  IFXCHAR* pMessage,
                  IFXCHAR* pFile,
                  U32 lineNum,
                  IFXCHAR* pExpression )
{
  static IFXCHAR szBoxMessage[_MESSAGE_LENGTH_MAX];
  swprintf(szBoxMessage, _MESSAGE_LENGTH_MAX,
    L"%ls\n\nFile:\t\t%ls\nLine:\t\t%d\nExpression:\t%ls\n",
    pMessage, pFile, lineNum, pExpression);
  fprintf(stderr, "Assertion failed: %ls\n", szBoxMessage);
  abort();
}

//---------------------------------------------------------------------------
//  IFXSetDefaultLocale
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE IFXSetDefaultLocale( void )
{
  IFXRESULT result = IFX_OK;
  char*     loc    = NULL;

  loc = setlocale(LC_CTYPE, "en_US.UTF-8");

  if (NULL == loc)
  {
    result = IFX_E_UNDEFINED;
  }

  return result;
}
#endif
