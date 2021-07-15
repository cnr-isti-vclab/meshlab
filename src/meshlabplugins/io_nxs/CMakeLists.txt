# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

# Only build if we have nexus
if (TARGET nexus)

	set(SOURCES io_nxs.cpp)

	set(HEADERS io_nxs.h)

	add_meshlab_plugin(io_nxs MODULE ${SOURCES} ${HEADERS})

	target_link_libraries(io_nxs PRIVATE nexus)

else()
	message(STATUS "Skipping io_nxs - missing nexus")
endif()
