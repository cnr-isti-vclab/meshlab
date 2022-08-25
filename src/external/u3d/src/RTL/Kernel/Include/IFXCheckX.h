//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
	@file	IFXCheckX.h

			This header defines utility functionality for use in sections of 
			code where it's okay to throw exceptions.

	@note	There's a hopefully used coding convention of adding an "X" 
			suffix to the end of the names of functions that throw 
			IFXException exceptions.  This is also done inconsitently for 
			class names as well.
*/


#if !defined( IFXCHECKX_H )
#define IFXCHECKX_H

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXDataTypes.h"
#include "IFXException.h"
#include "IFXDebug.h"
#include "IFXResult.h"

//***************************************************************************
//	Inline functions
//***************************************************************************

#if !defined( _DEBUG )


/**
	Inline utility function that checks the specified result code and if 
	it's a failure, it throws an IFXException.
*/
IFXFORCEINLINE void IFXCHECKX( IFXRESULT result )
{
	if ( IFXFAILURE( result ) )
		throw IFXException( result );
}

#else

/**
	Inline debug utility function used by the IFXCHECKX debug macro.

	@note	This function is never intended to be used directly by other 
			modules.
*/
inline void IFXCheckX_Inline( IFXRESULT result, const IFXCHAR* pString )
{
	if ( IFXFAILURE( result ) )
	{
		IFXTRACE_GENERIC( pString );
		IFXASSERT( FALSE );
		throw IFXException( result, pString );
	}
}

#endif

//***************************************************************************
//	Defines
//***************************************************************************

/**
	Inline utility macro that checks the specified statement and if it's zero, 
	it throws an IFXException with the specified result.
*/
#define IFXCHECKX_RESULT( statement, result )	\
			if ( !( statement ) )				\
			{									\
				IFXASSERT( FALSE );				\
				throw IFXException( result );	\
			}

#if defined( _DEBUG )
/**
	Debug macro to check the IFXRESULT returned by the parameter statement 
	that is specified.  If it's a failure code, an IFXException is thrown.
*/
#ifdef WIN32
#define IFXCHECKX( statement )	\
			IFXCheckX_Inline(	\
				( statement ),	\
				__WFILE__ L"(" __THISLINE__ L") : IFXCHECKX("  L#statement L") failed\n" )
#else
#define IFXCHECKX( statement )	\
			IFXCheckX_Inline(	\
				( statement ),	\
				__WFILE__ L"(" __THISLINE__ L") : IFXCHECKX("  L ## #statement L") failed\n" )
#endif
#endif

#endif
