# - Find Qhull
# Find the Qhull library
#
# Specify components:
#
# ::
#   libqhull       = Deprecated interface: use imported target Qhull::libqhull
#   qhullstatic    = Recommended alternative to re-entrant interface: use imported target Qhull::qhullstatic
#   qhull_r        = Recommended (re-entrant) interface: use imported target Qhull::qhull_r
#   qhullstatic_r  = Recommended (re-entrant) interface: use imported target Qhull::qhullstatic_r
#   qhullcpp       = C++ interface: use imported target Qhull::libqhullcpp
#
#  QHULL_FOUND - True if Qhull was found.
#
# Original Author:
# 2019 Ryan Pavlik <ryan.pavlik@collabora.com> <ryan.pavlik@gmail.com>
#
# Copyright 2019, Collabora, Ltd.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# SPDX-License-Identifier: BSL-1.0

set(QHULL_ROOT_DIR
    "${QHULL_ROOT_DIR}"
    CACHE PATH "Directory to search for Qhull")

# Look for a CMake config file
find_package(Qhull QUIET NO_MODULE)
set(_qh_found_any FALSE)
foreach(_qh libqhull qhullstatic qhull_r qhullstatic_r qhullcpp)
    if(TARGET Qhull::${_qh})
        set(Qhull_${_qh}_FOUND TRUE)
        set(_qh_found_any TRUE)
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
if(_qh_found_any)
    # Populate QHULL_bla_INCLUDE_DIR from imported targets
    foreach(_qh libqhull qhull_r qhullcpp)
        if(TARGET Qhull::${_qh})
            get_target_property(QHULL_${_qh}_INCLUDE_DIR Qhull::${_qh} INTERFACE_INCLUDE_DIRECTORIES)
        endif()
    endforeach()
    find_package_handle_standard_args(Qhull REQUIRED_VARS _qh_found_any HANDLE_COMPONENTS CONFIG_MODE)

else()
    find_path(
        QHULL_libqhull_INCLUDE_DIR
        NAMES libqhull/libqhull.h
        PATHS "${QHULL_ROOT_DIR}")
    find_path(
        QHULL_qhull_r_INCLUDE_DIR
        NAMES libqhull_r/libqhull_r.h
        PATHS "${QHULL_ROOT_DIR}")
    find_path(
        QHULL_qhullcpp_INCLUDE_DIR
        NAMES libqhullcpp/Qhull.h
        PATHS "${QHULL_ROOT_DIR}")
    foreach(_qh_lib libqhull qhull_r qhullstatic qhullstatic_r qhullcpp)
        find_library(
            QHULL_${_qh_lib}_LIBRARY
            NAMES ${_qh_lib} lib${_qh_lib}
            PATHS "${QHULL_ROOT_DIR}")
    endforeach()

    # Manually search for qhullstatic as a libqhull library with a static library suffix
    # (The extra condition is to avoid confusing a .dll import library with a static library)
    if(NOT QHULL_qhullstatic_LIBRARY AND NOT "${CMAKE_LINK_LIBRARY_SUFFIX}" STREQUAL "${CMAKE_STATIC_LIBRARY_SUFFIX}")
        set(_qh_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
        find_library(
            QHULL_qhullstatic_LIBRARY
            NAMES qhullstatic libqhullstatic libqhull
            PATHS "${QHULL_ROOT_DIR}")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${_qh_CMAKE_FIND_LIBRARY_SUFFIXES})
        unset(_qh_CMAKE_FIND_LIBRARY_SUFFIXES)
    endif()
    if(NOT Qhull_FIND_COMPONENTS)
        set(Qhull_FIND_COMPONENTS qhull_r)
    endif()

    set(_qh_required_vars)
    foreach(component ${Qhull_FIND_COMPONENTS})
        foreach(candidate libqhull qhull_r qhullstatic qhullstatic_r qhullcpp)
            if("${component}" STREQUAL "${candidate}")
                string(REPLACE "static" "" candidate_nonstatic "${candidate}")
                list(APPEND _qh_required_vars QHULL_${candidate}_LIBRARY QHULL_${candidate_nonstatic}_INCLUDE_DIR)
                if(QHULL_${candidate}_LIBRARY AND QHULL_${candidate_nonstatic}_INCLUDE_DIR)
                    set(Qhull_${candidate}_FOUND TRUE)
                endif()
            endif()
        endforeach()
    endforeach()

    find_package_handle_standard_args(Qhull REQUIRED_VARS ${_qh_required_vars} HANDLE_COMPONENTS)

    if(Qhull_qhull_FOUND AND NOT TARGET Qhull::libqhull)
        add_library(Qhull::libqhull SHARED IMPORTED)
        set_target_properties(Qhull::libqhull PROPERTIES IMPORTED_LOCATION "${QHULL_libqhull_LIBRARY}"
                                                      INTERFACE_INCLUDE_DIRECTORIES "${QHULL_libqhull_INCLUDE_DIR}")
    endif()
    if(Qhull_qhullstatic_FOUND AND NOT TARGET Qhull::qhullstatic)
        add_library(Qhull::qhullstatic STATIC IMPORTED)
        set_target_properties(Qhull::qhullstatic PROPERTIES IMPORTED_LOCATION "${QHULL_qhullstatic_LIBRARY}"
                                                            INTERFACE_INCLUDE_DIRECTORIES "${QHULL_libqhull_INCLUDE_DIR}")
    endif()
    if(Qhull_qhull_r_FOUND AND NOT TARGET Qhull::qhull_r)
        add_library(Qhull::qhull_r SHARED IMPORTED)
        set_target_properties(Qhull::qhull_r PROPERTIES IMPORTED_LOCATION "${QHULL_qhull_r_LIBRARY}"
                                                        INTERFACE_INCLUDE_DIRECTORIES "${QHULL_qhull_r_INCLUDE_DIR}")
    endif()
    if(Qhull_qhullstatic_r_FOUND AND NOT TARGET Qhull::qhullstatic_r)
        add_library(Qhull::qhullstatic_r STATIC IMPORTED)
        set_target_properties(
            Qhull::qhullstatic_r PROPERTIES IMPORTED_LOCATION "${QHULL_qhullstatic_r_LIBRARY}"
                                            INTERFACE_INCLUDE_DIRECTORIES "${QHULL_qhull_r_INCLUDE_DIR}")
    endif()
    if(Qhull_qhullcpp_FOUND AND NOT TARGET Qhull::qhullcpp)
        add_library(Qhull::qhullcpp STATIC IMPORTED)
        set_target_properties(Qhull::qhullcpp PROPERTIES IMPORTED_LOCATION "${QHULL_qhullcpp_LIBRARY}"
                                                         INTERFACE_INCLUDE_DIRECTORIES "${QHULL_qhullcpp_INCLUDE_DIR}")
    endif()

endif()

if(QHULL_FOUND)
    mark_as_advanced(QHULL_ROOT_DIR)
endif()

mark_as_advanced(
    QHULL_libqhull_INCLUDE_DIR
    QHULL_libqhull_LIBRARY
    QHULL_qhull_r_INCLUDE_DIR
    QHULL_qhull_r_LIBRARY
    QHULL_qhullcpp_INCLUDE_DIR
    QHULL_qhullcpp_LIBRARY
    QHULL_qhullstatic_LIBRARY
    QHULL_qhullstatic_r_LIBRARY)
