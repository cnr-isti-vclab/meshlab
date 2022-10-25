# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0
option(ALLOW_BUNDLED_EMBREE "Allow use of bundled embree dll (windows only)" ON)
option(ALLOW_SYSTEM_EMBREE "Allow use of system-provided embree" ON)

# looking for embree for the filter_embree
find_package(embree 3.0)
find_package(TBB)

set(TBB_WIN_DIR "${CMAKE_CURRENT_LIST_DIR}/oneapi-tbb-2021.6.0")
set(EMBREE_WIN_DIR "${CMAKE_CURRENT_LIST_DIR}/embree-3.13.5.x64.vc14.windows")

if(ALLOW_SYSTEM_EMBREE AND TARGET embree AND TBB_FOUND)
	message(STATUS "- embree - using system-provided library")
	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree tbb)
elseif(ALLOW_BUNDLED_EMBREE AND WIN32 AND EXISTS "${EMBREE_DIR}/lib/embree3.lib")
	message(STATUS "- embree - using bundled dll")

	set(TBB_DIR "${TBB_WIN_DIR}/lib/cmake/tbb")
	set(embree_DIR "${EMBREE_WIN_DIR}/lib/cmake/embree-3.13.5")
	find_package(embree)

	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree)
else()
	message(STATUS "- embree - skipping embree library")
endif()
