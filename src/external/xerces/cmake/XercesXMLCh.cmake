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

# check if the Windows API is defined as using wchar_t or unsigned
# short; if it's wchar_t, we need to map XMLCh to be wchar_t (this is
# safe because on Windows wchar_t is used to store UTF-16 codepoints,
# while it is not true on Unix)

include(CheckCXXSourceCompiles)
include(CheckTypeSize)
include(XercesIntTypes)

check_cxx_source_compiles("
int main() {
  const char16_t *unicode = u\"Test ünícodè → ©\";
  return 0;
}" HAVE_STD_char16_t)

if(HAVE_STD_char16_t)
  check_type_size("char16_t" SIZEOF_CHAR16_T LANGUAGE CXX)
  check_type_size("wchar_t" SIZEOF_WCHAR_T LANGUAGE CXX)

  if(NOT SIZEOF_CHAR16_T EQUAL 2)
    message(WARNING "char16_t is not a 16-bit type")
  elseif(WIN32 AND NOT SIZEOF_WCHAR_T EQUAL 2)
    message(WARNING "wchar_t is not a 16-bit type, and size differs from char16_t")
  else()
    list(APPEND xmlch_types char16_t)
  endif()
endif()

if(WIN32)
  check_cxx_source_compiles("
#include <windows.h>

wchar_t file[] = L\"dummy.file\";

int main() {
  DeleteFileW(file);
  return 0;
}"
      WINDOWS_wchar)

  if(WINDOWS_wchar)
    list(APPEND xmlch_types wchar_t)
  endif()
endif()

list(APPEND xmlch_types uint16_t)

string(REPLACE ";" "|" xmlch_type_help "${xmlch_types}")
list(GET xmlch_types 0 xerces_xmlch_type_default)
set(xmlch-type "${xerces_xmlch_type_default}" CACHE STRING "XMLCh type (${xmlch_type_help})")
set(xmlch_type "${xmlch-type}")

list(FIND xmlch_types "${xmlch_type}" xmlch_type_found)
if(xmlch_type_found EQUAL -1)
  message(FATAL_ERROR "${xmlch_type} xmlch_type unavailable")
endif()

set(XERCES_XMLCH_T ${XERCES_U16BIT_INT})
set(XERCES_USE_CHAR16_T 0)
set(XERCES_INCLUDE_WCHAR_H 0)
if(xmlch_type STREQUAL "char16_t")
  set(XERCES_XMLCH_T char16_t)
  set(XERCES_USE_CHAR16_T 1)
elseif(xmlch_type STREQUAL "wchar_t")
set(XERCES_XMLCH_T wchar_t)
set(XERCES_INCLUDE_WCHAR_H 1)
endif()
