# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have OpenCTM
if(TARGET external-openctm)

    set(SOURCES io_ctm.cpp)

    set(HEADERS io_ctm.h)

	add_meshlab_plugin(io_ctm ${SOURCES} ${HEADERS})

    target_link_libraries(io_ctm PRIVATE external-openctm)
else()
    message(STATUS "Skipping io_ctm - missing OpenCTM.")
endif()
