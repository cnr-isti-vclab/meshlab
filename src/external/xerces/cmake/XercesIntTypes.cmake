
# CMake build for xerces-c
#
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Integer type checks.

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)
include(CheckTypeSize)

check_include_file_cxx(cstdint HAVE_CSTDINT)

check_cxx_source_compiles("
#include <cstdint>

int main() {
  uint32_t v1 = 342;
  int64_t v2 = -23;
  return 0;
}" CSTDINT_FUNCTIONAL)

check_include_file_cxx(stdint.h HAVE_STDINT_H)
check_include_file_cxx(inttypes.h HAVE_INTTYPES_H)

if(HAVE_CSTDINT AND CSTDINT_FUNCTIONAL)
  set(XERCES_HAVE_CSTDINT TRUE)
else()
  set(XERCES_HAVE_CSTDINT FALSE)
endif()
set(XERCES_HAVE_STDINT_H ${HAVE_STDINT_H})
set(XERCES_HAVE_INTTYPES_H ${HAVE_INTTYPES_H})

# Standard typedefs
set(CMAKE_EXTRA_INCLUDE_FILES_SAVE ${CMAKE_EXTRA_INCLUDE_FILES})
set(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES} "stddef.h")
check_type_size("off_t" SIZEOF_OFF_T)
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("ssize_t" SIZEOF_SSIZE_T)
check_type_size("wchar_t" SIZEOF_WCHAR_T)
set(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES_SAVE})
set(HAVE_OFF_T ${SIZEOF_OFF_T})
set(HAVE_SIZE_T ${SIZEOF_SIZE_T})
set(HAVE_SSIZE_T ${SSIZEOF_SSIZE_T})
set(HAVE_WCHAR_T ${WCHAROF_WCHAR_T})
if(HAVE_SIZEOF_SIZE_T)
  set(XERCES_SIZE_T size_t)
  set(XERCES_SIZE_MAX SIZE_MAX)
else()
  set(XERCES_SIZE_T "unsigned long")
  set(XERCES_SIZE_MAX ULONG_MAX)
endif()
if(HAVE_SIZEOF_SSIZE_T)
  set(XERCES_SSIZE_T ssize_t)
  set(XERCES_SSIZE_MAX SSIZE_MAX)
else()
  set(XERCES_SSIZE_T long)
  set(XERCES_SSIZE_MAX LONG_MAX)
endif()

# Check type sizes
check_type_size("signed short"       SIZEOF_SHORT)
check_type_size("unsigned short"     SIZEOF_UNSIGNED_SHORT)
check_type_size("signed int"         SIZEOF_INT)
check_type_size("unsigned int"       SIZEOF_UNSIGNED_INT)
check_type_size("signed long"        SIZEOF_LONG)
check_type_size("unsigned long"      SIZEOF_UNSIGNED_LONG)
check_type_size("signed long long"   SIZEOF_LONG_LONG)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
check_type_size("__int64"            SIZEOF___INT64)
check_type_size("unsigned __int64"   SIZEOF_UNSIGNED__INT64)
check_type_size("unsigned char *"    SIZEOF_UNSIGNED_CHAR_P)

# If this failed, set size to zero to match autoconf
if(NOT SIZEOF___INT64)
  set(SIZEOF___INT64 0)
endif()
if(NOT SIZEOF_UNSIGNED__INT64)
  set(SIZEOF_UNSIGNED__INT64 0)
endif()

if(HAVE_CSTDINT OR HAVE_STDINT_H OR HAVE_INTTYPES_H)
  # Use standard sized types if possible
  set(XERCES_S16BIT_INT "int16_t")
  set(XERCES_U16BIT_INT "uint16_t")
  set(XERCES_S32BIT_INT "int32_t")
  set(XERCES_U32BIT_INT "uint32_t")
  set(XERCES_S64BIT_INT "int64_t")
  set(XERCES_U64BIT_INT "uint64_t")
else()
  # Fallback to basic language types
  if(SIZEOF_SHORT EQUAL 2)
    set(XERCES_S16BIT_INT "signed short")
  elseif(SIZEOF_INT EQUAL 2)
    set(XERCES_S16BIT_INT "int")
  else()
    message(FATAL_ERROR "Couldn't find a signed 16-bit type")
  endif()

  if(SIZEOF_UNSIGNED_SHORT EQUAL 2)
    set(XERCES_U16BIT_INT "unsigned short")
  elseif(SIZEOF_UNSIGNED_INT EQUAL 2)
    set(XERCES_U16BIT_INT "unsigned int")
  else()
    message(FATAL_ERROR "Couldn't find an unsigned 16-bit type")
  endif()

  if(SIZEOF_INT EQUAL 4)
    set(XERCES_S32BIT_INT "signed int")
  elseif(SIZEOF_LONG EQUAL 4)
    set(XERCES_S32BIT_INT "signed long")
  else()
    message(FATAL_ERROR "Couldn't find a signed 32-bit type")
  endif()

  if(SIZEOF_UNSIGNED_INT EQUAL 4)
    set(XERCES_U32BIT_INT "unsigned int")
  elseif(SIZEOF_UNSIGNED_LONG EQUAL 4)
    set(XERCES_U32BIT_INT "unsigned long")
  else()
    message(FATAL_ERROR "Couldn't find an unsigned 32-bit type")
  endif()

  if(SIZEOF_INT EQUAL 8)
    set(XERCES_S64BIT_INT "signed int")
  elseif(SIZEOF_LONG EQUAL 8)
    set(XERCES_S64BIT_INT "signed long")
  elseif(SIZEOF_LONG_LONG EQUAL 8)
    set(XERCES_S64BIT_INT "signed long long")
  elseif(SIZEOF___INT64 EQUAL 8)
    set(XERCES_S64BIT_INT "__int64")
  else()
    message(FATAL_ERROR "Couldn't find a signed 64-bit type")
  endif()

  if(SIZEOF_UNSIGNED_INT EQUAL 8)
    set(XERCES_U64BIT_INT "unsigned int")
  elseif(SIZEOF_UNSIGNED_LONG EQUAL 8)
    set(XERCES_U64BIT_INT "unsigned long")
  elseif(SIZEOF_UNSIGNED_LONG_LONG EQUAL 8)
    set(XERCES_U64BIT_INT "unsigned long long")
  elseif(SIZEOF_UNSIGNED__INT64 EQUAL 8)
    set(XERCES_U64BIT_INT "unsigned __int64")
  else()
    message(FATAL_ERROR "Couldn't find an unsigned 64-bit type")
  endif()
endif()
