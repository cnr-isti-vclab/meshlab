# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

# Only build if we have nexus
if (TARGET nexus)

	set(SOURCES filter_io_nxs.cpp)

	set(HEADERS filter_io_nxs.h)

	add_meshlab_plugin(filter_io_nxs MODULE ${SOURCES} ${HEADERS})

	target_link_libraries(filter_io_nxs PRIVATE nexus)

else()
	message(STATUS "Skipping filter_io_nxs - missing nexus")
endif()
