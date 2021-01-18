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

#ifndef U3D_NO_ASM
U32 g_eax=0, g_ebx=0, g_ecx=0, g_edx=0;
#endif // U3D_NO_ASM

//***************************************************************************
//  Local functions
//***************************************************************************

#ifndef U3D_NO_ASM
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
        "movl %%ebx, %%edi  \n\t"
        "cpuid              \n\t"
        "movl %%edi, %%ebx  \n\t"
        : "=a" (g_eax),
      "=c" (g_ecx),
      "=d" (g_edx)
    : "a" (op));
}
#endif
#endif // U3D_NO_ASM

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
#ifndef U3D_NO_ASM
  BOOL supported = FALSE;
  switch (feature) {
    case IFXCPUFeature_MMX: supported = (g_edx>>23)&1; break;
    case IFXCPUFeature_SSE: supported = (g_edx>>25)&1; break;
    case IFXCPUFeature_SSE2: supported = (g_edx>>26)&1; break;
    case IFXCPUFeature_SSE3: supported = g_ecx&1; break;
  }
  return supported;
#else
  return FALSE;
#endif // U3D_NO_ASM
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

#ifndef U3D_NO_ASM
#ifdef _MSC_VER
  cpuid(1, g_eax, g_ebx, g_ecx, g_edx);
#endif
#ifdef __MINGW32__
  cpuid(1);
#endif
#endif // U3D_NO_ASM

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
