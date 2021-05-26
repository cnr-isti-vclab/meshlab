# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_LIBIGL "Allow use of bundled LIBIGL source" ON)

set(LIBIGL_DIR ${CMAKE_CURRENT_LIST_DIR}/libigl-2.3.0)

if(ALLOW_BUNDLED_LIBIGL AND EXISTS "${LIBIGL_DIR}/include/igl/igl_inline.h")
	message(STATUS "- libigl - using bundled source")
	add_library(external-libigl INTERFACE)
	target_include_directories(external-libigl INTERFACE "${LIBIGL_DIR}/include/")
endif()
