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
	@file	IFXResult.h

			This module defines the IFXRESULT data type, IFXSUCCESS/IFXFAILURE
			testing macros and the various return codes (both warnings and errors)
			that are returned from different subsystems.
*/


#ifndef IFXResult_H
#define IFXResult_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXDataTypes.h"


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//---------------------------------------------------------------------------
//	IFXRESULT
//
//	This type is just a signed 32 bit value used to define function and
//	method return values.  Macros are available to dissect the various bit
//	fields which are defined as follows:
//
//		  MSB                                       LSB
//		+----------+-----------+-----------+-----------+
//		|    31    | 30 ... 28 | 27 ... 16 | 15 ... 00 |
//		+----------+-----------+-----------+-----------+
//		| Severity | Reserved  | Component |   Code    |
//		+----------+-----------+-----------+-----------+
//
//		Severity:	0 for success or 1 for failure.
//		Reserved:	Always 0.
//		Component:	12 bits that identify who generated the result.
//		Code:		16 bits that identify the result code.
//
//---------------------------------------------------------------------------

typedef	I32	IFXRESULT;


//***************************************************************************
//	Enumerations
//***************************************************************************


/**
	Defines the successful and failure severities.
*/
enum IFXResult_Severity
{
	IFXRESULT_SEVERITY_SUCCESS,
	IFXRESULT_SEVERITY_FAILURE
};


/**
	Defines the generic system component
*/
enum IFXResult_Component
{
	IFXRESULT_COMPONENT_GENERIC,

	// The following enumerator should only be used to determine the next 
	// available system component enumerator.
	IFXRESULT_COMPONENT_NEXT
};


//***************************************************************************
//	Defines.
//***************************************************************************


//---------------------------------------------------------------------------
//	IFXSUCCESS
//	IFXFAILURE
//
//	Return code testing macros.
//---------------------------------------------------------------------------

#define IFXSUCCESS( result ) \
			( IFXRESULT( result ) >= 0 )

#define IFXFAILURE( result ) \
			( IFXRESULT( result ) < 0 )

#define IFXRUN(result, code) if(IFXSUCCESS( result )) {result = code;}

//---------------------------------------------------------------------------
//	MAKE_IFXRESULT
//	MAKE_IFXRESULT_SUCCESS
//	MAKE_IFXRESULT_FAILURE
//
//	Return code construction macros.
//---------------------------------------------------------------------------

#define MAKE_IFXRESULT( severity, component, code ) \
			IFXRESULT( ( U32( severity ) << 31 ) | ( U32( component ) << 16 ) | ( U32( code ) ) )

#define MAKE_IFXRESULT_PASS( component, code ) \
			MAKE_IFXRESULT( IFXRESULT_SEVERITY_SUCCESS, component, code )

#define MAKE_IFXRESULT_FAIL( component, code ) \
			MAKE_IFXRESULT( IFXRESULT_SEVERITY_FAILURE, component, code )

//---------------------------------------------------------------------------
//	PULL_IFXRESULT_SEVERITY
//	PULL_IFXRESULT_COMPONENT
//	PULL_IFXRESULT_CODE
//
//	Return code deconstruction macros.
//---------------------------------------------------------------------------

#define PULL_IFXRESULT_SEVERITY( result ) \
			( ( (result) >> 31 ) & 0x00000001 )

#define PULL_IFXRESULT_COMPONENT( result ) \
			( ( (result) >> 16 ) & 0x00000FFF )

#define PULL_IFXRESULT_CODE( result ) \
			( ( result ) & 0x0000FFFF )


//---------------------------------------------------------------------------
//	Successful return codes:  IFXRESULT_COMPONENT_GENERIC.
//---------------------------------------------------------------------------

/// Successful.
#define IFX_OK								MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0000 )

/// Successful and the result is false.
#define IFX_FALSE							MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0000 )

/// Successful and the result is true.
#define IFX_TRUE							MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0001 )

/// Successful and the user requested cancel.
#define IFX_CANCEL							MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0002 )

//---------------------------------------------------------------------------
//	Warning return codes:  IFXRESULT_COMPONENT_GENERIC.
//---------------------------------------------------------------------------

/// Successful, but the file version was different than expected.
#define IFX_W_UNEXPECTED_FILE_VERSION		MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0003 )

/// Successful, but the item already exists.
#define IFX_W_ALREADY_EXISTS				MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0004 )

/// Successful, but the processing is finished.
#define IFX_W_FINISHED						MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0005 )

/// Successful, but plug-in library cannot be unloaded.
#define IFX_W_CANNOT_UNLOAD					MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_GENERIC, 0x0006 )

//---------------------------------------------------------------------------
//	Failure return codes:  IFXRESULT_COMPONENT_GENERIC.
//---------------------------------------------------------------------------

/// An undefined error occurred.
#define IFX_E_UNDEFINED						MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0000 )

/// Operation specified is not supported.
#define IFX_E_UNSUPPORTED					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0001 )

/// System memory request failed.
#define IFX_E_OUT_OF_MEMORY					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0002 )

/// File specified is invalid or cannot be found.
#define IFX_E_INVALID_FILE					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0003 )

/// Handle specified is invalid.
#define IFX_E_INVALID_HANDLE				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0004 )

/// Pointer specified is invalid.
#define IFX_E_INVALID_POINTER				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0005 )

/// One or more parameters are out of range.
#define IFX_E_INVALID_RANGE					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0006 )

/// Object is already initialized.
#define IFX_E_ALREADY_INITIALIZED			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0007 )

/// Object is not yet initialized.
#define IFX_E_NOT_INITIALIZED				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0008 )

/// Data is currently read-only.
#define IFX_E_CANNOT_CHANGE					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0009 )

/// Function execution was canceled.
#define IFX_E_ABORTED						MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x000A )

/// write failed (file or device), perhaps out of space.
#define IFX_E_WRITE_FAILED					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x000B )

/// read failed (file or device), perhaps end of file.
#define IFX_E_READ_FAILED					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x000C )

/// Could not find resource.
#define IFX_E_CANNOT_FIND					MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x000D )

/// Subsystem (video card, etc) out of memory.
#define IFX_E_SUBSYSTEM_OUT_OF_MEMORY		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x000E )

/// Subsystem failed.
#define IFX_E_SUBSYSTEM_FAILURE				MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x000F )

/// One or more parameters is not yet initialized.
#define IFX_E_PARAMETER_NOT_INITIALIZED		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0010 )

/// One or more parameters faulty, flawed or below expectations.
#define IFX_E_BAD_PARAM						MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0011 )

/// Operation is not done.
#define IFX_E_NOT_DONE						MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0012 )

#define IFX_E_KEY_ALREADY_EXISTS			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0013 )

/// Object is not found.
#define IFX_E_NOT_FOUND						IFX_E_CANNOT_FIND

/// Resource is not available.
#define IFX_E_RESOURCE_NOT_AVAILABLE		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_GENERIC, 0x0014 )

// SGMatrix errors.
#define IFX_E_INVALID_VECTOR		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_SCENEGRAPH, 0xa005)
#define IFX_E_INVALID_POINT			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_SCENEGRAPH, 0xa006)

#define IFX_E_INVALID_INDEX			MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_SCENEGRAPH, 0xa007)

// DX7 specific
#define IFX_E_DX7SURFACELOST		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_SCENEGRAPH, 0xa008)

//***************************************************************************
//	Additional includes
//***************************************************************************


#include "IFXResultComponentEngine.h"


#endif
