# Try to find the GMP library
# https://gmplib.org/
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(GMP 6.0.0)
# to require version 6.0.0 or newer of GMP.
#
# Once done this will define
#
#  GMP_FOUND - system has GMP lib with correct version
#  GMP_INCLUDE_DIRS - the GMP include directory
#  GMP_LIBRARIES - the GMP library
#  GMP_VERSION - GMP version
#
# Copyright (c) 2016 Jack Poulson, <jack.poulson@gmail.com>
# Copyright (c) 2019 Collabora, Ltd. (Ryan Pavlik <ryan.pavlik@collabora.com>)
# Redistribution and use is allowed according to the terms of the BSD license.

find_path(
  GMP_INCLUDE_DIR
  NAMES gmp.h
  PATHS $ENV{GMPDIR})


if(GMP_INCLUDE_DIR)
  # Since the GMP version macros may be in a file included by gmp.h of the form
  # gmp-.*[_]?.*.h (e.g., gmp-x86_64.h), we search each of them.
  file(GLOB GMP_HEADERS "${GMP_INCLUDE_DIR}/gmp.h" "${GMP_INCLUDE_DIR}/gmp-*.h")
  foreach(gmp_header_filename ${GMP_HEADERS})
    file(READ "${gmp_header_filename}" _gmp_version_header)
    string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION[ \t]+([0-9]+)"
                 _gmp_major_version_match "${_gmp_version_header}")
    if(_gmp_major_version_match)
      set(GMP_MAJOR_VERSION "${CMAKE_MATCH_1}")
      string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION_MINOR[ \t]+([0-9]+)"
                   _gmp_minor_version_match "${_gmp_version_header}")
      set(GMP_MINOR_VERSION "${CMAKE_MATCH_1}")
      string(REGEX MATCH
                   "define[ \t]+__GNU_MP_VERSION_PATCHLEVEL[ \t]+([0-9]+)"
                   _gmp_patchlevel_version_match "${_gmp_version_header}")
      set(GMP_PATCHLEVEL_VERSION "${CMAKE_MATCH_1}")
      set(GMP_VERSION
          ${GMP_MAJOR_VERSION}.${GMP_MINOR_VERSION}.${GMP_PATCHLEVEL_VERSION})
    endif()
  endforeach()
endif()

find_library(GMP_LIBRARY gmp PATHS $ENV{GMPDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP
                                  REQUIRED_VARS GMP_INCLUDE_DIR GMP_LIBRARY
                                  VERSION_VAR GMP_VERSION)
if(GMP_FOUND)
  set(GMP_LIBRARIES ${GMP_LIBRARY})
  set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})
endif()
mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)
