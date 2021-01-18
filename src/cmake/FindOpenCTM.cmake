# - Find OpenCTM
# Find the OpenCTM library
#
#  OpenCTM::OpenCTM - Imported target to use
#  OPENCTM_FOUND - True if OpenCTM was found.
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

set(OPENCTM_ROOT_DIR
    "${OPENCTM_ROOT_DIR}"
    CACHE PATH "Directory to search for OpenCTM"
)
find_path(
    OPENCTM_INCLUDE_DIR
    NAMES openctm.h
    PATHS "${OPENCTM_ROOT_DIR}"
    PATH_SUFFIXES openctm include include/openctm
)
find_library(
    OPENCTM_LIBRARY
    NAMES openctm
    PATHS "${OPENCTM_ROOT_DIR}"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenCTM DEFAULT_MSG OPENCTM_INCLUDE_DIR OPENCTM_LIBRARY
)

if(OPENCTM_FOUND)
    if(NOT TARGET OpenCTM::OpenCTM)
        add_library(OpenCTM::OpenCTM UNKNOWN IMPORTED)
        set_target_properties(
            OpenCTM::OpenCTM
            PROPERTIES IMPORTED_LOCATION "${OPENCTM_LIBRARY}"
                       INTERFACE_INCLUDE_DIRECTORIES "${OPENCTM_INCLUDE_DIR}"
        )
    endif()
    set(OPENCTM_INCLUDE_DIRS ${OPENCTM_INCLUDE_DIR})
    set(OPENCTM_LIBRARIES ${OPENCTM_LIBRARY})
    mark_as_advanced(OPENCTM_ROOT_DIR)
endif()

mark_as_advanced(OPENCTM_INCLUDE_DIR OPENCTM_LIBRARY)
