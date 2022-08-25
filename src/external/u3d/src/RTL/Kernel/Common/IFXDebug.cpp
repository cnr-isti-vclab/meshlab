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
@file	IFXDebug.cpp

This module defines some useful debug macros for performing asserts
and outputting debug messages according to a user selected debug level.
*/

#ifdef _DEBUG

//***************************************************************************
//  Includes, Defines
//***************************************************************************

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <wchar.h>

#include "IFXDebug.h"
#include "IFXOSUtilities.h"

#define _RESULT_VALUE_MESSAGE_LENGTH_MAX  16
#define _PREFIX_STRING            L"[IFX]"
#define _MESSAGE_LENGTH_MAX                 1024

static IFXDebugLevel  gs_debugLevel = IFXDEBUG_DEFAULT;
static IFXCHAR       gs_pDebugMessage[ _MESSAGE_LENGTH_MAX ];

const char* _GetStringFromIFXRESULT( const IFXRESULT  result );
const IFXCHAR* _GetStringFromIFXRESULT_Component( const U32  component );

//---------------------------------------------------------------------------
//  IFXAssert
//
//  This function is used to display Assert Box.
//---------------------------------------------------------------------------
int IFXAssert(
			  int bShowMessage,
			  const char* szFile,
			  unsigned int uLineNum,
			  const char* szExpression,
			  const char* szMessage,
			  int bLog
			  )
{
	int rVal = bShowMessage;
	IFXCHAR *cszFile = NULL, *cszMessage = NULL, *cszExpression = NULL;
	size_t s;

	s=mbstowcs(NULL, szFile, 0);
	if ((int)s != -1)
		cszFile = new IFXCHAR[s+1];
	if (NULL != cszFile) 
	{
		mbstowcs(cszFile, szFile, s);
		cszFile[s]=L'\0';
	}

	s=mbstowcs(NULL, szMessage, 0);
	if (s != (size_t)-1)
		cszMessage = new IFXCHAR[s+1];
	if (NULL != cszMessage) 
	{
		mbstowcs(cszMessage, szMessage, s);
		cszMessage[s]=L'\0';
	}

	s=mbstowcs(NULL, szExpression, 0);
	if (s != (size_t)-1)
		cszExpression = new IFXCHAR[s+1];
	if (NULL != cszExpression) 
	{
		mbstowcs(cszExpression, szExpression, s);
		cszExpression[s]=L'\0';
	}


	if (NULL != cszFile && NULL != cszMessage && NULL != cszExpression) 
	{
		if(bLog) 
		{
			IFXDebugTraceGeneric(L"%s(%d) : %s : %s\n", 
				cszFile, uLineNum, cszMessage, 
				cszExpression );
		}

		if(bShowMessage)
		{
			rVal = IFXOSDebugOutput( cszMessage, cszFile, uLineNum,
				cszExpression );
		}
	}

	if (cszFile) delete cszFile;
	if (cszMessage) delete cszMessage;
	if (cszExpression) delete cszExpression;

	return rVal;
}


//---------------------------------------------------------------------------
//  IFXDebugAssertBox
//
//  This function is exported outside.
//---------------------------------------------------------------------------
extern "C"
void IFXAPI IFXDebugAssertBox(
							  const char *exp,
							  const char* szFile,
							  unsigned int uLineNum,
							  const char *message,
							  int box,
							  int log
							  )
{
	static int bShowMessage = box;
	int rVal = IFXAssert(   bShowMessage,
		szFile,
		uLineNum,
		exp, message, log );

	bShowMessage = rVal & 1;
	if(rVal & 2) abort();
}


//---------------------------------------------------------------------------
//  IFXDebugStartUp
//
//  This function is used to perform any required startup tasks prior to
//  using the debug macros or functions in this module.  Currently, this only
//  involves initializing the gs_debugLevel module global.
//---------------------------------------------------------------------------
extern "C"
void IFXAPI IFXDebugStartUp( void )
{
	gs_debugLevel = IFXOSGetDebugLevel();
}

//---------------------------------------------------------------------------
//  IFXDebugShutDown
//
//  This function is used to perform any required shutdown tasks after
//  having called IFXDebugStartUp.
//---------------------------------------------------------------------------
extern "C"
void IFXAPI IFXDebugShutDown( void )
{
	gs_debugLevel = IFXDEBUG_DEFAULT;
}

//---------------------------------------------------------------------------
//  IFXDebugTraceCustom
//
//  This function is used to display to the output debug window a message for
//  an arbirary component and debug level.  The parameters are the same as
//  the standard printf function, except they are prefixed FIRST by two extra
//  parameters.  The first parameter specifies a system component identifier.
//  The second parameter specifies an IFXDebug_Level enumerated value.
//  Afterwards the standard printf parameters follow (i.e. a format string
//  followed by a variable arguement list).
//
//  Note:  Make sure the final message doesn't exceed _MESSAGE_LENGTH_MAX
//  characters.
//---------------------------------------------------------------------------
extern "C"
I32 IFXAPI IFXDebugTraceCustom( const U32        component,
							   const IFXDebugLevel  debugLevel,
							   const IFXCHAR*      pFormatString,
							   ... )
{
#ifndef HAS_NO_WPRINT
	if ( gs_debugLevel >= debugLevel )
	{
		const IFXCHAR  *pComponentString = _GetStringFromIFXRESULT_Component( component );

		IFXCHAR pMessage[ _MESSAGE_LENGTH_MAX ];
		wcscpy( pMessage, _PREFIX_STRING );
		if( pComponentString )
			wcscat( pMessage, pComponentString );
		wcscat( pMessage, L" " );
		swprintf( gs_pDebugMessage, _MESSAGE_LENGTH_MAX, pMessage );
		IFXOSOutputDebugString( gs_pDebugMessage );

		va_list args;
		va_start( args, pFormatString );
		vswprintf( gs_pDebugMessage, _MESSAGE_LENGTH_MAX,
			pFormatString,
			args );
		va_end( args );

		IFXOSOutputDebugString( gs_pDebugMessage );
	}
#endif
	return 0;
}

//---------------------------------------------------------------------------
//  IFXDebugTraceGeneric
//
//  This function is used to display to the output debug window a
//  IFXDEBUG_MESSAGE level message.  The parameters are the same as the
//  the standard printf function (i.e. a format string followed by a variable
//  arguement list).
//
//  Note:  Make sure the final message doesn't exceed _MESSAGE_LENGTH_MAX
//  characters.
//---------------------------------------------------------------------------
extern "C"
I32 IFXAPI IFXDebugTraceGeneric( const IFXCHAR* pFormatString, ... )
{
#ifndef HAS_NO_WPRINT
	if ( gs_debugLevel >= IFXDEBUG_MESSAGE )
	{
		const IFXCHAR  *pComponentString = 
			_GetStringFromIFXRESULT_Component( IFXRESULT_COMPONENT_GENERIC );

		IFXCHAR pMessage[ _MESSAGE_LENGTH_MAX ];
		wcscpy( pMessage, _PREFIX_STRING );
		if( pComponentString )
			wcscat( pMessage, pComponentString );
		wcscat( pMessage, L" " );
		swprintf( gs_pDebugMessage, _MESSAGE_LENGTH_MAX, pMessage );
		IFXOSOutputDebugString( gs_pDebugMessage );

		va_list args;
		va_start( args, pFormatString );
		vswprintf( gs_pDebugMessage, _MESSAGE_LENGTH_MAX,
			pFormatString, args );
		va_end( args );

		IFXOSOutputDebugString( gs_pDebugMessage );
	}
#endif
	return 0;
}

//---------------------------------------------------------------------------
//  _GetStringFromIFXRESULT
//
//  This function returns a pointer to a constant string that represents the
//  specified IFXRESULT.  If the result isn't recognized, NULL is returned.
//---------------------------------------------------------------------------
const char* _GetStringFromIFXRESULT( const IFXRESULT  result )
{
	const char  *pString  = NULL;

#define MAKE_RESULT_CASE( aResult ) case aResult: pString = #aResult; break

	switch ( result )
	{
		MAKE_RESULT_CASE( IFX_OK );
		MAKE_RESULT_CASE( IFX_TRUE );
		MAKE_RESULT_CASE( IFX_CANCEL );
		MAKE_RESULT_CASE( IFX_W_UNEXPECTED_FILE_VERSION );
		MAKE_RESULT_CASE( IFX_W_ALREADY_EXISTS );
		MAKE_RESULT_CASE( IFX_E_UNDEFINED );
		MAKE_RESULT_CASE( IFX_E_UNSUPPORTED );
		MAKE_RESULT_CASE( IFX_E_OUT_OF_MEMORY );
		MAKE_RESULT_CASE( IFX_E_INVALID_FILE );
		MAKE_RESULT_CASE( IFX_E_INVALID_HANDLE );
		MAKE_RESULT_CASE( IFX_E_INVALID_POINTER );
		MAKE_RESULT_CASE( IFX_E_INVALID_RANGE );
		MAKE_RESULT_CASE( IFX_E_ALREADY_INITIALIZED );
		MAKE_RESULT_CASE( IFX_E_NOT_INITIALIZED );
		MAKE_RESULT_CASE( IFX_E_CANNOT_CHANGE );
		MAKE_RESULT_CASE( IFX_E_ABORTED );
		MAKE_RESULT_CASE( IFX_E_WRITE_FAILED );
		MAKE_RESULT_CASE( IFX_E_READ_FAILED );
		MAKE_RESULT_CASE( IFX_E_CANNOT_FIND );
	}

#undef MAKE_RESULT_CASE

	return pString;
}

//---------------------------------------------------------------------------
//  _GetStringFromIFXRESULT_Component
//
//  This function returns a pointer to a constant string that represents the
//  specified system component.  If the component isn't recognized, NULL
//  is returned.
//---------------------------------------------------------------------------
const IFXCHAR* _GetStringFromIFXRESULT_Component( const U32  component )
{
	const IFXCHAR  *pString  = NULL;

#define MAKE_COMPONENT_CASE( aComponent, aString )  case aComponent: pString = aString; break

	switch ( component )
	{
		// Note:  The generic component isn't included here because we don't
		// want it to clutter the output debug window.
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_AUTHORGEOM,			L"[AuthorGeom]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_BITSTREAM,				L"[BitStream]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_CORE_SERVICES,			L"[CoreServices]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_DATAPACKET,			L"[DataPacket]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_IMAGE_TOOLS,			L"[ImageTools]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_LOADER,				L"[Loader]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_MODIFIER_DATAPACKET,	L"[DataPacket]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_MODIFIER,				L"[Modifier]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_MODIFIERCHAIN,			L"[ModifierChain]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_PALETTE_MANAGER,		L"[PaletteManager]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_READ_BUFFER,			L"[ReadBuffer]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_SCENEGRAPH,			L"[SceneGraph]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_SCHEDULER,				L"[Scheduler]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_TEXTURE_MANAGER,		L"[TextureManager]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_WRITE_BUFFER,			L"[WriteBuffer]" );
		MAKE_COMPONENT_CASE( IFXRESULT_COMPONENT_WRITER,				L"[Writer]" );
		MAKE_COMPONENT_CASE( IFXRESULT_CONVERTERS,						L"[Converters]" );
		MAKE_COMPONENT_CASE( IFXRESULT_NEIGHBORMESH,					L"[NeighborMesh]" );
		MAKE_COMPONENT_CASE( IFXRESULT_SUBDIV,							L"[Subdiv]" );
	}

#undef MAKE_COMPONENT_CASE

	return pString;
}


#endif
