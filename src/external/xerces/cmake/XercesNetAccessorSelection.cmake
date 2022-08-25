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

# netaccessor selection

option(network "Network support" ON)

if(network)
  find_library(SOCKET_LIBRARY socket)
  find_library(NSL_LIBRARY nsl)

  # netaccessors in order of preference

  # CURL

  # Requires select() which is UNIX only
  if(UNIX)
    find_package(CURL)
    if(CURL_FOUND)
      list(APPEND netaccessors curl)
    endif()
  endif()

  # Windows

  if(WIN32)
    check_include_file_cxx(winsock2.h HAVE_WINSOCK2_H)
    if(HAVE_WINSOCK2_H)
      set(winsock_available 1)
      list(APPEND netaccessors winsock)
    endif()
  endif()

  # socket

  check_include_file_cxx(sys/socket.h HAVE_SYS_SOCKET_H)
  if(HAVE_SYS_SOCKET_H)
    list(APPEND netaccessors socket)
  endif()

  # MacOS X CFURL

  set(cfurl_available 0)
  if(CMAKE_HOST_APPLE)
    check_include_file_cxx(CoreServices/CoreServices.h HAVE_CORESERVICES_CORESERVICES_H)
  find_library(CORE_SERVICES_LIBRARY CoreServices )
    if (HAVE_CORESERVICES_CORESERVICES_H AND CORE_SERVICES_LIBRARY)
      list(APPEND netaccessors cfurl)
      set(cfurl 1)
    endif()
  endif()

  string(REPLACE ";" "|" netaccessor_help "${netaccessors}")
  list(GET netaccessors 0 xerces_netaccessor_default)
  set(network-accessor "${xerces_netaccessor_default}" CACHE STRING "Network accessor (${netaccessor_help})")
  set(netaccessor "${network-accessor}")

  list(FIND netaccessors "${netaccessor}" netaccessor_found)
  if(netaccessor_found EQUAL -1)
    message(FATAL_ERROR "${netaccessor} netaccessor unavailable")
  endif()

  set(XERCES_USE_NETACCESSOR_CURL 0)
  set(XERCES_USE_NETACCESSOR_SOCKET 0)
  set(XERCES_USE_NETACCESSOR_CFURL 0)
  set(XERCES_USE_NETACCESSOR_WINSOCK 0)
  if(netaccessor STREQUAL "curl")
    set(XERCES_USE_NETACCESSOR_CURL 1)
  elseif(netaccessor STREQUAL "socket")
    set(XERCES_USE_NETACCESSOR_SOCKET 1)
  elseif(netaccessor STREQUAL "cfurl")
    set(XERCES_USE_NETACCESSOR_CFURL 1)
  elseif(netaccessor STREQUAL "winsock")
    set(XERCES_USE_NETACCESSOR_WINSOCK 1)
  else()
    message(FATAL_ERROR "Invalid netaccessor: \"${netaccessor}\"")
  endif()
else()
  set(netaccessor OFF)
endif(network)
