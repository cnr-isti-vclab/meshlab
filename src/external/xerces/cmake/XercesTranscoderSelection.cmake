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

# transcoder selection

# transcoders in order of preference

# ICU

if(ICU_FOUND)
  list(APPEND transcoders icu)
endif()

# MacOS

set(macosunicodeconverter_available 0)
if(CMAKE_HOST_APPLE)
  check_include_file_cxx(CoreServices/CoreServices.h HAVE_CORESERVICES_CORESERVICES_H)
  find_library(CORE_SERVICES_LIBRARY CoreServices )
  if (HAVE_CORESERVICES_CORESERVICES_H AND CORE_SERVICES_LIBRARY)
    list(APPEND transcoders macosunicodeconverter)
    set(macosunicodeconverter_available 1)
  endif()
endif()

# GNU iconv

check_include_file_cxx(iconv.h HAVE_ICONV_H)
check_include_file_cxx(wchar.h HAVE_WCHAR_H)
check_include_file_cxx(string.h HAVE_STRING_H)
check_include_file_cxx(stdlib.h HAVE_STDLIB_H)
check_include_file_cxx(stdio.h HAVE_STDIO_H)
check_include_file_cxx(ctype.h HAVE_CTYPE_H)
check_include_file_cxx(locale.h HAVE_LOCALE_H)
check_include_file_cxx(errno.h HAVE_ERRNO_H)
check_include_file_cxx(endian.h HAVE_ENDIAN_H)
check_include_file_cxx(machine/endian.h HAVE_MACHINE_ENDIAN_H)
check_include_file_cxx(arpa/nameser_compat.h HAVE_ARPA_NAMESER_COMPAT_H)
check_function_exists(iconv_open HAVE_ICONV_OPEN)
check_function_exists(iconv_close HAVE_ICONV_CLOSE)
check_function_exists(iconv HAVE_ICONV)

set(gnuiconv_available 0)
if(HAVE_ICONV_H AND HAVE_WCHAR_H AND HAVE_STRING_H AND HAVE_STDLIB_H AND
    HAVE_STDIO_H AND HAVE_CTYPE_H AND HAVE_LOCALE_H AND HAVE_ERRNO_H)
  if (HAVE_ENDIAN_H OR HAVE_MACHINE_ENDIAN_H OR HAVE_ARPA_NAMESER_COMPAT_H)
    if(HAVE_ICONV_OPEN AND HAVE_ICONV_CLOSE AND HAVE_ICONV)
      set(gnuiconv_available 1)
      list(APPEND transcoders gnuiconv)
    endif()
  endif()
endif()

# Windows

if(WIN32)
  set(windows_available 1)
  list(APPEND transcoders windows)
endif()

# iconv

check_include_file_cxx(wchar.h HAVE_WCHAR_H)
check_function_exists(mblen HAVE_MBLEN)
check_function_exists(wcstombs HAVE_WCSTOMBS)
check_function_exists(mbstowcs HAVE_MBSTOWCS)

set(iconv_available 0)
if(HAVE_WCHAR_H AND HAVE_MBLEN AND HAVE_WCSTOMBS AND HAVE_MBSTOWCS)
  set(iconv_available 1)
  list(APPEND transcoders iconv)
endif()

string(REPLACE ";" "|" transcoder_help "${transcoders}")
list(GET transcoders 0 xerces_transcoder_default)
set(transcoder "${xerces_transcoder_default}" CACHE STRING "Transcoder (${transcoder_help})")
set(transcoder "${transcoder}")

list(FIND transcoders "${transcoder}" transcoder_found)
if(transcoder_found EQUAL -1)
  message(FATAL_ERROR "${transcoder} transcoder unavailable")
endif()

set(XERCES_USE_TRANSCODER_ICU 0)
set(XERCES_USE_TRANSCODER_ICONV 0)
set(XERCES_USE_TRANSCODER_GNUICONV 0)
set(XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER 0)
set(XERCES_USE_TRANSCODER_WINDOWS 0)
if(transcoder STREQUAL "icu")
  set(XERCES_USE_TRANSCODER_ICU 1)
elseif(transcoder STREQUAL "iconv")
  set(XERCES_USE_TRANSCODER_ICONV 1)
elseif(transcoder STREQUAL "gnuiconv")
  set(XERCES_USE_TRANSCODER_GNUICONV 1)
elseif(transcoder STREQUAL "macosunicodeconverter")
  set(XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER 1)
elseif(transcoder STREQUAL "windows")
  set(XERCES_USE_TRANSCODER_WINDOWS 1)
else()
  message(FATAL_ERROR "Invalid transcoder: \"${transcoder}\"")
endif()
