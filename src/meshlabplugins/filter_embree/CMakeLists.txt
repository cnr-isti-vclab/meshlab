# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if (TARGET external-embree)
	set(SOURCES filter_embree.cpp)

	set(HEADERS filter_embree.h)

	add_meshlab_plugin(filter_embree ${SOURCES} ${HEADERS})

	target_link_libraries(filter_embree PRIVATE external-embree)
	if(OpenMP_CXX_FOUND)
		target_link_libraries(filter_embree PRIVATE OpenMP::OpenMP_CXX)
	endif()
else()
	message(
		STATUS "Skipping filter_embree - don't know about embree or omp on this system.")
endif()
