# - Find lib3ds
# Find the lib3ds model access library
#
#  Lib3ds::Lib3ds - Imported target to use
#  LIB3DS_FOUND - True if lib3ds was found.
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

set(LIB3DS_ROOT_DIR
    "${LIB3DS_ROOT_DIR}"
    CACHE PATH "Directory to search for lib3ds")
find_path(
    LIB3DS_INCLUDE_DIR
    NAMES lib3ds/types.h
    PATHS "${LIB3DS_ROOT_DIR}")
find_library(
    LIB3DS_LIBRARY
    NAMES 3ds 3ds-1
    PATHS "${LIB3DS_ROOT_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIB3DS DEFAULT_MSG LIB3DS_INCLUDE_DIR
                                  LIB3DS_LIBRARY)

if(LIB3DS_FOUND)
    if(NOT TARGET Lib3ds::Lib3ds)
        add_library(Lib3ds::Lib3ds UNKNOWN IMPORTED)
        set_target_properties(
            Lib3ds::Lib3ds
            PROPERTIES IMPORTED_LOCATION "${LIB3DS_LIBRARY}"
                       INTERFACE_INCLUDE_DIRECTORIES "${LIB3DS_INCLUDE_DIR}")
    endif()
    set(LIB3DS_INCLUDE_DIRS ${LIB3DS_INCLUDE_DIR})
    set(LIB3DS_LIBRARIES ${LIB3DS_LIBRARY})
    mark_as_advanced(LIB3DS_ROOT_DIR)
endif()

mark_as_advanced(LIB3DS_INCLUDE_DIR LIB3DS_LIBRARY)
