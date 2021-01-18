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
  @file IFXDataTypes

      Defines basic data types and precompiler stuff used by the U3D.
*/


#ifndef IFXDataTypes_h
#define IFXDataTypes_h

#include <stdio.h>
#include <sys/types.h>
#include <math.h>

#ifdef _MSC_VER
typedef unsigned __int64  U64;
typedef unsigned int    U32;
typedef unsigned short    U16;
typedef unsigned char   U8;
typedef unsigned int    UPTR;
typedef __int64     I64;
typedef int       I32;
typedef short     I16;
typedef char      I8;
#define U32_MAX ULONG_MAX
#else
#include <stdint.h>
typedef uint64_t  U64;
typedef uint32_t  U32;
typedef uint16_t  U16;
typedef uint8_t   U8;
typedef uintptr_t UPTR;
typedef int64_t  I64;
typedef int32_t  I32;
typedef int16_t  I16;
typedef int8_t   I8;
#define U32_MAX 4294967295U
#endif

typedef float     F32;
typedef double      F64;

typedef int       BOOL;   // Typedef matches Windows.H.
typedef wchar_t IFXCHAR;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef NULL
#define NULL 0
#endif

// Macros
#define IFXDELETE(p)    if ((p)) {delete (p); (p) = NULL;}
#define IFXDELETE_ARRAY(p)  if ((p)) {delete [] (p); (p) = NULL;}
#define IFXRELEASE(p)   if ((p)) {(p)->Release(); (p) = NULL;}
#define IFXADDREF(p)    if ((p)) { (p)->AddRef(); }

#define IFXARRAY_SIZE(p) (sizeof(p)/sizeof(p[0]))

#define IFXMAX(x,y) ((x) > (y)?(x):(y))
#define IFXMIN(x,y) ((x) < (y)?(x):(y))


// NOTE under VCC, *f() functions don't help, they just map
#define IFXSIN    sinf
#define IFXCOS    cosf
#define IFXASIN   asinf
#define IFXACOS   acosf
#define IFXPOW    powf
#define IFXSQRT   sqrtf
#define IFXTAN    tanf
#define IFXATAN   atanf
#define IFXATAN2  atan2f

#ifdef SUN32
#define   sinf  sin
#define   cosf  cos
#define  asinf  asin
#define  acosf  acos
#define   powf  pow
#define  sqrtf  sqrt
#define   tanf  tan
#define  atanf  atan
#define atan2f  atan2
#define  fabsf  fabs
#define floorf  floor
#endif

#define IFXPI       (3.1415926f)
#define IFXTO_RAD   (IFXPI/180.0f)
#define IFXTO_DEG (180.0f/IFXPI)

#define IFXINLINE inline
#ifdef _MSC_VER
#define IFXFORCEINLINE __forceinline
#else
#define IFXFORCEINLINE inline
#endif

typedef void* IFXHANDLE;

// Define IFX_U16_INDEX on compiler cmd line to
// change INDEX to a 16-bit index (useful for d3d)
#ifdef IFX_U16_INDEX
typedef U16   IFXGeomIndex;
#define     IFX_MAX_GEOMINDEX   USHRT_MAX
#else
typedef U32   IFXGeomIndex;
#define     IFX_MAX_GEOMINDEX   UINT_MAX
#endif

#if( MIPSEB || IL_BIG_ENDIAN || defined( __BIG_ENDIAN__ ) 						|| \
	( defined(   BYTE_ORDER ) && defined(   BIG_ENDIAN ) && (   BYTE_ORDER ==   BIG_ENDIAN ) )	|| \
	( defined(  _BYTE_ORDER ) && defined(  _BIG_ENDIAN ) && (  _BYTE_ORDER ==  _BIG_ENDIAN ) )	|| \
	( defined( __BYTE_ORDER ) && defined( __BIG_ENDIAN ) && ( __BYTE_ORDER == __BIG_ENDIAN ) ) )	// BIG_ENDIAN
#define SET_ENDIAN16(X) ((((X) & 0xff00) >> 8) | \
            (((X) & 0x00ff) << 8))
#define SET_ENDIAN32(X) ((((X) & 0xff000000u) >> 24) | \
            (((X) & 0x00ff0000u) >> 8) | \
            (((X) & 0x0000ff00u) << 8) | \
            (((X) & 0x000000ffu) << 24))
#define SET_ENDIAN64(X) \
           ((((X) & 0xff00000000000000ull) >> 56) | \
            (((X) & 0x00ff000000000000ull) >> 40) | \
            (((X) & 0x0000ff0000000000ull) >> 24) | \
            (((X) & 0x000000ff00000000ull) >>  8) | \
            (((X) & 0x00000000ff000000ull) <<  8) | \
            (((X) & 0x0000000000ff0000ull) << 24) | \
            (((X) & 0x000000000000ff00ull) << 40) | \
            (((X) & 0x00000000000000ffull) << 56))
#define SWAP32_IF_BIGENDIAN(value) value = SET_ENDIAN32(value)
#define SWAP64_IF_BIGENDIAN(value) value = SET_ENDIAN64(value)
#else	// LITTLE_ENDIAN
#define SET_ENDIAN16(X) (X)
#define SET_ENDIAN32(X) (X)
#define SET_ENDIAN64(X) (X)
#define SWAP32_IF_BIGENDIAN(value)
#define SWAP64_IF_BIGENDIAN(value)
#endif	// ENDIAN

#endif
