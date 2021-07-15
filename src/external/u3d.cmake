# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_U3D "Allow use of bundled u3d source" ON)

if(ALLOW_BUNDLED_U3D AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/u3d/CMakeLists.txt")
	message(STATUS "- u3d - using bundled source")

	set(MESSAGE_QUIET ON)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/u3d)
	unset(MESSAGE_QUIET)
endif()
