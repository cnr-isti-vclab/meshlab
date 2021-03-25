# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if (TARGET external-libE57Format)

    set(SOURCES io_e57.cpp)
    set(HEADERS io_e57.h)

    add_meshlab_plugin(io_e57 ${SOURCES} ${HEADERS})

else()
    message(STATUS "Skipping io_e57 - missing libE57Format in external directory as well as on system.")
endif()