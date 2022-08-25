//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXException.h

			This file contains the complete IFXException class.

	@note	All exceptions thrown by IFX code should by of type IFXException.
*/


#ifndef __IFXException_h__
#define __IFXException_h__


//***************************************************************************
//	Includes
//***************************************************************************


#include <stdarg.h>

#include "IFXDebug.h"
#include "IFXString.h"


//***************************************************************************
//	Defines
//***************************************************************************


// Sometimes exception throwing seems to get derailed (or we want the stack
// preserved)...so we cause assert.
//#define IFX_E_ASSERT


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//---------------------------------------------------------------------------
/**
	This class is the one type of exception explicitly thrown by IFX code.  
	It supports both IFXRESULT codes and string messages.  Helpful utilities 
	commonly used with this class are contained in the file IFXCheckX.h.

	@par	Example @e catch usage:
			@verbatim
			try
			{
				SomethingX();
			}
			catch( IFXException& rException )
			{
				rException.Log();
				throw;
			}
			@endverbatim

	@par	Example @e throw usage:
			@verbatim
			SomethingX()
			{
				// When only a string is specified, the associated 
				// IFXRESULT code will be IFX_E_UNDEFINED.
				throw IFXException( L"Something broke!" );
			}
			@endverbatim
			or
			@verbatim
			SomethingX()
			{
				throw IFXException(
						IFX_E_OUT_OF_MEMORY,
						L"System memory request failed." );
			}
			@endverbatim
*/
class IFXException
{
	public:
		/** Default constructor */
		IFXException( void )
		{
#ifdef IFX_E_ASSERT
			IFXASSERT( FALSE );
#endif
		}

		/** Constructor setting an IFXRESULT code. */
		IFXException( const IFXRESULT result )
		{
			m_result = result;
#ifdef IFX_E_ASSERT
			IFXASSERT( FALSE );
#endif
		}

		/** Constructor setting the message string directly. */
		IFXException( const IFXCHAR* pText )
		{
			m_result = IFX_E_UNDEFINED;
			m_text = pText;
#ifdef IFX_E_ASSERT
			IFXASSERT( FALSE );
#endif
		}

		/** Constructor setting the message string via a printf-style 
			interface. */
		IFXException( const IFXCHAR* pFmt, ... )
		{
			m_result = IFX_E_UNDEFINED;
			va_list ap;
			va_start( ap, pFmt );
//#ifdef IFX_USE_PLATFORM_ABSTRACTION
			m_text.VSPrintf( pFmt, ap );
//#endif
			va_end( ap );
#ifdef IFX_E_ASSERT
			IFXASSERT( FALSE );
#endif
		}

		/** Constructor setting the message string via a printf interface,
			and setting an IFXRESULT code. */
		IFXException( const IFXRESULT result, const IFXCHAR* pFmt, ... )
		{
			m_result = result;
			va_list ap;
			va_start( ap, pFmt );
//#ifdef IFX_USE_PLATFORM_ABSTRACTION
			m_text.VSPrintf( pFmt, ap );
//#endif
			va_end( ap );
#ifdef IFX_E_ASSERT
			IFXTRACE_GENERIC( L"ASSERTION: %ls\n", m_text.Raw() );
			IFXASSERT( FALSE );
#endif
		}

		/** Copy constructor */
		IFXException( const IFXException& rOther )
		{
			*this = rOther;
#ifdef IFX_E_ASSERT
			IFXASSERT( FALSE );
#endif
		}

		/** Destructor */
		virtual	~IFXException( void )
		{
		}

		/** Assignment operator */
		IFXException& operator=( const IFXException& rOther )
		{
			if ( this != &rOther )
			{
				m_result = rOther.m_result;
				m_text = rOther.m_text;
			}

			return *this;
		}

		/** Gets the IFXResult code. */
		IFXRESULT GetIFXResult( void )
		{
			return m_result;
		}

		/** Gets the exception message string. */
		const IFXString& GetText( void )
		{
			return m_text;
		}

		/** Use IFXTRACE to output information about this exception. */
		void Log( void )
		{
			IFXTRACE_GENERIC(
				L"EXCEPTION: 0x%08lX %ls\n",
				m_result,
				( NULL != m_text.Raw() ) ? m_text.Raw() : L"" );
		}

	protected:
		IFXRESULT	m_result;
		IFXString	m_text;
};


#endif
