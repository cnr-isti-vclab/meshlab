/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: Xerces_autoconf_config.hpp.in 834826 2009-11-11 10:03:53Z borisk $
 */

//
// There are two primary xerces configuration header files:
//
//	Xerces_autoconf_config.hpp
//
//      For configuration of items that must be accessable
//	through public headers. This file has limited information
//	and carefully works to avoid collision of macro names, etc.
//
//	This file is included by XercesDefs.h. In the event
//	of a non-configured platform, a similar header specific
//	to the platform will be included instead.
//
//	config.h
//
//      Generalized cmake-generated header file, with much more
//	information, used to supply configuration information
//	for use in implementation files.
//
// For CMake-based builds, this header is configured by cmake from the
// .cmake.in template file of the same name.


#ifndef XERCES_AUTOCONFIG_CONFIG_HPP
#define XERCES_AUTOCONFIG_CONFIG_HPP

// ---------------------------------------------------------------------------
//  These defines are set by cmake as appropriate for the platform.
// ---------------------------------------------------------------------------
#define XERCES_AUTOCONF 1
#cmakedefine XERCES_HAVE_SYS_TYPES_H 1
#cmakedefine XERCES_HAVE_CSTDINT 1
#cmakedefine XERCES_HAVE_STDINT_H 1
#cmakedefine XERCES_HAVE_INTTYPES_H 1
#cmakedefine XERCES_HAVE_INTRIN_H 1
#cmakedefine XERCES_HAVE_EMMINTRIN_H 1
#cmakedefine XERCES_INCLUDE_WCHAR_H 1

#define XERCES_S16BIT_INT @XERCES_S16BIT_INT@
#define XERCES_S32BIT_INT @XERCES_S32BIT_INT@
#define XERCES_S64BIT_INT @XERCES_S64BIT_INT@
#define XERCES_U16BIT_INT @XERCES_U16BIT_INT@
#define XERCES_U32BIT_INT @XERCES_U32BIT_INT@
#define XERCES_U64BIT_INT @XERCES_U64BIT_INT@
#define XERCES_XMLCH_T @XERCES_XMLCH_T@
#define XERCES_SIZE_T @XERCES_SIZE_T@
#define XERCES_SSIZE_T @XERCES_SSIZE_T@

#cmakedefine XERCES_HAS_CPP_NAMESPACE 1
#cmakedefine XERCES_STD_NAMESPACE 1
#cmakedefine XERCES_NEW_IOSTREAMS 1
#cmakedefine XERCES_NO_NATIVE_BOOL 1
#cmakedefine XERCES_LSTRSUPPORT 1
#cmakedefine XERCES_MFC_SUPPORT 1

#cmakedefine XERCES_HAVE_CPUID_INTRINSIC 1
#cmakedefine XERCES_HAVE_SSE2_INTRINSIC 1
#cmakedefine XERCES_HAVE_GETCPUID 1

#cmakedefine XERCES_NO_MATCHING_DELETE_OPERATOR 1

#cmakedefine XERCES_DLL_EXPORT 1
#cmakedefine XERCES_STATIC_LIBRARY 1
#define XERCES_PLATFORM_EXPORT @XERCES_PLATFORM_EXPORT@
#define XERCES_PLATFORM_IMPORT @XERCES_PLATFORM_IMPORT@
#define XERCES_TEMPLATE_EXTERN @XERCES_TEMPLATE_EXTERN@
#ifdef XERCES_DLL_EXPORT
#  define DLL_EXPORT
#endif

// ---------------------------------------------------------------------------
//  Include standard headers, if available, that we may rely on below.
// ---------------------------------------------------------------------------
#if defined(__cplusplus) && defined(XERCES_HAVE_CSTDINT)
#  include <cstdint>
#endif
#if XERCES_HAVE_STDINT_H
#  include <stdint.h>
#endif
#if XERCES_HAVE_INTTYPES_H
#  include <inttypes.h>
#endif
#if XERCES_HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if XERCES_INCLUDE_WCHAR_H
#  include <wchar.h>
#endif

// ---------------------------------------------------------------------------
//  XMLSize_t is the unsigned integral type.
// ---------------------------------------------------------------------------
typedef XERCES_SIZE_T				XMLSize_t;
typedef XERCES_SSIZE_T				XMLSSize_t;

#define XERCES_SIZE_MAX @XERCES_SIZE_MAX@
#define XERCES_SSIZE_MAX @XERCES_SSIZE_MAX@

// ---------------------------------------------------------------------------
//  Define our version of the XML character
// ---------------------------------------------------------------------------
typedef XERCES_XMLCH_T				XMLCh;

// ---------------------------------------------------------------------------
//  Define unsigned 16, 32, and 64 bit integers
// ---------------------------------------------------------------------------
typedef XERCES_U16BIT_INT			XMLUInt16;
typedef XERCES_U32BIT_INT			XMLUInt32;
typedef XERCES_U64BIT_INT			XMLUInt64;

// ---------------------------------------------------------------------------
//  Define signed 16, 32, and 64 bit integers
// ---------------------------------------------------------------------------
typedef XERCES_S16BIT_INT			XMLInt16;
typedef XERCES_S32BIT_INT			XMLInt32;
typedef XERCES_S64BIT_INT			XMLInt64;

// ---------------------------------------------------------------------------
//  XMLFilePos is the type used to represent a file position.
// ---------------------------------------------------------------------------
typedef XMLUInt64			        XMLFilePos;

// ---------------------------------------------------------------------------
//  XMLFileLoc is the type used to represent a file location (line/column).
// ---------------------------------------------------------------------------
typedef XMLUInt64			        XMLFileLoc;

// ---------------------------------------------------------------------------
//  Force on the Xerces debug token if it is on in the build environment
// ---------------------------------------------------------------------------
#if defined(_DEBUG)
#define XERCES_DEBUG
#endif

#endif
