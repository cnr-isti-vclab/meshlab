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

# message loader selection

# message loaders in order of preference
list(APPEND msgloaders inmemory)

if(ICU_FOUND)
  list(APPEND msgloaders icu)
endif()

include(CheckIncludeFileCXX)
include(CheckFunctionExists)
check_include_file_cxx(nl_types.h HAVE_NLTYPES_H)
check_function_exists(catopen HAVE_CATOPEN)
check_function_exists(catclose HAVE_CATCLOSE)
check_function_exists(catgets HAVE_CATGETS)
if(HAVE_NLTYPES_H AND HAVE_CATOPEN AND HAVE_CATCLOSE AND HAVE_CATGETS)
  set(iconv_available 1)
endif()
if(iconv_available)
  list(APPEND msgloaders iconv)
endif()

string(REPLACE ";" "|" msgloader_help "${msgloaders}")
list(GET msgloaders 0 xerces_msgloader_default)
set(message-loader "${xerces_msgloader_default}" CACHE STRING "Message loader (${msgloader_help})")
set(msgloader "${message-loader}")

list(FIND msgloaders "${msgloader}" msgloader_found)
if(msgloader_found EQUAL -1)
  message(FATAL_ERROR "${msgloader} message loader unavailable")
endif()

set(XERCES_USE_MSGLOADER_ICU 0)
set(XERCES_USE_MSGLOADER_ICONV 0)
set(XERCES_USE_MSGLOADER_INMEMORY 0)
if(msgloader STREQUAL "icu")
  set(XERCES_USE_MSGLOADER_ICU 1)
elseif(msgloader STREQUAL "iconv")
  set(XERCES_USE_MSGLOADER_ICONV 1)
elseif(msgloader STREQUAL "inmemory")
  set(XERCES_USE_MSGLOADER_INMEMORY 1)
else()
  message(FATAL_ERROR "Invalid message loader: \"${msgloader}\"")
endif()
