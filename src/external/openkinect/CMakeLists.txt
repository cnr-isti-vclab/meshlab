######################################################################################
# License
######################################################################################

# This file is part of the OpenKinect Project. http://www.openkinect.org
#
# Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
# for details.
#
# This code is licensed to you under the terms of the Apache License, version
# 2.0, or, at your option, the terms of the GNU General Public License,
# version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
# or the following URLs:
# http://www.apache.org/licenses/LICENSE-2.0
# http://www.gnu.org/licenses/gpl-2.0.txt
#
# If you redistribute this file in source form, modified or unmodified, you
# may:
#   1) Leave this header intact and distribute it under the same terms,
#      accompanying it with the APACHE20 and GPL20 files, or
#   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
#   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
# In all cases you must keep the copyright notice intact and include a copy
# of the CONTRIB file.
#
# Binary distributions must follow the binary distribution requirements of
# either License.

######################################################################################
# CMake directives
######################################################################################

#Require 2.6 or higher. 

cmake_minimum_required(VERSION 2.6)

######################################################################################
# Project declaration and options
######################################################################################

PROJECT(libfreenect)

set (PROJECT_VER_MAJOR 0)
set (PROJECT_VER_MINOR 0)
set (PROJECT_VER_PATCH 1)
set (PROJECT_VER 
  "${PROJECT_VER_MAJOR}.${PROJECT_VER_MINOR}.${PROJECT_VER_PATCH}")
set (PROJECT_APIVER 
  "${PROJECT_VER_MAJOR}.${PROJECT_VER_MINOR}")

OPTION(BUILD_EXAMPLES "Build example programs" ON)
OPTION(BUILD_FAKENECT "Build fakenect mock library" ON)
OPTION(BUILD_C_SYNC "Build c synchronous library" ON)
OPTION(BUILD_CV "Build OpenCV wrapper" OFF)
OPTION(BUILD_AS3_SERVER "Build the Actionscript 3 Server Example" OFF)
IF(PROJECT_OS_LINUX)
	OPTION(BUILD_CPACK "Build an RPM or DEB using CPack" OFF)
ENDIF(PROJECT_OS_LINUX)

# Doesn't work on windows yet.
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "libfreenect is not currently supported on this platform")
ENDIF()

######################################################################################
# CMake Modules
######################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

# Find the host operating system and architecture
include (FindOS)
# Set up installation directories
include (SetupDirectories)

# Find packages needed to build library
find_package(libusb-1.0 REQUIRED)

# Check the endianness of the system
include (TestBigEndian)
test_big_endian(BIG_ENDIAN)
if(BIG_ENDIAN)
  add_definitions(-DFN_BIGENDIAN)
endif()

######################################################################################
# CMake 
######################################################################################

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(DOC_OUTPUT_PATH ${CMAKE_BINARY_DIR}/doc)

# let CFLAGS env override this
if(CMAKE_C_FLAGS STREQUAL "")
  set(CMAKE_C_FLAGS "-O2")
endif()
add_definitions(-Wall)

# Pretty much everyone is going to need the main includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

# libfreenect.h includes libusb.h, so everyone needs this too
include_directories(${LIBUSB_1_INCLUDE_DIRS})

# Add library project
add_subdirectory (src)

# Add examples
IF(BUILD_EXAMPLES)
  add_subdirectory (examples)
ENDIF()

IF(BUILD_FAKENECT)
  add_subdirectory (fakenect)
ENDIF()

IF(BUILD_C_SYNC)
  add_subdirectory (wrappers/c_sync)
ENDIF()

IF(BUILD_CV)
  add_subdirectory (wrappers/opencv)
ENDIF()

IF(BUILD_AS3_SERVER)
  add_subdirectory(wrappers/actionscript)
ENDIF()

######################################################################################
# Extras
######################################################################################

# Create an uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/UninstallTarget.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/UninstallTarget.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/UninstallTarget.cmake)

# Create Debian/RPM Packages
# after make, use "fakeroot cpack" in the build Dir to complete

IF ( BUILD_CPACK )
  set(CPACK_PACKAGE_DESCRIPTION "libfreenect for kinect")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "libfreenect library for using kinect")
  set(CPACK_PACKAGE_NAME "libfreenect-dev")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libusb-1.0.0-dev")

  set(CPACK_PACKAGE_CONTACT "OpenKinect <openkinect@googlegroups.com>")
  #set(CPACK_PACKAGE_VENDOR "")
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VER_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VER_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VER_PATCH})
  set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

  set(CPACK_GENERATOR "DEB;RPM;")
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CMAKE_SYSTEM_PROCESSOR}")

  include(CPack)

  INSTALL(FILES "${CMAKE_BINARY_DIR}/lib/libfreenect.a" DESTINATION ${PROJECT_LIBRARY_INSTALL_DIR})
  INSTALL(FILES "include/libfreenect.h" DESTINATION ${PROJECT_INCLUDE_INSTALL_DIR})
  INSTALL(FILES "APACHE20" DESTINATION "share/doc/${CPACK_PACKAGE_NAME}")
  INSTALL(FILES "GPL2" DESTINATION "share/doc/${CPACK_PACKAGE_NAME}")
  INSTALL(FILES "README.asciidoc" DESTINATION "share/doc/${CPACK_PACKAGE_NAME}")
  INSTALL(FILES "doc/kinect_protocol.asciidoc" DESTINATION "share/doc/${CPACK_PACKAGE_NAME}")

ENDIF ( BUILD_CPACK )

