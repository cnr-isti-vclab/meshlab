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
//	IFXOSWin32Utilities.cpp
//
//	DESCRIPTION
//		This module defines our Lin32 specific utilities.
//
//	NOTES
//		To port to another operating system, rewrite the all of the IFXOS*.h
//	services.
//
//***************************************************************************


//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXOSUtilities.h"
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <sys/time.h>
#include <locale.h>

//***************************************************************************
//	Defines
//***************************************************************************

//***************************************************************************
//	Global data
//***************************************************************************

//***************************************************************************
//	Local data
//***************************************************************************
#define _MESSAGE_LENGTH_MAX               1024
#ifdef _DEBUG
BOOL g_bInitialized = FALSE;
#endif

#ifndef U3D_NO_ASM
#if defined( __GNUC__ ) && defined( __i386__ )  // ||  !defined( __x86_64__ )
U32 g_eax=0, g_ecx=0, g_edx=0;
#endif
#endif // U3D_NO_ASM

//***************************************************************************
//	Local function prototypes
//***************************************************************************


//***************************************************************************
//	Local functions
//***************************************************************************

#ifndef U3D_NO_ASM
static inline void cpuid(U32 op)
{
#if defined( __GNUC__ ) && defined( __i386__ )  // ||  !defined( __x86_64__ )
	asm(
		"movl %%ebx, %%edi	\n\t"
		"cpuid				\n\t"
		"movl %%edi, %%ebx  \n\t"
		: "=a" (g_eax),
          "=c" (g_ecx),
          "=d" (g_edx)
        : "a" (op));
#endif
}
#endif // U3D_NO_ASM

//***************************************************************************
//	Global functions
//***************************************************************************

extern "C"
U16 IFXAPI_CALLTYPE IFXOSGetSystemDefaultLangID( void )
{
	return 0x0800; // system default language
}

extern "C"
BOOL IFXAPI_CALLTYPE IFXOSCheckCPUFeature(EIFXCPUFeature feature)
{
#ifndef U3D_NO_ASM
	BOOL supported = FALSE;
#if defined( __GNUC__ ) && defined( __i386__ )  // ||  !defined( __x86_64__ )
	switch (feature) 
	{
		case IFXCPUFeature_MMX: supported = (g_edx>>23)&1; break;
		case IFXCPUFeature_SSE: supported = (g_edx>>25)&1; break;
		case IFXCPUFeature_SSE2: supported = (g_edx>>26)&1; break;
		case IFXCPUFeature_SSE3: supported = g_ecx&1; break;
	}
#endif

	return supported;
#else
	return FALSE;
#endif // U3D_NO_ASM
}

//---------------------------------------------------------------------------
//	IFXOSInitialize
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSInitialize( void )
{
#ifndef U3D_NO_ASM
	cpuid(1);
#endif // U3D_NO_ASM

#ifdef _DEBUG
	g_bInitialized = TRUE;
#endif
}

//---------------------------------------------------------------------------
//	IFXOSUninitialize
//---------------------------------------------------------------------------
extern "C"
void IFXAPI_CALLTYPE IFXOSUninitialize( void )
{
#ifdef _DEBUG
	g_bInitialized = FALSE;
#endif
}

//---------------------------------------------------------------------------
//	IFXOSGetTime
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
//	IFXOSSleep
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
//	IFXOSControlFP
//---------------------------------------------------------------------------
extern "C"
U32 IFXAPI_CALLTYPE IFXOSControlFP( U32 value, U32 mask )
{
	return 0;
}

#ifdef _DEBUG
//---------------------------------------------------------------------------
//	IFXOSGetDebugLevel
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
//	IFXOSOutputDebugString
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
//	IFXOSGetUtf8StrSize
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
		len = wcstombs( NULL, pWideCharString, 0 );

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
//	IFXOSGetWideCharStrSize
//---------------------------------------------------------------------------
extern "C"
IFXRESULT IFXAPI_CALLTYPE	IFXOSGetWideCharStrSize( 
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
//	IFXOSConvertWideCharStrToUtf8
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
//	IFXOSConvertUtf8StrToWideChar
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
//	IFXOSDebugOutput
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
