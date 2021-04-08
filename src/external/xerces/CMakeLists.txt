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

# Run "cmake" to generate the build files for your platform

cmake_minimum_required(VERSION 3.2.0)

# Use new variable expansion policy.
if (POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW)
endif(POLICY CMP0053)
if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif(POLICY CMP0054)
if (POLICY CMP0067)
  cmake_policy(SET CMP0067 NEW)
endif(POLICY CMP0067)

# Try C++14, then fall back to C++11 and C++98.  Used for feature tests
# for optional features.
set(CMAKE_CXX_STANDARD 14)

# Use folders (for IDE project grouping)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Source additional modules from the "cmake" directory
list(APPEND CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Read version information from configure.ac.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/configure.ac" configure
     REGEX "^AC_INIT\\(\\[xerces-c\\],\\[(.*)\\]\\)")
foreach(line IN LISTS configure)
  string(REGEX REPLACE "^AC_INIT\\(\\[xerces-c\\],\\[(.*)\\]\\)" "\\1" xerces_c_version "${line}")
  break()
endforeach()
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/configure.ac" configure
     REGEX "^INTERFACE_VERSION=.*")
foreach(line IN LISTS configure)
  string(REGEX REPLACE "^INTERFACE_VERSION=(.*)" "\\1" xerces_c_interface_version "${line}")
  break()
endforeach()
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/configure.ac" configure
     REGEX "^GRAMMAR_SERIALIZATION_LEVEL=.*")
foreach(line IN LISTS configure)
  string(REGEX REPLACE "^GRAMMAR_SERIALIZATION_LEVEL=(.*)" "\\1" xerces_c_grammar_serialization_level "${line}")
  break()
endforeach()

message(STATUS "Configuring Apache Xerces-C++ version ${xerces_c_version}")

# Project version
project(xerces-c
        VERSION "${xerces_c_version}"
        LANGUAGES C CXX)
enable_testing()

# For autotools header compatibility
set(PACKAGE "${PROJECT_NAME}")
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_TARNAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "c-dev@xerces.apache.org")
set(PACKAGE_URL "https://xerces.apache.org/xerces-c/")

set(VERSION "${PROJECT_VERSION}")
set(XERCES_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(XERCES_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(XERCES_VERSION_REVISION "${PROJECT_VERSION_PATCH}")
string(REPLACE "." ";" interface_version_parts "${xerces_c_interface_version}")
list(GET interface_version_parts 0 INTERFACE_VERSION_MAJOR)
list(GET interface_version_parts 1 INTERFACE_VERSION_MINOR)

set(INTERFACE_VERSION_D "${INTERFACE_VERSION_MAJOR}.${INTERFACE_VERSION_MINOR}")
set(INTERFACE_VERSION_U "${INTERFACE_VERSION_MAJOR}_${INTERFACE_VERSION_MINOR}")

set(XERCES_GRAMMAR_SERIALIZATION_LEVEL "${xerces_c_grammar_serialization_level}")

set(EXTRA_DIST
  xerces-c.spec
  xerces-c.pc.in
  CREDITS
  INSTALL
  KEYS
  LICENSE
  NOTICE
  README
  config/pretty-make
  doc
  projects
  reconf
  scripts/createBindingsFormatFromHPP.pl
  scripts/makeStringDefinition.pl
  tools
)

include(GNUInstallDirs)
include(XercesWarnings)
include(XercesIncludes)
include(XercesFunctions)
include(XercesDLL)
include(XercesIntTypes)
include(XercesPathDelimiters)
include(XercesICU)
include(XercesMutexMgrSelection)
include(XercesNetAccessorSelection)
include(XercesMsgLoaderSelection)
include(XercesTranscoderSelection)
include(XercesFileMgrSelection)
include(XercesXMLCh)
include(XercesOperatorDelete)
include(XercesBool)
include(XercesConst)
include(XercesInline)
include(XercesVolatile)
include(XercesLString)
include(XercesMFC)
include(XercesNamespaces)
include(XercesStdLibs)
include(XercesStdNamespace)
include(XercesSSE2)
include(XercesPathMax)
include(XercesWChar)
include(XercesIconvConst)
include(XercesLFS)

# Generate headers
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  @ONLY)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/xercesc/util/Xerces_autoconf_config.hpp.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/xercesc/util/Xerces_autoconf_config.hpp
  @ONLY)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/xercesc/util/XercesVersion.hpp.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/xercesc/util/XercesVersion.hpp
  @ONLY)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/style/dtd/entities.ent.in
  ${CMAKE_CURRENT_BINARY_DIR}/doc/style/dtd/entities.ent
  @ONLY)

# Generate pkg-config file
set(pkgconfig-dir "${CMAKE_INSTALL_LIBDIR}/pkgconfig" CACHE STRING "pkg-config installation directory (default ${CMAKE_INSTALL_LIBDIR}/pkgconfig)")
set(PKGCONFIGDIR "${pkgconfig-dir}")

set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "${CMAKE_INSTALL_PREFIX}")
set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xerces-c.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/xerces-c.pc)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/xerces-c.pc
  DESTINATION "${PKGCONFIGDIR}"
  COMPONENT "development")

# Process subdirectories
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(samples)

# Display configuration summary
message(STATUS "")
message(STATUS "Xerces-C++ configuration summary")
message(STATUS "--------------------------------")
message(STATUS "")
message(STATUS "  Version:                   ${xerces-c_VERSION}")
message(STATUS "  Library interface version: ${xerces_c_interface_version}")
message(STATUS "")
message(STATUS "  Installation directory:    ${prefix}")
message(STATUS "  C compiler:                ${CMAKE_C_COMPILER}")
message(STATUS "  C++ compiler:              ${CMAKE_CXX_COMPILER}")
message(STATUS "")
message(STATUS "  Build shared libraries:    ${BUILD_SHARED_LIBS}")
message(STATUS "  Path delimiters:           \"${path_delims}\"")
message(STATUS "  File Manager:              ${filemgr}")
message(STATUS "  Mutex Manager:             ${mutexmgr}")
message(STATUS "  Transcoder:                ${transcoder}")
message(STATUS "  NetAccessor:               ${netaccessor}")
message(STATUS "  Message Loader:            ${msgloader}")
message(STATUS "  XMLCh type:                ${xmlch_type}")
