# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have muparser
if(TARGET external-muparser)

    set(SOURCES filter_func.cpp)

    set(HEADERS filter_func.h filter_refine.h string_conversion.h)

	add_meshlab_plugin(filter_func ${SOURCES} ${HEADERS})

    target_link_libraries(filter_func PRIVATE external-muparser)

else()
    message(STATUS "Skipping filter_func - don't have muparser.")
endif()
