# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(BUILD_BUNDLED_SOURCES_WITHOUT_WARNINGS "Should warnings be disabled on bundled source code?" ON)
add_library(external-disable-warnings INTERFACE)
if(BUILD_BUNDLED_SOURCES_WITHOUT_WARNINGS)
	if(MSVC)
		# TODO
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(external-disable-warnings INTERFACE -w)
	endif()
endif()

# GLEW - required
include(${EXTERNAL_DIR}/glew.cmake)
