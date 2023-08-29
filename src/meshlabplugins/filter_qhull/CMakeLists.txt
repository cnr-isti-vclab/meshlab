# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have qhull
if(TARGET external-qhull)

	set(SOURCES filter_qhull.cpp qhull_tools.cpp)

	set(HEADERS filter_qhull.h qhull_tools.h)

	add_meshlab_plugin(filter_qhull ${SOURCES} ${HEADERS})

	target_link_libraries(filter_qhull PRIVATE external-qhull)

else()
	message(STATUS "Skipping filter_qhull - missing qhull")
endif()
