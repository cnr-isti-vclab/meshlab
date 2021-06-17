# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_NEXUS "Allow use of bundled nexus source" ON)

if(ALLOW_BUNDLED_NEXUS AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/nexus/CMakeLists.txt")
	message(STATUS "- nexus - using bundled source")
	set (BUILD_NXS_BUILD OFF)
	set (BUILD_NXS_EDIT OFF)
	set (BUILD_NXS_VIEW OFF)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/nexus)
endif()
