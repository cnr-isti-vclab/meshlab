# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if (TARGET external-embree)
	set(SOURCES filter_embree.cpp)

	set(HEADERS filter_embree.h)

	add_meshlab_plugin(filter_embree ${SOURCES} ${HEADERS})

	target_link_libraries(filter_embree PRIVATE external-embree)
else()
	message(
		STATUS "Skipping filter_embree - don't know about embree on this system.")
endif()
