# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0
option(ALLOW_BUNDLED_EMBREE "Allow use of bundled embree source" ON)
option(ALLOW_SYSTEM_EMBREE "Allow use of system-provided embree" ON)

# looking for embree for the filter_embree
FIND_PACKAGE(embree 3.0)
FIND_PACKAGE(TBB)

if(ALLOW_SYSTEM_EMBREE AND TARGET embree AND TBB_FOUND)
	message(STATUS "- embree - using system-provided library")
	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree tbb)
else()
	message(STATUS "- embree - skipping embree library")
endif()
