# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

# GLEW - required
include(${EXTERNAL_DIR}/glew.cmake)

if(NOT BUILD_MINI)
	message(STATUS "Searching for optional components")
	include(${EXTERNAL_DIR}/external.cmake)
endif()
