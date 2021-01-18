# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_U3D "Allow use of bundled u3d source" ON)

if(ALLOW_BUNDLED_U3D AND EXISTS "${EXTERNAL_DIR}/u3d/CMakeLists.txt")
	message(STATUS "- u3d - using bundled source")
	add_subdirectory(${EXTERNAL_DIR}/u3d)
endif()
